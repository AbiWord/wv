/* no PCLF version */


void wvPutFRD(FRD *item,wvStream *fd)
        {
        write_16ubit(fd,(U16)item->frd);
        }