#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "config.h"
#include "wv.h"

/*
void wvToggle(int ret,CHP *in,STSH *stsh,U8 toggle,type)

When the parameter of the sprm is set to 0 or 1, then
the CHP property is set to the parameter value.

*/

/*
When the parameter of the sprm is 128, then the CHP property is set to the
value that is stored for the property in the style sheet. CHP When the
parameter of the sprm is 129, the CHP property is set to the negation of the
value that is stored for the property in the style sheet CHP.
*/

/*
an argument might be made that instead of in being returned or negated that
it should be the looked up in the original chp through the istd that should 
be used, in which case this should be a macro that does the right thing. 
but im uncertain as to which is the correct one to do, ideas on a postcard 
to... etc etc

This argument which i left as a comment to the original function has been
bourne out in practice, so i converted this to a macro and did a lookup
on the original unmodified chp in the stylesheet to check against
*/

#define wvTOGGLE(ret,in,stsh,toggle,type) \
	{ \
	CHP ctemp; \
	if ((toggle == 0) || (toggle == 1))  \
		ret = toggle; \
	else \
		{ \
		\
		wvInitCHPFromIstd(&ctemp,in->istd,stsh); \
	\
		if (toggle == 128) \
			ret = ctemp.type; \
		else if (toggle == 129) \
			ret = !ctemp.type; \
		else \
			wvWarning("Strangle sprm toggle value, ignoring\n"); \
		} \
	}


/*
 spra value operand size
 0          1 byte (operand affects 1 bit)
 1          1 byte
 2          2 bytes
 3          4 bytes
 4          2 bytes
 5          2 bytes
 6          variable length -- following byte is size of operand
 7          3 bytes
*/
int wvSprmLen(int spra)
	{
	switch(spra)
		{
		case 0:	
		case 1:
			return(1);
		case 2:
 		case 4:
 		case 5:
			return(2);
		case 7:
			return(3);
		case 3:
			return(4);
		case 6:
			return(-1);
 			/*variable length -- following byte is size of operand*/
		default:
			wvError(("Incorrect spra value %d\n",spra));
		}
	return(-2);
	}

void wvInitSprm(Sprm *Sprm)
	{
	Sprm->ispmd = 0;
	Sprm->fSpec = 0;
	Sprm->sgc = 0;
	Sprm->spra = 0;
	}

void wvGetSprmFromU16(Sprm *Sprm,U16 sprm)
	{
#ifdef PURIFY
	wvInitSprm(Sprm);
#endif
	Sprm->ispmd = sprm & 0x01ff;
	Sprm->fSpec = (sprm & 0x0200)>>9;  
	Sprm->sgc = (sprm & 0x1c00)>>10;
	Sprm->spra = (sprm & 0xe000)>>13;
	}

int wvEatSprm(U16 sprm,U8 *pointer, U16 *pos)
	{
	int len;
	Sprm aSprm;
	wvTrace(("Eating sprm %x\n",sprm));
	wvGetSprmFromU16(&aSprm,sprm);
	if (sprm == sprmPChgTabs)
		{
		wvTrace(("sprmPChgTabs\n"));
		len = wvApplysprmPChgTabs(NULL,pointer,pos);
		len++;
		return(len);
		}
	else if ((sprm == sprmTDefTable) || (sprm == sprmTDefTable10))
		{
		wvTrace(("sprmTDefTable\\sprmTDefTable10\n"));
		len = bread_16ubit(pointer,pos);
		len--;
		}
	else
		{
		len = wvSprmLen(aSprm.spra);
		wvTrace(("wvSprmLen len is %d\n",len));
		if (len < 0)
			{
			len = bgetc(pointer,pos);
			len++;
			}
		}
	(*pos)+=len;
	return(len);
	}

