#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvInitOLST(OLST *item)
	{
	U8 i;
	for (i=0;i<9;i++)
		wvInitANLV(&item->rganlv[i]);
 	item->fRestartHdr=0;
 	item->fSpareOlst2=0;
 	item->fSpareOlst3=0;
 	item->fSpareOlst4=0;
	for (i=0;i<32;i++)
 		item->rgxch[i]=0;
	} 


