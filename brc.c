#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetBRC_internal(BRC *abrc,FILE *infd,U8 *pointer)
    {
    U8 temp8;

    abrc->dptLineWidth = dgetc(infd,&pointer);
    abrc->brcType = dgetc(infd,&pointer);
    abrc->ico = dgetc(infd,&pointer);
    temp8 = dgetc(infd,&pointer);

#ifdef PURIFY
    abrc->dptSpace = 0;
    abrc->fShadow = 0;
    abrc->fFrame = 0;
    abrc->reserved = 0;
#endif
   
    abrc->dptSpace = temp8 & 0x1f;
    abrc->fShadow = (temp8 & 0x20)>>5;
    abrc->fFrame = (temp8 & 0x40)>>6;
    abrc->reserved = (temp8 & 0x80)>>7;
    }

void wvGetBRC(BRC *abrc,FILE *infd)
    {
	wvGetBRC_internal(abrc,infd,NULL);
    }

void wvGetBRCFromBucket(BRC *abrc,U8 *pointer)
    {
	wvGetBRC_internal(abrc,NULL,pointer);
    }

void wvInitBRC10(BRC10 *item)
	{
	item->dxpLine2Width = 0;
	item->dxpSpaceBetween = 0;
	item->dxpLine1Width = 0;
	item->dxpSpace = 0;
	item->fShadow = 0;
	item->fSpare = 0;
	}

void wvGetBRC10_internal(BRC10 *item,FILE *infd,U8 *pointer)
    {
    U16 temp16;
	temp16 = dread_16ubit(infd,&pointer);
#ifdef PURIFY
	wvInitBRC10(item);
#endif
	item->dxpLine2Width = (temp16 & 0x0007);
	item->dxpSpaceBetween = (temp16 & 0x0038)>>3;
	item->dxpLine1Width = (temp16 & 0x01C0)>>6;
	item->dxpSpace = (temp16 & 0x3E00)>>9;
	item->fShadow = (temp16 & 0x4000)>>14;
	item->fSpare = (temp16 & 0x8000)>>15;
    }

void wvGetBRC10FromBucket(BRC10 *abrc10,U8 *pointer)
    {
	wvGetBRC10_internal(abrc10,NULL,pointer);
    }

void wvInitBRC(BRC *abrc)
    {
    abrc->dptLineWidth = 0;
    abrc->brcType = 0;
    abrc->ico = 0;
    abrc->dptSpace = 0;
    abrc->fShadow = 0;
    abrc->fFrame = 0;
    abrc->reserved = 0;
    }

void wvCopyBRC(BRC *dest, BRC *src)
    {
    dest->dptLineWidth = src->dptLineWidth;
    dest->brcType = src->brcType;
    dest->ico = src->ico;
    dest->dptSpace = src->dptSpace;
    dest->fShadow = src->fShadow;
    dest->fFrame = src->fFrame;
    dest->reserved = src->reserved;
    }

/* 
I'm not certain as to how this should work, but it will probably
never occur, its in here for the sake of completeness
*/
void wvConvertBRC10ToBRC(BRC *item,BRC10 *in)
	{
	wvInitBRC(item);
	item->dptSpace = in->dxpSpace;
	item->fShadow = in->fShadow;
	/*
	The border lines and their brc10 settings follow:

	line type        dxpLine1Width               dxpSpaceBetween dxpLine2Width

	no border        0                           0               0

	single line      1                           0               0
	border

	two single line  1                           1               1
	border

	fat solid border 4                           0               0

	thick solid      2                           0               0
	border

	dotted border    6 (special value meaning    0               0
					dotted line)

	hairline border  7(special value meaning     0               0
					hairline)
	*/
	if ((in->dxpLine1Width == 0) && (in->dxpSpaceBetween == 0) && (in->dxpLine2Width == 0) )
		item->brcType = 0;
	else if ((in->dxpLine1Width == 1) && (in->dxpSpaceBetween == 0) && (in->dxpLine2Width == 0) )
		item->brcType = 1; 
	else if ((in->dxpLine1Width == 1) && (in->dxpSpaceBetween == 1) && (in->dxpLine2Width == 1) )
		item->brcType = 3;
	else if ((in->dxpLine1Width == 4) && (in->dxpSpaceBetween == 0) && (in->dxpLine2Width == 0) )
		item->brcType = 3;
	else if ((in->dxpLine1Width == 2) && (in->dxpSpaceBetween == 0) && (in->dxpLine2Width == 0) )
		item->brcType = 2;
	else if ((in->dxpLine1Width == 6) && (in->dxpSpaceBetween == 0) && (in->dxpLine2Width == 0) )
		item->brcType = 6;
	else if ((in->dxpLine1Width == 7) && (in->dxpSpaceBetween == 0) && (in->dxpLine2Width == 0) )
		item->brcType = 5;
	else
		item->brcType=0;
	}