void wvApplySprmFromBucket(int version,U16 sprm,PAP *apap,CHP *achp,SEP *asep,STSH *stsh, U8 *pointer, U16 *pos)
	{
	BRC10 tempBRC10;
	U16 temp16;
	U8 temp8;
	PAP temppap;
	CHP tempchp;
	SEP tempsep;
	U8 toggle;

	/*bullet proofing*/
	if (apap == NULL)
		{
		wvInitPAP(&temppap);
		apap = &temppap;
		}
	if (achp == NULL)
		{
		wvInitCHP(&tempchp);
		achp = &tempchp;
		}
	if (asep == NULL)
		{
#ifdef PURIFY
		wvInitSEP(&tempsep);
#endif
		asep = &tempsep;
		}

	wvTrace(("sprm is %x\n",sprm));

	switch(sprm)
		{
		/*Beginning of PAP*/
		case sprmPIstd:
			apap->istd = bread_16ubit(pointer,pos);
			break;
		case sprmPIstdPermute:
			wvApplysprmPIstdPermute(apap,pointer,pos);
			break;
		case sprmPIncLvl:
			wvApplysprmPIncLvl(apap,pointer,pos);
			break;
		case sprmPJc:
			apap->jc = bgetc(pointer,pos);
			wvTrace(("jc is now %d\n",apap->jc));
			break;
		case sprmPFSideBySide:
			apap->fSideBySide = bgetc(pointer,pos);
			break;
		case sprmPFKeep:
			apap->fKeep = bgetc(pointer,pos);
			break;
		case sprmPFKeepFollow:
			apap->fKeepFollow = bgetc(pointer,pos);
			break;
		case sprmPFPageBreakBefore:
			apap->fPageBreakBefore = bgetc(pointer,pos);
			break;
		case sprmPBrcl:
			apap->brcl = bgetc(pointer,pos);
			break;
		case sprmPBrcp:
			apap->brcp = bgetc(pointer,pos);
			break;
		case sprmPIlvl:
			apap->ilvl = bgetc(pointer,pos);
			break;
		case sprmPIlfo:
			apap->ilfo = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPFNoLineNumb:
			apap->fNoLnn = bgetc(pointer,pos);
			break;
		case sprmPChgTabsPapx:
			wvApplysprmPChgTabsPapx(apap,pointer,pos);
			break;
		case sprmPDxaRight:
			apap->dxaRight = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPDxaLeft:
			apap->dxaLeft = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPNest:
			/*
			sprmPNest (opcode 0x4610) causes its operand, a two-byte dxa value to be
			added to pap.dxaLeft. If the result of the addition is less than 0, 0 is
			stored into pap.dxaLeft.
			*/
			temp16 = (S16)bread_16ubit(pointer,pos);
			apap->dxaLeft += temp16;
			if ( apap->dxaLeft < 0) apap->dxaLeft=0;
			break;
		case sprmPDxaLeft1:
			apap->dxaLeft1 = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPDyaLine:
			wvGetLSPDFromBucket(&apap->lspd,pointer);
			(*pos)+=4;
			break;
		case sprmPDyaBefore:
			apap->dyaBefore = bread_16ubit(pointer,pos);
			break;
		case sprmPDyaAfter:
			apap->dyaAfter = bread_16ubit(pointer,pos);
			break;
		case sprmPChgTabs:
			wvApplysprmPChgTabs(apap,pointer,pos);
			break;
		case sprmPFInTable:
			apap->fInTable = bgetc(pointer,pos);
			break;
		case sprmPFTtp:
			apap->fTtp = bgetc(pointer,pos);
			break;
		case sprmPDxaAbs:
			apap->dxaAbs = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPDyaAbs:
			apap->dyaAbs = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPDxaWidth:
			apap->dxaWidth = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPPc:
			wvApplysprmPPc(apap,pointer,pos);
			break;
		case sprmPBrcTop10:
			wvGetBRC10FromBucket(&tempBRC10,pointer);
			(*pos)+=2;
			wvConvertBRC10ToBRC(&apap->brcTop,&tempBRC10);
			break;
		case sprmPBrcLeft10:
			wvGetBRC10FromBucket(&tempBRC10,pointer);
			(*pos)+=2;
			wvConvertBRC10ToBRC(&apap->brcLeft,&tempBRC10);
			break;
		case sprmPBrcBottom10:
			wvGetBRC10FromBucket(&tempBRC10,pointer);
			(*pos)+=2;
			wvConvertBRC10ToBRC(&apap->brcBottom,&tempBRC10);
			break;
		case sprmPBrcRight10:
			wvGetBRC10FromBucket(&tempBRC10,pointer);
			(*pos)+=2;
			wvConvertBRC10ToBRC(&apap->brcRight,&tempBRC10);
			break;
		case sprmPBrcBetween10:
			wvGetBRC10FromBucket(&tempBRC10,pointer);
			(*pos)+=2;
			wvConvertBRC10ToBRC(&apap->brcBetween,&tempBRC10);
			break;
		case sprmPBrcBar10:
			wvGetBRC10FromBucket(&tempBRC10,pointer);
			(*pos)+=2;
			wvConvertBRC10ToBRC(&apap->brcBar,&tempBRC10);
			break;
		case sprmPDxaFromText10:	
			apap->dxaFromText = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPWr:
			apap->wr = bgetc(pointer,pos);
			break;
		case sprmPBrcTop:
			(*pos)+=wvGetBRCFromBucket(version,&apap->brcTop,pointer);
			break;
		case sprmPBrcLeft:
			(*pos)+=wvGetBRCFromBucket(version,&apap->brcLeft,pointer);
			break;
		case sprmPBrcBottom:
			(*pos)+=wvGetBRCFromBucket(version,&apap->brcBottom,pointer);
			break;
		case sprmPBrcRight:
			(*pos)+=wvGetBRCFromBucket(version,&apap->brcRight,pointer);
			break;
		case sprmPBrcBetween:
			(*pos)+=wvGetBRCFromBucket(version,&apap->brcBetween,pointer);
			break;
		case sprmPBrcBar:
			(*pos)+=wvGetBRCFromBucket(version,&apap->brcBar,pointer);
			break;
		case sprmPFNoAutoHyph:
			apap->fNoAutoHyph = bgetc(pointer,pos);
			break;
		case sprmPWHeightAbs:
			/* ???? apap->wHeightAbs*/
			(*pos)+=2;
			break;
		case sprmPDcs:
			wvGetDCSFromBucket(&apap->dcs,pointer);
			(*pos)+=2;
			break;
		case sprmPShd:
			wvGetSHDFromBucket(&apap->shd,pointer);
			(*pos)+=2;
			break;
		case sprmPDyaFromText:
			apap->dyaFromText = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPDxaFromText:
			apap->dxaFromText = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPFLocked:
			apap->fLocked = bgetc(pointer,pos);
			break;
		case sprmPFWidowControl:
			apap->fWidowControl = bgetc(pointer,pos);
			break;
		case sprmPFKinsoku:
			apap->fKinsoku = bgetc(pointer,pos);
			break;
		case sprmPFWordWrap:
			apap->fWordWrap = bgetc(pointer,pos);
			break;
		case sprmPFOverflowPunct:
			apap->fOverflowPunct = bgetc(pointer,pos);
			break;
		case sprmPFTopLinePunct:
			apap->fTopLinePunct = bgetc(pointer,pos);
			break;
		case sprmPFAutoSpaceDE:
			apap->fAutoSpaceDE = bgetc(pointer,pos);
			break;
		case sprmPFAutoSpaceDN:
			/* ???? apap->fAutoSpaceDN */
			(*pos)++;
			break;
		case sprmPWAlignFont:
			apap->wAlignFont = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmPFrameTextFlow:
			wvApplysprmPFrameTextFlow(apap,pointer,pos);
			break;
		case sprmPISnapBaseLine:
			/*obsolete: not applicable in Word97 and later versions*/
			(*pos)++;
			break;
		case sprmPNLvlAnm:
			/*obsolete: not applicable in Word97 and later version*/
			apap->nLvlAnm = bgetc(pointer,pos);
			break;
		case sprmPAnld:
			wvApplysprmPAnld(version,apap,pointer,pos);
			break;
		case sprmPPropRMark:
			wvApplysprmPPropRMark(apap,pointer,pos);
			break;
		case sprmPOutLvl:
			/*has no effect if pap.istd is < 1 or is > 9*/
			temp8 = bgetc(pointer,pos);
			if ((apap->istd >= 1) && (apap->istd <=9))
				apap->lvl=temp8;
			break;
		case sprmPFBiDi:
			/* ???? */
			bgetc(pointer,pos);
			break;
		case sprmPFNumRMIns:
			apap->fNumRMIns = bgetc(pointer,pos);
			break;
		case sprmPCrLf:
			/* ???? */
			(*pos)++;
			break;
		case sprmPNumRM:
			wvApplysprmPNumRM(apap,pointer,pos);
			break;
		case sprmPHugePapx:
			wvApplysprmPHugePapx(apap,pointer,pos);
			break;
		case sprmPFUsePgsuSettings:
			apap->fUsePgsuSettings = bgetc(pointer,pos);
			break;
		case sprmPFAdjustRight:
			apap->fAdjustRight = bgetc(pointer,pos);
			break;
		/*End of PAP*/


		/*Begin of CHP*/
		case sprmCFRMarkDel:
			achp->fRMarkDel = bgetc(pointer,pos);
			break;
		case sprmCFRMark:
			achp->fRMark = bgetc(pointer,pos);
			break;
		case sprmCFFldVanish:
			achp->fFldVanish = bgetc(pointer,pos);
			break;
		case sprmCPicLocation:
			/*
			This sprm moves the 4-byte operand of the sprm into the 
			chp.fcPic field. It simultaneously sets chp.fSpec to 1.
			*/
			achp->fcPic_fcObj_lTagObj = bread_32ubit(pointer,pos);
			achp->fSpec = 1;
			break;
		case sprmCIbstRMark:
			achp->ibstRMark = (S16) bread_16ubit(pointer,pos);
			break;
		case sprmCDttmRMark:
			wvGetDTTMFromBucket(&achp->dttmRMark,pointer);
			(*pos)+=4;
			break;
		case sprmCFData:
			achp->fData = bgetc(pointer,pos);
			break;
		case sprmCIdslRMark:
			achp->idslRMReason = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmCChs:
			wvApplysprmCChs(achp,pointer,pos);
			break;
		case sprmCSymbol:
			wvApplysprmCSymbol(achp,pointer,pos);
			break;
		case sprmCFOle2:
			achp->fOle2 = bgetc(pointer,pos);
			break;
		case sprmCHighlight:
			/* ico (fHighlight is set to 1 iff ico is not 0) */
			achp->icoHighlight = bgetc(pointer,pos);
			if (achp->icoHighlight) achp->fHighlight = 1;	/*?*/

			/*another possibility is...*/
			/* if (achp->ico) achp->fHighlight = 1; */
			/*
			or is it something else, who knows the entire documentation on
			the topic consist of the if and only if (iff) line, or maybe
			iff is a type for if, who knows eh ?
			*/
			break;
		case sprmCObjLocation:
			achp->fcPic_fcObj_lTagObj = (S32)bread_32ubit(pointer,pos);
			break;
		case sprmCIstd:
			achp->istd = bread_16ubit(pointer,pos);
			break;
		case sprmCIstdPermute:
			wvApplysprmCIstdPermute(achp,pointer,pos);   /*unfinished*/
			break;
		case sprmCDefault:
			wvApplysprmCDefault(achp,pointer,pos);
			break;
		case sprmCPlain:
			wvApplysprmCPlain(achp,stsh,pointer,pos);
			break;
		case sprmCFBold:
			toggle = bgetc(pointer,pos);
			wvTrace(("toggle here is %d, istd is %d\n",toggle,achp->istd));
			wvTOGGLE(achp->fBold,achp,stsh,toggle,fBold)
			break;
		case sprmCFItalic:
			toggle = bgetc(pointer,pos);
			wvTrace(("Italic is %d, sprm val is %d\n",achp->fItalic,toggle));
			wvTOGGLE(achp->fItalic,achp,stsh,toggle,fItalic)
			wvTrace(("Italic is now %d\n",achp->fItalic));
			break;
		case sprmCFStrike:
			toggle = bgetc(pointer,pos);
			wvTOGGLE(achp->fStrike,achp,stsh,toggle,fStrike)
			break;
		case sprmCFOutline:
			toggle = bgetc(pointer,pos);
			wvTOGGLE(achp->fOutline,achp,stsh,toggle,fOutline)
			break;
		case sprmCFShadow:
			toggle = bgetc(pointer,pos);
			wvTOGGLE(achp->fShadow,achp,stsh,toggle,fShadow)
			break;
		case sprmCFSmallCaps:
			toggle = bgetc(pointer,pos);
			wvTOGGLE(achp->fSmallCaps,achp,stsh,toggle,fSmallCaps)
			break;
		case sprmCFCaps:
			toggle = bgetc(pointer,pos);
			wvTOGGLE(achp->fCaps,achp,stsh,toggle,fCaps)
			break;
		case sprmCFVanish:
			wvTrace(("vanish modified\n"));
			toggle = bgetc(pointer,pos);
			wvTOGGLE(achp->fVanish,achp,stsh,toggle,fVanish)
			break;
		case sprmCFtcDefault:
			toggle = bgetc(pointer,pos);
			wvTOGGLE(achp->fBold,achp,stsh,toggle,fBold)
			break;
		case sprmCKul:
			achp->kul = bgetc(pointer,pos);
			break;
		case sprmCSizePos:
			wvApplysprmCSizePos(achp,pointer,pos);
			break;
		case sprmCDxaSpace:
			achp->dxaSpace = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmCIco:
			achp->ico = bgetc(pointer,pos);
			break;
		case sprmCHps:
			/*incorrect marked as being a byte in docs ?*/
			achp->hps = bread_16ubit(pointer,pos);
			break;
		case sprmCHpsInc:
			wvApplysprmCHpsInc(achp,pointer,pos);
			break;
		case sprmCHpsPos:
			/*incorrect marked as being a byte in docs ?*/
			achp->hpsPos = bread_16ubit(pointer,pos);
			break;
		case sprmCHpsPosAdj:
			wvApplysprmCHpsPosAdj(achp,pointer,pos); 
			break;
		case sprmCMajority:
			wvApplysprmCMajority(achp,stsh,pointer,pos);
			break;
		case sprmCIss:
			achp->iss = bgetc(pointer,pos);
			break;
		case sprmCHpsNew50:
			bgetc(pointer,pos);
			achp->hps = bread_16ubit(pointer,pos);
			break;
		case sprmCHpsInc1:
			wvApplysprmCHpsInc1(achp,pointer,pos);
			break;
		case sprmCHpsKern:
			/*the spec would you have you believe that this is a U8*/
			achp->hpsKern = bread_16ubit(pointer,pos);
			break;
		case sprmCMajority50:
			wvApplysprmCMajority50(achp,stsh,pointer,pos);
			break;
		case sprmCHpsMul:
			/*percentage to grow hps ??*/
			achp->hps = achp->hps * bread_16ubit(pointer,pos)/100;
			break;
		case sprmCYsri:
			/* ???? achp->ysri*/
			bgetc(pointer,pos);
			break;
		case sprmCRgFtc0:
			achp->ftcAscii = bread_16ubit(pointer,pos);
			break;
		case sprmCRgFtc1:
			achp->ftcFE = bread_16ubit(pointer,pos);
			break;
		case sprmCRgFtc2:
			achp->ftcOther = bread_16ubit(pointer,pos);
			break;
		case sprmCFDStrike:
			achp->fDStrike = bgetc(pointer,pos);
			break;
		case sprmCFImprint:
			achp->fImprint = bgetc(pointer,pos);
			break;
		case sprmCFSpec:
			achp->fSpec = bgetc(pointer,pos);
			break;
		case sprmCFObj:
			achp->fObj = bgetc(pointer,pos);
			break;
		case sprmCPropRMark:
			wvApplysprmCPropRMark(achp,pointer,pos);
			break;
		case sprmCFEmboss:
			achp->fEmboss = bgetc(pointer,pos);
			break;
		case sprmCSfxText:
			achp->sfxtText = bgetc(pointer,pos);
			break;
		case sprmCDispFldRMark:
			wvApplysprmCDispFldRMark(achp,pointer,pos);
			break;
		case sprmCIbstRMarkDel:
			achp->ibstRMarkDel = (S16) bread_16ubit(pointer,pos);
			break;
		case sprmCDttmRMarkDel:
			wvGetDTTMFromBucket(&achp->dttmRMarkDel,pointer);
			(*pos)+=4;
			break;
		case sprmCBrc:
			(*pos)+=wvGetBRCFromBucket(version,&achp->brc,pointer);
			break;
		case sprmCShd:
			wvGetSHDFromBucket(&apap->shd,pointer);
			(*pos)+=2;
		case sprmCIdslRMarkDel:
			/* achp->idslRMReasonDel ???? */
			(S16) bread_16ubit(pointer,pos);
			break;
		case sprmCFUsePgsuSettings:
			achp->fUsePgsuSettings = bgetc(pointer,pos);
			break;
		case sprmCRgLid0:
			achp->lidDefault = bread_16ubit(pointer,pos);
			break;
		case sprmCRgLid1:
			achp->lidFE = bread_16ubit(pointer,pos);
			break;
		case sprmCIdctHint:
			achp->idctHint = bgetc(pointer,pos);
			break;
		case sprmCFFtcAsciSymb:		/* not fully mentioned in spec*/
			achp->fFtcAsciSym = bgetc(pointer,pos);
			break;
		case sprmCCpg:			/* not fully mentioned in spec*/
			achp->cpg = bread_16ubit(pointer,pos);
			break;
		case sprmCLid:			/* 
								only used internally, never stored ( word 97 )
								but exists in earlier versions so...
								*/
			achp->lid = bread_16ubit(pointer,pos);
			break;
		/* End of CHP */


		/* Begin of SEP */
		case sprmScnsPgn:
			asep->cnsPgn = bgetc(pointer,pos);
			break;
		case sprmSiHeadingPgn:
			asep->iHeadingPgn = bgetc(pointer,pos);
			break;
		case sprmSOlstAnm:
			wvApplysprmSOlstAnm(version,asep,pointer,pos);
			break;
		case sprmSDxaColWidth:
		case sprmSDxaColSpacing:
			/* well then no one has docs for these two , they're 3 long
			but affects (i guess by name) a 89 long array so who
			knows
			*/
			bgetc(pointer,pos);
			bgetc(pointer,pos);
			bgetc(pointer,pos);
			break;
		case sprmSFEvenlySpaced:
			asep->fEvenlySpaced = bgetc(pointer,pos);
			break;
		case sprmSFProtected:
			asep->fUnlocked = bgetc(pointer,pos);
			break;
		case sprmSDmBinFirst:
			asep->dmBinFirst = bread_16ubit(pointer,pos);
			break;
		case sprmSDmBinOther:
			asep->dmBinFirst = bread_16ubit(pointer,pos);
			break;
		case sprmSBkc:
			asep->bkc = bgetc(pointer,pos);
			break;
		case sprmSFTitlePage:
			asep->fTitlePage = bgetc(pointer,pos);
			break;
		case sprmSCcolumns:
			asep->ccolM1 = bread_16ubit(pointer,pos);
			break;
		case sprmSDxaColumns:
			asep->dxaColumns = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmSFAutoPgn:
			asep->fAutoPgn = bgetc(pointer,pos);
			break;
		case sprmSNfcPgn:
			asep->nfcPgn = bgetc(pointer,pos);
			break;
		case sprmSDyaPgn:
			asep->dyaPgn = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmSDxaPgn:
			asep->dxaPgn = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmSFPgnRestart:
			asep->fPgnRestart = bgetc(pointer,pos);
			break;
		case sprmSFEndnote:
			asep->fEndNote = bgetc(pointer,pos);
			break;
		case sprmSLnc:
			asep->lnc = bgetc(pointer,pos);
			break;
		case sprmSGprfIhdt:
			asep->grpfIhdt = bgetc(pointer,pos);
			break;
		case sprmSNLnnMod:
			asep->nLnnMod = bread_16ubit(pointer,pos);
			break;
		case sprmSDxaLnn:
			asep->dxaLnn = (S16) bread_16ubit(pointer,pos);
			break;
		case sprmSDyaHdrTop:
			asep->dyaHdrTop = bread_16ubit(pointer,pos);
			break;
		case sprmSDyaHdrBottom:
			asep->dyaHdrBottom = bread_16ubit(pointer,pos);
			break;
		case sprmSLBetween:
			asep->fLBetween = bgetc(pointer,pos);
			break;
		case sprmSVjc:
			asep->fLBetween = bgetc(pointer,pos);
			break;
		case sprmSLnnMin:
			asep->lnnMin = (S16) bread_16ubit(pointer,pos);
			break;
		case sprmSPgnStart:
			asep->pgnStart = bread_16ubit(pointer,pos);
			break;
		case sprmSBOrientation:
			asep->dmOrientPage = bgetc(pointer,pos);
			break;
		case sprmSBCustomize:
			/*noone knows what this is*/
			bgetc(pointer,pos);
			break;
		case sprmSXaPage:
			asep->xaPage = bread_16ubit(pointer,pos);
			break;
		case sprmSYaPage:
			asep->yaPage = bread_16ubit(pointer,pos);
			break;
		case sprmSDxaLeft:
			asep->dxaLeft = bread_16ubit(pointer,pos);
			break;
		case sprmSDxaRight:
			asep->dxaRight = bread_16ubit(pointer,pos);
			break;
		case sprmSDyaTop:
			asep->dyaTop = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmSDyaBottom:
			asep->dyaBottom = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmSDzaGutter:
			asep->dzaGutter = bread_16ubit(pointer,pos);
			break;
		case sprmSDmPaperReq:
			asep->dmPaperReq = bread_16ubit(pointer,pos);
			break;
		case sprmSPropRMark:
			wvApplysprmSPropRMark(asep,pointer,pos);
			break;
		case sprmSFBiDi:	/* ?????? , what the hell are these three*/
		case sprmSFFacingCol:
		case sprmSFRTLGutter:
			bgetc(pointer,pos);
			break;
		case sprmSBrcTop:
			(*pos)+=wvGetBRCFromBucket(version,&asep->brcTop,pointer);
			break;
		case sprmSBrcLeft:
			(*pos)+=wvGetBRCFromBucket(version,&asep->brcLeft,pointer);
			break;
		case sprmSBrcBottom:	
			(*pos)+=wvGetBRCFromBucket(version,&asep->brcBottom,pointer);
			break;
		case sprmSBrcRight:
			(*pos)+=wvGetBRCFromBucket(version,&asep->brcRight,pointer);
			break;
		case sprmSPgbProp:
			asep->pgbProp = (S16)bread_16ubit(pointer,pos);
			break;
		case sprmSDxtCharSpace:
			asep->dxtCharSpace = (S32)bread_32ubit(pointer,pos);
			break;
		case sprmSDyaLinePitch:
			asep->dyaLinePitch = (S32)bread_32ubit(pointer,pos);
			break;
		case sprmSClm:
			/* who knows */
			bread_16ubit(pointer,pos);
			break;
		case sprmSTextFlow:
			asep->wTextFlow = (S16)bread_16ubit(pointer,pos);
			break;
		/* End of SEP */

		/* Place Holders for TAP, need to be implemented */
		case sprmTTableBorders:
			for (temp8=0;temp8<12;temp8++)
				{
				if (version == 0)
					bread_16ubit(pointer,pos);
				else
					bgetc(pointer,pos);
				}
			break;
		case sprmTSetBrc:
			if (version == 0)
				{
				int len;
				len = bgetc(pointer,pos);
				for (temp8=0;temp8<len;temp8++)
					bgetc(pointer,pos);
				}
			else
				{
				bgetc(pointer,pos);
				bgetc(pointer,pos);
				bgetc(pointer,pos);
				(*pos)+=wvGetBRCFromBucket(version,&achp->brc,pointer);
				}
			break;
		/* end of TAP */

			/*
		case sprmPicBrcl
			*/

		case sprmCFBiDi:		/* ???? */
		case sprmCFDiacColor:	/* ???? */
		case sprmCFBoldBi:		/* ???? */
		case sprmCFItalicBi:	/* ???? */
		case sprmCFtcBi:		/* ???? */
		case sprmCLidBi:		/* ???? */
		case sprmCIcoBi:		/* ???? */
		case sprmCHpsBi:		/* ???? */
		case sprmPRuler:		/* ???? */
		case sprmCIdCharType:	/* obsolete*/
		case sprmCKcd: 			/* ???? */
		case sprmCCharScale:	/* ???? */
		case sprmNoop:			/* no operand */
			break;
		default:
			wvEatSprm(sprm,pointer,pos);
			break;
		}
	}

