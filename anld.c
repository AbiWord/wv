#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "crc32.h"

void wvGetANLD(int version,ANLD *item,FILE *fd)
	{
	U8 temp8;
	int i;
#ifdef PURIFY
	wvInitANLD(item);
#endif
    item->nfc = getc(fd);
    item->cxchTextBefore = getc(fd);
    item->cxchTextAfter = getc(fd);
	temp8 = getc(fd);
    item->jc = temp8 & 0x03;
    item->fPrev = (temp8 & 0x04)>>2;
    item->fHang = (temp8 & 0x08)>>3;
    item->fSetBold = (temp8 & 0x10)>>4;
    item->fSetItalic = (temp8 & 0x20)>>5;
    item->fSetSmallCaps = (temp8 & 0x40)>>6;
    item->fSetCaps = (temp8 & 0x80)>>7;
	temp8 = getc(fd);
    item->fSetStrike = temp8 & 0x01;
    item->fSetKul = (temp8 & 0x02)>>1;
    item->fPrevSpace = (temp8 & 0x04)>>2;
    item->fBold = (temp8 & 0x08)>>3;
    item->fItalic = (temp8 & 0x10)>>4;
    item->fSmallCaps = (temp8 & 0x20)>>5;
    item->fCaps = (temp8 & 0x40)>>6;
    item->fStrike = (temp8 & 0x80)>>7;
	temp8 = getc(fd);
    item->kul = temp8 & 0x07;
    item->ico = (temp8 & 0xF1) >> 3;
    item->ftc = (S16)read_16ubit(fd);
    item->hps = read_16ubit(fd);
    item->iStartAt = read_16ubit(fd);
    item->dxaIndent = (S16)read_16ubit(fd);
    item->dxaSpace = read_16ubit(fd);
    item->fNumber1 = getc(fd);
    item->fNumberAcross = getc(fd);
    item->fRestartHdn = getc(fd);
    item->fSpareX = getc(fd);
	for (i=0;i<32;i++)
		{
		if (version == 0)
    		item->rgxch[i] = read_16ubit(fd);
		else
    		item->rgxch[i] = getc(fd);
		}
				
	}

void wvGetANLD_FromBucket(int version,ANLD *item,U8 *pointer8)
	{
	U8 temp8;
	int i;
#ifdef PURIFY
	wvInitANLD(item);
#endif
    item->nfc = dgetc(NULL,&pointer8);
    item->cxchTextBefore = dgetc(NULL,&pointer8);
    item->cxchTextAfter = dgetc(NULL,&pointer8);
	temp8 = dgetc(NULL,&pointer8);
    item->jc = temp8 & 0x03;
    item->fPrev = (temp8 & 0x04)>>2;
    item->fHang = (temp8 & 0x08)>>3;
    item->fSetBold = (temp8 & 0x10)>>4;
    item->fSetItalic = (temp8 & 0x20)>>5;
    item->fSetSmallCaps = (temp8 & 0x40)>>6;
    item->fSetCaps = (temp8 & 0x80)>>7;
	temp8 = dgetc(NULL,&pointer8);
    item->fSetStrike = temp8 & 0x01;
    item->fSetKul = (temp8 & 0x02)>>1;
    item->fPrevSpace = (temp8 & 0x04)>>2;
    item->fBold = (temp8 & 0x08)>>3;
    item->fItalic = (temp8 & 0x10)>>4;
    item->fSmallCaps = (temp8 & 0x20)>>5;
    item->fCaps = (temp8 & 0x40)>>6;
    item->fStrike = (temp8 & 0x80)>>7;
	temp8 = dgetc(NULL,&pointer8);
    item->kul = temp8 & 0x07;
    item->ico = (temp8 & 0xF1) >> 3;
    item->ftc = (S16)dread_16ubit(NULL,&pointer8);
    item->hps = dread_16ubit(NULL,&pointer8);
    item->iStartAt = dread_16ubit(NULL,&pointer8);
    item->dxaIndent = (S16)dread_16ubit(NULL,&pointer8);
    item->dxaSpace = dread_16ubit(NULL,&pointer8);
    item->fNumber1 = dgetc(NULL,&pointer8);
	if (item->fNumber1 == 46)
		wvTrace(("This level has not been modified, so you can't believe its nfc\n"));
    item->fNumberAcross = dgetc(NULL,&pointer8);
    item->fRestartHdn = dgetc(NULL,&pointer8);
    item->fSpareX = dgetc(NULL,&pointer8);
	for (i=0;i<32;i++)
		{
		if (version == 0)
    		item->rgxch[i] = dread_16ubit(NULL,&pointer8);
		else
    		item->rgxch[i] = dgetc(NULL,&pointer8);
		}
	}

void wvCopyANLD(ANLD *dest, ANLD *src)
	{
	int i;
    dest->nfc = src->nfc;
    dest->cxchTextBefore = src->cxchTextBefore;
    dest->cxchTextAfter = src->cxchTextAfter;
    dest->jc = src->jc;
    dest->fPrev = src->fPrev;
    dest->fHang = src->fHang;
    dest->fSetBold = src->fSetBold;
    dest->fSetItalic = src->fSetItalic;
    dest->fSetSmallCaps = src->fSetSmallCaps;
    dest->fSetCaps = src->fSetCaps;
    dest->fSetStrike = src->fSetStrike;
    dest->fSetKul = src->fSetKul;
    dest->fPrevSpace = src->fPrevSpace;
    dest->fBold = src->fBold;
    dest->fItalic = src->fItalic;
    dest->fSmallCaps = src->fSmallCaps;
    dest->fCaps = src->fCaps;
    dest->fStrike = src->fStrike;
    dest->kul = src->kul;
    dest->ico = src->ico;
    dest->ftc = src->ftc;
    dest->hps = src->hps;
    dest->iStartAt = src->iStartAt;
    dest->dxaIndent = src->dxaIndent;
    dest->dxaSpace = src->dxaSpace;
    dest->fNumber1 = src->fNumber1;
    dest->fNumberAcross = src->fNumberAcross;
    dest->fRestartHdn = src->fRestartHdn;
    dest->fSpareX = src->fSpareX;
	for (i=0;i<32;i++)
    	dest->rgxch[i] = src->rgxch[i]; 
	}

void wvInitANLD(ANLD *item)
	{
	int i;
    item->nfc = 0;
    item->cxchTextBefore = 0;
    item->cxchTextAfter = 0;
    item->jc = 0;
    item->fPrev = 0;
    item->fHang = 0;
    item->fSetBold = 0;
    item->fSetItalic = 0;
    item->fSetSmallCaps = 0;
    item->fSetCaps = 0;
    item->fSetStrike = 0;
    item->fSetKul = 0;
    item->fPrevSpace = 0;
    item->fBold = 0;
    item->fItalic = 0;
    item->fSmallCaps = 0;
    item->fCaps = 0;
    item->fStrike = 0;
    item->kul = 0;
    item->ico = 0;
    item->ftc = 0;
    item->hps = 0;
    item->iStartAt = 0;
    item->dxaIndent = 0;
    item->dxaSpace = 0;
    item->fNumber1 = 0;
    item->fNumberAcross = 0;
    item->fRestartHdn = 0;
    item->fSpareX = 0;
	for (i=0;i<32;i++)
    	item->rgxch[i] = 0;
	}

U32 wvCheckSumANLD(ANLD *item)
	{
	return(CalcCRC32( (unsigned char *)item, cbANLD, cbANLD, 0));
	}
