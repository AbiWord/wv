
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "wv.h"

extern int NORMAL;
extern int lastrowlen;
extern S16 lastcellwidth[65];
extern int nostyles;
extern FILE *erroroutput;

void copy_tap(tap *rettap,tap *intap)
	{
	int i;
	rettap->tlp.itl=intap->tlp.itl;
	rettap->tlp.fShading=intap->tlp.fShading;
	rettap->tlp.fColor=intap->tlp.fColor;
	rettap->tlp.fHdrRows=intap->tlp.fHdrRows;
	rettap->tlp.fLastRow=intap->tlp.fLastRow;
	rettap->tlp.fHdrCols=intap->tlp.fHdrCols;
	rettap->tlp.fLastCol=intap->tlp.fLastCol;
	rettap->tablewidth = intap->tablewidth;
	rettap->cell_no = intap->cell_no;
	rettap->rowheight=intap->rowheight;
	rettap->shade_no=intap->shade_no;

	for (i=0;i<65;i++)
		{
		rettap->cell_backs[i] = intap->cell_backs[i];
		rettap->cell_fronts[i] = intap->cell_fronts[i];
		rettap->cell_pattern[i] = intap->cell_pattern[i];
		rettap->cellwidth[i] = intap->cellwidth[i];
		}
	}


void init_pap(pap * retpap)
	{
	int i;
	retpap->fInTable = 0;
    retpap->fTtp= 0;
	retpap->justify = 0;
    retpap->ilvl = -1;
    retpap->ilfo = -1; /*index into hpllfo*/
    retpap->list_data=NULL;
    retpap->tableflag=0;
    retpap->leftmargin=0;
    retpap->rightmargin=0;
    retpap->firstline=0;
	retpap->istd=0;
	retpap->ourtap.tlp.itl=0;
	retpap->ourtap.tlp.fShading=0;
	retpap->ourtap.tlp.fColor=0;
	retpap->ourtap.tlp.fHdrRows=0;
	retpap->ourtap.tlp.fLastRow=0;
	retpap->ourtap.tlp.fHdrCols=0;
	retpap->ourtap.tlp.fLastCol=0;
    retpap->ourtap.tablewidth = 0;
	retpap->ourtap.cell_no = lastrowlen;
    retpap->ourtap.rowheight=0;
    retpap->ourtap.shade_no=0;
    for (i=0;i<65;i++)
        {
        retpap->ourtap.cell_backs[i] = 0;
        retpap->ourtap.cell_fronts[i] = 0;
        retpap->ourtap.cell_pattern[i] = 0;
        }
    if (lastrowlen > 63)	
	lastrowlen=63;
    for (i=0;i<lastrowlen+1;i++)
        {
        retpap->ourtap.cellwidth[i] = lastcellwidth[i];
        retpap->ourtap.tablewidth+=lastcellwidth[i];
        }

	retpap->brcBottom=0;
	retpap->brcLeft=0;
	retpap->brcRight=0;
	retpap->brcBetween=0;
	retpap->dxaWidth=0;
	retpap->dyaBefore=0;
	retpap->dyaAfter=0;
	retpap->begin=NULL;
	retpap->end=NULL;
	retpap->prespace=NULL;
	retpap->postspace=NULL;
	}