void wvApplysprmPIstdPermute(PAP *apap,U8 *pointer,U16 *pos)
	{
	U8 cch;
	U8 fLongg;
	U8 fSpare;
	U16 istdFirst;
	U16 istdLast;
	U16 *rgistd;
	U16 i;

	cch = dgetc(NULL,&pointer);
	(*pos)++;
	fLongg = dgetc(NULL,&pointer);
	(*pos)++;
	fSpare = dgetc(NULL,&pointer);
	(*pos)++;
	istdFirst = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	istdLast = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	if ((cch-6)/2 != 0)
		{
		rgistd = (U16 *)malloc(sizeof(U16) * ((cch-6)/2));
		for(i=0;i<(cch-6)/2;i++)
			{
			rgistd[i] = dread_16ubit(NULL,&pointer);
			(*pos)+=2;
			}
		}
	else
		rgistd = NULL;
	/*
	First check if pap.istd is greater than the istdFirst recorded in the sprm 
	and less than or equal to the istdLast recorded in the sprm If not, the sprm 
	has no effect. If it is, pap.istd is set to rgistd[pap.istd - istdFirst]
	*/

	if ((apap->istd > istdFirst) && (apap->istd<=istdLast))
		apap->istd = rgistd[apap->istd - istdFirst];
	wvFree(rgistd);
	}

