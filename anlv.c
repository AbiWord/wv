#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvInitANLV(ANLV *item)
	{
	item->nfc=0;
 	item->cxchTextBefore=0;
	item->cxchTextAfter=0;
 	item->jc=0;
	item->fPrev=0;
	item->fHang=0;
	item->fSetBold=0;
	item->fSetItalic=0;
	item->fSetSmallCaps=0;
	item->fSetCaps=0;
 	item->fSetStrike=0;
	item->fSetKul=0;
	item->fPrevSpace=0;
	item->fBold=0;
	item->fItalic=0;
	item->fSmallCaps=0;
	item->fCaps=0;
	item->fStrike=0;
	item->kul=0;
	item->ico=0;
	item->ftc=0;
	item->hps=0;
	item->iStartAt=0;
	item->dxaIndent=0;
	item->dxaSpace=0;
	} 
