typedef struct _ANLV
	{
	U8 nfc;
 	U8 cxchTextBefore;
	U8 cxchTextAfter;
 	U8 jc:2;
	U8 fPrev:1;
	U8 fHang:1;
	U8 fSetBold:1;
	U8 fSetItalic:1;
	U8 fSetSmallCaps:1;
	U8 fSetCaps:1;
 	U8 fSetStrike:1;
	U8 fSetKul:1;
	U8 fPrevSpace:1;
	U8 fBold:1;
	U8 FItalic:1;
	U8 fSmallCaps:1;
	U8 fCaps:1;
	U8 fStrike:1;
	U8 kul:3;
	U8 ico:5;
	S16 ftc;
	U16 hps;
	U16 iStartAt;
	U16 dxaIndent;
	U16 dxaSpace;
	} ANLV;
