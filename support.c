/*
Released under GPL, written by Caolan.McNamara@ul.ie.

Copyright (C) 1998,1999 
	Caolan McNamara

Real Life: Caolan McNamara           *  Doing: MSc in HCI
Work: Caolan.McNamara@ul.ie          *  Phone: +353-86-8790257
URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
How would you have done it?
*/

/*

this code is often all over the shop, being more of an organic entity
that a carefully planed piece of code, so no laughing there at the back!

and send me patches by all means, but think carefully before sending me
a patch that doesnt fix a bug or add a feature but instead just changes
the style of coding, i.e no more thousand line patches that fix my 
indentation.

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include "oledecod.h"

#ifdef HAVE_WMF
#	include "gdwmfapi.h"
/*
	extern listentry *ourlist;
	extern int list;
*/
#endif

extern pps_entry *stream_tree;
extern MsOle *ole_file;
wvStream_list *streams = NULL;

void
wvOLEFree (void)
{
    wvStream_list *tempList;
    freeOLEtree (stream_tree);	/* Does internal checking, so it doesn't matter
				   * we just call it with NULL.
				 */
    while (streams != NULL)
      {
	  wvStream_close (streams->stream);
	  tempList = streams->next;
	  wvFree (streams);
	  streams = tempList;
      }
    if (ole_file != NULL)
      {
	  ms_ole_destroy (&ole_file);
      }
}


void
wvStream_FILE_create (wvStream ** in, FILE * inner)
{
    wvInternalStream str;
    str.file_stream = inner;
    wvStream_create (in, FILE_STREAM, str);
}

void
wvStream_libole2_create (wvStream ** in, MsOleStream * inner)
{
    wvInternalStream str;
    str.libole_stream = inner;
    wvStream_create (in, LIBOLE_STREAM, str);
}

void
wvStream_memory_create (wvStream ** in, char *buf, size_t size)
{
    wvInternalStream str;
    MemoryStream *inner = (MemoryStream *)wvMalloc(sizeof(MemoryStream));

    inner->mem = buf;
    inner->size = size;
    inner->current = 0;

    str.memory_stream = inner;
    wvStream_create (in, MEMORY_STREAM, str);
}


void
wvStream_create (wvStream ** in, wvStreamKind kind, wvInternalStream inner)
{
    wvStream_list *listEntry;
    *in = (wvStream *) wvMalloc (sizeof (wvStream));
    (*in)->kind = kind;
    (*in)->stream = inner;
    listEntry = wvMalloc (sizeof (wvStream_list));
    listEntry->stream = (*in);
    listEntry->next = streams;
    streams = listEntry;
}

static size_t memorystream_read(MemoryStream *stream, void *buf, size_t count)
{
  size_t ret;

  if ( stream->current + count < stream->size)
    {  
      memcpy(buf, stream->mem + stream->current, count);
      stream->current += count;
      ret = count;
    }
  else
    {
      ret = stream->size - stream->current;
      memcpy(buf, stream->mem + stream->current, ret);
      memset(buf + ret , 0, count - ret);
      stream->current = stream->size;
      wvTrace(("read out of bounds\n"));
    }
  return ret;
}

U32
read_32ubit (wvStream * in)
{
    U32 ret;
#if defined(WORDS_BIGENDIAN) || !defined(MATCHED_TYPE)
    U16 temp1, temp2;
    temp1 = read_16ubit (in);
    temp2 = read_16ubit (in);
    ret = temp2;
    ret = ret << 16;
    ret += temp1;
#else
    if (in->kind == LIBOLE_STREAM)
      {
	  in->stream.libole_stream->read_copy (in->stream.libole_stream,
					       (guint8 *) & ret, 4);
      }
    else if (in->kind == FILE_STREAM)
      {
	  fread (&ret, sizeof (U8), 4, in->stream.file_stream);
      }
    else
      {
	memorystream_read(in->stream.memory_stream, &ret, 4);
      }
#endif
    return (ret);
}

U16
read_16ubit (wvStream * in)
{
    U16 ret;
#if defined(WORDS_BIGENDIAN) || !defined(MATCHED_TYPE)
    U8 temp1, temp2;
    temp1 = read_8ubit (in);
    temp2 = read_8ubit (in);
    ret = temp2;
    ret = ret << 8;
    ret += temp1;
#else
    if (in->kind == LIBOLE_STREAM)
      {
	  in->stream.libole_stream->read_copy (in->stream.libole_stream,
					       (guint8 *) & ret, 2);
      }
    else if (in->kind == FILE_STREAM)
      {
	  fread (&ret, sizeof (U8), 2, in->stream.file_stream);
      }
    else
      {
	memorystream_read(in->stream.memory_stream, &ret, 2);
      }


#endif
    return (ret);
}

U8
read_8ubit (wvStream * in)
{
    if (in->kind == LIBOLE_STREAM)
      {
	  U8 ret;
	  in->stream.libole_stream->read_copy (in->stream.libole_stream,
					       (guint8 *) & ret, 1);
	  return (ret);
      }
    else if (in->kind == FILE_STREAM)
      {
	  return (getc (in->stream.file_stream));
      }
    else
      {
	  U8 ret;
	  memorystream_read(in->stream.memory_stream, &ret, 1);
	  return ret;
      }
}

U32
wvStream_read (void *ptr, size_t size, size_t nmemb, wvStream * in)
{
    if (in->kind == LIBOLE_STREAM)
      {
	  return ((U32) in->stream.libole_stream->
		  read_copy (in->stream.libole_stream, ptr, size * nmemb));
      }
    else if (in->kind == FILE_STREAM)
      {
	  return (fread (ptr, size, nmemb, in->stream.file_stream));
      }
    else
      {
	return memorystream_read(in->stream.memory_stream, ptr, size * nmemb);
	return size * nmemb;
      }
}

