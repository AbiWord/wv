/* vim: set sw=8: -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/**
 * ms-ole-vba.c: MS Office VBA support
 *
 * Author:
 *    Michael Meeks (michael@imaginator.com)
 *
 * Copyright 2000 Helix Code, Inc.
 **/

/* #include <libole2-config.h> */
#include <libole2/ms-ole-vba.h>

#include <stdio.h>

#undef VBA_DEBUG


struct _MsOleVba {
	MsOleStream *s;
	GArray      *text;
	int          pos;
};

inline gboolean
ms_ole_vba_eof (MsOleVba *vba)
{
	return !vba || (vba->pos >= vba->text->len - 1);
}

char
ms_ole_vba_getc (MsOleVba *vba)
{
	g_assert (!ms_ole_vba_eof (vba));

	return g_array_index (vba->text, guint8, vba->pos++);
}

char
ms_ole_vba_peek (MsOleVba *vba)
{
	g_assert (!ms_ole_vba_eof (vba));

	return g_array_index (vba->text, guint8, vba->pos);
}

#if VBA_DEBUG > 1
static void
print_bin (guint16 dt)
{
	int i;

	printf ("|");
	for (i = 15; i >= 0; i--) {
		if (dt & (1 << i))
			printf ("1");
		else
			printf ("0");
		if (i == 8)
			printf ("|");
	}
	printf ("|");
}
#endif


/**
 * decompress_vba
 *   @vba	Place to store the uncompressed VBA
 *   @data	Pointer to start of compressed VBA
 *   @eos	lwa+1 of stream
 *
 * Purpose: lzw, arc like compression.
 *
 * Internal function.
 **/
static void
decompress_vba (MsOleVba *vba, guint8 *data, guint8 *eos)
{
#define BUF_SIZE    6144 /* a bottleneck */

	guint8	 buffer[BUF_SIZE];
	guint8  *ptr;
	guint8	*sptr;

	guint32	 len;
	guint32  pos;

	GArray  *ans = g_array_new (FALSE, FALSE, 1);


	vba->text = ans;
	vba->pos  = 0;

	len = MS_OLE_GET_GUINT16 (data + 1);

#if VBA_DEBUG > 0
	printf ("Length 0x%x\n", len);
#endif

	len  = (len & ~0xb000) + 1;
	ptr  = data + 3;
	sptr = ptr;
	pos  = 0;

	while (ptr < eos) {
#if VBA_DEBUG > 0
		printf ("My compressed stream (addr=%#x, len = %#x (%d)):\n",
		        ptr - data, len, len);
		ms_ole_dump (ptr, len);
#endif

		while ((ptr < sptr + len) && (ptr < eos)) {
			int	 shift;

			guint8	 flag_byte = *ptr++;

			/*
			 * The first byte is a flag byte.  Each bit in this byte
			 * determines what the next byte is.  If the bit is zero,
			 * the next byte is a character.  Otherwise the  next two
			 * bytes contain the number of characters to copy from the
			 * umcompresed buffer and where to copy them from (offset,
			 * length).
			 */
			for (shift = 0x01; shift < 0x100; shift = shift << 1) {
				if (ptr >= sptr + len)
					break;

				if (pos == BUF_SIZE) {
#if VBA_DEBUG > 0
					printf ("\nSomething extremely odd"
						" happens after %d bytes 0x%x\n\n",
						BUF_SIZE, MS_OLE_GET_GUINT16 (ptr));
					ms_ole_dump (ptr, len - (ptr - data));
#endif
					ptr       += 2;
					flag_byte  = *ptr++;
					pos        = 0;
					shift      = 0x01;
				}

				if (flag_byte & shift) {
					int	 i;
					int	 back;
					int	 clen;
					int	 shft;

					guint16	 dt = MS_OLE_GET_GUINT16 (ptr);

					if (pos <= 16)
						shft = 12;

					else if (pos <= 32)
						shft = 11;

					else if (pos <= 64)
						shft = 10;

					else if (pos <= 128)
						shft = 9;

					else if (pos <= 256)
						shft = 8;

					else if (pos <= 512)
						shft = 7;

					else if (pos <= 1024)
						shft = 6;

					else if (pos <= 2048)
						shft = 5;

					else
						shft = 4;

					back = (dt >> shft) + 1;
					clen = 0;

					for (i = 0; i < shft; i++)
						clen |= dt & (0x1 << i);
					clen += 3;

#if VBA_DEBUG > 1
					printf ("|match 0x%x (%d,%d) >> %d = %d, %d| pos = %d |\n",
						dt, (dt>>8), (dt&0xff), shft, back, clen, pos);
 					/* Perhaps dt & SHIFT = dist. to end of run */
					print_bin (dt);
					printf ("\n");
#endif
					for (i = 0; i < clen; i++) {
						guint8	 c;

						guint32	 srcpos = (BUF_SIZE + (pos%BUF_SIZE)) -
								   back;

						if (srcpos >= BUF_SIZE)
							srcpos-= BUF_SIZE;

						g_assert (srcpos >= 0);
						g_assert (srcpos < BUF_SIZE);

						c = buffer [srcpos];
						buffer [pos++ % BUF_SIZE] = c;
						g_array_append_val (ans, c);
#if VBA_DEBUG > 0
						printf ("%c", c);
#endif
					}
					ptr += 2;

				} else {
					buffer [pos++ % BUF_SIZE] = *ptr;
					g_array_append_val (ans, *ptr);
#if VBA_DEBUG > 0
					printf ("%c", *ptr);
#endif
					ptr++;
				}

				if ((ptr >= sptr + len) || (ptr >= eos)) {
					if ((ptr >= sptr + len) && (ptr < eos)) {
#if VBA_DEBUG > 0
						printf ("Reseting ptr.\n");
						printf ("ptr was %#x\n",
						        ptr - sptr);
						printf ("ptr is  %#x\n",
						        len);
#endif
						ptr = sptr + len;
					}
					break;
				}
			}  /* for (shift = 0x01; shift < 0x100; shift = shift << 1) */

		}  /* while ((ptr < sptr + len) && (ptr < eos)) */

#if VBA_DEBUG > 0
		printf ("ptr = %#X\n", ptr-sptr);
#endif
		if ((ptr + 3) < eos) {
			len  = MS_OLE_GET_GUINT16 (ptr);
			len  = (len & ~0xb000) + 1;
			ptr += 2;
			sptr = ptr;
			pos  = 0;
		}

	}  /* while (ptr < eos) */

	{
		char c;

		c = '\n';
		g_array_append_val (ans, c);

		c = '\0';
		g_array_append_val (ans, c);
	}
}

