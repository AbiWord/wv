#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
At offset
511 is a 1-byte count named crun, which is a count of paragraphs in PAPX 
FKPs. Beginning at offset 0 of the FKP is an array of crun+1 FCs, named 
rgfc, which records the beginning and limit FCs of crun paragraphs.

immediately following the fkp.rgfc is an array of 13 byte
entries called BXs. This array called the rgbx is in 1-to-1 correspondence
with the rgfc. The first byte of the ith BX entry contains a single byte
field which gives the word offset of the PAPX that belongs to the paragraph
whose beginning in FC space is rgfc[i] and whose limit is rgfc[i+1] in FC
space. The last 12 bytes of the ith BX entry contain a PHE structure that
stores the current paragraph height of the paragraph whose beginning in FC
space is rgfc[i] and whose limit is rgfc[i+1] in FC space.
*/

/*
The first byte of each BX is the word offset of the PAPX recorded for
the paragraph corresponding to this BX. .. If the byte stored is 0,
this represents a 1 line paragraph 15 pixels high with Normal style
(stc == 0) whose column width is 7980 dxas. The last 12 bytes of
the BX is a PHE structure which stores the current paragraph height
for the paragraph corresponding to the BX. If a plcfphe has an entry 
that maps to the FC for this paragraph, that entry's PHE overrides the PHE
stored in the FKP.11*fkp.crun+4 unused space. As new runs/paragraphs
are recorded in the FKP, unused space is reduced by 17 if CHPX/PAPX
is already recorded and is reduced by 17+sizeof(PAPX) if property is not
already recorded.
*/
void wvGetPAPX_FKP(int version,PAPX_FKP *fkp,U32 pn,FILE *fd)
	{
	int i;
	wvTrace(("papx fkp malloc\n"));

	fseek(fd,pn*PAGESIZE+(PAGESIZE-1),SEEK_SET);
	fkp->crun = getc(fd);
	fkp->rgfc = (U32 *)malloc(sizeof(U32) * (fkp->crun+1));
	fkp->rgbx = (BX *)malloc(sizeof(BX) * (fkp->crun));
	fkp->grppapx = (PAPX *)malloc(sizeof(PAPX) * (fkp->crun));
	fseek(fd,pn*PAGESIZE,SEEK_SET);
	for (i=0;i<fkp->crun+1;i++)
		{
		fkp->rgfc[i] = read_32ubit(fd);
		wvTrace(("rgfc is %x\n",fkp->rgfc[i]));
		}

	for (i=0;i<fkp->crun;i++)
		{
		if (version == 0)
			wvGetBX(&fkp->rgbx[i],fd);
		else 
			wvGetBX6(&fkp->rgbx[i],fd);
		}

	for (i=0;i<fkp->crun;i++)
		{
		if (fkp->rgbx[i].offset == 0)
			{
			wvTrace(("i is %d, using clear papx\n"));
			wvInitPAPX(&(fkp->grppapx[i]));
			}
		else
			{
			wvTrace(("papx index i is %d, offset is %x\n",i,pn*PAGESIZE+fkp->rgbx[i].offset*2));
			wvGetPAPX(version,&(fkp->grppapx[i]),pn*PAGESIZE+fkp->rgbx[i].offset*2,fd);
			}
		}
	}

/*
Using the FC, search the FCs FKP for the largest FC less than the character's FC,
    call it fcTest.
*/
U32 wvSearchNextLargestFCPAPX_FKP(PAPX_FKP *fkp,U32 currentfc)
	{
	U32 i=0;
	U8 until=fkp->crun+1;
	U32 fcTest=0;

	while (i<until)
		{
		if ( (wvNormFC(fkp->rgfc[i],NULL) < currentfc) && (wvNormFC(fkp->rgfc[i],NULL) > fcTest) )
			fcTest = wvNormFC(fkp->rgfc[i],NULL);
		else if (wvNormFC(fkp->rgfc[i],NULL) == currentfc)
			fcTest = currentfc+1;
		i++;
		}

	/*for the first paragraph return the current pos as the beginning*/
	/*
	if (fcTest == 0)
		fcTest = currentfc+1;
	*/
		
	return(fcTest);
	}

/*
Using the FC of the character, first search the FKP that describes the
character to find the smallest FC in the rgfc that is larger than the character
FC.
*/
U32 wvSearchNextSmallestFCPAPX_FKP(PAPX_FKP *fkp,U32 currentfc)
	{
	U32 i=0;
	U32 fcTest=0xffffffffL;
	U8 until=fkp->crun+1;
	
	while (i<until)
		{
		wvTrace(("Smallest %x, %x %x\n",currentfc,wvNormFC(fkp->rgfc[i],NULL),wvNormFC(fkp->rgfc[i],NULL)));
		if ( (wvNormFC(fkp->rgfc[i],NULL) > currentfc) && (wvNormFC(fkp->rgfc[i],NULL) < fcTest) )
			fcTest = wvNormFC(fkp->rgfc[i],NULL);
		i++;
		}
	return(fcTest);
	}

