#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
Word writes out the rglst as the plcflst by writing out, first, a short integer 
containing the number of LST structures to be written. It then enumerates 
through the rglst, writing out each LSTF structure. It then enumerates through 
the rglst again, deciding, for each LST, whether it has one level 
(LSTF.fSimpleList) or nine levels (!LSTF.fSimpleList). It then writes the 
appropriate number of LVL structures
*/
int wvGetLST(LST **lst,U16 *noofLST,U32 offset,U32 len,FILE *fd)
	{
	U16 i,j;
	*lst = NULL;

	if (len == 0)
		return(0);

	fseek(fd,offset,SEEK_SET);
	wvTrace(("offset is %x, len is %d\n",offset,len));

	*noofLST = read_16ubit(fd);
	wvTrace(("noofLST is %d\n",*noofLST));
	if (*noofLST == 0)
		return(0);

	*lst = (LST *) malloc(*noofLST * sizeof(LST));
	if (*lst== NULL)
		{
		wvError(("NO MEM 1, failed to alloc %d bytes\n",*noofLST * sizeof(LST)));
		return(1);
		}

	for (i=0;i<*noofLST;i++)
		{
		wvGetLSTF(&((*lst)[i].lstf),fd);
		if ((*lst)[i].lstf.fSimpleList)
			{
			(*lst)[i].lvl = (LVL *)malloc(sizeof(LVL));
			(*lst)[i].current_no = (U32 *)malloc(sizeof(U32));
			}
		else
			{
			(*lst)[i].lvl = (LVL *)malloc(9 * sizeof(LVL));
			(*lst)[i].current_no = (U32 *)malloc(9 * sizeof(U32));
			}
		}
	for (i=0;i<*noofLST;i++)
		{
		wvTrace(("getting lvl, the id is %x\n",(*lst)[i].lstf.lsid));
		if ((*lst)[i].lstf.fSimpleList)
			{
			wvTrace(("simple 1\n"));
			wvGetLVL(&((*lst)[i].lvl[0]),fd);
			(*lst)[i].current_no[0] = (*lst)[i].lvl[0].lvlf.iStartAt;
			}
		else
			{
			wvTrace(("complex 9\n"));
			for (j=0;j<9;j++)
				{
				wvGetLVL(&((*lst)[i].lvl[j]),fd);
				(*lst)[i].current_no[j] = (*lst)[i].lvl[j].lvlf.iStartAt;
				}
			}
		}
	return(0);
	}

void wvGetLSTF(LSTF *item,FILE *fd)
	{
	int i;
	U8 temp8;
    item->lsid = read_32ubit(fd);
	wvTrace(("lsid is %x\n",item->lsid));
	item->tplc = read_32ubit(fd);
    for (i=0;i<9;i++)
		item->rgistd[i]  = read_16ubit(fd);
	temp8 = getc(fd);
	item->fSimpleList = temp8 & 0x01;
    item->fRestartHdn = (temp8 & 0x02)>>1;
    item->reserved1 = (temp8 & 0xFC)>>2;
    item->reserved2 = getc(fd);
	}


int wvGetLSTF_PLCF(LSTF **lstf,U32 **pos,U32 *nolstf,U32 offset,U32 len,FILE *fd)
	{
	U32 i;
	if (len == 0)
		{
		*lstf = NULL;
		*pos = NULL;
		*nolstf = 0;
		}
	else
        {
        *nolstf=(len-4)/(cbLSTF+4);
        *pos = (U32 *) malloc( (*nolstf+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",(*nolstf+1) * sizeof(U32)));
            return(1);
            }

        *lstf= (LSTF *) malloc(*nolstf* sizeof(LSTF));
        if (*lstf== NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*nolstf* sizeof(LSTF)));
			free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<=*nolstf;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nolstf;i++)
            wvGetLSTF(&((*lstf)[i]),fd);
        }
	return(0);
	}


void wvReleaseLST(LST **lst,U16 noofLST)
	{
	int i,j;
	if ((lst == NULL) && (*lst == NULL))
		return;
	for (i=0;i<noofLST;i++)
		{
		if ((*lst)[i].lstf.fSimpleList)
			wvReleaseLVL(&((*lst)[i].lvl[0]));
		else
			for (j=0;j<9;j++)
				wvReleaseLVL(&((*lst)[i].lvl[j]));
		wvFree((*lst)[i].current_no);
		}
	wvFree(*lst);
	}

/*
Using the LFO's List ID, search the rglst for the LST with that List ID.
*/
LST *wvSearchLST(U32 id,LST *lst,U16 noofLST)
	{
	int i;
	for(i=0;i<noofLST;i++)
		{
		if (lst[i].lstf.lsid == id)
			return(&(lst[i]));
		}
	wvWarning("Couldn't find list id %x\n",id);
	return(NULL);
	}
