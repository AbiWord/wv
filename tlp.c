#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvCopyTLP(TLP *dest,TLP *src)
	{
	dest->fBorders = src->fBorders;
	dest->fShading = src->fShading;
	dest->fFont = src->fFont;
	dest->fColor = src->fColor;
	dest->fBestFit = src->fBestFit;
	dest->fHdrRows = src->fHdrRows;
	dest->fLastRow = src->fLastRow;
	dest->fHdrCols = src->fHdrCols;
	dest->fLastCol = src->fLastCol;
	}

void wvInitTLP(TLP *item)
	{
	item->fBorders = 0;
	item->fShading = 0;
	item->fFont = 0;
	item->fColor = 0;
	item->fBestFit = 0;
	item->fHdrRows = 0;
	item->fLastRow = 0;
	item->fHdrCols = 0;
	item->fLastCol = 0;
	}
