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
#include "config.h"
#include "wv.h"

#ifdef HAVE_WMF
#	include "gdwmfapi.h"
/*
	extern listentry *ourlist;
	extern int list;
*/
#endif
		

U32 read_32ubit(wvStream *in)
	{
	U32 ret;
#if defined(WORDS_BIGENDIAN) || !defined(MATCHED_TYPE)
	U16 temp1,temp2;
	temp1 = read_16ubit(in);
	temp2 = read_16ubit(in); 
	ret = temp2;
	ret = ret << 16;
	ret += temp1;
#else
	fread(&ret,sizeof(U8),4, (FILE *)in);
#endif
	return(ret);
	}

U32 sread_32ubit(const U8 *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = sread_16ubit(in);
	temp2 = sread_16ubit(in+2);
	ret = temp2;
	ret = ret << 16;
	ret+=temp1;
	return(ret);
	}

U32 bread_32ubit(U8 *in,U16 *pos)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = sread_16ubit(in);
	temp2 = sread_16ubit(in+2);
	ret = temp2;
	ret = ret << 16;
	ret+=temp1;
	(*pos)+=4;
	return(ret);
	}

U16 read_16ubit(wvStream *in)
	{
	U16 ret;
#if defined(WORDS_BIGENDIAN) || !defined(MATCHED_TYPE)
	U8 temp1,temp2;
	temp1 = read_8ubit(in);
	temp2 = read_8ubit(in);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
#else
	fread(&ret,sizeof(U8),2, (FILE *)in);
#endif
	return(ret);
	}


U16 sread_16ubit(const U8 *in)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = *in;
	temp2 = *(in+1);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	return(ret);
	}

U16 bread_16ubit(U8 *in,U16 *pos)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = *in;
	temp2 = *(in+1);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	(*pos)+=2;
	return(ret);
	}

U32 dread_32ubit(wvStream *in,U8 **list)
	{
	U8 *temp;
	U32 ret;
	if (in != NULL)
		return(read_32ubit(in));
	else
		{
		temp = *list;
		(*list)+=4;
		ret = sread_32ubit(temp);
		return(ret);
		}
	}

U16 dread_16ubit(wvStream *in,U8 **list)
	{
	U8 *temp;
	U16 ret;
	if (in != NULL)
		return(read_16ubit(in));
	else
		{
		temp = *list;
		(*list)+=2;
		ret = sread_16ubit(temp);
		return(ret);
		}
	}

U8 read_8ubit(wvStream* in)
	{
	return(getc((FILE *)in));
	}

U8 dread_8ubit(wvStream *in,U8 **list)
	{
	U8 *temp;
	if (in != NULL)
		return(read_8ubit(in));
	else
		{
		temp = *list;
		(*list)++;
		return(sread_8ubit(temp));
		}
	}

U8 sread_8ubit(const U8 *in)
	{
	return(*in);
	}

U8 bread_8ubit(U8 *in,U16 *pos)
	{
	(*pos)++;
	return(*in);
	}
	
	
size_t wvStream_read(void *ptr, size_t size, size_t nmemb, wvStream *stream)
	{
		return(fread(ptr, size, nmemb, (FILE *)stream));
	}

void wvStream_rewind(wvStream *stream) 
	{
	rewind((FILE *) stream);
	}	

int wvStream_goto(wvStream *stream, long position)
	{
	return(fseek((FILE *)stream, position, SEEK_SET));
	}

int wvStream_offset(wvStream *stream, long offset)
	{
	return(fseek((FILE *)stream, offset, SEEK_CUR));
	}

int wvStream_offset_from_end(wvStream *stream, long offset)
	{
	return(fseek((FILE *)stream, offset, SEEK_END));
	}
	
long wvStream_tell(wvStream *stream) 
	{
	return(ftell((FILE *)stream));
	}
	
int wvStream_close(wvStream *stream) 
	{
	return(fclose((FILE *)stream));
	}
