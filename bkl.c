#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetBKL(BKL *item,FILE *fd)
	{
	item->ibkf = (S16)read_16ubit(fd);
	}
