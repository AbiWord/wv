#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutTC(TC *tc, wvStream *fd)
{
  U16 temp16;
  BRC10 brc10;

  /* assumes word8 */
  temp16 |= item->fFirstMerged;
  temp16 |= item->fMerged<<1;
  temp16 |= item->fVertical<<2;
  temp16 |= item->fBackward<<3;
  temp16 |= item->fRotateFont<<4;
  temp16 |= item->fVertMerge<<5;
  temp16 |= item->fVertRestart<<6;
  temp16 |= item->vertAlign<<7;
  temp16 |= item->fUnused<<9;
  write_16ubit(fd, temp16);
  
  write_16ubit(fd, item->wUnused);

  wvPutBRC(item->brcTop, fd);
  wvPutBRC(item->brcLeft, fd);
  wvPutBRC(item->brcBottom, fd);
  wvPutBRC(item->brcRight, fd);
}
