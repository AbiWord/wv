/* foo */


void wvPutBKD(BKD *item,wvStream *fd)
{
	U16 temp16 = (U16)0;

	write_16ubit(fd, (U16)item->ipgd_itxbs);
	write_16ubit(fd, (U16)item->dcpDepend);

	temp16 |= item->icol;
	temp16 |= item->fTableBreak << 8;
	temp16 |= item->ColumnBreak << 9;
	temp16 |= item->fMarked << 10;
	temp16 |= item->Unk << 11;
	temp16 |= item->TextOverflow << 12;
	temp16 |= item->reserved1 << 13;

	write_16ubit(fp, temp16);
}