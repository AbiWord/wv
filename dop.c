#include <string.h>
#include <stdio.h>
#include "wv.h"

DOP dop;

void wvGetCOPTS(COPTS *item,FILE *fd)
	{
	U16 temp16;
	temp16 = read_16ubit(fd);

	item->fNoTabForInd1  = temp16 & 0x0001;
    item->fNoSpaceRaiseLower = (temp16 & 0x0002) >> 1;
    item->fSuppressSpbfAfterPageBreak = (temp16 & 0x0004) >> 2;
    item->fWrapTrailSpaces = (temp16 & 0x0008) >> 3;
    item->fMapPrintTextColor = (temp16 & 0x0010) >> 4;
    item->fNoColumnBalance = (temp16 & 0x0020) >> 5;
    item->fConvMailMergeEsc = (temp16 & 0x0040) >> 6;
    item->fSupressTopSpacing = (temp16 & 0x0080) >> 7;
    item->fOrigWordTableRules = (temp16 & 0x0100) >> 8;
    item->fTransparentMetafiles = (temp16 & 0x0200) >> 9;
    item->fShowBreaksInFrames = (temp16 & 0x0400) >> 10;
    item->fSwapBordersFacingPgs = (temp16 & 0x800) >> 11;
    item->reserved = (temp16 & 0xf000) >> 12;
	}


