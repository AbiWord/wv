void wvPutASUMY(ASUMY *item,wvStream *fd)
{
         write_32ubit(fd, (U32)item->lLevel);
}
