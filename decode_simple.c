#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
how this works,
we seek to the beginning of the text, we loop for a count of charaters that is stored in the fib.

the piecetable divides the text up into various sections, we keep track of our location vs
the next entry in that table, when we reach that location, we seek to the position that
the table tells us to go.

there are special cases for coming to the end of a section, and for the beginning and ends of
pages. for the purposes of headers and footers etc.
*/
void wvDecodeSimple(wvParseStruct *ps)
	{
	STSH stsh;
	PAPX_FKP para_fkp;
	CHPX_FKP char_fkp;
	PAP apap;
        CHP achp;
	U32 piececount=0,i,j=0;
	U32 beginfc,endfc;
	U32 begincp,endcp;
	int chartype;
	U16 eachchar;
	U32 para_fcFirst, para_fcLim=0xffffffff;
	U32 char_fcFirst, char_fcLim=0xffffffff;
	BTE *btePapx, *bteChpx;
	U32 *posPapx, *posChpx;
	U32 para_intervals, char_intervals;
	U16 charset;
	U8 state=0;
	int para_pendingclose=0,char_pendingclose=0;

	/*we will need the stylesheet to do anything useful with layout and look*/
	wvGetSTSH(&stsh,ps->fib.fcStshf,ps->fib.lcbStshf,ps->tablefd);

	/*we will need the table of names to answer questions like the name of the doc*/
	if ( (wvQuerySupported(&ps->fib,NULL) == 2) || (wvQuerySupported(&ps->fib,NULL) == 3) )
		wvGetSTTBF6(&ps->anSttbfAssoc,ps->fib.fcSttbfAssoc,ps->fib.lcbSttbfAssoc,ps->tablefd);
	else /*word 97*/
		wvGetSTTBF(&ps->anSttbfAssoc,ps->fib.fcSttbfAssoc,ps->fib.lcbSttbfAssoc,ps->tablefd);

	/* 
	despite what some parts of the spec might have you believe you still need to 
	get the piecetable from even simple files, some simple files can have 8bit
	chars in one part, and 16bit chars in another, so you have to watch out for
	that
	*/
	wvGetCLX(wvQuerySupported(&ps->fib,NULL),&ps->clx,ps->fib.fcClx,ps->fib.lcbClx,ps->tablefd);
	if (ps->clx.nopcd == 0) wvBuildCLXForSimple6(&ps->clx,&ps->fib);	/* for word 6 and just in case */
	
	/*
	we will need the paragraph bounds table to make decisions as to where a para
	begins and ends
	*/
	if ( (wvQuerySupported(&ps->fib,NULL) == 2) || (wvQuerySupported(&ps->fib,NULL) == 3))
		{
    	wvGetBTE_PLCF6(&btePapx,&posPapx,&para_intervals,ps->fib.fcPlcfbtePapx,ps->fib.lcbPlcfbtePapx,ps->tablefd);
    	wvListBTE_PLCF(&btePapx,&posPapx,&para_intervals);
		}
	else	/* word 97 */
    	wvGetBTE_PLCF(&btePapx,&posPapx,&para_intervals,ps->fib.fcPlcfbtePapx,ps->fib.lcbPlcfbtePapx,ps->tablefd);

	/*
	 * get bounds of character runs so we can get character formatting properties
	*/
	if ( (wvQuerySupported(&ps->fib,NULL) == 2) || (wvQuerySupported(&ps->fib,NULL) == 3))
		{
    	wvGetBTE_PLCF6(&bteChpx,&posChpx,&char_intervals,ps->fib.fcPlcfbteChpx,ps->fib.lcbPlcfbteChpx,ps->tablefd);
    	wvListBTE_PLCF(&bteChpx,&posChpx,&char_intervals);
		}
	else	/* word 97 */
    	wvGetBTE_PLCF(&bteChpx,&posChpx,&char_intervals,ps->fib.fcPlcfbteChpx,ps->fib.lcbPlcfbteChpx,ps->tablefd);



	/*
	The text of the file starts at fib.fcMin, but we will use the piecetable 
	records rather than this basic seek.
	fseek(ps->mainfd,ps->fib.fcMin,SEEK_SET);
	*/

	/*
	If !fib.fComplex, the document text stream is represented by the text
	beginning at fib.fcMin up to (but not including) fib.fcMac.
	*/
	
	/*
	if (ps->fib.fcMac != (S32)(wvNormFC(ps->clx.pcd[ps->clx.nopcd-1].fc,NULL)+ps->clx.pos[ps->clx.nopcd]))
	*/
	if ( ps->fib.fcMac != wvGetEndFCPiece(ps->clx.nopcd-1,&ps->clx) )
		wvError("fcMac is not the same as the piecetable %x %x!\n",ps->fib.fcMac,wvGetEndFCPiece(ps->clx.nopcd-1,&ps->clx));

	charset = wvAutoCharset(&ps->clx);

	wvInitPAPX_FKP(&para_fkp);
	wvInitCHPX_FKP(&char_fkp);
	   
	wvHandleDocument(ps,DOCBEGIN);

	
	/*for each piece*/
	for (piececount=0;piececount<ps->clx.nopcd;piececount++)
		{
		chartype = wvGetPieceBoundsFC(&beginfc,&endfc,&ps->clx,piececount);
		fseek(ps->mainfd,beginfc,SEEK_SET);

		wvGetPieceBoundsCP(&begincp,&endcp,&ps->clx,piececount);
		for (i=begincp,j=beginfc;i<endcp;i++,j += wvIncFC(chartype))
			{
			/* paragraph properties */
			if (j == para_fcLim)
				{
				wvHandleElement(ps, PARAEND, (void*)&apap);
				para_pendingclose = 0;
				}
			
			if ((para_fcLim == 0xffffffff) || (para_fcLim == j))
				{
				wvTrace("j i is %x %d\n",j,i);
				wvReleasePAPX_FKP(&para_fkp);
				wvGetSimpleParaBounds(wvQuerySupported(&ps->fib,NULL),&para_fkp,&para_fcFirst,&para_fcLim,i,&ps->clx, btePapx, posPapx, para_intervals, ps->mainfd);
				wvTrace("para begins at %x ends %x\n", para_fcFirst, para_fcLim);
				}

			if (j == para_fcFirst)
				{
				wvAssembleSimplePAP(&apap, para_fcLim, &para_fkp, &stsh);
				wvHandleElement(ps, PARABEGIN, (void*)&apap);
				para_pendingclose = 1;
				}

			/* character properties */
			if (j == char_fcLim)
				{
				wvHandleElement(ps, CHARPROPEND, (void*)&achp);
				char_pendingclose = 0;
				}
			
			if ((char_fcLim == 0xffffffff) || (char_fcLim == j))
				{
				wvTrace("j i is %x %d\n", j, i);
				wvReleaseCHPX_FKP(&char_fkp);
				wvGetSimpleCharBounds(wvQuerySupported(&ps->fib, NULL),
						      &char_fkp, &char_fcFirst, &char_fcLim,
						      i, &ps->clx, bteChpx, posChpx,
						      char_intervals, ps->mainfd);
				wvTrace("char begins at %x ends %x\n", char_fcFirst, char_fcLim);
				}

			if (j == char_fcFirst)
				{
				wvTrace("assembling CHP...\n");
				// a CHP's base style is in the para style */
				achp.istd = apap.istd;
				wvAssembleSimpleCHP(&achp, char_fcLim, &char_fkp, &stsh);
				wvTrace("CHP assembled.\n");
				wvHandleElement(ps, CHARPROPBEGIN, (void*)&achp);
				char_pendingclose = 1;
				}

			eachchar = wvGetChar(ps->mainfd, chartype);

			wvOutputTextChar(eachchar, chartype, charset, &state, ps);
			}
		}
	
	if (char_pendingclose)
		wvHandleElement(ps, CHARPROPEND, (void*)&achp);
	if (para_pendingclose)
		wvHandleElement(ps, PARAEND, (void*)&apap);

	wvReleasePAPX_FKP(&para_fkp);
	wvReleaseCHPX_FKP(&char_fkp);
	wvHandleDocument(ps,DOCEND);

	wvReleaseSTTBF(&ps->anSttbfAssoc);
        wvFree(btePapx);
	wvFree(posPapx);
        wvFree(bteChpx);
	wvFree(posChpx);
	if (ps->fib.fcMac != ftell(ps->mainfd))
		wvError("fcMac did not match end of input !\n");
	wvReleaseCLX(&ps->clx);
	wvReleaseSTSH(&stsh);
	}


