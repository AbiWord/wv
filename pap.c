#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
To apply a UPX.papx to a UPE.pap, set UPE.pap.istd equal to UPX.papx.istd, and 
then apply the UPX.papx.grpprl to UPE.pap.
*/
void wvAddPAPXFromBucket(PAP *apap,UPXF *upxf,STSH *stsh)
	{
	U8 *pointer;
	U16 i=0;
	U16 sprm;
	apap->istd = upxf->upx.papx.istd;
	if (upxf->cbUPX <= 2)
		return;
	wvTrace(("no is %d\n",upxf->cbUPX));
#if 0	
	fprintf(stderr,"\n");
	while (i < upxf->cbUPX-2)
		{
		fprintf(stderr,"%x (%d)\n",*(upxf->upx.papx.grpprl+i),*(upxf->upx.papx.grpprl+i));
		i++;
		}
	fprintf(stderr,"\n");
	i=0;
#endif

	/*
	while (i < upxf->cbUPX-2)	
	*/
	while (i < upxf->cbUPX-4)	/* the end of the list is at -2, but there has to be a full sprm of
								 len 2 as well*/
		{
		sprm = bread_16ubit(upxf->upx.papx.grpprl+i,&i);
		wvTrace(("sprm is %x\n",sprm));
		pointer = upxf->upx.papx.grpprl+i;
		if (i < upxf->cbUPX-2)
			wvApplySprmFromBucket(0,sprm,apap,NULL,NULL,stsh,pointer,&i);
		}
	}

void wvAddPAPXFromBucket6(PAP *apap,UPXF *upxf,STSH *stsh)
	{
	U8 *pointer;
	U16 i=0;
	U16 sprm;
	U8 sprm8;
	apap->istd = upxf->upx.papx.istd;
	if (upxf->cbUPX <= 2)
		return;
	wvTrace(("no is %d\n",upxf->cbUPX));

	i=0;
	while (i < upxf->cbUPX-3)	/* the end of the list is at -2, but there has to be a full sprm of
								 len 1 as well*/
		{
		sprm8 = bgetc(upxf->upx.papx.grpprl+i,&i);
		wvTrace(("pap word 6 sprm is %x (%d)\n",sprm8,sprm8));
		sprm = (U16)wvGetrgsprmWord6(sprm8);
		wvTrace(("pap word 6 sprm is converted to %x\n",sprm));
		pointer = upxf->upx.papx.grpprl+i;
		/* hmm, maybe im wrong here, but there appears to be corrupt 
		 * word 6 sprm lists being stored in the file
		 */
		if (i < upxf->cbUPX-2)
			wvApplySprmFromBucket(1,sprm,apap,NULL,NULL,stsh,pointer,&i);
		}
	}


void wvInitPAPFromIstd(PAP *apap,U16 istdBase,STSH *stsh)
	{
	if (istdBase == istdNil)
		wvInitPAP(apap);
	else 
		{
		if (istdBase >= stsh->Stshi.cstd)
			{
			wvError(("ISTD out of bounds, requested %d of %d\n",istdBase,stsh->Stshi.cstd));
			wvInitPAP(apap);	/*it can't hurt to try and start with a blank istd*/
			return;
			}
		else
			{
			if (stsh->std[istdBase].cupx==0)	/*empty slot in the array, i don't think this should happen*/
				{
				wvTrace(("Empty style slot used (chp)\n"));
				wvInitPAP(apap);
				}
			else
				wvCopyPAP(apap,&(stsh->std[istdBase].grupe[0].apap));
			}
		}
	}

