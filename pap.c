#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"

/*
To apply a UPX.papx to a UPE.pap, set UPE.pap.istd equal to UPX.papx.istd, and
then apply the UPX.papx.grpprl to UPE.pap.
*/
void
wvAddPAPXFromBucket (PAP * apap, UPXF * upxf, STSH * stsh, wvStream * data)
{
    U8 *pointer;
    U16 i = 0;
    U16 sprm;
    apap->istd = upxf->upx.papx.istd;
    if (upxf->cbUPX <= 2)
	return;
    wvTrace (("no is %d\n", upxf->cbUPX));
#ifdef SPRMTEST
    fprintf (stderr, "\n");
    while (i < upxf->cbUPX - 2)
      {
	  fprintf (stderr, "%x (%d) ", *(upxf->upx.papx.grpprl + i),
		   *(upxf->upx.papx.grpprl + i));
	  i++;
      }
    fprintf (stderr, "\n");
    i = 0;
#endif
    /*
       while (i < upxf->cbUPX-2)
     */
    while (i < upxf->cbUPX - 4)	/* the end of the list is at -2, but there has to be a full sprm of
				   len 2 as well */
      {
	  sprm = bread_16ubit (upxf->upx.papx.grpprl + i, &i);
#ifdef SPRMTEST
	  wvError (("sprm is %x\n", sprm));
#endif
	  pointer = upxf->upx.papx.grpprl + i;
	  if (i < upxf->cbUPX - 2)
	      wvApplySprmFromBucket (WORD8, sprm, apap, NULL, NULL, stsh,
				     pointer, &i, data);
      }
}

void
wvAddPAPXFromBucket6 (PAP * apap, UPXF * upxf, STSH * stsh)
{
    U8 *pointer;
    U16 i = 0;
    U16 sprm;
    U8 sprm8;
    apap->istd = upxf->upx.papx.istd;
    if (upxf->cbUPX <= 2)
	return;
    wvTrace (("no is %d\n", upxf->cbUPX));

#ifdef SPRMTEST
    fprintf (stderr, "\n");
    while (i < upxf->cbUPX - 2)
      {
	  fprintf (stderr, "%x (%d) ", *(upxf->upx.papx.grpprl + i),
		   *(upxf->upx.papx.grpprl + i));
	  i++;
      }
    fprintf (stderr, "\n");
    i = 0;
#endif

    while (i < upxf->cbUPX - 3)	/* the end of the list is at -2, but there has to be a full sprm of
				   len 1 as well */
      {
	  sprm8 = bread_8ubit (upxf->upx.papx.grpprl + i, &i);
#ifdef SPRMTEST
	  wvError (("pap word 6 sprm is %x (%d)\n", sprm8, sprm8));
#endif
	  sprm = (U16) wvGetrgsprmWord6 (sprm8);
#ifdef SPRMTEST
	  wvError (("pap word 6 sprm is converted to %x\n", sprm));
#endif
	  pointer = upxf->upx.papx.grpprl + i;
	  /* hmm, maybe im wrong here, but there appears to be corrupt
	   * word 6 sprm lists being stored in the file
	   */
	  if (i < upxf->cbUPX - 2)
	      wvApplySprmFromBucket (WORD6, sprm, apap, NULL, NULL, stsh,
				     pointer, &i, NULL);
      }
}


void
wvInitPAPFromIstd (PAP * apap, U16 istdBase, STSH * stsh)
{
    if (istdBase == istdNil)
	wvInitPAP (apap);
    else
      {
	  if (istdBase >= stsh->Stshi.cstd)
	    {
		wvError (
			 ("ISTD out of bounds, requested %d of %d\n",
			  istdBase, stsh->Stshi.cstd));
		wvInitPAP (apap);	/*it can't hurt to try and start with a blank istd */
		return;
	    }
	  else
	    {
		if (stsh->std[istdBase].cupx == 0)	/*empty slot in the array, i don't think this should happen */
		  {
		      wvTrace (("Empty style slot used (chp)\n"));
		      wvInitPAP (apap);
		  }
		else
		  {
		    wvCopyPAP (apap, &(stsh->std[istdBase].grupe[0].apap));
		    strncpy(apap->stylename,stsh->std[istdBase].xstzName, sizeof(apap->stylename));
		  }
	    }
      }
}

