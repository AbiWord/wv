#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
Word writes out the pllfo first by writing out a PL of LFO structures. 
It then enumerates through each LFO to figure out how many LFOLVLs each 
one has (LFO.clfolvl), and writes out, in order, each LFOLVL structure 
followed by its corresponding LVL structure (if LFOLVL.fFormatting is set).
*/

int wvGetLFO_records(LFO **lfo,LFOLVL **lfolvl,LVL **lvl,U32 *nolfo,U32 *nooflvl,U32 offset,U32 len,FILE *fd)
	{
	U32 i;
	*nooflvl=0;
	wvTrace(("lfo begins at %x len %d\n",offset,len));
	wvGetLFO_PLF(lfo,nolfo,offset,len,fd);

	for (i=0;i<*nolfo;i++)
		*nooflvl += (*lfo)[i].clfolvl;
	wvTrace(("pos %x %d\n",ftell(fd),*nooflvl));
	wvTrace(("nolfo is %d nooflvl is %d\n",*nolfo,*nooflvl));

	if (*nooflvl == 0)
		{
		*lfolvl = NULL;
		*lvl = NULL;
		return(0);
		}

	*lfolvl = (LFOLVL *)malloc(sizeof(LFOLVL) * *nooflvl);
	*lvl = (LVL *)malloc(sizeof(LVL) * *nooflvl);
	
	i=0;
	while (i<*nooflvl)
		{
		wvInitLVL(&((*lvl)[i]));
		wvTrace(("pos now %x %d\n",ftell(fd),*nooflvl));
		wvGetLFOLVL(&((*lfolvl)[i]),fd);
		if (wvInvalidLFOLVL(&((*lfolvl)[i])) )
			continue;
		if ((*lfolvl)[i].fFormatting)
			{
			wvTrace(("formatting set\n"));
			wvGetLVL(&((*lvl)[i]),fd);
			}
		i++;
		}
	return(0);
	}

int wvGetLFO_PLF(LFO **lfo,U32 *nolfo,U32 offset,U32 len,FILE *fd)
	{
	U32 i;
	if (len == 0)
		{
		*lfo = NULL;
		*nolfo = 0;
		}
	else
        {
        fseek(fd,offset,SEEK_SET);
        *nolfo=read_32ubit(fd);

        *lfo= (LFO *) malloc(*nolfo* sizeof(LFO));
        if (*lfo== NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*nolfo* sizeof(LFO)));
            return(1);
            }
        for(i=0;i<*nolfo;i++)
            wvGetLFO(&((*lfo)[i]),fd);
        }
	return(0);
	}

void wvGetLFO(LFO *item,FILE *fd)
	{
	int i;
	item->lsid = read_32ubit(fd);     
    item->reserved1 = read_32ubit(fd);
    item->reserved2 = read_32ubit(fd);  
    item->clfolvl = getc(fd);  
	for(i=0;i<3;i++)
    	item->reserved3[i] = getc(fd);
	}

void wvGetLFOLVL(LFOLVL *item,FILE *fd)
	{
	U8 temp8;
#ifdef PURIFY
	wvInitLFOLVL(item);
#endif
	item->iStartAt = read_32ubit(fd);
	temp8 = getc(fd);
    item->ilvl = temp8 & 0x0F;
    item->fStartAt = (temp8 & 0x10) >> 4;
    item->fFormatting = (temp8 & 0x20) >> 5;
    item->reserved1 = (temp8 & 0xC0) >> 6;
    item->reserved2 = getc(fd);
    item->reserved3 = getc(fd);
    item->reserved4 = getc(fd);
	}

void wvInitLFOLVL(LFOLVL *item)
	{
	item->iStartAt = 0;
    item->ilvl = 0;
    item->fStartAt = 0;
    item->fFormatting = 0;
    item->reserved1 = 0;
    item->reserved2 = 0;
    item->reserved3 = 0;
    item->reserved4 = 0;
	}

int wvInvalidLFOLVL(LFOLVL *item)
	{
	
	if (item->iStartAt != 0xffffffff)
		return(0);
#if 0	
	/* 
	a bloody russian doc, from Sergey V. Udaltsov <svu@pop.convey.ru> caused 
	the removal of this section 
	*/
    if (item->ilvl != 0xf)
		return(0);
    if (item->fStartAt != 1)
		return(0);
    if (item->fFormatting != 1)
		return(0);
    if (item->reserved1 != 0x3) 
		return(0);
    if (item->reserved2 != 0xff) 
		return(0);
    if (item->reserved3 != 0xff) 
		return(0);
    if (item->reserved4 != 0xff) 
		return(0);
#endif
	wvWarning(("invalid list entry, trucking along happily anyway\n"));
	return(1);
	}

int wvReleaseLFO_records(LFO **lfo,LFOLVL **lfolvl,LVL **lvl,U32 nooflvl)
	{
	U32 i;
	wvTrace(("releaseing %d lvl records\n",nooflvl));
	wvFree(*lfo);
	wvFree(*lfolvl);
	for(i=0;i<nooflvl;i++)
		wvReleaseLVL(&((*lvl)[i]));
	wvFree(*lvl);
	return(0);
	}
