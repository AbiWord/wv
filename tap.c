#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvCopyTAP(TAP *dest,TAP *src)
	{
	int i;

 	dest->jc  = src->jc;
 	dest->dxaGapHalf = src->dxaGapHalf;
 	dest->dyaRowHeight = src->dyaRowHeight;
 	dest->fCantSplit = src->fCantSplit;
 	dest->fTableHeader = src->fTableHeader;

 	wvCopyTLP(&dest->tlp,&src->tlp);

 	dest->lwHTMLProps = src->lwHTMLProps;
 	dest->fCaFull = src->fCaFull;
	dest->fFirstRow = src->fFirstRow;
	dest->fLastRow = src->fLastRow;
	dest->fOutline = src->fOutline;
	dest->reserved = src->reserved;
 	dest->itcMac = src->itcMac;
	dest->dxaAdjust = src->dxaAdjust;
 	dest->dxaScale = src->dxaScale;
 	dest->dxsInch = src-> dxsInch;

	for (i=0;i<itcMax+1;i++)
 		dest->rgdxaCenter[i] = src->rgdxaCenter[i];
	for (i=0;i<itcMax+1;i++)
 		dest->rgdxaCenterPrint[i] = src->rgdxaCenterPrint[i];
	for (i=0;i<itcMax;i++)
 		wvCopyTC(&(dest->rgtc[i]),&(src->rgtc[i]));
	for (i=0;i<itcMax;i++)
 		wvCopySHD(&(dest->rgshd[i]),&(src->rgshd[i]));
	for (i=0;i<6;i++)
		wvCopyBRC(&(dest->rgbrcTable[i]),&(dest->rgbrcTable[i]));
	}

void wvInitTAP(TAP *item)
	{
	int i;

 	item->jc  = 0;
 	item->dxaGapHalf = 0;
 	item->dyaRowHeight = 0;
 	item->fCantSplit = 0;
 	item->fTableHeader = 0;

 	wvInitTLP(&item->tlp);

 	item->lwHTMLProps = 0;
 	item->fCaFull = 0;
	item->fFirstRow = 0;
	item->fLastRow = 0;
	item->fOutline = 0;
	item->reserved = 0;
 	item->itcMac = 0;
	item->dxaAdjust = 0;
 	item->dxaScale = 0;
 	item->dxsInch = 0;

	for (i=0;i<itcMax+1;i++)
 		item->rgdxaCenter[i] = 0;
	for (i=0;i<itcMax+1;i++)
 		item->rgdxaCenterPrint[i] = 0;
	for (i=0;i<itcMax;i++)
 		wvInitTC(&(item->rgtc[i]));
	for (i=0;i<itcMax;i++)
 		wvInitSHD(&(item->rgshd[i]));
	for (i=0;i<6;i++)
		wvInitBRC(&(item->rgbrcTable[i]));
	}
