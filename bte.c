#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetBTE(BTE *bte,FILE *fd)
	{
	U32 temp32 = read_32ubit(fd);
#ifdef PURIFY
	wvInitBTE(bte);
#endif
	bte->pn = temp32 & 0x003fffffL;
	bte->unused = (temp32 & 0xffc00000L)>>22;
	}

void wvInitBTE(BTE *bte)
	{
	bte->pn = 0;
	bte->unused = 0;
	}

void wvListBTE_PLCF(BTE **bte,U32 **pos,U32 *nobte)
	{
	U32 i=0;
	for (i=0;i<*nobte;i++)
		wvTrace(("range %x %x is pn %d\n",(*pos)[i],(*pos)[i+1],(*bte)[i].pn));
	}

int wvGetBTE_PLCF6(BTE **bte,U32 **pos,U32 *nobte,U32 offset,U32 len,FILE *fd)
	{
	U32 i;
	if (len == 0)
		{
		*bte = NULL;
		*pos = NULL;
		*nobte = 0;
		}
	else
        {
        *nobte=(len-4)/(cb6BTE+4);
		wvTrace(("no of bte is %d at %x\n",*nobte,offset));
        *pos = (U32 *) malloc( (*nobte+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",(*nobte+1) * sizeof(U32)));
            return(1);
            }

        *bte = (BTE *) malloc(*nobte * sizeof(BTE));
        if (*bte == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*nobte * sizeof(BTE)));
			free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<=*nobte;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nobte;i++)
			{
            wvInitBTE(&((*bte)[i]));
            (*bte)[i].pn = read_16ubit(fd);
			}
        }
	return(0);
	}

int wvGetBTE_PLCF(BTE **bte,U32 **pos,U32 *nobte,U32 offset,U32 len,FILE *fd)
	{
	U32 i;
	if (len == 0)
		{
		*bte = NULL;
		*pos = NULL;
		*nobte = 0;
		}
	else
        {
        *nobte=(len-4)/(cbBTE+4);
		wvTrace(("no of bte is %d at %x\n",*nobte,offset));
        *pos = (U32 *) malloc( (*nobte+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",(*nobte+1) * sizeof(U32)));
            return(1);
            }

        *bte = (BTE *) malloc(*nobte * sizeof(BTE));
        if (*bte == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*nobte * sizeof(BTE)));
			free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<=*nobte;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nobte;i++)
            wvGetBTE(&((*bte)[i]),fd);
        }
	return(0);
	}

void wvCopyBTE(BTE *dest,BTE *src)
	{
	dest->pn = src->pn;
	dest->unused = src->unused;
	}

int wvGetBTE_FromFC(BTE *bte, U32 currentfc, BTE *list,U32 *fcs, int nobte)
	{
	int i=0;
	while(i<nobte)
		{
		wvTrace(("currentfc %x bte %x %x\n",currentfc,wvNormFC(fcs[i],NULL),wvNormFC(fcs[i+1],NULL)));
		if ( (currentfc >= wvNormFC(fcs[i],NULL)) && (currentfc < wvNormFC(fcs[i+1],NULL)) )
			{
			wvTrace(("valid\n"));
			wvCopyBTE(bte,&list[i]);
			return(0);
			}
		i++;
		}
	wvCopyBTE(bte,&list[i-1]);
	return(0);
	/*
	return(1);
	*/
	}
