#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetRR(RR *item,wvStream *fd)
	{
	item->cb = (S16)read_16ubit(fd);
	item->cbSzRecip = (S16)read_16ubit(fd);
	} 
