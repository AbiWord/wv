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
void wvDecodeSimple(FIB *fib,state_data *myhandle,FILE *mainfd,FILE *tablefd,FILE *data)
	{
	STSH stsh;
	CLX clx;
	STTBF anSttbfAssoc;
	expand_data expandhandle;
	PAPX_FKP fkp;
	PAP apap;
	U32 piececount=0,i,j=0;
	U32 beginfc,endfc;
	U32 begincp,endcp;
	int chartype;
	U16 eachchar;
	U32 fcFirst,fcLim=0xffffffff;
	BTE *btePapx;
	U32 *posPapx;
	U32 intervals;
	U8 state=0;

	expandhandle.sd = myhandle;

	/*we will need the stylesheet to do anything useful with layout and look*/
	wvGetSTSH(&stsh,fib->fcStshf,fib->lcbStshf,tablefd);

	/*we will need the table of names to answer questions like the name of the doc*/
	wvGetSTTBF(&anSttbfAssoc,fib->fcSttbfAssoc,fib->lcbSttbfAssoc,tablefd);

	expandhandle.anSttbfAssoc = &anSttbfAssoc;

	/* 
	despite what some parts of the spec might have you believe you still need to 
	get the piecetable from even simple files, some simple files can have 8bit
	chars in one part, and 16bit chars in another, so you have to watch out for
	that
	*/
	wvGetCLX(&clx,fib->fcClx,fib->lcbClx,tablefd);

	/*
	we will need the paragraph bounds table to make decisions as to where a piece
	begins and ends
	*/
    wvGetBTE_PLCF(&btePapx,&posPapx,&intervals,fib->fcPlcfbtePapx,fib->lcbPlcfbtePapx,tablefd);

	/*
	The text of the file starts at fib.fcMin, but we will use the piecetable 
	records rather than this.
	fseek(mainfd,fib->fcMin,SEEK_SET);
	*/

	/*
	If !fib.fComplex, the document text stream is represented by the text
	beginning at fib.fcMin up to (but not including) fib.fcMac.
	*/

	if (fib->fcMac != (S32)(wvNormFC(clx.pcd[clx.nopcd-1].fc,NULL)+clx.pos[clx.nopcd]))
		wvError("fcMac is not the same as the piecetable %x %x!\n",fib->fcMac,wvNormFC(clx.pcd[clx.nopcd-1].fc,NULL)+clx.pos[clx.nopcd]);

	expandhandle.charset = wvAutoCharset(&clx);

	wvInitPAPX_FKP(&fkp);

	wvBeginDocument(&expandhandle);
	
	/*for each piece*/
	for (piececount=0;piececount<clx.nopcd;piececount++)
		{
		chartype = wvGetPieceBoundsFC(&beginfc,&endfc,&clx,piececount);
		fseek(mainfd,beginfc,SEEK_SET);
		wvGetPieceBoundsCP(&begincp,&endcp,&clx,piececount);
		for (i=begincp,j=beginfc;i<endcp;i++,j += wvIncFC(chartype))
			{
			if (j == fcLim)
				wvEndPara(&expandhandle);
			
			if ((fcLim == 0xffffffff) || (fcLim == j))
				{
				wvTrace("j i is %x %d\n",j,i);
				wvReleasePAPX_FKP(&fkp);
				wvGetSimpleParaBounds(&fkp,&fcFirst,&fcLim,i,&clx, btePapx, posPapx,intervals,mainfd);
				wvTrace("para beings at %x ends %x\n",fcFirst,fcLim);
				}

			if (j == fcFirst)
				{
				wvAssembleSimplePAP(&apap,fcLim,&fkp,&stsh);
				expandhandle.apap = &apap;
				wvBeginPara(&expandhandle);
				}

			eachchar = wvGetChar(mainfd,chartype);

			wvOutputTextChar(eachchar,chartype,expandhandle.charset,&state);
			/*
			wvOutputHtml4(eachchar,chartype,expandhandle.charset);
			*/
			}
		}

	if (j == fcLim)
		{
		wvReleasePAPX_FKP(&fkp);
		wvEndPara(&expandhandle);
		}

	wvEndDocument(&expandhandle);

	wvReleaseSTTBF(&anSttbfAssoc);
    wvFree(btePapx);
	wvFree(posPapx);
	if (fib->fcMac != ftell(mainfd))
		wvError("fcMac did not match end of input !\n");
	wvReleaseCLX(&clx);
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
int wvGetSimpleParaBounds(PAPX_FKP *fkp,U32 *fcFirst, U32 *fcLim, U32 currentcp,CLX *clx, BTE *bte, U32 *pos,int nobte,FILE *fd)
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
	wvGetPAPX_FKP(fkp,entry.pn,fd);

	fseek(fd,currentpos,SEEK_SET);

	return(wvGetIntervalBounds(fcFirst,fcLim,currentfc,fkp->rgfc,fkp->crun+1));
	}

int wvGetIntervalBounds(U32 *fcFirst, U32 *fcLim, U32 currentfc, U32 *rgfc, U32 nopos)
	{
	U32 i=0;
	while(i<nopos)
		{
		if ( (wvNormFC(rgfc[i],NULL) >= currentfc) && (currentfc < wvNormFC(rgfc[i+1],NULL)) )
			{
			*fcFirst = wvNormFC(rgfc[i],NULL);
			*fcLim = wvNormFC(rgfc[i+1],NULL);
			return(0);
			}
		i++;
		}
	return(1);
	}
