#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutLSPD(LSPD *item, wvStream *fd)
{
  write_16ubit(fd, item->dyaLine);
  write_16ubit(fd, item->fMultLinespace);
}
