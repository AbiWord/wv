#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wvexporter.h"

/* TODO: code wvPutBlip(), wvPutMetafile() */

void wvPutFBSE(FBSE *item, wvStream *fd)
{
  int i;

  write_8ubit(fd, item->btWin32);
  write_8ubit(fd, item->btMacOS);

  for(i=0;i<16;i++)
    write_8ubit(fd, item->rgbUid[i]);

  write_16ubit(fd, item->tag);
  write_32ubit(fd, item->size);
  write_32ubit(fd, item->cRef);
  write_32ubit(fd, item->foDelay);
  write_8ubit(fd, item->usage);
  write_8ubit(fd, item->cbName);
  write_8ubit(fd, item->unused2);
  write_8ubit(fd, item->unused3);
}