static guint8 *
seek_sig (guint8 *data, int len)
{
	int i;
	guint8 vba_sig[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1, 0x1 };

	for (i = 0; i < len; i++) {
		guint8 *p = data;
		int j;

		for (j = 0; j < sizeof (vba_sig); j++) {
			if (*p++ != vba_sig [j])
				break;
		}
		if (j == sizeof (vba_sig))
			return p;

		data++;
	}

	return NULL;
}

static guint8 *
find_compressed_vba (guint8 *data, MsOlePos len)
{
	guint8	*sig;
	guint32  offset;
	guint32  offpos;

	if (!(sig = seek_sig (data, len))) {
		g_warning ("No VBA kludge signature");
		return NULL;
	}

	offpos = MS_OLE_GET_GUINT32 (sig) + 0xd0 - 0x6b - 0x8;

#if VBA_DEBUG > 0
	printf ("Offpos : 0x%x -> \n", offpos);
#endif

	offset = MS_OLE_GET_GUINT32 (sig + offpos);

	if (len < offset + 3) {
		g_warning ("Too small for offset 0x%x\n", offset);
		return NULL;
	}

#if VBA_DEBUG > 0
	printf ("Offset is 0x%x\n", offset);
#endif

	return data + offset;
}


/**
 * ms_ole_vba_open:
 * @s: the stream pointer.
 *
 * Attempt to open a stream as a VBA stream, and commence
 * decompression of it.
 *
 * Return value: NULL if not a VBA stream or fails.
 **/
MsOleVba *
ms_ole_vba_open (MsOleStream *s)
{
	const guint8 gid [16] = { 0x1,  0x16, 0x1,  0x0,
				  0x6,  0xb6, 0x0,  0xff,
				  0xff, 0x1,  0x1,  0x0,
				  0x0,  0x0,  0x0,  0xff };
	int	     i;
	guint8      *data, *vba_data;
	guint8       sig [16];
	MsOleVba    *vba;


	g_return_val_if_fail (s != NULL, NULL);

	if (s->size < 16)
		return NULL;

	s->lseek     (s, 0, MsOleSeekSet);
	s->read_copy (s, sig, 16);

	for (i = 0; i < 16; i++)
		if (sig [i] != gid [i]) {
			/*
			 * Version ??
			*/
			if (i == 4)
			  if (sig [i] == 0x4 )
			  	continue;
			return NULL;
		}

	data = g_new (guint8, s->size);

	s->lseek (s, 0, MsOleSeekSet);
	if (!s->read_copy (s, data, s->size)) {
		g_warning ("Strange: failed read");
		g_free (data);
		return NULL;
	}

	if (!(vba_data = find_compressed_vba (data, s->size))) {
		g_free (data);
		return NULL;
	}

	if (MS_OLE_GET_GUINT8 (vba_data) != 1)
		g_warning ("Digit 0x%x != 1...", MS_OLE_GET_GUINT8 (vba_data));

	vba      = g_new0 (MsOleVba, 1);
	vba->s   = s;
	vba->pos = 0;

	decompress_vba (vba, vba_data, data + s->size);
	g_free (data);

	return vba;
}

/**
 * me_ols_vba_close:
 * @vba:
 *
 *   Free the resources associated with this vba
 * stream.
 **/
void
ms_ole_vba_close (MsOleVba *vba)
{
	if (vba) {
		g_array_free (vba->text, TRUE);
		vba->text = NULL;

		g_free (vba);
	}
}