void
wvCopyPAP (PAP * dest, PAP * src)
{
    memcpy (dest, src, sizeof (PAP));
}


void
wvInitPAP (PAP * item)
{
    int i;
    item->istd = 0;
    item->jc = 0;
    item->fKeep = 0;
    item->fKeepFollow = 0;
    item->fPageBreakBefore = 0;
    item->fBrLnAbove = 0;
    item->fBrLnBelow = 0;
    item->fUnused = 0;
    item->pcVert = 0;
    item->pcHorz = 0;
    item->brcp = 0;
    item->brcl = 0;
    item->reserved1 = 0;
    item->ilvl = 0;
    item->fNoLnn = 0;
    item->ilfo = 0;
    item->nLvlAnm = 0;
    item->reserved2 = 0;
    item->fSideBySide = 0;
    item->reserved3 = 0;
    item->fNoAutoHyph = 0;
    item->fWidowControl = 1;
    item->dxaRight = 0;
    item->dxaLeft = 0;
    item->dxaLeft1 = 0;
    /*
       wvInitLSPD(&item->lspd);
     */
    item->lspd.fMultLinespace = 1;
    item->lspd.dyaLine = 240;

    item->dyaBefore = 0;
    item->dyaAfter = 0;

    wvInitPHE (&item->phe, 0);

    item->fCrLf = 0;
    item->fUsePgsuSettings = 0;
    item->fAdjustRight = 0;
    item->reserved4 = 0;
    item->fKinsoku = 0;
    item->fWordWrap = 0;
    item->fOverflowPunct = 0;
    item->fTopLinePunct = 0;
    item->fAutoSpaceDE = 0;
    item->fAtuoSpaceDN = 0;
    item->wAlignFont = 0;
    item->fVertical = 0;
    item->fBackward = 0;
    item->fRotateFont = 0;
    item->reserved5 = 0;
    item->reserved6 = 0;
    item->fInTable = 0;
    item->fTtp = 0;
    item->wr = 0;
    item->fLocked = 0;

    wvInitTAP (&item->ptap);

    item->dxaAbs = 0;
    item->dyaAbs = 0;
    item->dxaWidth = 0;

    wvInitBRC (&item->brcTop);
    wvInitBRC (&item->brcLeft);
    wvInitBRC (&item->brcBottom);
    wvInitBRC (&item->brcRight);
    wvInitBRC (&item->brcBetween);
    wvInitBRC (&item->brcBar);

    item->dxaFromText = 0;
    item->dyaFromText = 0;
    item->dyaHeight = 0;
    item->fMinHeight = 0;

    wvInitSHD (&item->shd);
    wvInitDCS (&item->dcs);
    item->lvl = 9;
    item->fNumRMIns = 0;
    wvInitANLD (&item->anld);
    item->fPropRMark = 0;
    item->ibstPropRMark = 0;
    wvInitDTTM (&item->dttmPropRMark);
    wvInitNUMRM (&item->numrm);
    item->itbdMac = 0;
    for (i = 0; i < itbdMax; i++)
	item->rgdxaTab[i] = 0;
    for (i = 0; i < itbdMax; i++)
	wvInitTBD (&item->rgtbd[i]);

    item->fBidi = 0;
}

/*
1) Having found the index i of the FC in an FKP that marks the character stored
in the file immediately after the paragraph's paragraph mark,

1 is done in Simple mode through wvGetSimpleParaBounds which places this index
in fcLim by default

2) it is necessary to use the word offset stored in the first byte of the
fkp.rgbx[i - 1] to find the PAPX for the paragraph.

3) Using papx.istd to index into the properties stored for the style sheet ,

4) the paragraph properties of the style are copied to a local PAP.

5) Then the grpprl stored in the PAPX is applied to the local PAP,

6) and papx.istd along with fkp.rgbx.phe are moved into the local PAP.

7) The process thus far has created a PAP that describes what the paragraph properties
of the paragraph were at the last full save.
*/

