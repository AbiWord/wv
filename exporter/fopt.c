#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wvexporter-priv.h"

void wvPutFAnchor(FAnchor *item,wvStream *fd)
         {
         /* It is supposed to be a RECT, but its only 4 long so... */
    	 write_8ubit(fd, (U8)item->left);
    	 write_8ubit(fd, (U8)item->right);
    	 write_8ubit(fd, (U8)item->top);
    	 write_8ubit(fd, (U8)item->bottom);
         }

void wvPutFOPTE(FOPTE *afopte,wvStream *fd)
     {
     U16 dtemp = (U16)0;

	 dtemp |= afopte->pid;
	 dtemp |= afopte->fBid << 14;
	 dtemp |= afopte->fComplex << 15;

	 write_16ubit(fd, dtemp);
	 write_32ubit(fd, afopte->op);
     }

/* TODO: check to make sure that this is correct */

void wvPutFOPTEArray(FOPTE **fopte,MSOFBH *msofbh,wvStream *fd)
         {
         U32 i,j,count=0;
         U32 no=0;

         while (count < msofbh->cbLength)
                 {
                 count += 4;
                 wvPutFOPTE(&(*fopte)[no],fd);
                 no++;
                 }

         for(i=0;i<no;i++)
                 {
                 if ( (*fopte)[i].fComplex )
                         for(j=0;j<(*fopte)[i].op;j++)
                           write_8ubit(fd, (*fopte)[i].entry[j]);
                 }
         }
