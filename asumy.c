#include <string.h>
#include <stdio.h>
#include "wv.h"

void wvGetASUMY(ASUMY *item,wvStream *fd)
	{
	item->lLevel = (S32)read_32ubit(fd);
	}