void wvApplysprmPIncLvl(PAP *apap,U8 *pointer,U16 *pos)
	{
	U8 temp8;
	S8 tempS8;
	temp8 = bgetc(pointer,pos);
	/*
	If pap.stc is < 1 or > 9, sprmPIncLvl has no effect. Otherwise, if the value
	stored in the byte has its highest order bit off, the value is a positive
	difference which should be added to pap.istd and pap.lvl and then pap.stc
	should be set to min(pap.istd, 9). If the byte value has its highest order
	bit on, the value is a negative difference which should be sign extended to
	a word and then subtracted from pap.istd and pap.lvl. Then pap.stc should be
	set to max(1, pap.istd).

	Now... hang on a sec coz

	Note that the storage and behavior of styles has changed radically since
	Word 2 for Windows, beginning with nFib 63. Some of the differences are:
	<chomp>
	* The style code is called an istd, rather than an stc.

	So, for the purposes of this filter, we ignore the stc component of the
	instructions

	*/
	
	/*
	if ((apap->stc < 1) || (apap->stc > 9))
		return;
	*/
	if ((apap->istd < 1) || (apap->istd > 9))
		return;

	if ((temp8 & 0x80) >> 7 == 0)
		{
		apap->istd += temp8;
		apap->lvl += temp8;
		/*
		apap->stc = min(apap->istd, 9);
		*/
		}
	else
		{
		tempS8 = (S8)temp8;
		apap->istd += tempS8;
		apap->lvl += tempS8;
		/*
		apap->stc = max(1, apap->istd);
		*/
		}
	}

void wvApplysprmPChgTabsPapx(PAP *apap,U8 *pointer,U16 *pos)
	{
	S16 temp_rgdxaTab[itbdMax];
	/*
	S8 temp_rgtbd[itbdMax];
	*/
	TBD temp_rgtbd[itbdMax];
	int i,j,k=0,oldpos;
	U8 cch,itbdDelMax;
	S16 *rgdxaDel;
	U8 itbdAddMax;
	S16 *rgdxaAdd;
	/*
	S8 *rgtbdAdd;
	*/
	TBD *rgtbdAdd;

	oldpos = *pos;
	cch = dgetc(NULL,&pointer);
	(*pos)++;
	itbdDelMax = dgetc(NULL,&pointer);
	(*pos)++;
	if (itbdDelMax != 0)
		{
		rgdxaDel = (S16 *)malloc(sizeof(U16) * itbdDelMax);
		for(i=0;i<itbdDelMax;i++)
			{
			rgdxaDel[i] = dread_16ubit(NULL,&pointer);
			(*pos)+=2;
			}
		}
	else
		rgdxaDel = NULL;
	itbdAddMax = dgetc(NULL,&pointer);
	(*pos)++;
	if (itbdAddMax != 0)
		{
		rgdxaAdd = (S16 *)malloc(sizeof(U16) * itbdAddMax);
		for(i=0;i<itbdAddMax;i++)
			{
			rgdxaAdd[i] = dread_16ubit(NULL,&pointer);
			(*pos)+=2;
			}
		/*
		rgtbdAdd = (S8 *)malloc(itbdAddMax);
		*/
		rgtbdAdd = (TBD *)malloc(itbdAddMax*sizeof(TBD));
		for(i=0;i<itbdAddMax;i++)
			{
			wvGetTBDFromBucket(&rgtbdAdd[i],pointer);
			/*
			rgtbdAdd[i] = dgetc(NULL,&pointer);
			*/
			(*pos)++;
			}
		}
	else
		{
		rgdxaAdd=NULL;
		rgtbdAdd=NULL;
		}
	if (*pos-oldpos != cch+1)
		wvError(("Offset Problem in wvApplysprmPChgTabsPapx\n"));
	
	/*
	When sprmPChgTabsPapx is interpreted, the rgdxaDel of the sprm is applied
	first to the pap that is being transformed. This is done by deleting from
	the pap the rgdxaTab entry and rgtbd entry of any tab whose rgdxaTab value
	is equal to one of the rgdxaDel values in the sprm. It is guaranteed that
	the entries in pap.rgdxaTab and the sprm's rgdxaDel and rgdxaAdd are
	recorded in ascending dxa order.

	Then the rgdxaAdd and rgtbdAdd entries are merged into the pap's rgdxaTab
	and rgtbd arrays so that the resulting pap rgdxaTab is sorted in ascending
	order with no duplicates.
	*/
	for(j=0;j<apap->itbdMac;j++)
		{
		for(i=0;i<itbdDelMax;i++)
			{
			if (rgdxaDel[i] != apap->rgdxaTab[j])
				{
				temp_rgdxaTab[k] = apap->rgdxaTab[j];
				wvCopyTBD(&temp_rgtbd[k++],&apap->rgtbd[j]);
				/*
				temp_rgtbd[k++] = apap->rgtbd[j];
				*/
				}
			}
		}
	apap->itbdMac = k;
	k=0;
	j=0;
	i=0;
	while ((j<apap->itbdMac) && (i < itbdAddMax))
		{
		wvTrace(("i %d j apap->itbdMac %d %d\n",i,j,apap->itbdMac));
		wvTrace(("temp_rgdxaTab[j]\n",temp_rgdxaTab[j]));
		wvTrace(("rgdxaAdd[i]\n",rgdxaAdd[i]));
		if ( (j<apap->itbdMac) && (temp_rgdxaTab[j] < rgdxaAdd[i]) )
			{
			apap->rgdxaTab[k] = temp_rgdxaTab[j];
			wvCopyTBD(&apap->rgtbd[k++],&temp_rgtbd[j++]);
			/*
			apap->rgtbd[k++] = temp_rgtbd[j++];
			*/
			}
		else if ((j<apap->itbdMac) && (temp_rgdxaTab[j] == rgdxaAdd[i]))
			{
			apap->rgdxaTab[k] = rgdxaAdd[i];
			wvCopyTBD(&apap->rgtbd[k++],&rgtbdAdd[i++]);
			/*
			apap->rgtbd[k++] = rgtbdAdd[i++];
			*/
			j++;
			}
		else /*if (i < itbdAddMax)*/
			{
			apap->rgdxaTab[k] = rgdxaAdd[i];
			wvCopyTBD(&apap->rgtbd[k++],&rgtbdAdd[i++]);
			/*
			apap->rgtbd[k++] = rgtbdAdd[i++];
			*/
			}
		}

	apap->itbdMac = k;
	wvFree(rgtbdAdd);
	wvFree(rgdxaAdd);
	wvFree(rgdxaDel);
	}

