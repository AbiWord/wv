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
void wvDecodeComplex(FIB *fib,FILE *mainfd,FILE *tablefd,FILE *data)
	{
	CLX clx;
	U32 piececount=0,i,j,temppiece;
	U32 beginfc,endfc,discard;
	U32 begincp,endcp;
	int chartype;
	U16 eachchar;
	U32 fcFirst,fcLim=0xffffffffL,tfcLim=0xffffffffL;
	BTE *btePapx;
	U32 *posPapx;
	U32 intervals;
	U16 charset;
	U8 state=0;
	

	/* 
	despite what some parts of the spec might have you believe you still need to 
	get the piecetable from even simple files, some simple files can have 8bit
	chars in one part, and 16bit chars in another, so you have to watch out for
	that
	*/
	wvGetCLX(&clx,fib->fcClx,fib->lcbClx,tablefd);

	/*
	we will need the paragraph bounds table to make decisions as to where a table
	begins and ends
	*/
    wvGetBTE_PLCF(&btePapx,&posPapx,&intervals,fib->fcPlcfbtePapx,fib->lcbPlcfbtePapx,tablefd);



	charset = wvAutoCharset(&clx);
	
	/*for each piece*/
	for (piececount=0;piececount<clx.nopcd;piececount++)
		{
		chartype = wvGetPieceBoundsFC(&beginfc,&endfc,&clx,piececount);
		wvGetPieceBoundsCP(&begincp,&endcp,&clx,piececount);
		wvTrace("begin end %x %x %x %x\n",beginfc,endfc,begincp,endcp);
		fseek(mainfd,beginfc,SEEK_SET);
		for (i=begincp,j=beginfc;i<endcp;i++,j += wvIncFC(chartype))
			{
			if (j == tfcLim)
				{
				wvTrace("tfcLim is %x, i is %d\n",tfcLim,i);
				temppiece = wvGetPieceFromCP(i+1,&clx);
				wvGetComplexParaBounds(&fcLim,&discard,i+1,&clx, btePapx, posPapx,intervals,temppiece,mainfd);
				wvTrace("after tests fcLim %x\n",fcLim);
				}
			
			/*
			if (j == fcLim)
				wvEndPara();
			*/

			if ((fcLim == 0xffffffffL) || (fcLim == j))
				{
				wvTrace("before tests j is %x\n",j);
				wvGetComplexParaBounds(&fcFirst,&tfcLim,i,&clx, btePapx, posPapx,intervals,piececount,mainfd);
				wvTrace("tfcLim is %x\n",tfcLim);
				fcLim = 0xfffffffeL;
				}
	

			/*
			if (j == fcFirst)
				wvBeginPara(NULL,NULL);
			*/

			eachchar = wvGetChar(mainfd,chartype);

			wvOutputTextChar(eachchar,chartype,charset,&state);
			/*
			wvOutputHtml4(eachchar,chartype,charset);
			*/
			}
		}

	/*
	if (fcLim != 0xffffffffL)
		wvEndPara();
	*/

	wvReleaseCLX(&clx);
	}

/*
To find the beginning of the paragraph containing a character in a complex
document, it's first necessary to 

1) search for the piece containing the character in the piece table. 

2) Then calculate the FC in the file that stores the character from the piece 
	table information. 
	
3) Using the FC, search the FCs FKP for the largest FC less than the character's 
	FC, call it fcTest. 
	
4) If the character at fcTest-1 is contained in the current piece, then the 
	character corresponding to that FC in the piece is the first character of 
	the paragraph. 
	
5) If that FC is before or marks the beginning of the piece, scan a piece at a 
time towards the beginning of the piece table until a piece is found that 
contains a paragraph mark. 

(This can be done by using the end of the piece FC, finding the largest FC in 
its FKP that is less than or equal to the end of piece FC, and checking to see 
if the character in front of the FKP FC (which must mark a paragraph end) is 
within the piece.)

6) When such an FKP FC is found, the FC marks the first byte of paragraph text.
*/

