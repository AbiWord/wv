#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"

/* return value: 1 == success 0 == failure */
int
wvGetPICF (wvVersion ver, PICF * apicf, wvStream * fd)
{
    FILE *f;
    U8 temp;
    U32 i;
    long pos = wvStream_tell (fd);

    apicf->lcb = read_32ubit (fd);
    apicf->cbHeader = read_16ubit (fd);
    wvTrace (("size of pic is %x (%d)\n", apicf->cbHeader, apicf->cbHeader));
    apicf->mfp_mm = (S16) read_16ubit (fd);
    wvTrace (("mm type is %d\n", apicf->mfp_mm));
    apicf->mfp_xExt = (S16) read_16ubit (fd);
    apicf->mfp_yExt = (S16) read_16ubit (fd);
    apicf->mfp_hMF = (S16) read_16ubit (fd);
    if (apicf->mfp_mm == 99)
	wvGetBITMAP (&(apicf->obj.bitmap), fd);
    else
	wvGetrc (&(apicf->obj.arc), fd);
    apicf->dxaGoal = (S16) read_16ubit (fd);
    apicf->dyaGoal = (S16) read_16ubit (fd);
    apicf->mx = (S16) read_16ubit (fd);
    apicf->my = (S16) read_16ubit (fd);
    apicf->dxaCropLeft = (S16) read_16ubit (fd);
    apicf->dyaCropTop = (S16) read_16ubit (fd);
    apicf->dxaCropRight = (S16) read_16ubit (fd);
    apicf->dyaCropBottom = (S16) read_16ubit (fd);
    temp = read_8ubit (fd);

    apicf->brcl = temp & 0x0F;
    apicf->fFrameEmpty = (temp & 0x10) >> 4;
    apicf->fBitmap = (temp & 0x20) >> 5;
    wvTrace (("bitmap is %d\n", apicf->fBitmap));
    apicf->fDrawHatch = (temp & 0x40) >> 6;
    apicf->fError = (temp & 0x80) >> 7;

    apicf->bpp = read_8ubit (fd);
    wvGetBRC (ver, &(apicf->brcTop), fd);
    wvGetBRC (ver, &(apicf->brcLeft), fd);
    wvGetBRC (ver, &(apicf->brcBottom), fd);
    wvGetBRC (ver, &(apicf->brcRight), fd);
    apicf->dxaOrigin = (S16) read_16ubit (fd);
    apicf->dyaOrigin = (S16) read_16ubit (fd);
    if (ver == WORD8)
	apicf->cProps = (S16) read_16ubit (fd);
    else
	apicf->cProps = 0;
    pos = wvStream_tell (fd) - pos;
    for (i = pos; i < apicf->cbHeader; i++)
	read_8ubit (fd);
    wvTrace (("pos is finally %x\n", wvStream_tell (fd)));
    wvTrace (("len of data is %d\n", apicf->lcb - apicf->cbHeader));
    wvTrace (
	     ("ends at %x\n",
	      wvStream_tell (fd) + apicf->lcb - apicf->cbHeader));
    f = tmpfile ();
    if (f == NULL)
      {
	  wvError (("Couldnt create tmpfile: %s\n", strerror (errno)));
	  apicf->rgb = NULL;
	  return 0;
      }
    /*
       sprintf(buffer,"/tmp/newtest-%d",s++);
       f = fopen(buffer,"w+b");
     */
    i = 0;

    if (apicf->mfp_mm < 90)
      {
	  U32 len;
	  U32 j;
	  U8 bmp_header[40];
	  U32 header_len;
	  U32 colors_used;
	  U16 bpp;

	  wvTrace (("test\n"));
	  len = apicf->lcb - apicf->cbHeader;

	  i = wvEatOldGraphicHeader (fd, len);
	  wvTrace (("len is %d, header len guess is %d\n", len, i));
	  if (i + 2 >= len)
	    {
		wvTrace (("all read ok methinks\n"));
		apicf->rgb = NULL;
		return 1;
	    }
	  len -= i;

	  /*
	     a msofbtSpContainer amsofbh
	     a msofbtSp amsofbh 
	     a fopt
	     a msofbtBSE amsofbh
	     a BSE amsofbh
	     a FBSE for a dib, and then
	     all this dib information
	   */
	  len += 14;
	  wvTrace (("len is now %d\n", len));


	  for(j=0;j< sizeof(bmp_header);j++)
	    bmp_header[j] = read_8ubit (fd);

	  bpp = bmp_header[14] + (bmp_header[15] << 8);

	  if ( bpp < 9)
	  {
	    colors_used = bmp_header[32] 
	      + (bmp_header[33] << 8)
	      + (bmp_header[34] << 16)
	      + (bmp_header[35] << 24);
	  }
	      else
	  {
	      colors_used = 0;
	  }

	  header_len = 14 + 40 + 4 * colors_used;

	  fputc (0x42, f); /* B */
	  fputc (0x4D, f); /* M */

	  fputc (len & 0x000000FF, f);
	  fputc ((len & 0x0000FF00) >> 8, f);
	  fputc ((len & 0x00FF0000) >> 16, f);
	  fputc ((len & 0xFF000000) >> 24, f);

	  fputc (0x00, f);
	  fputc (0x00, f);
	  fputc (0x00, f);
	  fputc (0x00, f);

	  fputc (header_len & 0x000000FF, f);
	  fputc ((header_len & 0x0000FF00) >> 8, f);
	  fputc ((header_len & 0x00FF0000) >> 16, f);
	  fputc ((header_len & 0xFF000000) >> 24, f);


	  for(j=0;j< sizeof(bmp_header);j++)
	    fputc(bmp_header[j],f);
	  
	  for (; i < apicf->lcb - apicf->cbHeader-sizeof(bmp_header); i++)
	    fputc (read_8ubit (fd), f);

      }
    else
      {
	for (; i < apicf->lcb - apicf->cbHeader; i++)
	  fputc (read_8ubit (fd), f);
      }

    rewind (f);
    wvStream_FILE_create (&apicf->rgb, f);
    return 1;
}

