#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

U32 wvGetFAnchor(FAnchor *item,FILE *fd)
	{
	/* It is supposed to be a RECT, but its only 4 long so... */
	item->left = getc(fd);
	item->right = getc(fd);
	item->top = getc(fd);
	item->bottom = getc(fd);
	return(4);
	}

void wvInitFOPTEArray(FOPTE **fopte)
	{
	*fopte=NULL;
	}

void wvReleaseFOPTEArray(FOPTE **fopte)
	{
	U32 i=0;
	if (*fopte)
		{
		while((*fopte)[i].pid != 0)
			{
			wvFree((*fopte)[i].entry);
			i++;
			}
		wvFree(*fopte);
		}
	}

U32 wvGetFOPTEArray(FOPTE **fopte,MSOFBH *msofbh,FILE *fd)
	{
	U32 i,j,count=0;
	U32 no = msofbh->cbLength/6;
	*fopte = (FOPTE *)malloc(sizeof(FOPTE) * no);
	no=0;
	while (count < msofbh->cbLength)
		{
		wvTrace(("count %x %x, pos %x\n",count,msofbh->cbLength,ftell(fd)));
		count += wvGetFOPTE(&(*fopte)[no],fd);
		no++;
		}
	*fopte = (FOPTE *)realloc(*fopte,sizeof(FOPTE) * (no+1));
	for(i=0;i<no;i++)
		{
		if ( (*fopte)[i].fComplex )
			for(j=0;j<(*fopte)[i].op;j++)
			(*fopte)[i].entry[j] = getc(fd);	
		}
	(*fopte)[i].pid=0;
	wvTrace(("returning %x\n",count));
	return(count);
	}


void wvReleaseFOPTE(FOPTE *afopte)
	{
	wvFree(afopte->entry);
	}

U32 wvGetFOPTE(FOPTE *afopte,FILE *fd)
    {
    U16 dtemp;
	wvTrace(("pos is %x\n",ftell(fd)));
    dtemp = read_16ubit(fd);
	wvTrace(("dtemp is %x\n",dtemp));
#ifdef PURIFY
    afopte->pid = 0;
    afopte->fBid = 0;
    afopte->fComplex = 0;
#endif
    afopte->pid = (dtemp & 0x3fff);
    afopte->fBid = ((dtemp & 0x4000)>>14);
    afopte->fComplex = ((dtemp & 0x8000)>>15);
    afopte->op = read_32ubit(fd);

    wvTrace(("orig %x,pid is %x %d, val is %x\n",dtemp,afopte->pid,afopte->pid,afopte->op));
    if ( afopte->fComplex )
		{
        wvTrace(("1 complex len is %d (%x)\n",afopte->op,afopte->op));
		afopte->entry = (U8 *)malloc(afopte->op);
		return(afopte->op+6);
        }

	afopte->entry = NULL;
    return(6);
    }
