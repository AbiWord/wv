#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "wvinternal.h"

void wvInitTBD(TBD *item)
	{
	item->jc=0;
	item->tlc=0;
	item->reserved=0;
	}

void wvCopyTBD(TBD *dest,TBD *src)
	{
	dest->jc = src->jc;
	dest->tlc = src->tlc;
	dest->reserved = src->reserved;
	}

void wvGetTBD(TBD *item,FILE *fd)
	{
	wvGetTBD_internal(item,fd,NULL);
	}

void wvGetTBDFromBucket(TBD *item,U8 *pointer)
	{
	wvGetTBD_internal(item,NULL,pointer);
	}


void wvGetTBD_internal(TBD *item,FILE *fd,U8 *pointer)
	{
	U8 temp8;
	temp8 = dgetc(fd,&pointer);
#ifdef PURIFY
	wvInitTBD(item);
#endif
	item->jc = temp8 & 0x07;
	item->tlc = (temp8 & 0x38) >> 3;
	item->reserved = (temp8 & 0xC0) >> 6;
	}
