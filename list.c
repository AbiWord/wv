#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "wv.h"

int wvIsListEntry(PAP *apap,int version)
	{
	if (version == 0)
		{
		if (apap->ilfo) 
			return(1);
		else
			return(0);
		}
	else
		{
		if (apap->nLvlAnm)
			{
			wvError(("old style pap, thats a list\n"));
			return(1);
			}
		else
			return(0);
		}
	return(0);
	}

/*
Paragraph List Formatting

Given a paragraph and its corresponding PAP, the following process must be
followed to find out the paragraph's list information:

   * Using the pap.ilfo, look up the LFO record in the pllfo with that
     (1-based) index.

   * Using the LFO, and the pap.ilvl, check to see if there are any
     overrides for this particular level. If so, and if the override
     pertains to both formatting and start-at value, use the LVL record from
     the correct LFOLVL in the LFO, and skip to step 5.

   * If the override does not pertain to either formatting or start-at
     value, we must look up the LST for this list. Using the LFO's List ID,
     search the rglst for the LST with that List ID.

   * Now, take from this LST any information (formatting or start-at value)
     we still need after consulting the LFO.

   * Once we've got the correct LVL record, apply the lvl.grpprlPapx to the
     PAP. It may adjust the indents and tab settings for the paragraph.

   * Use the other information in the LVL, such as the start at, number
     text, and grpprlChpx, to determine the appearance of the actual
     paragraph number text.
*/
int wvGetListEntryInfo(U32 **nos,LVL *retlvl,LFO **retlfo,PAP *apap,LFO **lfo,LFOLVL *lfolvl,LVL *lvl,U32 *nolfo, LST *lst, U32 noofLST,int version)
	{
	LST *alst=NULL;
	int i,number=0;

	if ( (apap->ilfo == 2047) || (version != 0) )
		{
		/*word 6 anld, parse that instead*/
		retlvl->lvlf.iStartAt = apap->anld.iStartAt;
		retlvl->lvlf.nfc = apap->anld.nfc;
		retlvl->lvlf.jc = apap->anld.jc;
		retlvl->lvlf.fLegal = 0;	/*?*/
		retlvl->lvlf.fNoRestart = 1;	/*?*/
		retlvl->lvlf.fPrev = apap->anld.fPrev;
		retlvl->lvlf.fPrevSpace = apap->anld.fPrevSpace;
		retlvl->lvlf.fWord6 = 1;
		retlvl->lvlf.rgbxchNums[0] = 0;	/*wrong for now*/
		retlvl->lvlf.ixchFollow = 2;	/*wrong for now*/
		retlvl->lvlf.dxaSpace = apap->anld.dxaSpace;
		retlvl->lvlf.dxaIndent = apap->anld.dxaIndent;
		retlvl->lvlf.cbGrpprlChpx = 0;	/* wrong */
		retlvl->lvlf.cbGrpprlPapx = 0;	/* wrong */
		retlvl->lvlf.reserved1 = 0;
		retlvl->lvlf.reserved2 = 0;
		retlvl->grpprlChpx = NULL;	/* wrong */
		retlvl->grpprlPapx = NULL;	/* wrong */
		retlvl->numbertext = NULL;	/* wrong */

		/*
		realloc the lfo list to be one bigger, 
		*/
		*nolfo++;
		*lfo = (LFO *)realloc(*lfo,sizeof(LFO) * (*nolfo));
		*nos = (U32 *)realloc(*nos,sizeof(U32) * 9 * (*nolfo));
		apap->ilfo = *nolfo;
		*retlfo = &((*lfo)[apap->ilfo-1]);
		apap->ilvl = 0;	/* i dunno yet */

		/*
		and set the ilfo and ilvl of the list to point to that fake entry instead
		and we'll have to repeat the procedure for the liststartnos
		*/
		return(0);
		}
	else if (apap->ilfo == 0) 
		{
		/* no number */
		return(0);
		}
	if (apap->ilfo > *nolfo) 
		{
		wvWarning("ilfo no %d, is greater than the number of existing lfo's (%d)\n",apap->ilfo,*nolfo);
		return(1);
		}

	/*
	Using the pap.ilfo, look up the LFO record in the pllfo with that
    (1-based) index. == (*lfo)[apap->ilfo]
	*/

	*retlfo = &((*lfo)[apap->ilfo-1]);

	if ((*lfo)[apap->ilfo-1].clfolvl)
		{
   		/* 
		Using the LFO, and the pap.ilvl, check to see if there are any 
		overrides for this particular level. If so, and if the override
     	pertains to both formatting and start-at value, use the LVL record 
		from the correct LFOLVL in the LFO, and skip to step 5.
		*/
		wvTrace(("some overridden levels\n"));

		/* 
		there are some levels overridden, find out if the level being overridden
		is the same as the level the paragraph wants
		*/
		for (i=0;i<apap->ilfo-1;i++)
			number+=(*lfo)[i].clfolvl;

		for(i=0;i<(*lfo)[apap->ilfo-1].clfolvl;i++)
			{
			if (lfolvl[i+number].ilvl == apap->ilvl)
				{
				/* the requested level is overridden */
				if ((lfolvl[i+number].fFormatting) && (lfolvl[i+number].fStartAt))
					{
					/*save the existing lvl and swap in this new one instead*/
					alst = wvSearchLST((*lfo)[apap->ilfo-1].lsid,lst,noofLST);

					/*use the LVL record from the correct LFOLVL in the LFO*/
					wvCopyLVL(retlvl,&(lvl[i+number]));
					}
				else if (lfolvl[i+number].fStartAt)
					{
					alst = wvSearchLST((*lfo)[apap->ilfo-1].lsid,lst,noofLST);

					/* the lvl is the standard one with a new startat value */
					wvCopyLVL(retlvl,&(alst->lvl[apap->ilvl]));
					retlvl->lvlf.iStartAt = lfolvl[i+number].iStartAt;
					}
				else if (lfolvl[i+number].fFormatting)
					{
					alst = wvSearchLST((*lfo)[apap->ilfo-1].lsid,lst,noofLST);

					/* the lvl is the overridden one, with the original startat */
					wvCopyLVL(retlvl,&(lvl[i+number]));
					retlvl->lvlf.iStartAt = alst->lvl[apap->ilvl].lvlf.iStartAt;
					}
				}
			}
		}		
	
	if (alst == NULL)
		{
		/* 
		if there no overridden levels i assume that we 
		search for the appropiate LST 
		*/
		wvTrace(("no overridden levels, searching using val %d %x\n",apap->ilfo-1,(*lfo)[apap->ilfo-1].lsid));
		alst = wvSearchLST((*lfo)[apap->ilfo-1].lsid,lst,noofLST);
		wvCopyLVL(retlvl,&(alst->lvl[apap->ilvl]));
		wvTrace(("string len is %d",retlvl->numbertext[0]));
		wvTrace(("offset is %d\n",retlvl->lvlf.rgbxchNums[0]));
		}

	if (alst == NULL)
		{
		wvError(("No LST found for list\n"));
		return(1);
		}

	return(0);
	}
