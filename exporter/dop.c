/* TODO: code wvPutDOP */


void wvGetCOPTS(COPTS *item,wvStream *fd)
{
	U16 temp16 = (U16)0;

	temp16 |= item->fNoTabForInd;
	temp16 |= item->fNoSpaceRaiseLower << 1;
	temp16 |= item->fSuppressSpbfAfterPageBreak << 2;
	temp16 |= item->fWrapTrailSpaces << 3;
	temp16 |= item->fMapPrintTextColor << 4;
	temp16 |= item->fNoColumnBlalance << 5;
	temp16 |= item->fConvMailMergeEsc << 6;
	temp16 |= item->fSuppressTopSpacing << 7;
	temp16 |= item->fOrigWordTableRules << 8;
	temp16 |= item->fTransparentMetafiles << 9;
	temp16 |= item->fShowBreaksInFrames << 10;
	temp16 |= item->fSwapBordersFacingPgs << 11;
	temp16 |= item->reserved << 12;

	write_16ubit(fd, temp16);
}