void wvReleasePAPX_FKP(PAPX_FKP *fkp)
	{
	int i;
	wvTrace(("papx fkp b freeed\n"));
	wvFree(fkp->rgfc);
	fkp->rgfc=NULL;
	wvFree(fkp->rgbx);
	fkp->rgbx=NULL;
	for (i=0;i<fkp->crun;i++)
		wvReleasePAPX(&(fkp->grppapx[i]));
	fkp->crun=0;
	wvFree(fkp->grppapx);
	fkp->grppapx=NULL;
	wvTrace(("papx fkp e freeed\n"));
	}

void wvInitPAPX_FKP(PAPX_FKP *fkp)
	{
	fkp->rgfc=NULL;
	fkp->rgbx=NULL;
	fkp->crun=0;
	fkp->grppapx=NULL;
	}

int wvGetIndexFCInFKP_PAPX(PAPX_FKP *fkp,U32 currentfc)
	{
	U32 i=1;	/*was 0, there is something slightly out of sync in the system*/
	U8 until=fkp->crun+1;
	
	while (i<until)
		{
		wvTrace(("current fc is %x, %x\n",currentfc,wvNormFC(fkp->rgfc[i],NULL)));
		if (wvNormFC(fkp->rgfc[i],NULL) == currentfc)
			return(i);
		i++;
		}
	/*
	 basically read 
	 Algorithm to determine paragraph properties for a paragraph &
	 Formatted Disk Page for PAPXs, somehow the currentfc sent in was wrong
	 or my understanding is !
	 */
	wvError(("Shite, fix me\n"));
	/*return 1 to make things continue on their merry way*/
	return(1);
	}


/* Character properties 
 * -basically just like PAPX FKPs above
 * however, rather than an array of BX structs in rgbx,
 * there is an array of bytes (giving the word offset to the CHPX) in rgb
 * -JB
 */

void wvGetCHPX_FKP(int version, CHPX_FKP *fkp, U32 pn, FILE *fd)
	{
	int i;
	wvTrace(("chpx fkp malloc\n"));

	fseek(fd, pn*PAGESIZE+(PAGESIZE-1), SEEK_SET);
	fkp->crun = getc(fd);
	fkp->rgfc = (U32 *)malloc(sizeof(U32) * (fkp->crun+1));
	fkp->rgb = (U8 *)malloc(sizeof(U8) * (fkp->crun));
	fkp->grpchpx = (CHPX *)malloc(sizeof(CHPX) * (fkp->crun));
	fseek(fd, pn*PAGESIZE, SEEK_SET);
	for (i=0;i<fkp->crun+1;i++)
		{
		fkp->rgfc[i] = read_32ubit(fd);
		wvTrace(("rgfc is %x\n",fkp->rgfc[i]));
		}

	for (i=0;i<fkp->crun;i++)
		{
		fkp->rgb[i] = getc(fd);
		}

	for (i=0;i<fkp->crun;i++)
		{
		if (fkp->rgb[i] == 0)
		     {
			wvTrace(("i is %d, using clear chpx\n"));
			wvInitCHPX(&(fkp->grpchpx[i]));
		     }
		else
		     {
			wvTrace(("chpx index i is %d, offset is %x\n", i,
				(pn * PAGESIZE) + (fkp->rgb[i] * 2)));
			wvGetCHPX(version, &(fkp->grpchpx[i]), 
				  (pn*PAGESIZE) + (fkp->rgb[i] * 2), fd);
			}
		}
	}

void wvReleaseCHPX_FKP(CHPX_FKP *fkp)
	{
	int i;
	wvTrace(("chpx fkp b freeed\n"));
	wvFree(fkp->rgfc);
	fkp->rgfc=NULL;
	wvFree(fkp->rgb);
	fkp->rgb=NULL;
	for (i=0;i<fkp->crun;i++)
		wvReleaseCHPX(&(fkp->grpchpx[i]));
	fkp->crun=0;
	wvFree(fkp->grpchpx);
	fkp->grpchpx=NULL;
	wvTrace(("chpx fkp e freeed\n"));
	}

void wvInitCHPX_FKP(CHPX_FKP *fkp)
	{
	fkp->rgfc=NULL;
	fkp->rgb=NULL;
	fkp->crun=0;
	fkp->grpchpx=NULL;
	}

