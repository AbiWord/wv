#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include "config.h"
#include "wv.h"
#include "oledecod.h"

#include "wvexporter.h"

#if defined(WORDS_BIGENDIAN) || !defined(MATCHED_TYPE)

        /* TODO: convert from big to little endian */

	#define TO_LE_32(i) (i)
	#define TO_LE_16(i) (i)
	#define TO_LE_8(i)  (i)

#else

	/* noop macros for little-endian machines */

	#define TO_LE_32(i) (i)
	#define TO_LE_16(i) (i)
	#define TO_LE_8(i)  (i)

#endif

extern MsOle *ole_file;

wvDocument* wvDocument_create(const char* filename) 
    {
    wvDocument* ret;
    
    if(ms_ole_create((MsOle**)(&ret), filename) != MS_OLE_ERR_OK)
        {
        free(ret);
        return(NULL);
        }
    else
        {
        ole_file=(MsOle*)ret;
        return(ret);
        }
    }

wvStream* wvStream_new(wvDocument* ole_file, const char* name)
    {
    MsOleStream* temp_stream; 
    wvStream* ret;
    ms_ole_stream_open(&temp_stream, ole_file, "/", name, 'w');
    wvStream_libole2_create(&ret, temp_stream);
    return(ret);
    }
    
int write_32ubit(wvStream *in, U32 out)
{

	guint32 cpy = (guint32)TO_LE_32(out);
	int nwr = 0;

	if (in->kind == LIBOLE_STREAM)
		{
			nwr = (int)in->stream.libole_stream->write(in->stream.libole_stream, (guint8 *)&cpy, 32);
		}
	else
		{
			assert(in->kind == FILE_STREAM);
			nwr = (int) fwrite(&cpy, sizeof(guint32), 1, in->stream.file_stream);
		}

	return nwr;
}

int write_16ubit(wvStream *in, U16 out)
{

	guint16 cpy = (guint16)TO_LE_16(out);
	int nwr = 0;

	if (in->kind == LIBOLE_STREAM)
		{
			nwr = (int)in->stream.libole_stream->write(in->stream.libole_stream, (guint8 *)&cpy, 16);
		}
	else
		{
			assert(in->kind == FILE_STREAM);
			nwr = (int) fwrite(&cpy, sizeof(guint16), 1, in->stream.file_stream);
		}

	return nwr;
}

int write_8ubit(wvStream *in, U8 out)
{
	guint8 cpy = (guint8)TO_LE_8(out);
	int nwr = 0;
    wvTrace(("About to write 16-bit value"));

	if (in->kind == LIBOLE_STREAM)
		{
			nwr = (int) in->stream.libole_stream->write(in->stream.libole_stream, (guint8 *)&cpy, 8);
		}
	else
		{
			assert(in->kind == FILE_STREAM);
			nwr = (int) fwrite(&cpy, sizeof(guint8), 1, in->stream.file_stream);
		}

	return nwr;
}

int wvStream_write(void *ptr, size_t size, size_t nmemb, wvStream *in)
{
	int nwr = 0;

	if (in->kind == LIBOLE_STREAM)
		{
			nwr = (int) in->stream.libole_stream->write(in->stream.libole_stream, ptr, size * nmemb);
		}
	else
		{
			assert(in->kind == FILE_STREAM);
			nwr = (int) fwrite(ptr, size, nmemb, in->stream.file_stream);
		}

	return nwr;
}
