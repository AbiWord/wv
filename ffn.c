#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvGetFFN(FFN *item,FILE *fd)
	{
	int len,i;
	U8 temp8;
	item->cbFfnM1 = getc(fd);
	temp8 = getc(fd);
	item->prq = temp8&0x03;
	item->fTrueType = (temp8&0x04)>>2;
	item->reserved1 = (temp8&0x08)>>3;
	item->ff = (temp8&0x70)>>4;
	item->reserved2 = (temp8&0x80)>>7;
	item->wWeight = (S16)read_16ubit(fd);
	item->chs = getc(fd);
	item->ixchSzAlt = getc(fd);
	wvGetPANOSE(&(item->panose),fd);
	wvGetFONTSIGNATURE(&(item->fs),fd);
	len = item->cbFfnM1-39;
	len = len/2;
	/*
	item->xszFfn = (U16) malloc(sizeof(U16) * len));
	*/
	if (len>65) len=65;
	for (i=0;i<len;i++)
		item->xszFfn[i] = read_16ubit(fd);
	}

void wvGetFFN_STTBF(FFN_STTBF *item,U32 offset,U32 len,FILE *fd)
	{
	int i;
	   wvTrace(("reading fonts...\n"));
	if (len==0)
		{
		item->nostrings=0;
		item->ffn = NULL;
		}
	else
		{
		fseek(fd,offset,SEEK_SET);
		item->extendedflag=read_16ubit(fd);
		if (item->extendedflag == 0xFFFF)
			item->nostrings=read_16ubit(fd);
		else
			item->nostrings=item->extendedflag;
		item->extradatalen=read_16ubit(fd);
		item->ffn = (FFN *)malloc(item->nostrings*sizeof(FFN));
		for (i=0;i<item->nostrings;i++)
		     {
			wvGetFFN(&(item->ffn[i]),fd);
			wvTrace(("font %d: %s\n", i, wvWideStrToMB(item->ffn[i].xszFfn)));
		     }
		}

	   wvTrace(("done reading fonts.\n"));
	}

void wvReleaseFFN_STTBF(FFN_STTBF *item)
	{
	if (item->ffn != NULL)
		free(item->ffn);
	}


/* 
This has to be extended in the future to return a list of possible 
font names, as the FFN spec mentions. It currently on does the first
one.
*/
char *wvGetFontnameFromCode(FFN_STTBF *item,int fontcode)
    {
	if (fontcode >= item->nostrings)
		return(NULL);

	return(wvWideStrToMB(item->ffn[fontcode].xszFfn));
    }

	
#if 0
int wvGetFRD_PLCF(FRD **frd,U32 **pos,int *nofrd,U32 offset,U32 len,FILE *fd)
	{
	int i;
	if (len == 0)
		{
		*frd = NULL;
		*pos = NULL;
		*nofrd = 0;
		}
	else
        {
        *nofrd=(len-4)/6;
        *pos = (U32 *) malloc( (*nofrd+1) * sizeof(U32));
        if (*pos == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",(*nofrd+1) * sizeof(U32)));
            return(1);
            }

        *frd = (FRD *) malloc(*nofrd * sizeof(FRD));
        if (*frd == NULL)
            {
            wvError(("NO MEM 1, failed to alloc %d bytes\n",*nofrd * sizeof(FRD)));
			free(pos);
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<*nofrd+1;i++)
            (*pos)[i]=read_32ubit(fd);
        for(i=0;i<*nofrd;i++)
            wvGetFRD(&((*frd)[i]),fd);
        }
	return(0);
	}
#endif
