typedef struct _SEP
	{
 	U8 bkc;
 	U8 fTitlePage;
	S8 fAutoPgn;
 	U8 nfcPgn;
 	U8 fUnlocked;
	U8 cnsPgn;
 	U8 fPgnRestart;
 	U8 fEndNote;
 	U8 lnc;
 	S8 grpfIhdt;
 	U16 nLnnMod;
 	S32 dxaLnn;
 	S16 dxaPgn;
 	S16 dyaPgn;
 	S8 fLBetween;
 	S8 vjc;
 	U16 dmBinFirst;
 	U16 dmBinOther;
 	U16 dmPaperReq;
 	BRC brcTop;
 	BRC brcLeft;
 	BRC brcBottom;
 	BRC brcRight;
 	S16 fPropRMark;
	S16 ibstPropRMark;
 	DTTM dttmPropRMark;
 	S32 dxtCharSpace;
 	S32 dyaLinePitch;
 	U16 clm;
    S16 reserved1;
 	U8 dmOrientPage;
 	U8 iHeadingPgn;
 	U16 pgnStart;
	S16 lnnMin;
 	S16 wTextFlow;
 	S16 reserved2;
 	S16 pgbProp;
 	U16 pgbApplyTo:3;
	U16 pgbPageDepth:2;
	U16 pgbOffsetFrom:3;
	U16 reserved:8;
 	U32 xaPage;
 	U32 yaPage;
 	U32 xaPageNUp;
 	U32 yaPageNUp;
 	U32 dxaLeft;
 	U32 dxaRight;
 	S32 dyaTop;
 	S32 dyaBottom;
 	U32 dzaGutter;
 	U32 dyaHdrTop;
 	U32 dyaHdrBottom;
 	S32 ccolM1;
 	S8 fEvenlySpaced;
 	S8 reserved3;
 	S32 dxaColumns;
 	S32 rgdxaColumnWidthSpacing[89];
 	S32 dxaColumnWidth;
 	U8 dmOrientFirst;
 	U8 fLayout;
 	S16 reserved4;
 	OLST olstAnm;
	} SEP;

	/*
The standard SEP is all zeros except as follows:

 bkc           2 (new page)

 dyaPgn        720 twips (equivalent to .5 in)

 dxaPgn        720 twips

 fEndnote      1 (True)

 fEvenlySpaced 1 (True)

 xaPage        12240 twips

 yaPage        15840 twips

 xaPageNUp     12240 twips

 yaPageNUp     15840 twips

 dyaHdrTop     720 twips

 dyaHdrBottom  720 twips

 dmOrientPage  1 (portrait orientation)

 dxaColumns    720 twips

 dyaTop        1440 twips

 dxaLeft       1800 twips

 dyaBottom     1440 twips

 dxaRight      1800 twips

 pgnStart      1

cbSEP (count of bytes of SEP) is 704(decimal), 2C0(hex).

*/