void wvCopyPAP(PAP *dest,PAP *src)
	{
	int i;
	dest->istd = src->istd ;		
	dest->jc  = src->jc ;		
	dest->fKeep  = src->fKeep ;	
	dest->fKeepFollow  = src->fKeepFollow ;
	dest->fPageBreakBefore = src->fPageBreakBefore ;
	dest->fBrLnAbove = src->fBrLnAbove;
	dest->fBrLnBelow = src->fBrLnBelow ;
	dest->fUnused = src->fUnused ;	
	dest->pcVert = src->pcVert ;
	dest->pcHorz =src->pcHorz ;
	dest->brcp = src->brcp ;
	dest->brcl = src->brcl ;
	dest->reserved1 = src->reserved1 ;				
	dest->ilvl = src->ilvl ;				
	dest->fNoLnn = src->fNoLnn ;			
	dest->ilfo = src->ilfo ;		
	dest->nLvlAnm = src->nLvlAnm ;
	dest->reserved2 = src->reserved2 ;			
	dest->fSideBySide = src->fSideBySide ;	
	dest->reserved3 = src->reserved3 ;	
	dest->fNoAutoHyph = src->fNoAutoHyph ;				
	dest->fWidowControl = src->fWidowControl ;			
	dest->dxaRight = src->dxaRight ;			
	dest->dxaLeft = src->dxaLeft ;		
	dest->dxaLeft1 = src->dxaLeft1 ;	

	wvCopyLSPD(&dest->lspd,&src->lspd);

	dest->dyaBefore = src->dyaBefore ;				
	dest->dyaAfter = src->dyaAfter ;			

	wvCopyPHE(&dest->phe,&src->phe,src->fTtp);

	dest->fCrLf = src->fCrLf ;		
	dest->fUsePgsuSettings = src->fUsePgsuSettings ;			
	dest->fAdjustRight = src->fAdjustRight ;			
	dest->reserved4 = src->reserved4 ;			
	dest->fKinsoku = src->fKinsoku ;		
	dest->fWordWrap = src->fWordWrap ;	
	dest->fOverflowPunct = src->fOverflowPunct ;			
	dest->fTopLinePunct = src->fTopLinePunct ;		
	dest->fAutoSpaceDE = src->fAutoSpaceDE ;	
	dest->fAtuoSpaceDN = src->fAtuoSpaceDN ;
	dest->wAlignFont = src->wAlignFont ;				
	dest->fVertical = src->fVertical ;			
	dest->fBackward = src->fBackward ;		
	dest->fRotateFont = src->fRotateFont ;		
	dest->reserved5 = src->reserved5;		
	dest->reserved6 = src->reserved6 ;		
	dest->fInTable = src->fInTable ;	
	dest->fTtp = src->fTtp ;	
	dest->wr = src->wr ;	
	dest->fLocked = src->fLocked ;		

	wvCopyTAP(&dest->ptap,&src->ptap) ;

	dest->dxaAbs = src->dxaAbs ;
	dest->dyaAbs = src->dyaAbs ;			
	dest->dxaWidth = src->dxaWidth ;			

	wvCopyBRC(&dest->brcTop,&src->brcTop);
	wvCopyBRC(&dest->brcLeft,&src->brcLeft);
	wvCopyBRC(&dest->brcBottom,&src->brcBottom);
	wvCopyBRC(&dest->brcRight,&src->brcRight);
	wvCopyBRC(&dest->brcBetween,&src->brcBetween);
	wvCopyBRC(&dest->brcBar,&src->brcBetween);

	dest->dxaFromText = src->dxaFromText;
	dest->dyaFromText = src->dyaFromText;
	dest->dyaHeight = src->dyaHeight;
	dest->fMinHeight = src->fMinHeight;

	wvCopySHD(&dest->shd,&src->shd);
	wvCopyDCS(&dest->dcs,&src->dcs);
	dest->lvl = src->lvl;
	dest->fNumRMIns = src->fNumRMIns;
	wvCopyANLD(&dest->anld,&src->anld);
	dest->fPropRMark  = src->fPropRMark ;
	dest->ibstPropRMark  = src->ibstPropRMark ;				
	wvCopyDTTM(&dest->dttmPropRMark,&src->dttmPropRMark);
	wvCopyNUMRM(&dest->numrm,&src->numrm);	
	dest->itbdMac = src->itbdMac;
	for (i=0;i<itbdMax;i++)
		dest->rgdxaTab[i] = src->rgdxaTab[i];
	for (i=0;i<itbdMax;i++)
		wvCopyTBD(&dest->rgtbd[i],&src->rgtbd[i]);
	}


