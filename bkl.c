#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"

void wvGetBKL(BKL *item,wvStream *fd)
	{
	item->ibkf = (S16)read_16ubit(fd);
	}

int wvGetBKL_PLCF(BKL **bkl,U32 **pos,U32 *nobkl,U32 offset,U32 len,wvStream *fd)
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
        *pos = (U32 *) wvMalloc( (*nobkl+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",(*nobkl+1) * sizeof(U32)));
            return(1);
            }

	*nobkl = (*nobkl ? *nobkl : 1);
        *bkl= (BKL *) wvMalloc(*nobkl* sizeof(BKL));
        if (*bkl== NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*nobkl* sizeof(BKL)));
            wvFree(pos);
            return(1);
            }
        wvStream_goto(fd,offset);
        for(i=0;i<=*nobkl;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nobkl;i++)
            wvGetBKL(&((*bkl)[i]),fd);
        }
    return(0);
    }

