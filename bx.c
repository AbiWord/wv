#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetBX(BX *item,FILE *fd)
	{
	item->offset = getc(fd);
	wvGetPHE(&item->phe,0,fd);
	}

void wvGetBX6(BX *item,FILE *fd)
	{
	item->offset = getc(fd);
	wvGetPHE6(&item->phe,fd);
	}
