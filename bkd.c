#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetBKD(BKD *item,FILE *fd)
	{
	U16 temp16;
	item->ipgd_itxbxs = (S16)read_16ubit(fd);
	item->dcpDepend = (S16)read_16ubit(fd);
	temp16 = read_16ubit(fd);
	item->icol = temp16 & 0x00FF;
	item->fTableBreak = (temp16 & 0x0100) >> 8;
	item->fColumnBreak = (temp16 & 0x0200) >> 9;
	item->fMarked = (temp16 & 0x0400) >> 10;
	item->fUnk = (temp16 & 0x0800) >> 11;
	item->fTextOverflow = (temp16 & 0x1000) >> 12;
	item->reserved1 = (temp16 & 0xE000) >> 13;
	} 
