#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvInitLVL(LVL *lvl)
	{
	lvl->grpprlPapx=NULL;
	lvl->grpprlChpx=NULL;
	lvl->numbertext=NULL;
	wvInitLVLF(&(lvl->lvlf));
	}

void wvCopyLVL(LVL *dest,LVL *src)
	{
	int len,i;
	wvReleaseLVL(dest);
	wvInitLVL(dest);

	wvCopyLVLF(&(dest->lvlf),&(src->lvlf));


	if (src->lvlf.cbGrpprlPapx > 0)
		{
		dest->grpprlPapx = (U8 *)malloc(src->lvlf.cbGrpprlPapx);
		for(i=0;i<src->lvlf.cbGrpprlPapx;i++)
			dest->grpprlPapx[i] = src->grpprlPapx[i];
		}		
	else
		dest->grpprlPapx = NULL;

	if (src->lvlf.cbGrpprlChpx > 0)
		{
		dest->grpprlChpx = (U8 *)malloc(src->lvlf.cbGrpprlChpx);
		for(i=0;i<src->lvlf.cbGrpprlChpx;i++)
			dest->grpprlChpx[i] = src->grpprlChpx[i];
		}	
	else
		dest->grpprlChpx = NULL;
	
	if (src->numbertext==NULL)
		dest->numbertext=NULL;
	else
		{
		len = src->numbertext[0];
		dest->numbertext = (U16 *)malloc(sizeof(U16)*(len +2));
		dest->numbertext[0] = len;
		for(i=1;i<len+1;i++)
			dest->numbertext[i] = src->numbertext[i];
		dest->numbertext[i] = 0;
		}
	}

void wvGetLVL(LVL *lvl,FILE *fd)
	{
	int len,i;
	wvGetLVLF(&(lvl->lvlf),fd);
	if (lvl->lvlf.cbGrpprlPapx > 0)
		{
		lvl->grpprlPapx = (U8 *)malloc(lvl->lvlf.cbGrpprlPapx);
		for(i=0;i<lvl->lvlf.cbGrpprlPapx;i++)
			lvl->grpprlPapx[i] = getc(fd);
		}		
	else
		lvl->grpprlPapx = NULL;
	if (lvl->lvlf.cbGrpprlChpx > 0)
		{
		lvl->grpprlChpx = (U8 *)malloc(lvl->lvlf.cbGrpprlChpx);
		for(i=0;i<lvl->lvlf.cbGrpprlChpx;i++)
			lvl->grpprlChpx[i] = getc(fd);
		}	
	else
		lvl->grpprlChpx = NULL;
	len = read_16ubit(fd);
	if (len > 0)
		{
		lvl->numbertext = (U16 *)malloc(sizeof(U16)*(len +2));
		lvl->numbertext[0] = len;
		for(i=1;i<len+1;i++)
			lvl->numbertext[i] = read_16ubit(fd);
		lvl->numbertext[i] = 0;
		}
	else
		lvl->numbertext = NULL;
	}

void wvReleaseLVL(LVL *lvl)
	{
	if (lvl == NULL)
		return;
	wvFree(lvl->grpprlPapx);
	wvFree(lvl->grpprlChpx);
	wvFree(lvl->numbertext);
	}

void wvCopyLVLF(LVLF *dest,LVLF *src)
	{
	int i;
	dest->iStartAt = src->iStartAt;
    dest->nfc = src->nfc;
    dest->jc = src->jc;
   	dest->fLegal = src->fLegal;
    dest->fNoRestart = src->fNoRestart;
    dest->fPrev = src->fPrev;
    dest->fPrevSpace = src->fPrevSpace;
    dest->fWord6 = src->fWord6;
    dest->reserved1 = src->reserved1;
	for (i=0;i<9;i++)
    	dest->rgbxchNums[i] = src->rgbxchNums[i];
    dest->ixchFollow = src->ixchFollow;
    dest->dxaSpace = src->dxaSpace;
    dest->dxaIndent = src->dxaIndent;
    dest->cbGrpprlChpx = src->cbGrpprlChpx;
    dest->cbGrpprlPapx = src->cbGrpprlPapx;
    dest->reserved2 = src->reserved2;
	}

void wvGetLVLF(LVLF *item,FILE *fd)
	{
	U8 temp8;
	int i;
#ifdef PURIFY
	wvInitLVLF(item);
#endif
	item->iStartAt = read_32ubit(fd);
	wvTrace(("iStartAt is %d\n",item->iStartAt));
    item->nfc = getc(fd);
	temp8 = getc(fd);
    item->jc = temp8 & 0x03;
   	item->fLegal = (temp8 & 0x04) >>2;
    item->fNoRestart = (temp8 & 0x08) >> 3;
    item->fPrev = (temp8 & 0x10) >> 4;
    item->fPrevSpace = (temp8 & 0x20) >> 5;
    item->fWord6 = (temp8 & 0x40) >> 6;
    item->reserved1 = (temp8 & 0x80) >> 7;
	for (i=0;i<9;i++)
    	item->rgbxchNums[i] = getc(fd); 
    item->ixchFollow = getc(fd);;
    item->dxaSpace = read_32ubit(fd);      
    item->dxaIndent = read_32ubit(fd);     
    item->cbGrpprlChpx = getc(fd);
    item->cbGrpprlPapx = getc(fd);
    item->reserved2 = read_16ubit(fd);;  
	}


void wvInitLVLF(LVLF *item)
	{
	int i;
	item->iStartAt = 1;
    item->nfc = 0;
    item->jc = 0;
   	item->fLegal = 0;
    item->fNoRestart = 0;
    item->fPrev = 0;
    item->fPrevSpace = 0;
    item->fWord6 = 0;
    item->reserved1 = 0;
	for (i=0;i<9;i++)
    	item->rgbxchNums[i] = 0;
    item->ixchFollow = 0;
    item->dxaSpace = 0;
    item->dxaIndent = 0;
    item->cbGrpprlChpx = 0;
    item->cbGrpprlPapx = 0;
    item->reserved2 = 0;
	}