void
wvStream_rewind (wvStream * in)
{
    if (in->kind == LIBOLE_STREAM)
      {
	  in->stream.libole_stream->lseek (in->stream.libole_stream, 0,
					   MsOleSeekSet);
      }
    else if (in->kind == FILE_STREAM)
      {
	  rewind (in->stream.file_stream);
      }
    else
      {
	in->stream.memory_stream->current = 0;
      }
}

U32
wvStream_goto (wvStream * in, long position)
{
    if (in->kind == LIBOLE_STREAM)
      {
	  return ((U32) in->stream.libole_stream->
		  lseek (in->stream.libole_stream, position, MsOleSeekSet));
      }
    else if (in->kind == FILE_STREAM)
      {
	  return ((U32) fseek (in->stream.file_stream, position, SEEK_SET));
      }
    else
      {
	in->stream.memory_stream->current = position;
        return 0;
      }
}

U32
wvStream_offset (wvStream * in, long offset)
{
    if (in->kind == LIBOLE_STREAM)
      {
	  return ((U32) in->stream.libole_stream->
		  lseek (in->stream.libole_stream, offset, MsOleSeekCur));
      }
    else if (in->kind == FILE_STREAM)
      {
	  return ((U32) fseek (in->stream.file_stream, offset, SEEK_CUR));
      }
    else
      {
	in->stream.memory_stream->current += offset;
	return  in->stream.memory_stream->current;
      }
}

U32
wvStream_offset_from_end (wvStream * in, long offset)
{
    if (in->kind == LIBOLE_STREAM)
      {
	  return ((U32) in->stream.libole_stream->
		  lseek (in->stream.libole_stream, offset, MsOleSeekEnd));
      }
    else if(in->kind == FILE_STREAM)
      {
	  return ((U32) fseek (in->stream.file_stream, offset, SEEK_END));
      }
    else
      {
	in->stream.memory_stream->current = 
	in->stream.memory_stream->size + offset;
        return in->stream.memory_stream->current;
      }
}

U32
wvStream_tell (wvStream * in)
{
    if (in->kind == LIBOLE_STREAM)
      {
	  return ((U32) in->stream.libole_stream->
		  tell (in->stream.libole_stream));
      }
    else if(in->kind == FILE_STREAM)
      {
	  return ((U32) ftell (in->stream.file_stream));
      }
    else
      {
	return (in->stream.memory_stream->current);
      }
}

U32
wvStream_size (wvStream * in)
{
  U32 size;

  long offset = wvStream_tell(in);
  wvStream_offset_from_end(in,0);
  size = wvStream_tell(in);
  wvStream_offset(in,offset);

  return size;
}

U32
wvStream_close (wvStream * in)
{
    if ( !in )
      return 0;

    if (in->kind == LIBOLE_STREAM)
      {
	  U32 ret = (U32) ms_ole_stream_close (&in->stream.libole_stream);
	  wvFree (in);
	  return (ret);
      }
    else
    if (in->kind == FILE_STREAM)
      {
	  U32 ret;
	  ret = (U32) fclose (in->stream.file_stream);
	  wvFree (in);
	  return (ret);
      }
    else
    if (in->kind == MEMORY_STREAM)
      {
	  free (in->stream.memory_stream->mem);
	  free (in->stream.memory_stream);
	  wvFree (in);
	  return 0;
      }
    else abort();
}

/* wvStream-kind-independent functions below */

U32
sread_32ubit (const U8 * in)
{
    U16 temp1, temp2;
    U32 ret;
    temp1 = sread_16ubit (in);
    temp2 = sread_16ubit (in + 2);
    ret = temp2;
    ret = ret << 16;
    ret += temp1;
    return (ret);
}

U32
bread_32ubit (U8 * in, U16 * pos)
{
    U16 temp1, temp2;
    U32 ret;
    temp1 = sread_16ubit (in);
    temp2 = sread_16ubit (in + 2);
    ret = temp2;
    ret = ret << 16;
    ret += temp1;
    (*pos) += 4;
    return (ret);
}

U32
dread_32ubit (wvStream * in, U8 ** list)
{
    U8 *temp;
    U32 ret;
    if (in != NULL)
	return (read_32ubit (in));
    else
      {
	  temp = *list;
	  (*list) += 4;
	  ret = sread_32ubit (temp);
	  return (ret);
      }
}

U16
sread_16ubit (const U8 * in)
{
    U8 temp1, temp2;
    U16 ret;
    temp1 = *in;
    temp2 = *(in + 1);
    ret = temp2;
    ret = ret << 8;
    ret += temp1;
    return (ret);
}

U16
bread_16ubit (U8 * in, U16 * pos)
{
    U8 temp1, temp2;
    U16 ret;
    temp1 = *in;
    temp2 = *(in + 1);
    ret = temp2;
    ret = ret << 8;
    ret += temp1;
    (*pos) += 2;
    return (ret);
}

U16
dread_16ubit (wvStream * in, U8 ** list)
{
    U8 *temp;
    U16 ret;
    if (in != NULL)
	return (read_16ubit (in));
    else
      {
	  temp = *list;
	  (*list) += 2;
	  ret = sread_16ubit (temp);
	  return (ret);
      }
}

U8
sread_8ubit (const U8 * in)
{
    return (*in);
}

U8
bread_8ubit (U8 * in, U16 * pos)
{
    (*pos)++;
    return (*in);
}

U8
dread_8ubit (wvStream * in, U8 ** list)
{
    U8 *temp;
    if (in != NULL)
	return (read_8ubit (in));
    else
      {
	  temp = *list;
	  (*list)++;
	  return (sread_8ubit (temp));
      }
}
