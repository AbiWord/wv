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
#include "wv.h"
#ifdef HAVE_WMF
#include "gdwmfapi.h"

#include <sys/mman.h>

extern listentry *ourlist;
extern int list;
#endif

U32 read_32ubit(FILE *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = read_16ubit(in);
	temp2 = read_16ubit(in); 
	ret = temp2;
	ret = ret << 16;
	ret += temp1;
	return(ret);
	}

U32 sread_32ubit(U8 *in)
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

U16 read_16ubit(FILE *in)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = getc(in);
	temp2 = getc(in);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	return(ret);
	}


U16 sread_16ubit(U8 *in)
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

U32 dread_32ubit(FILE *in,U8 **list)
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

U16 dread_16ubit(FILE *in,U8 **list)
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

U8 dgetc(FILE *in,U8 **list)
	{
	U8 *temp;
	if (in != NULL)
		return(getc(in));
	else
		{
		temp = *list;
		(*list)++;
		return(sgetc(temp));
		}
	}

U8 sgetc(U8 *in)
	{
	return(*in);
	}

U8 bgetc(U8 *in,U16 *pos)
	{
	(*pos)++;
	return(*in);
	}
