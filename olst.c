#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "wvinternal.h"

void wvInitOLST(OLST *item)
	{
	U8 i;
	for (i=0;i<9;i++)
		wvInitANLV(&item->rganlv[i]);
 	item->fRestartHdr=0;
 	item->fSpareOlst2=0;
 	item->fSpareOlst3=0;
 	item->fSpareOlst4=0;
	for (i=0;i<64;i++)
 		item->rgxch[i]=0;
	} 


void wvGetOLST_internal(int version,OLST *item,FILE *fd,U8 *pointer)
	{
	U8 i;
	for (i=0;i<9;i++)
		wvGetANLV_internal(&item->rganlv[i],fd,pointer);
 	item->fRestartHdr=dgetc(fd,&pointer);
 	item->fSpareOlst2=dgetc(fd,&pointer);
 	item->fSpareOlst3=dgetc(fd,&pointer);
 	item->fSpareOlst4=dgetc(fd,&pointer);
	if (version == 0)
		{
		for (i=0;i<32;i++)
			item->rgxch[i]=dread_16ubit(fd,&pointer);
		}
	else
		{
		for (i=0;i<64;i++)
			item->rgxch[i]=dgetc(fd,&pointer);
		}
	}

void wvGetOLST(int version,OLST *item,FILE *fd)
	{
	wvGetOLST_internal(version,item,fd,NULL);
	}

void wvGetOLSTFromBucket(int version,OLST *item,U8 *pointer)
	{
	wvGetOLST_internal(version,item,NULL,pointer);
	}
