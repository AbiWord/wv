#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutASUMYI(ASUMYI *asu,wvStream *fd)
{
         U16 temp16 = (U16)0;

         temp16 |= asu->fValid;
         temp16 |= asu->fView << 1;
         temp16 |= asu->iViewBy << 3;
         temp16 |= asu->fUpdateProps << 4;
         temp16 |= asu->reserved << 5;
         write_16ubit(fd, temp16);

         write_16ubit(fd, asu->wDlgLevel);
         write_16ubit(fd, asu->lHighestLevel);
         write_16ubit(fd, asu->lCurrentLevel);
}