int
wvAssembleSimplePAP (wvVersion ver, PAP * apap, U32 fc, PAPX_FKP * fkp,
		     STSH * stsh, wvStream * data)
{
    PAPX *papx;
    int index;
    UPXF upxf;
    int ret = 0;
#ifdef SPRMTEST
    int i;
#endif
    /*index is the i in the text above */
    index = wvGetIndexFCInFKP_PAPX (fkp, fc);

    wvTrace (("index is %d, using %d\n", index, index - 1));
    papx = &(fkp->grppapx[index - 1]);

    if (papx)
      {
	  wvTrace (("istd index is %d\n", papx->istd));
	  wvInitPAPFromIstd (apap, papx->istd, stsh);
      }
    else
	wvInitPAPFromIstd (apap, istdNil, stsh);

    if ((papx) && (papx->cb > 2))
      {
	  ret = 1;
#ifdef SPRMTEST
	  fprintf (stderr, "cbUPX is %d\n", papx->cb);
	  for (i = 0; i < papx->cb - 2; i++)
	      fprintf (stderr, "%x ", papx->grpprl[i]);
	  fprintf (stderr, "\n");
#endif
	  upxf.cbUPX = papx->cb;
	  upxf.upx.papx.istd = papx->istd;
	  upxf.upx.papx.grpprl = papx->grpprl;
	  if (ver == WORD8)
	      wvAddPAPXFromBucket (apap, &upxf, stsh, data);
	  else
	      wvAddPAPXFromBucket6 (apap, &upxf, stsh);
      }

    if (papx)
	apap->istd = papx->istd;

    if (fkp->rgbx != NULL)
      wvCopyPHE (&apap->phe, &(fkp->rgbx[index - 1].phe), apap->fTtp);
    return (ret);
}

void
wvReleasePAPX (PAPX * item)
{
    item->cb = 0;
    item->istd = 0;
    wvFree (item->grpprl);
    item->grpprl = NULL;
}

void
wvInitPAPX (PAPX * item)
{
    item->cb = 0;
    item->istd = 0;
    item->grpprl = NULL;
}

void
wvGetPAPX (wvVersion ver, PAPX * item, U8 * page, U16 * pos)
{
    U16 cw;
    cw = bread_8ubit (&(page[*pos]), pos);
    if ((cw == 0) && (ver == WORD8))	/* only do this for word 97 */
      {
	  wvTrace (("cw was pad %d\n", cw));
	  cw = bread_8ubit (&(page[*pos]), pos);
	  wvTrace (("cw was %d\n", cw));
      }
    item->cb = cw * 2;
    item->istd = bread_16ubit (&(page[*pos]), pos);
    wvTrace (("papx istd is %x\n", item->istd));
    wvTrace (("no of bytes is %d\n", item->cb));
    if (item->cb > 2)
      {
	  item->grpprl = (U8 *) malloc (item->cb - 2);
	  memcpy (item->grpprl, &(page[*pos]), (item->cb) - 2);
      }
    else
	item->grpprl = NULL;
}


int
isPAPConform (PAP * current, PAP * previous)
{
    if ((current) && (previous))
	if (wvEqualBRC (&current->brcLeft, &previous->brcLeft))
	    if (wvEqualBRC (&current->brcRight, &previous->brcRight))
		if (current->dxaWidth == previous->dxaWidth)
		    if (current->fInTable == previous->fInTable)
			return (1);
    return (0);
}




void
wvCopyConformPAP (PAP * dest, PAP * src)
{
    if (src)
      {
#ifdef PURIFY
	  wvInitPAP (dest);
#endif
	  dest->brcLeft = src->brcLeft;
	  dest->brcRight = src->brcRight;
	  dest->dxaWidth = src->dxaWidth;
	  dest->fInTable = src->fInTable;
      }
    else
	wvInitPAP (dest);
}
