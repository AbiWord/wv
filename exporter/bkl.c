#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutBKL(BKL *item,wvStream *fd)
{
	write_16ubit(fd, (U16)item->ibkf);
}