int wvApplysprmPChgTabs(PAP *apap,U8 *pointer,U16 *pos)
	{
	S16 temp_rgdxaTab[itbdMax];
	TBD temp_rgtbd[itbdMax];
	U8 cch;
 	U8 itbdDelMax;
 	S16 *rgdxaDel;
 	S16 *rgdxaClose;
 	U8 itbdAddMax;
 	S16 *rgdxaAdd;
 	TBD *rgtbdAdd;
	U8 i,j,k=0;

	wvTrace(("entering wvApplysprmPChgTabs\n"));
	/*
	itbdDelMax and itbdAddMax are defined to be equal to 50. This means that the
	largest possible instance of sprmPChgTabs is 354. When the length of the
	sprm is greater than or equal to 255, the cch field will be set equal to
	255. When cch == 255, the actual length of the sprm can be calculated as
	follows: length = 2 + itbdDelMax * 4 + itbdAddMax * 3.
	*/

	cch = dgetc(NULL,&pointer);
	wvTrace(("cch is %d\n",cch));
	(*pos)++;
	itbdDelMax = dgetc(NULL,&pointer);
	(*pos)++;


	if (itbdDelMax != 0)
		{
		rgdxaDel = (S16*)malloc(sizeof(S16) * itbdDelMax);
		rgdxaClose = (S16*)malloc(sizeof(S16) * itbdDelMax);
		for (i=0;i<itbdDelMax;i++)
			{
			rgdxaDel[i] = dread_16ubit(NULL,&pointer);
			(*pos)+=2;
			}
		for (i=0;i<itbdDelMax;i++)
			{
			rgdxaClose[i] = dread_16ubit(NULL,&pointer);
			(*pos)+=2;
			}
		}
	else
		{
		rgdxaDel = NULL;
		rgdxaClose = NULL;
		}
	itbdAddMax = dgetc(NULL,&pointer);
	wvTrace(("itbdAddMax is %d\n",itbdAddMax));
	(*pos)++;
	if (itbdAddMax != 0)
		{
		rgdxaAdd = (S16*)malloc(sizeof(S16) * itbdAddMax);
		rgtbdAdd = (TBD*)malloc(itbdAddMax*sizeof(TBD));
		for (i=0;i<itbdAddMax;i++)
			{
			rgdxaAdd[i] = dread_16ubit(NULL,&pointer);
			(*pos)+=2;
			}
		for (i=0;i<itbdAddMax;i++)
			{
			/*
			rgtbdAdd[i] = dgetc(NULL,&pointer);
			*/
			wvGetTBDFromBucket(&rgtbdAdd[i],pointer);
			(*pos)++;
			}
		}
	else
		{
		rgdxaAdd = NULL;
		rgtbdAdd = NULL;
		}

	if (cch == 225)
		cch = 2 + itbdDelMax * 4 + itbdAddMax * 3;

	/*
	When sprmPChgTabs is interpreted, the rgdxaDel of the sprm is applied first
	to the pap that is being transformed. This is done by deleting from the pap
	the rgdxaTab entry and rgtbd entry of any tab whose rgdxaTab value is within
	the interval [rgdxaDel[i] - rgdxaClose[i], rgdxaDel[i] + rgdxaClose[i]] It
	is guaranteed that the entries in pap.rgdxaTab and the sprm's rgdxaDel and
	rgdxaAdd are recorded in ascending dxa order.

	Then the rgdxaAdd and rgtbdAdd entries are merged into the pap's rgdxaTab
	and rgtbd arrays so that the resulting pap rgdxaTab is sorted in ascending
	order with no duplicates.
	*/
	if (apap == NULL)
		{
		wvFree(rgdxaDel);
		wvFree(rgtbdAdd);
		wvFree(rgdxaAdd);
		wvFree(rgdxaClose);
		return(cch);
		}

	wvTrace(("here %d\n",apap->itbdMac));
	for(j=0;j<apap->itbdMac;j++)
		{
		for(i=0;i<itbdDelMax;i++)
			{
			if ( (apap->rgdxaTab[j] < rgdxaDel[i] - rgdxaClose[i])
				|| (apap->rgdxaTab[j] < rgdxaDel[i] + rgdxaClose[i]) )
				{
				temp_rgdxaTab[k] = apap->rgdxaTab[j];
				wvCopyTBD(&temp_rgtbd[k++],&apap->rgtbd[j]);
				}
			}
		}
	apap->itbdMac = k;

	k=0;
	j=0;
	i=0;
	while ((j<apap->itbdMac) && (i < itbdAddMax))
		{
		if ( (j<apap->itbdMac) && (temp_rgdxaTab[j] < rgdxaAdd[i]) )
			{
			apap->rgdxaTab[k] = temp_rgdxaTab[j];
			wvCopyTBD(&apap->rgtbd[k++],&temp_rgtbd[j++]);
			}
		else if ((j<apap->itbdMac) && (temp_rgdxaTab[j] == rgdxaAdd[i]))
			{
			apap->rgdxaTab[k] = rgdxaAdd[i];
			wvCopyTBD(&apap->rgtbd[k++],&rgtbdAdd[i++]);
			j++;
			}
		else /*if (i < itbdAddMax)*/
			{
			apap->rgdxaTab[k] = rgdxaAdd[i];
			wvCopyTBD(&apap->rgtbd[k++],&rgtbdAdd[i++]);
			}
		}

	wvTrace(("here\n"));
	apap->itbdMac = k;
	wvFree(rgdxaDel);
	wvFree(rgtbdAdd);
	wvFree(rgdxaAdd);
	wvFree(rgdxaClose);
	wvTrace(("Exiting Successfully\n"));

	return(cch);
	}

void wvApplysprmPPc(PAP *apap,U8 *pointer,U16 *pos)
	{
	U8 temp8;
	struct _temp
		{
		U32 reserved:4;
		U32 pcVert:2;
		U32 pcHorz:2;
		} temp;
    

	temp8 = bgetc(pointer,pos);
#ifdef PURIFY
	temp.pcVert = 0;
	temp.pcHorz = 0;
#endif
	temp.pcVert = (temp8 & 0x0C) >> 4;
	temp.pcHorz = (temp8 & 0x03) >> 6;

	/*
	sprmPPc is interpreted by moving pcVert to pap.pcVert if pcVert != 3 and by
	moving pcHorz to pap.pcHorz if pcHorz != 3. 
	*/

	if (temp.pcVert != 3)
		apap->pcVert = temp.pcVert;
	if (temp.pcHorz != 3)
		apap->pcHorz = temp.pcHorz;
	}

void wvApplysprmPFrameTextFlow(PAP *apap,U8 *pointer,U16 *pos)
	{
	U16 temp16 = bread_16ubit(pointer,pos);

	apap->fVertical = temp16 & 0x0001;
	apap->fBackward = (temp16 & 0x0002) >> 1;
	apap->fRotateFont = (temp16 & 0x0004) >> 2;
	}

void wvApplysprmPAnld(int version,PAP *apap,U8 *pointer, U16 *pos)
	{
	dgetc(NULL,&pointer);
	(*pos)++;
	wvGetANLD_FromBucket(version,&apap->anld,pointer);
	if (version == 0)
		(*pos)+=cbANLD;
	else
		(*pos)+=cb6ANLD;
	}

void wvApplysprmPPropRMark(PAP *apap,U8 *pointer,U16 *pos)
	{
	dgetc(NULL,&pointer);
	/*
	sprmPPropRMark is interpreted by moving the first parameter
	byte to pap.fPropRMark, the next two bytes to pap.ibstPropRMark, and the
	remaining four bytes to pap.dttmPropRMark.
	*/
	apap->fPropRMark = dgetc(NULL,&pointer);
	(*pos)++;
	apap->ibstPropRMark = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	wvGetDTTMFromBucket(&apap->dttmPropRMark,pointer);
	(*pos)+=4;
	}
	
void wvApplysprmPNumRM(PAP *apap,U8 *pointer, U16 *pos)
	{
	dgetc(NULL,&pointer);
	(*pos)++;
	wvGetNUMRMFromBucket(&apap->numrm,pointer);
	(*pos)+=cbNUMRM;
	}

void wvApplysprmPHugePapx(PAP *apap, U8 *pointer, U16 *pos)
	{
	/*
	sprmPHugePapx is stored in PAPX FKPs in place of the grpprl of a PAPX which
	would otherwise be too big to fit in an FKP (as of this writing, 488 bytes
	is the size of the largest PAPX which can fit in an FKP). The parameter fc
	gives the location of the grpprl in the data stream. The first word at that
	fc counts the number of bytes in the grpprl (not including the byte count
	itself). A sprmPHugePapx should therefore only be found in a PAPX FKP and
	should be the only sprm in that PAPX's grpprl.
	*/
	wvError(("This document has an unsupported sprm (sprmPHugePapx), please mail "));
	wvError(("Caolan.McNamara@ul.ie with this document, as i haven't been able to "));
	wvError(("get any examples of it so as to figure out how to handle it\n"));
	(*pos)+=4;
	}

void wvApplysprmCChs(CHP *achp,U8 *pointer,U16 *pos)
	{
	/*
	When this sprm is interpreted, the first byte of the operand is moved to
	chp.fChsDiff and the remaining word is moved to chp.chse.
	*/
	achp->fChsDiff = dgetc(NULL,&pointer);
	(*pos)++;
	/*achp->chse ???? */ 
	/* the doc says to set this, but it doesnt exist anywhere else in the docs */
	dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	}

void wvApplysprmCSymbol(CHP *achp,U8 *pointer,U16 *pos)
	{
	/*
	This sprm's operand is 4 bytes. The first 2 hold the font code; the last 2
	hold a character specifier. When this sprm is interpreted, the font code is
	moved to chp.ftcSym and the character specifier is moved to chp.xchSym and
	chp.fSpec is set to 1.
	*/
	achp->ftcSym = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	achp->xchSym = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	achp->fSpec=1;
	}
	
void wvApplysprmCIstdPermute(CHP *achp,U8 *pointer,U16 *pos)
	{
	U8 cch;
	U8 fLongg;
	U8 fSpare;
	U16 istdFirst;
	U16 istdLast;
	U16 *rgistd;
	U16 i;

	cch = dgetc(NULL,&pointer);
	(*pos)++;
	fLongg = dgetc(NULL,&pointer);
	(*pos)++;
	fSpare = dgetc(NULL,&pointer);
	(*pos)++;
	istdFirst = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	istdLast = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	if ((cch-6)/2 != 0)
		{
		rgistd = (U16 *)malloc(sizeof(U16) * ((cch-6)/2));
		for(i=0;i<(cch-6)/2;i++)
			{
			rgistd[i] = dread_16ubit(NULL,&pointer);
			(*pos)+=2;
			}
		}
	else
		rgistd = NULL;
	/*
	first check if chp.istd is greater than the
	istdFirst recorded in the sprm and less than or equal to the istdLast
	recorded in the sprm If not, the sprm has no effect. If it is, chp.istd is
	set to rgstd[chp.istd - istdFirst] and any chpx stored in that rgstd entry
	is applied to the chp.

	Note that it is possible that an istd may be recorded in the rgistd that
	refers to a paragraph style. This will no harmful consequences since the
	istd for a paragraph style should never be recorded in chp.istd.
	*/

	if ((achp->istd > istdFirst) && (achp->istd<=istdLast))
		{
		achp->istd = rgistd[achp->istd - istdFirst];
		/*
		if really a chp style
			wvAddCHPXFromUPEBucket(achp,&(stsh->std[achp->istd].grupe[0].chpx),stsh);
		else
			complain;
		*/
		}
	wvFree(rgistd);
	}

void wvApplysprmCDefault(CHP *achp,U8 *pointer,U16 *pos)
	{
	/*
	sprmCDefault (opcode 0x2A32) clears the fBold, fItalic, fOutline, fStrike,
	fShadow, fSmallCaps, fCaps, fVanish, kul and ico fields of the chp to 0. It
	was first defined for Word 3.01 and had to be backward compatible with Word
	3.00 so it is a variable length sprm whose count of bytes is 0. It consists
	of the sprmCDefault opcode followed by a byte of 0.
	*/
	dgetc(NULL,&pointer);
	(*pos)++;
	achp->fBold = 0;
	achp->fItalic = 0;
	achp->fOutline = 0;
	achp->fStrike = 0;
	achp->fShadow = 0;
	achp->fSmallCaps = 0;
	achp->fCaps = 0;
	achp->fVanish = 0;
	achp->kul = 0;
	achp->ico = 0;
	}

