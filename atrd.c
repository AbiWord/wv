#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetATRD(ATRD *item,FILE *fd)
	{
	int i;
	for(i=0;i<10;i++)
		item->xstUsrInitl[i] = read_16ubit(fd);
	item->ibst = (S16)read_16ubit(fd);
	item->ak = read_16ubit(fd);
	item->grfbmc = read_16ubit(fd);
	item->lTagBkmk = (S32)read_32ubit(fd);
	}

int wvGetATRD_PLCF(ATRD **atrd,U32 **pos,U32 *noatrd,U32 offset,U32 len,FILE *fd)
	{
	int i;
	if (len == 0)
		{
		*atrd = NULL;
		*pos = NULL;
		*noatrd = 0;
		}
	else
        {
        *noatrd=(len-4)/(cbATRD+4);
        *pos = (U32 *) malloc( (*noatrd+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",(*noatrd+1) * sizeof(U32)));
            return(1);
            }

        *atrd = (ATRD *) malloc(*noatrd * sizeof(ATRD));
        if (*atrd == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*noatrd * sizeof(ATRD)));
			free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<*noatrd+1;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*noatrd;i++)
            wvGetATRD(&((*atrd)[i]),fd);
        }
	return(0);
	}
