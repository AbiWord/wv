#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutBRC10(BRC *item, wvStream *fd)
{
  /* todo someday?? what's it good for? */
}

void wvPutBRC6(BRC *item, wvStream *fd)
{
  /* non-word8 support */
  U16 temp16 = 0;

  temp16 |= item->dptLineWidth;
  temp16 |= item->brcType<<3;
  temp16 |= item->fShadow<<5;
  temp16 |= item->ico<<6;
  temp16 |= item->dptSpace<<11;

  write_16ubit(fd, temp16);
}

void wvPutBRC(BRC *item, wvStream *fd)
{
  U8 temp8 = 0;

  /* word 8 support */

  write_8ubit(fd, item->dptLineWidth);
  write_8ubit(fd, item->brcType);
  write_8ubit(fd, item->ico);

  temp8 |= item->dptSpace;
  temp8 |= item->fShadow << 5;
  temp8 |= item->fFrame << 6;
  temp8 |= item->reserved << 7;

  write_8ubit(fd, temp8);
}