void wvApplysprmCPlain(CHP *achp,STSH *stsh,U8 *pointer,U16 *pos)
	{
	U8 fSpec;
	/*
	the style sheet CHP is copied over the original CHP preserving the 
	fSpec setting from the original CHP.
	*/
	fSpec = achp->fSpec;
	wvInitCHPFromIstd(achp,achp->istd,stsh);
	}


U8 wvToggle(U8 in,U8 toggle)
	{
	/*
	When the parameter of the sprm is set to 0 or 1, then
	the CHP property is set to the parameter value.
	*/
	if ((toggle == 0) || (toggle == 1))
		return(toggle);
	/*
	When the parameter of the sprm is 128, then the CHP property is set to the
	value that is stored for the property in the style sheet. CHP When the
	parameter of the sprm is 129, the CHP property is set to the negation of the
	value that is stored for the property in the style sheet CHP.
	*/

	/*
	an argument might be made that instead of in being returned or negated that
	it should be the looked up in the original chp through the istd that should 
	be used, in which case this should be a macro that does the right thing. 
	but im uncertain as to which is the correct one to do, ideas on a postcard 
	to... etc etc
	*/
	if (toggle == 128)
		return(in);
	else if (toggle == 129)
		return(!in);
	wvWarning("Strangle sprm toggle value, ignoring\n");
	return(in);
	}

void wvApplysprmCSizePos(CHP *achp,U8 *pointer,U16 *pos)
	{
	U8 prevhpsPos;
	U16 temp8;
	struct _temp
		{
		U32 hpsSize:8;
		U32 cInc:7;
		U32 fAdjust:1;
		U32 hpsPos:8;
		} temp;
	temp.hpsSize = dgetc(NULL,&pointer);
	(*pos)++;
	temp8 = dgetc(NULL,&pointer);
	(*pos)++;
	temp.cInc = (temp8 & 0x7f) >> 8;
	temp.fAdjust = (temp8 & 0x80) >> 7;
	temp.hpsPos= dgetc(NULL,&pointer);
	(*pos)++;
	
	/*
	if hpsSize != 0 then chp.hps is set to hpsSize. 

	If cInc is != 0, the cInc is interpreted as a 7 bit twos complement
	number and the procedure described below for interpreting sprmCHpsInc is
	followed to increase or decrease the chp.hps by the specified number of
	levels. 
	
	If hpsPos is != 128, then chp.hpsPos is set equal to hpsPos. 
	
	If fAdjust is on , hpsPos != 128 and hpsPos != 0 and the previous value of
	chp.hpsPos == 0, then chp.hps is reduced by one level following the method
	described for sprmCHpsInc. 
	
	If fAdjust is on, hpsPos == 0 and the previous value of chp.hpsPos != 0, 
	then the chp.hps value is increased by one level using the method described 
	below for sprmCHpsInc.
	*/

	if (temp.hpsSize != 0)
		achp->hps = temp.hpsSize;

	if (temp.cInc != 0)
		{
		}

	prevhpsPos = achp->hpsPos;

	if (temp.hpsPos != 128)
		achp->hpsPos = temp.hpsPos;
	/*else ? who knows ?*/
	if ((temp.fAdjust) && (temp.hpsPos != 128) && (temp.hpsPos != 0) && (achp->hpsPos == 0))
		/*reduce level */;
	if ((temp.fAdjust) && (temp.hpsPos == 0) && (achp->hpsPos != 0))
		/*increase level*/;

	/*
	This depends on an implementation of sprmCHpsInc, read wvApplysprmCHpsInc for
	some comments on the whole matter
	*/
	
	wvError(("This document has an unsupported sprm (sprmCSizePos), please mail "));
	wvError(("Caolan.McNamara@ul.ie with this document, as i haven't been able to "));
	wvError(("get any examples of it so as to figure out how to handle it\n"));

	}

void wvApplysprmCHpsInc(CHP *achp,U8 *pointer,U16 *pos)
	{
	U8 param;
	/*
	sprmCHpsInc(opcode 0x2A44) is a three-byte sprm consisting of the sprm
	opcode and a one-byte parameter. 
	
	Word keeps an ordered array of the font sizes that are defined for the fonts 
	recorded in the system file with each font size transformed into an hps. 
	
	The parameter is a one-byte twos complement number. Word uses this number 
	to calculate an index in the font size array to determine the new hps for a 
	run. When Word interprets this sprm and the parameter is positive, it searches 
	the array of font sizes to find the index of the smallest entry in the font 
	size table that is greater than the current chp.hps.It then adds the 
	parameter minus 1 to the index and maxes this with the index of the last array 
	entry. It uses the result as an index into the font size array and assigns that 
	entry of the array to chp.hps.

	When the parameter is negative, Word searches the array of font sizes to
	find the index of the entry that is less than or equal to the current
	chp.hps. It then adds the negative parameter to the index and does a min of
	the result with 0. The result of the min function is used as an index into
	the font size array and that entry of the array is assigned to chp.hps.
	sprmCHpsInc is stored only in grpprls linked to piece table entries.
	*/

	wvError(("This document has an unsupported sprm (sprmCHpsInc), please mail "));
	wvError(("Caolan.McNamara@ul.ie with this document, as i haven't been able to "));
	wvError(("get any examples of it so as to figure out how to handle it\n"));

	param = dgetc(NULL,&pointer);

	/*
	Now for christ sake !!, how on earth would i have an "ordered array of the 
	font sizes that are defined for the fonts recorded in the system file", that
	sounds to me that i would have to have access to the fonts on the actual
	machine that word was last run on !, it sounds to me that this sprm might only
	be used during the editing of a file, so im going to have to ignore it because
	it complete goobledegook to me
	*/

	}

void wvApplysprmCHpsPosAdj(CHP *achp,U8 *pointer,U16 *pos)
	{
	U8 param;
	/*
	sprmCHpsPosAdj (opcode 0x2A46) causes the hps of a run to be reduced the
	first time text is superscripted or subscripted and causes the hps of a run
	to be increased when superscripting/subscripting is removed from a run. 
	
	The one byte parameter of this sprm is the new hpsPos value that is to be 
	stored in chp.hpsPos. 
	
	If the new hpsPos is not equal 0 (meaning that the text is to be super/
	subscripted), Word first examines the current value of chp.hpsPos
	to see if it is equal to 0. 
	
	If so, Word uses the algorithm described for sprmCHpsInc to decrease chp.hps 
	by one level. 
	
	If the new hpsPos == 0 (meaning the text is not super/subscripted), 
	Word examines the current chp.hpsPos to see if it is not equal to 0. If it is 
	not (which means text is being restored to normal position), Word uses the 
	sprmCHpsInc algorithm to increase chp.hps by one level.  
	
	After chp.hps is adjusted, the parameter value is stored in chp.hpsPos. 
	*/

	wvError(("This document has an partially unsupported sprm (sprmCHpsPosAdj), please mail "));
	wvError(("Caolan.McNamara@ul.ie with this document, as i haven't been able to "));
	wvError(("get any examples of it so as to figure out how to handle it\n"));

	param = dgetc(NULL,&pointer);
	(*pos)++;

	/*
	please see wvApplysprmCHpsInc for why this is unfinished
	*/

	if ( (param != 0) && (achp->hpsPos == 0) )
		/*decrease chp.hps*/;
	else if ( (param == 0) && (achp->hpsPos != 0) )
		/*increase chp.hps*/;
	
	achp->hpsPos = param;


	}

void wvApplysprmCMajority(CHP *achp,STSH *stsh,U8 *pointer,U16 *pos)
	{
	U16 i;
	CHP base;
	CHP orig;
	UPXF upxf;
	/*
	Bytes 0 and 1 of
	sprmCMajority contains the opcode, byte 2 contains the length of the
	following list of character sprms. . Word begins interpretation of this sprm
	by applying the stored character sprm list to a standard chp. That chp has
	chp.istd = istdNormalChar. chp.hps=20, chp.lid=0x0400 and chp.ftc = 4. Word
	then compares fBold, fItalic, fStrike, fOutline, fShadow, fSmallCaps, fCaps,
	ftc, hps, hpsPos, kul, qpsSpace and ico in the original CHP with the values
	recorded for these fields in the generated CHP.. If a field in the original
	CHP has the same value as the field stored in the generated CHP, then that
	field is reset to the value stored in the style's CHP. If the two copies
	differ, then the original CHP value is left unchanged.
	*/
	wvTrace(("This document has a sprm (sprmCMajority), that ive never seen in practice please mail "));
	wvTrace(("Caolan.McNamara@ul.ie with this document, as i haven't been able to "));
	wvTrace(("get any examples of it so as to figure out if its handled correctly\n"));
	
	wvInitCHP(&base);
	base.ftc=4;

	/*generate a UPE and run wvAddCHPXFromBucket*/

	upxf.cbUPX = dgetc(NULL,&pointer);
	(*pos)++;
	upxf.upx.chpx.grpprl = (U8 *)malloc(upxf.cbUPX);

	for (i=0;i<upxf.cbUPX;i++)
		{
		upxf.upx.chpx.grpprl[i] = dgetc(NULL,&pointer);
		(*pos)++;
		}

	wvTrace(("achp istd is %d\n",achp->istd));
	
	wvAddCHPXFromBucket(&base,&upxf,stsh);

	wvTrace(("achp istd is %d\n",achp->istd));

	wvTrace(("my underline started as %d\n",achp->kul));

	wvInitCHPFromIstd(&orig,achp->istd,stsh);

	/* this might be a little wrong, review after doing dedicated CHP's*/
	if (achp->fBold == base.fBold)
		achp->fBold = orig.fBold;
	if (achp->fItalic == base.fItalic)
		achp->fItalic = orig.fItalic;
	if (achp->fStrike == base.fStrike)
		achp->fStrike = orig.fStrike;
	if (achp->fOutline == base.fOutline)
		achp->fOutline = orig.fOutline;
	if (achp->fShadow == base.fShadow)
		achp->fShadow = orig.fShadow;
	if (achp->fSmallCaps == base.fSmallCaps)
		achp->fSmallCaps = orig.fSmallCaps;
	if (achp->fCaps == base.fCaps)
		achp->fCaps = orig.fCaps;
	if (achp->ftc == base.ftc)
		achp->ftc = orig.ftc;
	if (achp->hps == base.hps)
		achp->hps = orig.hps;
	if (achp->hpsPos == base.hpsPos)
		achp->hpsPos = orig.hpsPos;
	if (achp->kul == base.kul)
		achp->kul = orig.kul;
	/* ???? 
	if (achp->qpsSpace == base.qpsSpace)
		achp->qpsSpace = orig.qpsSpace;
	*/
	if (achp->ico == base.ico)
		achp->ico = orig.ico;

	/* 
	these ones are mentioned in a different part of the spec, that
	doesnt have as much weight as the above, but i'm going to add them
	anyway
	*/
	if (achp->fVanish == base.fVanish)
		achp->fVanish = orig.fVanish;
	wvTrace(("%d\n",base.dxaSpace));
	wvTrace(("%d\n",achp->dxaSpace));
	if (achp->dxaSpace == base.dxaSpace)
		achp->dxaSpace = orig.dxaSpace;
	if (achp->lidDefault == base.lidDefault)
		achp->lidDefault = orig.lidDefault;
	if (achp->lidFE == base.lidFE)
		achp->lidFE = orig.lidFE;
	wvFree(upxf.upx.chpx.grpprl);


	wvTrace(("my underline ended as %d\n",achp->kul));
	}

