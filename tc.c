#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvCopyTC(TC *dest,TC *src)
	{
	dest->rgf = src->rgf;
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
	item->rgf = 0;
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