void init_pap_from_istd(U16 istd,style *sheet,pap *retpap)
	{
	retpap->fInTable  = sheet[retpap->istd].thepap.fInTable;
    retpap->fTtp = sheet[retpap->istd].thepap.fTtp;
    retpap->ilvl = sheet[retpap->istd].thepap.ilvl;
    retpap->ilfo = sheet[retpap->istd].thepap.ilfo;
    retpap->justify= sheet[retpap->istd].thepap.justify;
    retpap->list_data = sheet[retpap->istd].thepap.list_data;
    retpap->anld = sheet[retpap->istd].thepap.anld;
    retpap->leftmargin = sheet[retpap->istd].thepap.leftmargin;
    retpap->rightmargin = sheet[retpap->istd].thepap.rightmargin;
    retpap->firstline = sheet[retpap->istd].thepap.firstline;
    retpap->tableflag= sheet[retpap->istd].thepap.tableflag;
    retpap->brcBottom= sheet[retpap->istd].thepap.brcBottom;
    retpap->brcLeft= sheet[retpap->istd].thepap.brcLeft;
    retpap->brcRight= sheet[retpap->istd].thepap.brcRight;
    retpap->brcBetween= sheet[retpap->istd].thepap.brcBetween;
	retpap->dxaWidth=sheet[retpap->istd].thepap.dxaWidth;
	retpap->dyaAfter=sheet[retpap->istd].thepap.dyaAfter;
	retpap->dyaBefore=sheet[retpap->istd].thepap.dyaBefore;
	retpap->begin=sheet[retpap->istd].begin;
	retpap->end=sheet[retpap->istd].end;
	retpap->prespace=sheet[retpap->istd].prespace;
	retpap->postspace=sheet[retpap->istd].postspace;
	copy_tap(&(retpap->ourtap),&(sheet[retpap->istd].thepap.ourtap));
	}

void init_chp(chp * achp)
    {
    achp->istd=10;

    achp->fBold=0;
    achp->fItalic=0;
	achp->fRMarkDel=0;
	achp->fOutline=0;
	achp->fFldVanish=0;
    achp->fSmallCaps=0;
    achp->fCaps=0;
	achp->fVanish=0;

    achp->fRMark=0;
    achp->fSpec=0;
	achp->fStrike=0;
    achp->fObj=0;
	achp->fShadow=0;
	achp->fLowerCase=0;
	achp->fData=0;
    achp->fOle2=0;

	achp->fEmboss=0;
	achp->fImprint=0;
	achp->fDStrike=0;
	achp->fUsePgsuSettings=0;
	achp->Reserved1=0;

	achp->Reserved2=0;

	achp->ftc=0;
    achp->ftcAscii=0;
    achp->ftcFE=0;
    achp->ftcOther=0;

    achp->fontsize=NORMAL;
    achp->supersubscript=0;
    achp->fontcode=0;
    achp->fontspec=0;
    achp->color[0]='\0';
    achp->underline=0;
    achp->idctHint=0;
	achp->fcPic=-1;

	achp->ibstRMark=0;
	achp->ibstRMarkDel=0;

	achp->dttmRMark.mint=0;
	achp->dttmRMark.hr=0;
	achp->dttmRMark.dom=0;
	achp->dttmRMark.mon=0;
	achp->dttmRMark.yr=0;
	achp->dttmRMark.wdy=0;

	achp->dttmRMarkDel.mint=0;
	achp->dttmRMarkDel.hr=0;
	achp->dttmRMarkDel.dom=0;
	achp->dttmRMarkDel.mon=0;
	achp->dttmRMarkDel.yr=0;
	achp->dttmRMarkDel.wdy=0;

	achp->fPropRMark=0;
	achp->ibstPropRMark=0;
	achp->dttmPropRMark.mint=0;
	achp->dttmPropRMark.hr=0;
	achp->dttmPropRMark.dom=0;
	achp->dttmPropRMark.mon=0;
	achp->dttmPropRMark.yr=0;
	achp->dttmPropRMark.wdy=0;
	achp->sfxtText=0;
    }


