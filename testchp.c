void wvCopyCHP(CHP *dest)
	{
	int i;
	
	dest->fBold = src->fBold;
	dest->FItalic = src->FItalic;
	dest->fRMarkDel = src->fRMarkDel;
	dest->fOutline = src->fOutline;
	dest->fFldVanish = src->fFldVanish;
	dest->fSmallCaps = src->fSmallCaps;
	dest->fCaps = src->fCaps;
	dest->fVanish = src->fVanish;
	dest->fRMark = src->fRMark;
	dest->fSpec = src->fSpec;
	dest->fStrike = src->fStrike;
	dest->fObj = src->fObj;
	dest->fShadow = src->fShadow;
	dest->fLowerCase = src->fLowerCase;
	dest->fData = src->fData;
	dest->fOle2 = src->fOle2;
	dest->fEmboss = src->fEmboss;
	dest->fImprint = src->fImprint;
	dest->fDStrike = src->fDStrike;
	dest->fUsePgsuSettings = src->fUsePgsuSettings;
	dest->reserved1 = src->reserved1;
	dest->reserved2 = src->reserved2;
	dest->ftc = src->ftc;
	dest->ftcAscii = src->ftcAscii;
	dest->ftcFE = src->ftcFE;
	dest->ftcOther = src->ftcOther;
	dest->hps=20src->hps=20;
	dest->dxaSpace = src->dxaSpace;
	dest->iss = src->iss;
	dest->kul = src->kul;
	dest->fSpecSymbol = src->fSpecSymbol;
	dest->ico = src->ico;
	dest->reserved3 = src->reserved3;
	dest->fSysVanish = src->fSysVanish;
	dest->hpsPos = src->hpsPos;
	dest->super_sub = src->super_sub;
	dest->lid = src->lid;
	dest->lidDefault = x0400src->lidDefaultx0400;
	dest->lidFE = x0400src->lidFEx0400;
	dest->idct = src->idct;
	dest->idctHint = src->idctHint;
	dest->wCharScale = 100src->wCharScale;
	dest->fcPic_fcObj_lTagObj = -1src->fcPic_fcObj_lTagObj;
	dest->ibstRMark = src->ibstRMark;
	dest->ibstRMarkDel = src->ibstRMarkDel;

	wvCopyDTTM(&dest->dttmRMark,&src->dttmRMark);
	wvCopyDTTM(&dest->dttmRMarkDel,&src->dttmRMarkDel);

	dest->reserved4 = src->reserved4;
	dest->istd = istdNormalCharsrc->istd;
	dest->ftcSym = src->ftcSym;
	dest->xchSym = src->xchSym;
	dest->idslRMReason = src->idslRMReason;
	dest->idslReasonDel = src->idslReasonDel;
	dest->ysr = src->ysr;
	dest->chYsr = src->chYsr;
	dest->cpg = src->cpg;
	dest->hpsKern = src->hpsKern;
	dest->icoHighlight = src->icoHighlight;
	dest->fHighlight = src->fHighlight;
	dest->kcd = src->kcd;
	dest->fNavHighlight = src->fNavHighlight;
	dest->fChsDiff = src->fChsDiff;
	dest->fMacChs = src->fMacChs;
	dest->fFtcAsciSym = src->fFtcAsciSym;
	dest->reserved5 = src->reserved5;
	dest->fPropMark = src->fPropMark;
	dest->ibstPropRMark = src->ibstPropRMark;

	wvCopyDTTM(&dest->dttmPropRMark,&src->dttmPropRMark);

	dest->sfxtText = src->sfxtText;
	dest->reserved6 = src->reserved6;
	dest->reserved7 = src->reserved7;
	dest->reserved8 = src->reserved8;
	dest->reserved9 = src->reserved9;

	wvCopyDTTM(&dest->reserved10,&src->reserved10);

	dest->fDispFldRMark = src->fDispFldRMark;
	dest->ibstDispFldRMark = src->ibstDispFldRMark;

	wvCopyDTTM(&dest->dttmDispFldRMark,&src->dttmDispFldRMark);

	for (i=0;i<16;i++)
		dest->xstDispFldRMark[i] = src->xstDispFldRMark[i];

	wvCopySHD(&dest->shd,&src->shd);

	wvCopyBRC(&dest->brc,&src->brc);
	}
