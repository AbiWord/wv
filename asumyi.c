#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetASUMYI(ASUMYI *asu,FILE *fd)
	{
	U16 temp16 = read_16ubit(fd);

	asu->fValid = temp16&0x0001;
	asu->fView = (temp16&0x0002)>>1;
	asu->iViewBy = (temp16&0x000C)>>2;
	asu->fUpdateProps = (temp16&0x0010)>>4;
	asu->reserved = (temp16&0xFFE0)>>5;

	asu->wDlgLevel = read_16ubit(fd);
	asu->lHighestLevel = read_32ubit(fd);
	asu->lCurrentLevel = read_32ubit(fd);
	}