void init_chp_from_istd(U16 istd,style *sheet,chp *retchp)
    {
	if (istd >= nostyles)
		{
		error(erroroutput,"istd greater than no of styles !!\n");
		return;
		}
	retchp->istd = istd;


	/*	
	If bold is set off its off, if the default is off and theres no bold then
	its off, otherwise if on.
	*/
	if (sheet[istd].bold != NULL) 
		{
		if (!(strcmp(sheet[istd].bold,"off")))
	        retchp->fBold=0;
		else
			retchp->fBold = sheet[istd].thechp.fBold;
		}
	else if (sheet[istd].Default != NULL)
		{
		if (!(strcmp(sheet[istd].Default,"off")))
			retchp->fBold=0;
		else
			retchp->fBold = sheet[istd].thechp.fBold;
		}
	else
		retchp->fBold = sheet[istd].thechp.fBold;

	if (sheet[istd].italic != NULL) 
		{
		if (!(strcmp(sheet[istd].italic,"off")))
	        retchp->fItalic=0;
		else
    		retchp->fItalic = sheet[istd].thechp.fItalic;
		}
	else if (sheet[istd].Default != NULL)
		{
		if (!(strcmp(sheet[istd].Default,"off")))
			retchp->fItalic=0;
		else
    		retchp->fItalic = sheet[istd].thechp.fItalic;
		}
	else
    	retchp->fItalic = sheet[istd].thechp.fItalic;

    retchp->fRMarkDel = sheet[istd].thechp.fRMarkDel;
    retchp->fOutline = sheet[istd].thechp.fOutline;
    retchp->fFldVanish = sheet[istd].thechp.fFldVanish;
    retchp->fSmallCaps = sheet[istd].thechp.fSmallCaps;
    retchp->fCaps = sheet[istd].thechp.fCaps;
    retchp->fVanish = sheet[istd].thechp.fVanish;

    retchp->fRMark=sheet[istd].thechp.fRMark;
    retchp->fSpec=sheet[istd].thechp.fSpec;
    retchp->fStrike=sheet[istd].thechp.fStrike;
    retchp->fObj=sheet[istd].thechp.fObj;
    retchp->fShadow=sheet[istd].thechp.fShadow;
    retchp->fLowerCase=sheet[istd].thechp.fLowerCase;
    retchp->fData=sheet[istd].thechp.fData;
    retchp->fOle2=sheet[istd].thechp.fOle2;

    retchp->fEmboss=sheet[istd].thechp.fEmboss;
    retchp->fImprint=sheet[istd].thechp.fImprint;
    retchp->fDStrike=sheet[istd].thechp.fDStrike;
    retchp->fUsePgsuSettings=sheet[istd].thechp.fUsePgsuSettings;
    retchp->Reserved1=sheet[istd].thechp.Reserved1;

    retchp->Reserved2=sheet[istd].thechp.Reserved2;

    retchp->ftc=sheet[istd].thechp.ftc;

    retchp->ftcAscii=sheet[istd].thechp.ftcAscii;
    retchp->ftcFE=sheet[istd].thechp.ftcFE;
    retchp->ftcOther=sheet[istd].thechp.ftcOther;
    retchp->fontsize=sheet[istd].thechp.fontsize;
    retchp->supersubscript=sheet[istd].thechp.supersubscript;
    retchp->fontcode=sheet[istd].thechp.fontcode;
    retchp->fontspec=sheet[istd].thechp.fontspec;
    strcpy(retchp->color,sheet[istd].thechp.color);
    retchp->underline=sheet[istd].thechp.underline; 
    retchp->idctHint=sheet[istd].thechp.idctHint;
    retchp->fcPic =sheet[istd].thechp.fcPic;

    retchp->ibstRMark=sheet[istd].thechp.ibstRMark;
    retchp->ibstRMarkDel=sheet[istd].thechp.ibstRMarkDel;


    retchp->dttmRMark=sheet[istd].thechp.dttmRMark;
    retchp->dttmRMarkDel=sheet[istd].thechp.dttmRMarkDel;


    retchp->fPropRMark=sheet[istd].thechp.fPropRMark;
    retchp->ibstPropRMark=sheet[istd].thechp.ibstPropRMark;
    retchp->dttmPropRMark=sheet[istd].thechp.dttmPropRMark;
    retchp->sfxtText =sheet[istd].thechp.sfxtText;
    }

