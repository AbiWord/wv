#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "wvinternal.h"

int wvGetTC_internal(int version,TC *tc,FILE *infd,U8 *pointer)
    {
    U16 temp16;
	BRC10 brc10;

#ifdef PURIFY
    wvInitTC(tc);
#endif

	temp16 = dread_16ubit(infd,&pointer);

	tc->fFirstMerged = temp16 & 0x0001;
    tc->fMerged = (temp16 & 0x0002) >> 1;
    tc->fVertical = (temp16 & 0x0004) >> 2;
    tc->fBackward = (temp16 & 0x0008) >> 3;
    tc->fRotateFont = (temp16 & 0x0010) >> 4;
    tc->fVertMerge = (temp16 & 0x0020) >> 5;
    tc->fVertRestart = (temp16 & 0x0040) >> 6;
    tc->vertAlign = (temp16 & 0x0180) >> 7;
    tc->fUnused = (temp16 & 0xFE00) >> 9;


	if (version==0)
		{
    	tc->wUnused = dread_16ubit(infd,&pointer);
		wvGetBRC_internal(&tc->brcTop,infd,pointer);
		pointer+=cbBRC;
		wvGetBRC_internal(&tc->brcLeft,infd,pointer);
		pointer+=cbBRC;
		wvGetBRC_internal(&tc->brcBottom,infd,pointer);
		pointer+=cbBRC;
		wvGetBRC_internal(&tc->brcRight,infd,pointer);
		pointer+=cbBRC;
		}
	else
		{
    	tc->wUnused = 0;
		wvGetBRC10_internal(&brc10,infd,pointer);
		wvConvertBRC10ToBRC(&tc->brcTop,&brc10);
		pointer+=cb6BRC;
		wvGetBRC10_internal(&brc10,infd,pointer);
		wvConvertBRC10ToBRC(&tc->brcLeft,&brc10);
		pointer+=cb6BRC;
		wvGetBRC10_internal(&brc10,infd,pointer);
		wvConvertBRC10ToBRC(&tc->brcBottom,&brc10);
		pointer+=cb6BRC;
		wvGetBRC10_internal(&brc10,infd,pointer);
		wvConvertBRC10ToBRC(&tc->brcRight,&brc10);
		pointer+=cb6BRC;
		return(cb6TC);
		}
	return(cbTC);
    }

int wvGetTCFromBucket(int version,TC *abrc,U8 *pointer)
    {
    return(wvGetTC_internal(version,abrc,NULL,pointer));
    }


void wvCopyTC(TC *dest,TC *src)
	{
    dest->fFirstMerged = src->fFirstMerged;
    dest->fMerged = src->fMerged;
    dest->fVertical = src->fVertical;
    dest->fBackward = src->fBackward;
    dest->fRotateFont = src->fRotateFont;
    dest->fVertMerge = src->fVertMerge;
    dest->fVertRestart = src->fVertRestart;
    dest->vertAlign = src->vertAlign;
    dest->fUnused = src->fUnused;
    dest->wUnused = src->wUnused;

    wvCopyBRC(&src->brcTop,&dest->brcTop);
    wvCopyBRC(&src->brcLeft,&dest->brcLeft);
    wvCopyBRC(&src->brcBottom,&dest->brcBottom);
    wvCopyBRC(&src->brcRight,&dest->brcRight);
	}

void wvInitTC(TC *item)
	{
    item->fFirstMerged = 0;
    item->fMerged = 0;
    item->fVertical = 0;
    item->fBackward = 0;
    item->fRotateFont = 0;
    item->fVertMerge = 0;
    item->fVertRestart = 0;
    item->vertAlign = 0;
    item->fUnused = 0;
    item->wUnused = 0;

    wvInitBRC(&item->brcTop);
    wvInitBRC(&item->brcLeft);
    wvInitBRC(&item->brcBottom);
    wvInitBRC(&item->brcRight);
	}
