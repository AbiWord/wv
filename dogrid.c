#include <string.h>
#include <stdio.h>
#include "wv.h"

void wvGetDOGRID(DOGRID *dog,FILE *fd)
	{
	U16 temp16;
	dog->xaGrid = read_16ubit(fd);
	dog->yaGrid = read_16ubit(fd);
	dog->dxaGrid = read_16ubit(fd);
	dog->dyaGrid = read_16ubit(fd);

	temp16 = read_16ubit(fd);

	dog->dyGridDisplay = temp16&0x007F;
	dog->fTurnItOff = (temp16&0x0080)>>7;
	dog->dxGridDisplay = (temp16&0x7F00)>>8;
	dog->fFollowMargins = (temp16&0x8000)>>15;
	}
