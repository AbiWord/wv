/* I hope I don't have to write a wvPutBKL_PCLF!!*/


void wvPutBKL(BKL *item,wvStream *fd)
{
	write_16ubit(fd, (U16)item->ibkf);
}