void wvGetDOP(DOP *dop,U32 fcDop,U32 lcbDop,FILE *fd)
	{
	U16 temp16;
	U32 temp32;
	int i;

	if (lcbDop <= 0)
		return;
	fseek(fd,fcDop,SEEK_SET);

	temp16 = read_16ubit(fd);

	dop->fFacingPages = temp16&0x0001;
	dop->fWidowControl = (temp16&0x0002)>>1;
	dop->fPMHMainDoc = (temp16&0x0004)>>2;
	dop->grfSuppression = (temp16&0x18)>>3;
	dop->fpc = (temp16&0x0060)>>5;
	error(stderr,"fpc is %d\n",dop->fpc);
	dop->reserved1 = (temp16&0x0080)>>7;
	dop->grpfIhdt = (temp16&0xFF00)>>8;
	
	temp16 = read_16ubit(fd);

	dop->rncFtn = temp16&0x0003;
	dop->nFtn = (temp16&0xFFFC)>>2;

	temp16 = read_16ubit(fd);

	dop->fOutlineDirtySave = temp16&0x0001;
	dop->reserved2 = (temp16&0x00FE)>>1;
	dop->fOnlyMacPics = (temp16&0x0100)>>8;
	dop->fOnlyWinPics = (temp16&0x0200)>>9;
	dop->fLabelDoc = (temp16&0x0400)>>10;
	dop->fHyphCapitals = (temp16&0x0800)>>11;
	dop->fAutoHyphen = (temp16&0x1000)>>12;
	dop->fFormNoFields = (temp16&0x2000)>>13;
	dop->fLinkStyles = (temp16&0x4000)>>14;
	dop->fRevMarking = (temp16&0x8000)>>15;

	temp16 = read_16ubit(fd);

	dop->fBackup = temp16&0x0001;
	dop->fExactCWords = (temp16&0x0002)>>1;
	dop->fPagHidden = (temp16&0x0004)>>2;
	dop->fPagResults = (temp16&0x0008)>>3;
	dop->fLockAtn = (temp16&0x0010)>>4;
	dop->fMirrorMargins = (temp16&0x0020)>>5;
	dop->reserved3 = (temp16&0x0040)>>6;
	dop->fDfltTrueType = (temp16&0x0080)>>7;
	dop->fPagSuppressTopSpacing = (temp16&0x0100)>>8;
	dop->fProtEnabled = (temp16&0x0200)>>9;
	dop->fDispFormFldSel = (temp16&0x0400)>>10;
	dop->fRMView = (temp16&0x0800)>>11;
	dop->fRMPrint = (temp16&0x1000)>>12;
	dop->reserved4 = (temp16&0x2000)>>13;
	dop->fLockRev = (temp16&0x4000)>>14;
	dop->fEmbedFonts = (temp16&0x8000)>>15;


	/*
	not used in word 8 as far as i know, but are in previous versions, 
	*/
	wvGetCOPTS(&dop->copts,fd);

	dop->dxaTab = read_16ubit(fd);
	dop->wSpare = read_16ubit(fd);
	dop->dxaHotZ = read_16ubit(fd);
	dop->cConsecHypLim = read_16ubit(fd);
	dop->wSpare2 = read_16ubit(fd);

	wvGetDTTM(&dop->dttmCreated,fd);
	wvGetDTTM(&dop->dttmRevised,fd);
	wvGetDTTM(&dop->dttmLastPrint,fd);

	dop->nRevision = read_16ubit(fd);
	dop->tmEdited = read_32ubit(fd);
	dop->cWords = read_32ubit(fd);
	dop->cCh = read_32ubit(fd);
	dop->cPg = read_16ubit(fd);
	dop->cParas = read_32ubit(fd);

	temp16 = read_16ubit(fd);
	
	dop->rncEdn = temp16&0x0003;
	dop->nEdn = (temp16&0xFFFC)>>2;

	temp16 = read_16ubit(fd);

	dop->epc = temp16&0x0003;
	dop->nfcFtnRef = (temp16&0x003C)>>2;
	dop->nfcEdnRef = (temp16&0x03C0)>>6;
	dop->fPrintFormData = (temp16&0x0400)>>10;
	dop->fSaveFormData = (temp16&0x0800)>>11;
	dop->fShadeFormData = (temp16&0x1000)>>12;
	dop->reserved6 = (temp16&0x6000)>>13;
	dop->fWCFtnEdn = (temp16&0x8000)>>15;

	dop->cLines = read_32ubit(fd);
	dop->cWordsFtnEnd = read_32ubit(fd);
	dop->cChFtnEdn = read_32ubit(fd);
	dop->cPgFtnEdn = read_16ubit(fd);
	dop->cParasFtnEdn = read_32ubit(fd);
	dop->cLinesFtnEdn = read_32ubit(fd);
	dop->lKeyProtDoc = read_32ubit(fd);

	
	temp16 = read_16ubit(fd);

	dop->wvkSaved = temp16&0x0007;
	dop->wScaleSaved = (temp16&0x0FF8)>>3;
	dop->zkSaved = (temp16&0x3000)>>12;
	dop->fRotateFontW6 = (temp16&0x4000)>>14;
	dop->iGutterPos = (temp16&0x8000)>>15;

	temp32 = read_32ubit(fd);

	dop->fNoTabForInd = temp32&0x00000001;
	dop->fNoSpaceRaiseLower = (temp32&0x00000002)>>1;
	dop->fSupressSpbfAfterPageBreak = (temp32&0x00000004)>>2;
	dop->fWrapTrailSpaces = (temp32&0x00000008)>>3;
	dop->fMapPrintTextColor = (temp32&0x00000010)>>4;
	dop->fNoColumnBalance = (temp32&0x00000020)>>5;
	dop->fConvMailMergeEsc = (temp32&0x00000040)>>6;
	dop->fSupressTopSpacing = (temp32&0x00000080)>>7;
	dop->fOrigWordTableRules = (temp32&0x00000100)>>8;
	dop->fTransparentMetafiles = (temp32&0x00000200)>>9;
	dop->fShowBreaksInFrames = (temp32&0x00000400)>>10;
	dop->fSwapBordersFacingPgs = (temp32&0x00000800)>>11;
	dop->reserved7 = (temp32&0x0000F000)>>12;
	dop->fSuppressTopSpacingMac5 = (temp32&0x00010000)>>16;
	dop->fTruncDxaExpand  = (temp32&0x00020000)>>17;
	dop->fPrintBodyBeforeHdr = (temp32&0x00040000)>>18;
	dop->fNoLeading = (temp32&0x00080000)>>19;
	dop->reserved8 = (temp32&0x00100000)>>20;
	dop->fMWSmallCaps = (temp32&0x00200000)>>21;
	dop->reserved9 = (temp32&0xFFC00000)>>22;

	dop->adt = read_16ubit(fd);
	wvGetDOPTYPOGRAPHY(&dop->doptypography,fd);
	wvGetDOGRID(&dop->dogrid,fd);

	temp16 = read_16ubit(fd);

	dop->reserved10 = temp16&0x0001;
	dop->lvl = (temp16&0x001E)>>1;
	dop->fGramAllDone = (temp16&0x0020)>>5;
	dop->fGramAllClean = (temp16&0x0040)>>6;
	dop->fSubsetFonts = (temp16&0x0080)>>7;
	dop->fHideLastVersion = (temp16&0x0100)>>8;
	dop->fHtmlDoc = (temp16&0x0200)>>9;
	dop->reserved11 = (temp16&0x0400)>>10;
	dop->fSnapBorder = (temp16&0x0800)>>11;
	dop->fIncludeHeader = (temp16&0x1000)>>12;
	dop->fIncludeFooter = (temp16&0x2000)>>13;
	dop->fForcePageSizePag = (temp16&0x4000)>>14;
	dop->fMinFontSizePag = (temp16&0x8000)>>15;

	temp16 = read_16ubit(fd);

	dop->fHaveVersions = temp16&0x0001;
	dop->fAutoVersion = (temp16&0x0002)>>1;
	dop->reserved11 = (temp16&0xFFFC)>>2;

	wvGetASUMYI(&dop->asumyi,fd);

	dop->cChWS = read_32ubit(fd);
	dop->cChWSFtnEdn = read_32ubit(fd);
	dop->grfDocEvents = read_32ubit(fd);

	temp32 = read_32ubit(fd);

	dop->fVirusPrompted = temp32&0x00000001;
	dop->fVirusLoadSafe = (temp32&0x00000002)>>1;
	dop->KeyVirusSession30 = (temp32&0xFFFFFFFC)>>2;
	
	for(i=0;i<30;i++)
		dop->Spare[i] = getc(fd);

	dop->reserved12 = read_32ubit(fd);
	dop->reserved13 = read_32ubit(fd);
	dop->cDBC = read_32ubit(fd);
	dop->cDBCFtnEdn = read_32ubit(fd);
	dop->reserved14 = read_32ubit(fd);
	dop->new_nfcFtnRef = read_16ubit(fd);
	dop->new_nfcEdnRef = read_16ubit(fd);
	dop->hpsZoonFontPag = read_16ubit(fd);
	dop->dywDispPag = read_16ubit(fd);

	/*
	if ((dop->fLockAtn) || (dop->fLockRev))
		fprintf(stderr,"doc protection key is %x\n",dop->lKeyProtDoc);
	*/
	}