void wvApplysprmCHpsInc1(CHP *achp,U8 *pointer,U16 *pos)
	{
	/*
	This sprm is interpreted by adding the two byte increment
	stored as the opcode of the sprm to chp.hps. If this result is less than 8,
	the chp.hps is set to 8. If the result is greater than 32766, the chp.hps is
	set to 32766.
	*/
	dgetc(NULL,&pointer);
	(*pos)++;
	achp->hps += dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	if (achp->hps < 8)
		achp->hps=8;
	else if (achp->hps > 32766)
		achp->hps=32766;
	}


void wvApplysprmCMajority50(CHP *achp,STSH *stsh,U8 *pointer,U16 *pos)
	{
	U16 i;
	CHP base;
	CHP orig;
	UPXF upxf;
	/*
	Bytes 0 and 1 of
	sprmCMajority contains the opcode, byte 2 contains the length of the
	following list of character sprms. . Word begins interpretation of this sprm
	by applying the stored character sprm list to a standard chp. That chp has
	chp.istd = istdNormalChar. chp.hps=20, chp.lid=0x0400 and chp.ftc = 4. Word
	then compares fBold, fItalic, fStrike, fOutline, fShadow, fSmallCaps, fCaps,
	ftc, hps, hpsPos, kul, qpsSpace and ico in the original CHP with the values
	recorded for these fields in the generated CHP.. If a field in the original
	CHP has the same value as the field stored in the generated CHP, then that
	field is reset to the value stored in the style's CHP. If the two copies
	differ, then the original CHP value is left unchanged.
	*/
	wvTrace(("This document has a sprm (sprmCMajority50), that ive never seen in practice please mail "));
	wvTrace(("Caolan.McNamara@ul.ie with this document, as i haven't been able to "));
	wvTrace(("get any examples of it so as to figure out if its handled correctly\n"));
	
	wvInitCHP(&base);
	base.ftc=4;

	/*generate a UPE and run wvAddCHPXFromBucket*/

	upxf.cbUPX = dgetc(NULL,&pointer);
	(*pos)++;
	upxf.upx.chpx.grpprl = (U8 *)malloc(upxf.cbUPX);

	for (i=0;i<upxf.cbUPX;i++)
		{
		upxf.upx.chpx.grpprl[i] = dgetc(NULL,&pointer);
		(*pos)++;
		}
	
	wvAddCHPXFromBucket(&base,&upxf,stsh);

	wvInitCHPFromIstd(&orig,achp->istd,stsh);

	/* this might be a little wrong, review after doing dedicated CHP's*/
	wvTrace(("istd is %d\n",achp->istd));
	if (achp->fBold == base.fBold)
		achp->fBold = orig.fBold;
	if (achp->fItalic == base.fItalic)
		achp->fItalic = orig.fItalic;
	if (achp->fStrike == base.fStrike)
		achp->fStrike = orig.fStrike;
	if (achp->fSmallCaps == base.fSmallCaps)
		achp->fSmallCaps = orig.fSmallCaps;
	if (achp->fCaps == base.fCaps)
		achp->fCaps = orig.fCaps;
	if (achp->ftc == base.ftc)
		achp->ftc = orig.ftc;
	if (achp->hps == base.hps)
		achp->hps = orig.hps;
	if (achp->hpsPos == base.hpsPos)
		achp->hpsPos = orig.hpsPos;
	if (achp->kul == base.kul)
		achp->kul = orig.kul;
	if (achp->ico == base.ico)
		achp->ico = orig.ico;
	if (achp->fVanish == base.fVanish)
		achp->fVanish = orig.fVanish;
	if (achp->dxaSpace == base.dxaSpace)
		achp->dxaSpace = orig.dxaSpace;
	}

void wvApplysprmCPropRMark(CHP *achp,U8 *pointer,U16 *pos)
	{
	dgetc(NULL,&pointer);	/*len*/
	(*pos)++;
	achp->fPropRMark = dgetc(NULL,&pointer); 
	(*pos)++;
	achp->ibstPropRMark = (S16)dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	wvGetDTTMFromBucket(&achp->dttmPropRMark,pointer);
	(*pos)+=4;
	}

void wvApplysprmCDispFldRMark(CHP *achp,U8 *pointer,U16 *pos)
	{
	/*
	is interpreted by moving the first
	parameter byte to chp.fDispFldRMark, the next two bytes to
	chp.ibstDispFldRMark, the next four bytes to chp.dttmDispFldRMark, 
	and the remaining 32 bytes to chp.xstDispFldRMark.
	*/

	int i;
	dgetc(NULL,&pointer);    /*len*/
	(*pos)++;
	achp->fDispFldRMark = dgetc(NULL,&pointer);
	(*pos)++;
	achp->ibstDispFldRMark = (S16)dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	wvGetDTTMFromBucket(&achp->dttmDispFldRMark,pointer);
	(*pos)+=4;
	pointer+=4;
	for (i=0;i<16;i++)
		{
		achp->xstDispFldRMark[i] = dread_16ubit(NULL,&pointer);
		(*pos)+=2;
		}
	}


void wvApplysprmSOlstAnm(int version,SEP *asep,U8 *pointer,U16 *pos)
	{
	U8 len = dgetc(NULL,&pointer);
	wvGetOLSTFromBucket(version,&asep->olstAnm,pointer);
	if (len != cbOLST)
		wvError(("OLST len is different from expected\n"));
	(*pos)+=len;
	}

void wvApplysprmSPropRMark(SEP *asep,U8 *pointer,U16 *pos)
	{
	dgetc(NULL,&pointer);
	/*
	sprmPPropRMark is interpreted by moving the first parameter
	byte to pap.fPropRMark, the next two bytes to pap.ibstPropRMark, and the
	remaining four bytes to pap.dttmPropRMark.
	*/
	asep->fPropRMark = dgetc(NULL,&pointer);
	(*pos)++;
	asep->ibstPropRMark = dread_16ubit(NULL,&pointer);
	(*pos)+=2;
	wvGetDTTMFromBucket(&asep->dttmPropRMark,pointer);
	(*pos)+=4;
	}


SprmName rgsprmPrm[0x80] = 
{sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmPIncLvl, sprmPJc,
sprmPFSideBySide, sprmPFKeep, sprmPFKeepFollow, sprmPFPageBreakBefore,
sprmPBrcl, sprmPBrcp, sprmPIlvl, sprmNoop, sprmPFNoLineNumb, sprmNoop,
sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
sprmNoop, sprmPFInTable, sprmPFTtp, sprmNoop, sprmNoop, sprmNoop, sprmPPc,
sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
sprmPWr, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
sprmPFNoAutoHyph, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
sprmPFLocked, sprmPFWidowControl, sprmNoop, sprmPFKinsoku, sprmPFWordWrap,
sprmPFOverflowPunct, sprmPFTopLinePunct, sprmPFAutoSpaceDE,
sprmPFAutoSpaceDN, sprmNoop, sprmNoop, sprmPISnapBaseLine, sprmNoop,
sprmNoop, sprmNoop, sprmCFStrikeRM, sprmCFRMark, sprmCFFldVanish, sprmNoop,
sprmNoop, sprmNoop, sprmCFData, sprmNoop, sprmNoop, sprmNoop, sprmCFOle2,
sprmNoop, sprmCHighlight, sprmCFEmboss, sprmCSfxText, sprmNoop, sprmNoop,
sprmNoop, sprmCPlain, sprmNoop, sprmCFBold, sprmCFItalic, sprmCFStrike,
sprmCFOutline, sprmCFShadow, sprmCFSmallCaps, sprmCFCaps, sprmCFVanish,
sprmNoop, sprmCKul, sprmNoop, sprmNoop, sprmNoop, sprmCIco, sprmNoop,
sprmCHpsInc, sprmNoop, sprmCHpsPosAdj, sprmNoop, sprmCIss, sprmNoop,
sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
sprmNoop, sprmNoop, sprmCFDStrike, sprmCFImprint, sprmCFSpec, sprmCFObj,
sprmPicBrcl, sprmPOutLvl, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
sprmPPnbrRMarkNot};

