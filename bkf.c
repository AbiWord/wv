#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetBKF(BKF *item,FILE *fd)
	{
	U16 temp16;
	item->ibkl = (S16)read_16ubit(fd);
	temp16 = read_16ubit(fd);
	item->itcFirst = temp16 & 0x007F;
    item->fPub = (temp16 & 0x0080) >> 7;
    item->itcLim = (temp16 & 0x7F00) >> 8;
    item->fCol = (temp16 & 0x8000) >> 15;
	}


int wvGetBKF_PLCF(BKF **bkf,U32 **pos,U32 *nobkf,U32 offset,U32 len,FILE *fd)
	{
	U32 i;
	if (len == 0)
		{
		*bkf = NULL;
		*pos = NULL;
		*nobkf = 0;
		}
	else
        {
        *nobkf=(len-4)/(cbBKF+4);
        *pos = (U32 *) malloc( (*nobkf+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError("NO MEM 1, failed to alloc %d bytes\n",(*nobkf+1) * sizeof(U32));
            return(1);
            }

        *bkf= (BKF *) malloc(*nobkf* sizeof(BKF));
        if (*bkf== NULL)
            {
            wvError("NO MEM 1, failed to alloc %d bytes\n",*nobkf* sizeof(BKF));
			free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<=*nobkf;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nobkf;i++)
            wvGetBKF(&((*bkf)[i]),fd);
        }
	return(0);
	}
