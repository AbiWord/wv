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
	int len;
	wvReleaseLVL(dest);
	wvInitLVL(dest);

	wvCopyLVLF(&(dest->lvlf),&(src->lvlf));


	if (src->lvlf.cbGrpprlPapx > 0)
		{
		dest->grpprlPapx = (U8 *)malloc(src->lvlf.cbGrpprlPapx);
		memcpy(dest->grpprlPapx,src->grpprlPapx,src->lvlf.cbGrpprlPapx);
		}		
	else
		dest->grpprlPapx = NULL;

	if (src->lvlf.cbGrpprlChpx > 0)
		{
		dest->grpprlChpx = (U8 *)malloc(src->lvlf.cbGrpprlChpx);
		memcpy(dest->grpprlChpx,src->grpprlChpx,src->lvlf.cbGrpprlChpx);
		}	
	else
		dest->grpprlChpx = NULL;
	
	if (src->numbertext==NULL)
		dest->numbertext=NULL;
	else
		{
		len = src->numbertext[0];
		dest->numbertext = (U16 *)malloc(sizeof(U16)*(len +2));
		memcpy(dest->numbertext,src->numbertext,len+2);
		}
	}

void wvGetLVL(LVL *lvl,FILE *fd)
	{
	int len;
	wvGetLVLF(&(lvl->lvlf),fd);
	if (lvl->lvlf.cbGrpprlPapx > 0)
		{
		lvl->grpprlPapx = (U8 *)malloc(lvl->lvlf.cbGrpprlPapx);
		fread(lvl->grpprlPapx,sizeof(U8),lvl->lvlf.cbGrpprlPapx,fd);
		}		
	else
		lvl->grpprlPapx = NULL;
	if (lvl->lvlf.cbGrpprlChpx > 0)
		{
		lvl->grpprlChpx = (U8 *)malloc(lvl->lvlf.cbGrpprlChpx);
		fread(lvl->grpprlChpx,sizeof(U8),lvl->lvlf.cbGrpprlChpx,fd);
		}	
	else
		lvl->grpprlChpx = NULL;
	len = read_16ubit(fd);
	if (len > 0)
		{
		lvl->numbertext = (U16 *)malloc(sizeof(U16)*(len +2));
		lvl->numbertext[0] = len;
		fread(&(lvl->numbertext[1]),sizeof(U16),len,fd);
		lvl->numbertext[len-1] = 0;
		}
	else
		lvl->numbertext = NULL;
	wvTrace(("len is %d\n",len));
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
	memcpy(dest,src,sizeof(LVLF));
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