/*
When a document is recorded in non-complex format, the bounds of the
paragraph that contains a particular character can be found by 

1) calculating the FC coordinate of the character, 

2) searching the bin table to find an FKP page that describes that FC, 

3) fetching that FKP, and 

4) then searching the FKP to find the interval in the rgfc that encloses the character. 

5) The bounds of the interval are the fcFirst and fcLim of the containing paragraph. 

Every character greater than or equal to fcFirst and less than fcLim is part of
the containing paragraph.

*/
int wvGetSimpleParaBounds(int version,PAPX_FKP *fkp,U32 *fcFirst, U32 *fcLim, U32 currentcp,CLX *clx, BTE *bte, U32 *pos,int nobte,FILE *fd)
	{
	U32 currentfc;
	BTE entry;
	long currentpos;

	currentfc = wvConvertCPToFC(currentcp,clx);

	if (currentfc==0xffffffffL)
		{
		wvError("Para Bounds not found !\n");
		return(1);
		}


	if (0 != wvGetBTE_FromFC(&entry,currentfc, bte,pos,nobte))
		{
		wvError("BTE not found !\n");
		return(1);
		}
	currentpos = ftell(fd);
	/*The pagenumber of the FKP is entry.pn */

	wvTrace("pn is %d\n",entry.pn);
	wvGetPAPX_FKP(version,fkp,entry.pn,fd);

	fseek(fd,currentpos,SEEK_SET);

	return(wvGetIntervalBounds(fcFirst,fcLim,currentfc,fkp->rgfc,fkp->crun+1));
	}