U32
wvEatOldGraphicHeader (wvStream * fd, U32 len)
{
    U32 X, entry, count = 0, test;
    U16 pad;
    test = read_32ubit (fd);	/*0x00090001 */
    if (test != 0x00090001L)
	wvError (("Old Graphic\n"));
    count += 4;
    test = read_16ubit (fd);	/*0x0300 */
    if (test != 0x0300)
	wvError (("Old Graphic\n"));
    count += 2;

    read_32ubit (fd);		/*changes */
    count += 4;
    test = read_16ubit (fd);	/*0x0000 */
    if (test != 0x00000000L)
	wvError (("Old Graphic\n"));
    count += 2;
    X = read_32ubit (fd);	/*changes, lets call this X */
    wvError (("X is %x\n", X));
    count += 4;
    test = read_16ubit (fd);		/*0x0000 */
    if (test != 0x00000000L)
	wvError (("Old Graphic\n"));
    count += 2;

    /*
       while ( entry != X)
     */
    do
      {
	  entry = read_32ubit (fd);
	  count += 4;
	  wvTrace (
		   ("Entry is %x, %x, count is %d\n", entry,
		    wvStream_tell (fd), count));
	  switch (entry)
	    {
	    case 3:
		read_16ubit (fd);
		count += 2;
		wvTrace (
			 ("suspect that we are finished, count %d, len %d\n",
			  count, len));
		break;
	    default:
		{
		    U32 lene2 = entry - 2;
		    U32 i;
		    wvTrace (
			     ("lene2 is %d, predict end of %x\n", len,
			      wvStream_tell (fd) + (entry - 2) * 2));
		    /* RIES (rvt@dds.nl)
		       prolly a dirty patch because I check count
		       everytime it's incremnented against lene2.
		       This seems twork very well I tried it on around 15.000
		       word documents and it seems to work! */
		    for (i = 0; i < lene2; i++)
		      {
		          if ( (count + 1) >= len) return (count);
			  test = read_16ubit (fd);
			  if ((i == 0)
			      && ((test == 0x0f43) || (test == 0x0b41)))
			    {
				wvTrace (
					 ("Found a Bitmap, Will strip header and return with bitmap data\n"));
				count += 2;
    		        	if ( (count + 1) >= len) return (count);
				pad = test;
				test = read_32ubit (fd);	/*0x00cc0020 */
				if (test != 0x00cc0020)
				    wvTrace (("Old Graphic\n"));
				count += 4;
    		        	if ( (count + 1) >= len) return (count);

				if (pad == 0x0f43)
				  {
				      test = read_16ubit (fd);	/*0x0000 */
				      if (test != 0x0000)
					  wvTrace (("Old Graphic\n"));
				      count += 2;
			              if ( (count + 1) >= len) return (count);
				  }

				read_16ubit (fd);	/*width */
				count += 2;
    		        	if ( (count + 1) >= len) return (count);
				read_16ubit (fd);	/*height */
				count += 2;
    		        	if ( (count + 1) >= len) return (count);
				test = read_32ubit (fd);	/*0x00000000L */
				if (test != 0x00000000L)
				    wvTrace (("Old Graphic\n"));
				count += 4;
    		        	if ( (count + 1) >= len) return (count);
				read_16ubit (fd);	/*width */
				count += 2;
    		        	if ( (count + 1) >= len) return (count);
				read_16ubit (fd);	/*height */
				count += 2;
    		        	if ( (count + 1) >= len) return (count);
				test = read_32ubit (fd);	/*0x00000000L */
				if (test != 0x00000000L)
				    wvTrace (("Old Graphic\n"));
				count += 4;
				return (count);
			    }
			  count += 2;
	        	  if ( (count + 1) >= len) return (count);
		      }
		}
		break;
	    }
      } while (count + 2 < len);
    wvTrace (("Entry is %x %x, %d\n", entry, wvStream_tell (fd), count));
    return (count);
}