void merge_chps(chp *blank,chp *modified,chp *result)
	{
	if (blank->fBold != modified->fBold) result->fBold = modified->fBold;
	if (blank->fItalic != modified->fItalic) result->fItalic = modified->fItalic;
	if (blank->fRMarkDel!= modified->fRMarkDel) result->fRMarkDel= modified->fRMarkDel;
	if (blank->fOutline!= modified->fOutline) result->fOutline= modified->fOutline;
	if (blank->fFldVanish!= modified->fFldVanish) result->fFldVanish= modified->fFldVanish;
	if (blank->fSmallCaps != modified->fSmallCaps) result->fSmallCaps= modified->fSmallCaps;
	if (blank->fCaps != modified->fCaps) result->fCaps= modified->fCaps;
	if (blank->fVanish != modified->fVanish) result->fVanish= modified->fVanish;

	if (blank->fRMark!= modified->fRMark) result->fRMark= modified->fRMark;
	if (blank->fSpec!= modified->fSpec) result->fSpec= modified->fSpec;
	if (blank->fStrike!= modified->fStrike) result->fStrike= modified->fStrike;
	if (blank->fObj!= modified->fObj) result->fObj= modified->fObj;
	if (blank->fShadow!= modified->fShadow) result->fShadow= modified->fShadow;
	if (blank->fLowerCase != modified->fLowerCase) result->fLowerCase= modified->fLowerCase;
	if (blank->fData!= modified->fData) result->fData= modified->fData;
	if (blank->fOle2!= modified->fOle2) result->fOle2= modified->fOle2;

	if (blank->fEmboss!= modified->fEmboss) result->fEmboss= modified->fEmboss;
	if (blank->fImprint!= modified->fImprint) result->fImprint= modified->fImprint;
	if (blank->fDStrike!= modified->fDStrike) result->fDStrike= modified->fDStrike;
	if (blank->fUsePgsuSettings!= modified->fUsePgsuSettings) result->fUsePgsuSettings= modified->fUsePgsuSettings;
	if (blank->Reserved1!= modified->Reserved1) result->Reserved1= modified->Reserved1;

	if (blank->Reserved2!= modified->Reserved2) result->Reserved2= modified->Reserved2;

	if (blank->ftc!= modified->ftc) result->ftc= modified->ftc;

	if (blank->ftcAscii!= modified->ftcAscii) result->ftcAscii= modified->ftcAscii;
	if (blank->ftcFE!= modified->ftcFE) result->ftcFE= modified->ftcFE;
	if (blank->ftcOther!= modified->ftcOther) result->ftcOther= modified->ftcOther;
	if (blank->fontsize!= modified->fontsize) result->fontsize= modified->fontsize;
	if (blank->supersubscript!= modified->supersubscript) result->supersubscript= modified->supersubscript;
	if (blank->fontcode!= modified->fontcode) result->fontcode= modified->fontcode;
	if (blank->fontspec!= modified->fontspec) result->fontspec= modified->fontspec;
	if (blank->underline!= modified->underline) result->underline= modified->underline;
	if (blank->idctHint!= modified->idctHint) result->idctHint= modified->idctHint;
	if (blank->fcPic!= modified->fcPic) result->fcPic= modified->fcPic;
	if (strcmp(blank->color,modified->color)) strcpy(result->color,modified->color);
	if (blank->sfxtText!= modified->sfxtText) result->sfxtText= modified->sfxtText;

	if (blank->ibstRMark != modified->ibstRMark) result->ibstRMark = modified->ibstRMark;
	if (blank->ibstRMarkDel != modified->ibstRMarkDel) result->ibstRMarkDel = modified->ibstRMarkDel;
/*
	if (blank->dttmRMark != modified->dttmRMark) result->dttmRMark = modified->dttmRMark;
	if (blank->dttmRMarkDel != modified->dttmRMarkDel) result->dttmRMarkDel = modified->dttmRMarkDel;
*/
	
	if (blank->fPropRMark != modified->fPropRMark) result->fPropRMark= modified->fPropRMark;
	if (blank->ibstPropRMark != modified->ibstPropRMark) result->ibstPropRMark= modified->ibstPropRMark;
	if (blank->sfxtText != modified->sfxtText) result->sfxtText= modified->sfxtText;
	}
