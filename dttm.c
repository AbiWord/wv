#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetDTTM(DTTM *item,FILE *fd)
	{
	wvCreateDTTM(item,read_16ubit(fd),read_16ubit(fd));
	}

void wvGetDTTMFromBucket(DTTM *item,U8 *pointer)
	{
	wvCreateDTTM(item,dread_16ubit(NULL,&pointer),dread_16ubit(NULL,&pointer));
	}


void wvCreateDTTM(DTTM *dttm,U16 temp1_16,U16 temp2_16)
    {
    dttm->mint = temp1_16 & 0x003F;
    dttm->hr = (temp1_16 & 0x07C0)>>6;
    dttm->dom = (temp1_16 & 0xF800)>>11;

    dttm->mon = temp2_16 & 0x000F;
    dttm->yr = (temp2_16 & 0x1FF0)>>4;
    dttm->wdy = (temp2_16 & 0xE000)>>13;
    }


void wvInitDTTM(DTTM *dttm)
    {
    dttm->mint = 0;
    dttm->hr = 0;
    dttm->dom = 0;

    dttm->mon = 0;
    dttm->yr = 0;
    dttm->wdy = 0;
    }

void wvCopyDTTM(DTTM *dest,DTTM *src)
    {
    dest->mint = src->mint;
    dest->hr = src->hr;
    dest->dom = src->dom;

    dest->mon = src->mon;
    dest->yr = src->yr;
    dest->wdy = src->wdy;
    }
