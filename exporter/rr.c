#include "wvexporter.h"

void wvPutRR(RR *item, wvStream *fd)
{
  write_16ubit(fd, (U16)item->cv);
  write_16ubit(fd, (U16)item->cvSzRecip);
}
