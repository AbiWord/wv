#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutTBD(TBD *item, wvStream *fd)
{
  U8 temp8 = 0;

  temp8 |= item->jc;
  temp8 |= item->tlc << 3;
  temp8 |= item->reserved << 6;

  write_8ubit(fd, temp8);
}
