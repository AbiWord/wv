/* no PCLF */

void wvPutFDOA(FDOA *item,wvStream *fd)
         {
         	write_32ubit(fd, (U32)item->fc);
         	write_16ubit(fd, (U16)item->ctxbx);
         }
