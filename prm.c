#include <stdlib.h>
#include <stdio.h>
#include "wv.h"


void wvGetPRM(PRM *item,FILE *fd)
	{
	U16 temp16;
	temp16 = read_16ubit(fd);
	item->fComplex = temp16 & 0x0001;

	if (item->fComplex)
		{
		item->var1.isprm = (temp16 & 0x00fe) >> 1;
		item->var1.val = (temp16 & 0xff00) >> 8;
		}
	else
		item->var2.igrpprl = (temp16 & 0xfffe) >> 1;
	}

void wvInitPRM(PRM *item)
	{
	item->fComplex = 0;
	item->var2.igrpprl = 0;
	}
