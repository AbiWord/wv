#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetFONTSIGNATURE(FONTSIGNATURE *fs,FILE *fd)
	{
	int i;
	for (i=0;i<4;i++)
		fs->fsUsb[i] = read_32ubit(fd);
	for (i=0;i<2;i++)
		fs->fsCsb[i] = read_32ubit(fd);
	}

void wvGetPANOSE(PANOSE *item,FILE *fd)
	{
	item->bFamilyType=getc(fd);
    item->bSerifStyle=getc(fd);
    item->bWeight=getc(fd);
    item->bProportion=getc(fd);
    item->bContrast=getc(fd);
    item->bStrokeVariation=getc(fd);
    item->bArmStyle=getc(fd);
    item->bLetterform=getc(fd);
    item->bMidline=getc(fd);
    item->bXHeight=getc(fd);
	}
