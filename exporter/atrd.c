/* do I need to code a wvPutATRD_PLCF?? that would be hard ;-( */

void wvPutATRD(ATRD *item, wvStream *fd)
{
      int i;
      for(i=0;i<10;i++)
          write_16ubit(item->xstUsrInitl[i]);

      write_16ubit(fd, (U16)item->ibst);
      write_16ubit(fd, item->ak);
      write_16ubit(fd, item->grfbmc);
      write_32ubit(fd, (U32)item->lTagBkmk);
}