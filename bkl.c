#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetBKL(BKL *item,FILE *fd)
	{
	item->ibkf = (S16)read_16ubit(fd);
	}

int wvGetBKL_PLCF(BKL **bkl,U32 **pos,U32 *nobkl,U32 offset,U32 len,FILE *fd)
    {
    U32 i;
    if (len == 0)
        {
        *bkl = NULL;
        *pos = NULL;
        *nobkl = 0;
        }
    else
        {
        *nobkl=(len-4)/(cbBKL+4);
        *pos = (U32 *) malloc( (*nobkl+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",(*nobkl+1) * sizeof(U32)));
            return(1);
            }

        *bkl= (BKL *) malloc(*nobkl* sizeof(BKL));
        if (*bkl== NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*nobkl* sizeof(BKL)));
            free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<=*nobkl;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nobkl;i++)
            wvGetBKL(&((*bkl)[i]),fd);
        }
    return(0);
    }