SprmName wvGetrgsprmPrm(U16 in)
	{
	if (in > 0x80)
		{
		wvError(("Impossible rgsprmPrm value\n"));
		return(sprmNoop);
		}
	return(rgsprmPrm[in]);
	}


SprmName rgsprmWord6[256] = 
	{
	sprmNoop/*          0*/,
	sprmNoop/*		    1*/,
	sprmPIstd/*         2*/,
	sprmPIstdPermute/*  3*/,
	sprmPIncLvl/*       4*/,
	sprmPJc/*           5*/,
	sprmPFSideBySide/*  6*/,
	sprmPFKeep/*        7*/,
	sprmPFKeepFollow/*  8*/,
	sprmPFPageBreakBefore/*  9*/,	/* added F */
	sprmPBrcl/*         10*/,
	sprmPBrcp/*         11*/,
	sprmPAnld/*         12*/,
	sprmPNLvlAnm/*      13*/,
	sprmPFNoLineNumb/*  14*/,
	sprmPChgTabsPapx/*  15*/,
	sprmPDxaRight/*     16*/,
	sprmPDxaLeft/*      17*/,
	sprmPNest/*         18*/,
	sprmPDxaLeft1/*     19*/,
	sprmPDyaLine/*      20*/,
	sprmPDyaBefore/*    21*/,
	sprmPDyaAfter/*     22*/,
	sprmPChgTabs/*      23*/,
	sprmPFInTable/*     24*/,
	sprmPFTtp/*         25*/,	/* added F */
	sprmPDxaAbs/*       26*/,
	sprmPDyaAbs/*       27*/,
	sprmPDxaWidth/*     28*/,
	sprmPPc/*           29*/,
	sprmPBrcTop10/*     30*/,
	sprmPBrcLeft10/*    31*/,
	sprmPBrcBottom10/*  32*/,
	sprmPBrcRight10/*   33*/,
	sprmPBrcBetween10/* 34*/,
	sprmPBrcBar10/*     35*/,
	sprmPDxaFromText10/*   36*/,	/* new name */
	sprmPWr/*           37*/,
	sprmPBrcTop/*       38*/,
	sprmPBrcLeft/*      39*/,
	sprmPBrcBottom/*    40*/,
	sprmPBrcRight/*     41*/,
	sprmPBrcBetween/*   42*/,
	sprmPBrcBar/*       43*/,
	sprmPFNoAutoHyph/*  44*/,
	sprmPWHeightAbs/*   45*/,
	sprmPDcs/*          46*/,
	sprmPShd/*          47*/,
	sprmPDyaFromText/*  48*/,
	sprmPDxaFromText/*  49*/,
	sprmPFLocked/*      50*/,
	sprmPFWidowControl/*  51*/,
	sprmNoop/*          52*/,
	sprmNoop/*          53*/,
	sprmNoop/*          54*/,
	sprmNoop/*          55*/,
	sprmNoop/*          56*/,
	sprmNoop/*          57*/,
	sprmNoop/*          58*/,
	sprmNoop/*          59*/,
	sprmNoop/*          60*/,
	sprmNoop/*          61*/,
	sprmNoop/*          62*/,
	sprmNoop/*          63*/,
	sprmNoop/*          64*/,
	sprmCFStrikeRM/*    65*/,
	sprmCFRMark/*       66*/,
	sprmCFFldVanish/*   67*/,
	sprmCPicLocation/*  68*/,
	sprmCIbstRMark/*    69*/,
	sprmCDttmRMark/*    70*/,
	sprmCFData/*        71*/,
	sprmCIdslRMark/*     72*/,	/* new name */
	sprmCChs/*         73*/,	/* new name */
	sprmCSymbol/*       74*/,
	sprmCFOle2/*        75*/,
	sprmNoop/*          76*/,
	sprmNoop/*          77*/,
	sprmNoop/*          78*/,
	sprmNoop/*          79*/,
	sprmCIstd/*         80*/,
	sprmCIstdPermute/*  81*/,
	sprmCDefault/*      82*/,
	sprmCPlain/*        83*/,
	sprmNoop/*          84*/,
	sprmCFBold/*        85*/,
	sprmCFItalic/*      86*/,
	sprmCFStrike/*      87*/,
	sprmCFOutline/*     88*/,
	sprmCFShadow/*      89*/,
	sprmCFSmallCaps/*   90*/,
	sprmCFCaps/*        91*/,
	sprmCFVanish/*      92*/,
	sprmCFtc/*          93*/,
	sprmCKul/*          94*/,
	sprmCSizePos/*      95*/,
	sprmCDxaSpace/*     96*/,
	sprmCLid/*          97*/,
	sprmCIco/*          98*/,
	sprmCHps/*          99*/,
	sprmCHpsInc/*       100*/,
	sprmCHpsPos/*       101*/,
	sprmCHpsPosAdj/*    102*/,
	sprmCMajority/*     103*/,
	sprmCIss/*          104*/,
	sprmCHpsNew50/*     105*/,
	sprmCHpsInc1/*      106*/,
	sprmCHpsKern/*      107*/,
	sprmCMajority50/*   108*/,
	sprmCHpsMul/*       109*/,
	sprmCYsri/*    		110*/,	/* new name */
	sprmNoop/*          111*/,
	sprmNoop/*          112*/,
	sprmNoop/*          113*/,
	sprmNoop/*          114*/,
	sprmNoop/*          115*/,
	sprmNoop/*          116*/,
	sprmCFSpec/*        117*/,
	sprmCFObj/*         118*/,
	sprmPicBrcl/*       119*/,
	sprmPicScale/*      120*/,
	sprmPicBrcTop/*     121*/,
	sprmPicBrcLeft/*    122*/,
	sprmPicBrcBottom/*  123*/,
	sprmPicBrcRight/*   124*/,
	sprmNoop/*          125*/,
	sprmNoop/*          126*/,
	sprmNoop/*          127*/,
	sprmNoop/*          128*/,
	sprmNoop/*          129*/,
	sprmNoop/*          130*/,
	sprmScnsPgn/*      	131*/,	/* new name */
	sprmSiHeadingPgn/*  132*/,
	sprmSOlstAnm/*      133*/,
	sprmNoop/*          134*/,
	sprmNoop/*          135*/,
	sprmSDxaColWidth/*  136*/,
	sprmSDxaColWidth/*  137*/,	/* new name */
	sprmSFEvenlySpaced/*138*/,
	sprmSFProtected/*   139*/,
	sprmSDmBinFirst/*   140*/,
	sprmSDmBinOther/*   141*/,
	sprmSBkc/*          142*/,
	sprmSFTitlePage/*   143*/,
	sprmSCcolumns/*     144*/,
	sprmSDxaColumns/*   145*/,
	sprmSFAutoPgn/*     146*/,
	sprmSNfcPgn/*       147*/,
	sprmSDyaPgn/*       148*/,
	sprmSDxaPgn/*       149*/,
	sprmSFPgnRestart/*  150*/,
	sprmSFEndnote/*     151*/,
	sprmSLnc/*          152*/,
	sprmSGprfIhdt/*     153*/,
	sprmSNLnnMod/*      154*/,
	sprmSDxaLnn/*       155*/,
	sprmSDyaHdrTop/*    156*/,
	sprmSDyaHdrBottom/* 157*/,
	sprmNoop/*          158*/,
	sprmSVjc/*          159*/,
	sprmSLnnMin/*       160*/,
	sprmSPgnStart/*     161*/,
	sprmSBOrientation/* 162*/,
	sprmSBCustomize/*   163*/,
	sprmSXaPage/*       164*/,
	sprmSYaPage/*       165*/,
	sprmSDxaLeft/*      166*/,
	sprmSDxaRight/*     167*/,
	sprmSDyaTop/*       168*/,
	sprmSDyaBottom/*    169*/,
	sprmSDzaGutter/*    170*/,
	sprmSDmPaperReq/*   171*/,
	sprmNoop/*          172*/,
	sprmNoop/*          173*/,
	sprmNoop/*          174*/,
	sprmNoop/*          175*/,
	sprmNoop/*          176*/,
	sprmNoop/*          177*/,
	sprmNoop/*          178*/,
	sprmNoop/*          179*/,
	sprmNoop/*          180*/,
	sprmNoop/*          181*/,
	sprmTJc/*           182*/,
	sprmTDxaLeft/*      183*/,
	sprmTDxaGapHalf/*   184*/,
	sprmTFCantSplit/*   185*/,
	sprmTTableHeader/*  186*/,
	sprmTTableBorders/* 187*/,
	sprmTDefTable10/*   188*/,
	sprmTDyaRowHeight/* 189*/,
	sprmTDefTable/*     190*/,
	sprmTDefTableShd/*  191*/,
	sprmTTlp/*          192*/,
	sprmTSetBrc/*       193*/,
	sprmTInsert/*       194*/,
	sprmTDelete/*       195*/,
	sprmTDxaCol/*       196*/,
	sprmTMerge/*        197*/,
	sprmTSplit/*        198*/,
	sprmTSetBrc10/*     199*/,
	sprmTSetShd/*       200*/,
#if 0
	sprmNoop/*          201*/,
	sprmNoop/*          202*/,
	sprmNoop/*          203*/,
	sprmNoop/*          204*/,
	sprmNoop/*          205*/,
	sprmNoop/*          206*/,
	sprmNoop/*          207*/,
	sprmMax/*           208*/
#endif
	};

SprmName wvGetrgsprmWord6(U8 in)
	{
	return(rgsprmWord6[in]);
	}