void wvGetDOPTYPOGRAPHY(DOPTYPOGRAPHY *dopt,FILE *fd)
	{
	int i;
	U16 temp16=read_16ubit(fd);
	
	dopt->fKerningPunct = temp16&0x0001;
	dopt->iJustification = (temp16&0x0006)>>1;
	dopt->iLevelOfKinsoku = (temp16&0x0018)>>3;
	dopt->f2on1 = (temp16&0x0020)>>5;
	dopt->reserved = (temp16&0xFFC0)>>6;

	dopt->cchFollowingPunct = read_16ubit(fd);
	dopt->cchLeadingPunct = read_16ubit(fd);

	for (i=0;i<101;i++)
		dopt->rgxchFPunct[i] = read_16ubit(fd);

	for (i=0;i<51;i++)
		dopt->rgxchLPunct[i] = read_16ubit(fd);
	}


void wvGetDOGRID(DOGRID *dog,FILE *fd)
	{
	U16 temp16;
	dog->xaGrid = read_16ubit(fd);
	dog->yaGrid = read_16ubit(fd);
	dog->dxaGrid = read_16ubit(fd);
	dog->dyaGrid = read_16ubit(fd);

	temp16 = read_16ubit(fd);

	dog->dyGridDisplay = temp16&0x007F;
	dog->fTurnItOff = (temp16&0x0080)>>7;
	dog->dxGridDisplay = (temp16&0x7F00)>>8;
	dog->fFollowMargins = (temp16&0x8000)>>15;
	}