int wvGetSimpleCharBounds(int version, CHPX_FKP *fkp, U32 *fcFirst, U32 *fcLim, 
			  U32 currentcp, CLX *clx, BTE *bte, U32 *pos, int nobte,
			  FILE *fd)
	{
	U32 currentfc;
	BTE entry;
	long currentpos;

	currentfc = wvConvertCPToFC(currentcp, clx);

	if (currentfc==0xffffffffL)
		{
		wvError("Char Bounds not found !\n");
		return(1);
		}


	if (0 != wvGetBTE_FromFC(&entry, currentfc, bte, pos, nobte))
		{
		wvError("BTE not found !\n");
		return(1);
		}
	currentpos = ftell(fd);
	/*The pagenumber of the FKP is entry.pn */

	wvTrace("pn is %d\n",entry.pn);
	wvGetCHPX_FKP(version, fkp, entry.pn, fd);

	fseek(fd, currentpos, SEEK_SET);

	return(wvGetIntervalBounds(fcFirst, fcLim, currentfc, fkp->rgfc, fkp->crun+1));
	}

int wvGetIntervalBounds(U32 *fcFirst, U32 *fcLim, U32 currentfc, U32 *rgfc, U32 nopos)
	{
	U32 i=0;
	while(i<nopos-1)
		{
		wvTrace("searching...%x %x %x\n",currentfc,wvNormFC(rgfc[i],NULL),wvNormFC(rgfc[i+1],NULL));
		if ( (wvNormFC(rgfc[i],NULL) >= currentfc) && (currentfc <= wvNormFC(rgfc[i+1],NULL)) )
			{
			*fcFirst = wvNormFC(rgfc[i],NULL);
			*fcLim = wvNormFC(rgfc[i+1],NULL);
			return(0);
			}
		i++;
		}
	*fcFirst = wvNormFC(rgfc[nopos-2],NULL);
	*fcLim = wvNormFC(rgfc[nopos-1],NULL);
	wvTrace("I'd rather not see this happen at all :-)\n");
	return(0);
	}