/*
To find the end of a paragraph for a character in a complex format file,
again 

1) it is necessary to know the piece that contains the character and the
FC assigned to the character. 

2) Using the FC of the character, first search the FKP that describes the 
character to find the smallest FC in the rgfc that is larger than the character 
FC. 

3) If the FC found in the FKP is less than or equal to the limit FC of the 
piece, the end of the paragraph that contains the character is at the FKP FC 
minus 1. 

4) If the FKP FC that was found was greater than the FC of the end of the 
piece, scan piece by piece toward the end of the document until a piece is 
found that contains a paragraph end mark. 

5) It's possible to check if a piece contains a paragraph mark by using the 
FC of the beginning of the piece to search in the FKPs for the smallest FC in 
the FKP rgfc that is greater than the FC of the beginning of the piece. 

If the FC found is less than or equal to the limit FC of the
piece, then the character that ends the paragraph is the character
immediately before the FKP FC.
*/
int wvGetComplexParaBounds(U32 *fcFirst, U32 *fcLim, U32 currentcp,CLX *clx, BTE *bte, U32 *pos,int nobte,U32 piece,FILE *fd)
	{
	U32 currentfc;
	BTE entry;
	PAPX_FKP fkp;
	long currentpos;
	currentfc = wvConvertCPToFC(currentcp,clx);

	wvTrace("current fc is %x\n",currentfc);

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

	wvGetPAPX_FKP(&fkp,entry.pn,fd);

	wvGetComplexParafcFirst(fcFirst,currentfc,clx, bte, pos,nobte,piece,&fkp,fd);
	wvGetComplexParafcLim(fcLim,currentfc,clx, bte, pos,nobte,piece,&fkp,fd);

	fseek(fd,currentpos,SEEK_SET);
	return(0);
	}

int wvGetComplexParafcLim(U32 *fcLim,U32 currentfc,CLX *clx, BTE *bte, U32 *pos,int nobte,U32 piece,PAPX_FKP *fkp,FILE *fd)
	{
	U32 fcTest,beginfc;
	BTE entry;
	*fcLim=0xffffffffL;
	fcTest = wvSearchNextSmallestFCPAPX_FKP(fkp,currentfc);

	wvTrace("fcTest is %x, pcd is %x\n",fcTest,wvNormFC(clx->pcd[piece+1].fc,NULL));

	if (fcTest <= wvNormFC(clx->pcd[piece+1].fc,NULL))
		{
		*fcLim = fcTest-1;
		}
	else
		{
		wvTrace("piece is %d\n",piece);
		/*get end fc of previous piece*/
		piece++;
		while (piece < clx->nopcd) 
			{
			beginfc = wvNormFC(clx->pcd[piece].fc,NULL);
			wvGetBTE_FromFC(&entry,beginfc,bte,pos,nobte);
			wvGetPAPX_FKP(fkp,entry.pn,fd);
			fcTest = wvSearchNextSmallestFCPAPX_FKP(fkp,beginfc);
			if (fcTest <= wvNormFC(clx->pcd[piece+1].fc,NULL))
				{
				*fcLim = fcTest-1;
				break;
				}
			piece--;
			}
		}
	wvTrace("fcLim is %x\n",*fcLim);
	return(0);
	}


int wvGetComplexParafcFirst(U32 *fcFirst,U32 currentfc,CLX *clx, BTE *bte, U32 *pos,int nobte,U32 piece,PAPX_FKP *fkp,FILE *fd)
	{
	U32 fcTest,endfc;
	BTE entry;
	fcTest = wvSearchNextLargestFCPAPX_FKP(fkp,currentfc);

	wvTrace("fcTest (s) is %x\n",fcTest);


	if (wvQuerySamePiece(fcTest-1,clx,piece))
		{
		wvTrace("same piece\n");
		*fcFirst = fcTest-1;
		}
	else
		{
		wvTrace("piece is %d\n",piece);
		/*get end fc of previous piece*/
		piece--;
		while (piece != 0xffffffffL) 
			{
			endfc = wvNormFC(clx->pcd[piece+1].fc,NULL);
			wvGetBTE_FromFC(&entry,endfc,bte,pos,nobte);
			wvGetPAPX_FKP(fkp,entry.pn,fd);
			fcTest = wvSearchNextLargestFCPAPX_FKP(fkp,endfc);
			if (wvQuerySamePiece(fcTest-1,clx,piece))
				{
				*fcFirst = fcTest-1;
				break;
				}
			piece--;
			}
		
		}
	return(0);
	}
