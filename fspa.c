#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetFSPA(FSPA *item,FILE *fd)
	{
	U16 temp16;

 	item->spid = read_32ubit(fd);;
 	item->xaLeft = (S32) read_32ubit(fd);
 	item->yaTop = (S32) read_32ubit(fd);
 	item->xaRight = (S32) read_32ubit(fd);
 	item->yaBottom = (S32) read_32ubit(fd);
	temp16 = read_16ubit(fd);
 	item->fHdr = temp16 & 0x0001;
    item->bx = (temp16 & 0x0006) >> 1;
    item->by = (temp16 & 0x0018) >> 3;
	item->wr = (temp16 & 0x01E0) >> 5;
	item->wrk = (temp16 & 0x1E00) >> 9;
    item->fRcaSimple = (temp16 & 0x2000) >> 13;
	item->fBelowText =(temp16 & 0x4000) >> 14; 
	item->fAnchorLock = (temp16 & 0x8000) >> 15;
 	item->cTxbx = (S32) read_32ubit(fd);
	}


int wvGetFSPA_PLCF(FSPA **fspa,U32 **pos,U32 *nofspa,U32 offset,U32 len,FILE *fd)
	{
	int i;
	if (len == 0)
		{
		*fspa = NULL;
		*pos = NULL;
		*nofspa = 0;
		}
	else
        {
        *nofspa=(len-4)/30;
        *pos = (U32 *) malloc( (*nofspa+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError("NO MEM 1, failed to alloc %d bytes\n",(*nofspa+1) * sizeof(U32));
            return(1);
            }

        *fspa= (FSPA *) malloc(*nofspa* sizeof(FSPA));
        if (*fspa== NULL)
            {
            wvError("NO MEM 1, failed to alloc %d bytes\n",*nofspa* sizeof(FSPA));
			free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<*nofspa+1;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nofspa;i++)
            wvGetFSPA(&((*fspa)[i]),fd);
        }
	return(0);
	}
