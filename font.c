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

void wvInitFONTSIGNATURE(FONTSIGNATURE *fs)
	{
	int i;
	for (i=0;i<4;i++)
		fs->fsUsb[i] = 0;
	for (i=0;i<2;i++)
		fs->fsCsb[i] = 0;
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

void wvInitPANOSE(PANOSE *item)
	{
	item->bFamilyType=0;
    item->bSerifStyle=0;
    item->bWeight=0;
    item->bProportion=0;
    item->bContrast=0;
    item->bStrokeVariation=0;
    item->bArmStyle=0;
    item->bLetterform=0;
    item->bMidline=0;
    item->bXHeight=0;
	}
