#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutASUMY(ASUMY *item,wvStream *fd)
{
         write_32ubit(fd, (U32)item->lLevel);
}
