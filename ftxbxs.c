#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetFTXBXS(FTXBXS *item,FILE *fd)
	{
	item->cTxbx_iNextReuse = (S32)read_32ubit(fd);
	item->cReusable = (S32)read_32ubit(fd);
	item->fReusable = (S16)read_16ubit(fd);
	item->reserved = (S32)read_32ubit(fd);
	item->lid = (S32)read_32ubit(fd);
	item->txidUndo = (S32)read_32ubit(fd);
	}