void wvInitPAP(PAP *item)
	{
	int i;
	item->istd = 0;
	item->jc  = 0;
	item->fKeep  = 0;
	item->fKeepFollow  = 0;
	item->fPageBreakBefore = 0;
	item->fBrLnAbove = 0;
	item->fBrLnBelow = 0;
	item->fUnused = 0;
	item->pcVert = 0;
	item->pcHorz =0;
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
	item->fWidowControl = 0;
	item->dxaRight = 0;
	item->dxaLeft = 0;
	item->dxaLeft1 = 0;

	wvInitLSPD(&item->lspd);

	item->dyaBefore = 0;
	item->dyaAfter = 0;

	wvInitPHE(&item->phe,0);

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

	wvInitTAP(&item->ptap);

	item->dxaAbs = 0;
	item->dyaAbs = 0;
	item->dxaWidth = 0;

	wvInitBRC(&item->brcTop);
	wvInitBRC(&item->brcLeft);
	wvInitBRC(&item->brcBottom);
	wvInitBRC(&item->brcRight);
	wvInitBRC(&item->brcBetween);
	wvInitBRC(&item->brcBar);

	item->dxaFromText = 0;
	item->dyaFromText = 0;
	item->dyaHeight = 0;
	item->fMinHeight = 0;

	wvInitSHD(&item->shd);
	wvInitDCS(&item->dcs);
	item->lvl = 0;
	item->fNumRMIns = 0;
	wvInitANLD(&item->anld);
	item->fPropRMark  = 0;
	item->ibstPropRMark  = 0;
	wvInitDTTM(&item->dttmPropRMark);
	wvInitNUMRM(&item->numrm);	
	item->itbdMac = 0;
	for (i=0;i<itbdMax;i++)
		item->rgdxaTab[i] = 0;
	for (i=0;i<itbdMax;i++)
		wvInitTBD(&item->rgtbd[i]);
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

int wvAssembleSimplePAP(int version,PAP *apap,U32 fc,PAPX_FKP *fkp,STSH *stsh)
	{
	PAPX *papx;
	int index,i;
	UPXF upxf;
	int ret=0;
	/*index is the i in the text above*/
	index = wvGetIndexFCInFKP_PAPX(fkp,fc);
	
	wvTrace(("index is %d, using %d\n",index,index-1));
	papx = &(fkp->grppapx[index-1]);
	
	if (papx)
		{
		wvTrace(("istd index is %d\n",papx->istd));
		wvInitPAPFromIstd(apap,papx->istd,stsh);
		}
	else
		wvInitPAPFromIstd(apap,istdNil,stsh);

	if ((papx) && (papx->cb > 2))
		{
		ret=1;
		wvTrace(("cbUPX is %d\n",papx->cb));
		for (i=0;i<papx->cb-2;i++)
			wvTrace(("-->%x ",papx->grpprl[i]));
		wvTrace(("\n"));
		upxf.cbUPX = papx->cb;
		upxf.upx.papx.istd = papx->istd;
		upxf.upx.papx.grpprl = papx->grpprl;
		if (version == 0)
			wvAddPAPXFromBucket(apap,&upxf,stsh);
		else
			wvAddPAPXFromBucket6(apap,&upxf,stsh);
		}

	if (papx)
		apap->istd = papx->istd;
	
	wvCopyPHE(&apap->phe,&(fkp->rgbx[index-1].phe),apap->fTtp);
	return(ret);
	}

void wvReleasePAPX(PAPX *item)
	{
	item->cb=0;
	item->istd=0;
	wvFree(item->grpprl);
	item->grpprl=NULL;
	}

void wvInitPAPX(PAPX *item)
	{
	item->cb=0;
	item->istd=0;
	item->grpprl=NULL;
	}

void wvGetPAPX(int version,PAPX *item,U32 offset,FILE *fd)
	{
	U8 cw,i;
	fseek(fd,offset,SEEK_SET);
	cw = getc(fd);
	if ( (cw == 0) && (version == 0) )	/* only do this for word 97 */
		{
		wvTrace(("cw was pad %d\n",cw));
		cw = getc(fd);
		wvTrace(("cw was %d\n",cw));
		}
	item->cb=cw*2;
	if (item->cb > 2)
		item->grpprl = (U8 *)malloc(item->cb-2);
	else
		item->grpprl = NULL;
		
	item->istd = read_16ubit(fd);
	wvTrace(("papx istd is %x\n",item->istd));
	for (i=2;i<item->cb;i++)
		{
		item->grpprl[i-2] = getc(fd);
		wvTrace(("papx byte is %x\n",item->grpprl[i-2]));
		}
	}


int isPAPConform(PAP *current,PAP *previous)
	{
	if ((current) && (previous))
		if (wvEqualBRC(&current->brcLeft,&previous->brcLeft))
			if (wvEqualBRC(&current->brcRight,&previous->brcRight))
				if (current->dxaWidth == previous->dxaWidth)
					if (current->fInTable == previous->fInTable)
						return(1);
	return(0);
	}




void wvCopyConformPAP(PAP *dest,PAP *src)
	{
	if (src)
		{
#ifdef PURIFY
		wvInitPAP(dest);
#endif
		dest->brcLeft = src->brcLeft;
		dest->brcRight = src->brcRight;
		dest->dxaWidth = src->dxaWidth;
		dest->fInTable = src->fInTable;
		}
	else
		wvInitPAP(dest);
	}










