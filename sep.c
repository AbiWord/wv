#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
The standard SEP is all zeros except as follows:
 bkc           2 (new page)
 dyaPgn        720 twips (equivalent to .5 in)
 dxaPgn        720 twips
 fEndnote      1 (True)
 fEvenlySpaced 1 (True)
 xaPage        12240 twips
 yaPage        15840 twips
 xaPageNUp     12240 twips
 yaPageNUp     15840 twips
 dyaHdrTop     720 twips
 dyaHdrBottom  720 twips
 dmOrientPage  1 (portrait orientation)
 dxaColumns    720 twips
 dyaTop        1440 twips
 dxaLeft       1800 twips
 dyaBottom     1440 twips
 dxaRight      1800 twips
 pgnStart      1
cbSEP (count of bytes of SEP) is 704(decimal), 2C0(hex).
*/

void wvInitSEP(SEP *item)
	{
	U8 i;
 	item->bkc=2;
 	item->fTitlePage=0;
	item->fAutoPgn=0;
 	item->nfcPgn=0;
 	item->fUnlocked=0;
	item->cnsPgn=0;
 	item->fPgnRestart=0;
 	item->fEndNote=1;
 	item->lnc=0;
 	item->grpfIhdt=0;
 	item->nLnnMod=0;
 	item->dxaLnn=0;
 	item->dxaPgn=720;
 	item->dyaPgn=720;
 	item->fLBetween=0;
 	item->vjc=0;
 	item->dmBinFirst=0;
 	item->dmBinOther=0;
 	item->dmPaperReq=0;

 	wvInitBRC(&item->brcTop);
 	wvInitBRC(&item->brcLeft);
 	wvInitBRC(&item->brcBottom);
 	wvInitBRC(&item->brcRight);

 	item->fPropRMark=0;
	item->ibstPropRMark=0;

 	wvInitDTTM(&item->dttmPropRMark);

 	item->dxtCharSpace=0;
 	item->dyaLinePitch=0;
 	item->clm=0;
    item->reserved1=0;
 	item->dmOrientPage=0;
 	item->iHeadingPgn=0;
 	item->pgnStart=1;
	item->lnnMin=0;
 	item->wTextFlow=0;
 	item->reserved2=0;
 	item->pgbProp=0;
 	item->pgbApplyTo=0;
	item->pgbPageDepth=0;
	item->pgbOffsetFrom=0;
	item->reserved=0;
 	item->xaPage=12240;
 	item->yaPage=15840;
 	item->xaPageNUp=12240;
 	item->yaPageNUp=15840;
 	item->dxaLeft=1800;
 	item->dxaRight=1800;
 	item->dyaTop=1440;
 	item->dyaBottom=1440;
 	item->dzaGutter=0;
 	item->dyaHdrTop=720;
 	item->dyaHdrBottom=720;
 	item->ccolM1=0;
 	item->fEvenlySpaced=1;
 	item->reserved3=0;
 	item->dxaColumns=720;
	for (i=0;i<89;i++)
 		item->rgdxaColumnWidthSpacing[i]=0;
 	item->dxaColumnWidth=0;
 	item->dmOrientFirst=0;
 	item->fLayout=0;
 	item->reserved4=0;
 	wvInitOLST(&item->olstAnm);
	} 

void wvGetSEPX(SEPX *item,FILE *fd)
	{
	U16 i;
	item->cb = read_16ubit(fd);
	item->grpprl = (U8 *)malloc(item->cb);
	for (i=0;i<item->cb;i++)
		item->grpprl[i] = getc(fd);
	}

void wvReleaseSEPX(SEPX *item)
	{
	wvFree(item->grpprl);
	}


void wvAddSEPXFromBucket(SEP *asep,SEPX *item,STSH *stsh)
	{
	U8 *pointer;
    U16 i=0;
    U16 sprm;
    while (i < item->cb)
        {
        sprm = bread_16ubit(item->grpprl+i,&i);
        pointer = item->grpprl+i;
        wvApplySprmFromBucket(0,sprm,NULL,NULL,asep,stsh,pointer,&i);
        }
	}















