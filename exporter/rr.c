#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutRR(RR *item, wvStream *fd)
{
  write_16ubit(fd, (U16)item->cb);
  write_16ubit(fd, (U16)item->cbSzRecip);
}
