#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "wv.h"

extern FILE *erroroutput;
extern int sectionpagenumber;
extern sep *currentsep;
extern int nostyles;
extern int dorevs;
extern U32 mainend;

void wvAddPAP_FromBucket(pap *apap,U8 *pointer8,U16 len,style *sheet)
    {
    U8 *pointer;
	U16 sprm;
	U16 i=0;


	if (apap == NULL)
		{
		wvError(("NULL pap in wvAddPAP_FromBucket\n"));
		return;
		}

	if (pointer8 == NULL)
		return;
    
    while (i<len)
        {
        sprm = sread_16ubit(pointer8+i);
        i+=2;
        
        pointer = pointer8+i;
        
        decode_sprm(NULL,sprm,apap,NULL,NULL,&i,&pointer,sheet,apap->istd);
        }
    }


/* 
If the pap has a table, then fill_table_info will search through
the paps for the pap that describes the table in more detail, and
copy that information into this pap
*/
void fill_table_info(pap *apap,U32 tapfc1, U32 *plcfbtePapx,U32 intervals, 
	FILE *mainfd,style *sheet,list_info *a_list_info)
	{
	pap *tappap=NULL;
	int tempfck;

	do
		{
		if (tappap != NULL)
			{
			free(tappap);
			tappap=NULL;
			}

		error(erroroutput,"tappap nexts are %x\n",tapfc1);
		tempfck = find_FKPno_papx(tapfc1,plcfbtePapx,intervals);
		if (tempfck != -1)
			tappap = get_pap(tempfck,mainfd,tapfc1,&tapfc1,sheet,a_list_info);
		else
			break;
		}
	while( (tappap != NULL) && (!tappap->fTtp) );
	error(erroroutput,"finished search for the tap here\n");
	/*
	at this stage tappap has the correct row structure stored in it
	that we want apap to have
	*/
	if (tappap != NULL)
		{
		copy_tap(&(apap->ourtap),&(tappap->ourtap));
		error(erroroutput,"no of cells is %d, %d\n",apap->ourtap.cell_no,tappap->ourtap.cell_no);
		free(tappap);
		tappap=NULL;
		}
	}

chp *get_chp(U32 pageindex,FILE *in, FILE *data, U32 charindex, U32 *nextfc,style *sheet,U16 istd)
	{
	long pos = ftell(in);
	S16 i;
	U16 l;
	U16 offset;
	long fullen;
	U8 cb;
	U16 clist=0;
	chp *retchp;


	fseek(in,0,SEEK_END);
	fullen = ftell(in);

	error(erroroutput,"\nlooking for CHP in page %d given istd %d\n",pageindex,istd);
	/*this no refers to 512*pageindex ?*/
	error(erroroutput,"\n%x\n",(512*pageindex));


	*nextfc = find_next_smallest_fc(charindex,pageindex,in,&i,NULL);
	error(erroroutput,"next char lim is %d (%x)\n",*nextfc,*nextfc);

	retchp = (chp *) malloc(sizeof(chp));
	if (retchp == NULL)
		{
		fprintf(erroroutput,"no mem available\n");
		exit(-1);
		}

	error(erroroutput,"the istd of this chp is %d\n",istd);

	init_chp_from_istd(istd,sheet,retchp);

	if (i == -1)
		offset=0;
	else
		{
		/*leap ahead to the BX we want*/
		fseek(in,(long)i,SEEK_CUR);

		/*the first byte is the one we want*/
		offset = getc(in);
		error(erroroutput,"CHP: chpx offset is %ld %x, going to %x\n",offset,offset,(512*(pageindex))+offset*2);
		}

	if (offset == 0)
		{
		error(erroroutput,"CHP: special chpx offset leaving now\n");
		fseek(in,pos,SEEK_SET);
		error(erroroutput,"at this end bold is %d\n",retchp->fBold);
		}
	else
		{
	
		/*this section is fucked up, why does it appear that i have to use twice
		the offset i get out of this value ?*/
		fseek(in,(512*(pageindex))+offset*2,SEEK_SET);
		cb = getc(in);
		error(erroroutput,"CHP: cb was %d\n",cb);


		if (cb != 0)
			{
			l=0;
			while(l<cb)
				{
				clist = read_16ubit(in);
				l+=2;
				error(erroroutput,"decoding sprm %X\n",clist);
				decode_sprm(in,clist,NULL,retchp,NULL,&l,NULL,sheet,istd);
				}
			}

		fseek(in,pos,SEEK_SET);
		error(erroroutput,"at end bold is %d\n",retchp->fBold);
		}
	
	if (retchp->fSmallCaps)
		retchp->fontsize-=2;

	if ((retchp->fRMarkDel) && (dorevs))
		{
		error(erroroutput,"delete mark is on\n");
		retchp->fStrike=1;
		}

	if ((retchp->fRMark) && (dorevs))
		{
		error(erroroutput,"addition mark is on\n");
		retchp->underline=1;
		}

	error(erroroutput,"fcPic in chp is %x\n",retchp->fcPic);

	return(retchp);
	}

void init_sep(sep *asep)
	{
	asep->bkc=2;
	asep->fTitlePage=0;

	asep->fAutoPgn=0;

	asep->nfcPgn=0;
    asep->fUnlocked=0;
    asep->cnsPgn=0;
	asep->fPgnRestart=0;
	asep->fEndNote=1;
	asep->lnc=0;
    asep->grpfIhdt=0;
	asep->nLnnMod=0;
	asep->ccolM1=0;
	asep->pgnStart=1;
    asep->xaPage=12240;
    asep->yaPage=15840;
    asep->dxaLeft=1800;
    asep->dxaRight=1800;
    asep->dyaTop=1440;
    asep->dyaBottom=1440;
    asep->dzaGutter=0;
	}

sep *get_sep(U32 offset,FILE *in)
	{
	sep *asep;
	U16 clist;
	U16 cb,l;
	long pos=ftell(in);

	asep = (sep *) malloc(sizeof(sep));
	if (asep == NULL)
		{
		error(erroroutput,"arse\n");
		exit(-1);
		}

	init_sep(asep);	

	fseek(in,offset,SEEK_SET);
	error(erroroutput,"sepx at %x",offset);
	cb = read_16ubit(in);
	error(erroroutput,"no of bytes is %d",cb);

	if (cb != 0)
		{
		l=0;
		while(l<cb)
			{
			clist = read_16ubit(in);
			l+=2;
			error(erroroutput,"decoding sprm %X\n",clist);
			decode_sprm(in,clist,NULL,NULL,asep,&l,NULL,NULL,0);
			}
		}
	
	fseek(in,pos,SEEK_SET);

	if(asep->fPgnRestart)
		{
		sectionpagenumber=asep->pgnStart;
		/*pagenumber = asep->pgnStart;*/
		}

	currentsep = asep;

	error(erroroutput,"sep x and y is %d %d\n",asep->xaPage,asep->yaPage);
	return(asep);
	}

pap *get_pap(U32 pageindex,FILE *in, U32 charindex, U32 *nextfc, style *sheet,list_info *a_list_info)
	{
	long pos = ftell(in);
	U8 k;
	S16 i;
	U8 BX[13];
	U16 offset;
	U16 len;
	U16 cw;
	U32 clist;
	pap *retpap=NULL;
	sep fakesep;

	error(erroroutput,"\nlooking for PAP in page %d\n",pageindex);
	/*this no refers to 512*pageindex ?*/
	error(erroroutput,"\n%x\n",(512*pageindex));

	*nextfc = find_next_smallest_fc(charindex,pageindex,in,&i,NULL);

	retpap = (pap *) malloc(sizeof(pap));
	if (retpap == NULL)
		{
		fprintf(erroroutput,"no mem available\n");
		exit(-1);
		}
	init_pap(retpap);

	/*leap ahead to the BX we want*/
	fseek(in,i*13,SEEK_CUR);
	error(erroroutput,"the pap offset is %x\n",ftell(in));

	/*pull out BX word offset*/
	for(k=0;k<13;k++)
		BX[k] = getc(in);
	/*the first byte is the one we want*/
	offset = BX[0];
	error(erroroutput,"papx offset is %ld %x, going to %x\n",offset,offset,(512*(pageindex))+offset*2);
	if (BX[0] == 0)
		error(erroroutput,"special papx offset ?\n");
	/*this section is fucked up, why does it appear that i have to use twice
	the offset i get out of this value ?*/
	fseek(in,(512*(pageindex))+offset*2,SEEK_SET);
	cw = getc(in);
	if (cw == 0)
		{
		error(erroroutput,"cw was pad %d\n",cw);
		cw = getc(in);
		error(erroroutput,"cw was %d\n",cw);
		cw=cw*2;
		error(erroroutput,"so bytes are %d\n",cw);
		}
	else
		cw=cw*2; /*when the pap is in a stsh then this is not done*/
		
	error(erroroutput,"cw bytes are %d\n",cw);

	len=0;
	retpap->istd = read_16ubit(in);
	len+=2;
	error(erroroutput,"istd index is %d\n",retpap->istd);
	/*
	take the istd and get the style from the decoded style list
	and shove in the starting values values from that
	*/
	if (retpap->istd > nostyles)
		{
		error(erroroutput,"WARNING invalid istd %d/%x\n",retpap->istd,retpap->istd);
		retpap->istd=0;
		}

	init_pap_from_istd(retpap->istd,sheet,retpap);
	error(erroroutput,"BEGIN, ilfo is %d",retpap->ilfo);

	while (len < cw)
		{
		clist = read_16ubit(in);
		len+=2;
		/*
		and *why* do i have to take this byte out to get what i want ?
		*/
		error(erroroutput,"clist %x \n",clist);
		decode_sprm(in,clist,retpap,NULL,&fakesep,&len,NULL,sheet,retpap->istd);
		}
	error(erroroutput,"END");
	fseek(in,pos,SEEK_SET);
	return(retpap);
	}

U32 find_FC_sepx(U32 cp,U32 *sepcp,textportions *portions)
	{
	U32 j=1;
	while (j<=portions->section_nos)
		{
		if (cp < portions->section_cps[j])
			{
			wvTrace(("the sep cp is %x\n",portions->section_cps[j]-1));
			wvTrace(("the sep fc is %x\n",portions->section_fcs[j-1].fcSepx));
			*sepcp = portions->section_cps[j]-1;
			return(portions->section_fcs[j-1].fcSepx);
			break;
			}
		j++;
		}
	wvError(("no sep FC found\n"));
	return(0xffffffffL);
	}

int find_FKPno_papx(U32 fc,U32 *plcfbtePapx,U32 intervals)
	{
	U32 j=1;
	while (j<=intervals)
		{
		if (fc < plcfbtePapx[j])
			{
			wvTrace(("it was less that it\n"));
			wvTrace(("so the FKP thatll have the limits of this is %d\n",plcfbtePapx[j+intervals]));
			return(plcfbtePapx[j+intervals]);
			break;
			}
		j++;
		}
	wvError(("no FKP pap found, using last entry instead\n"));
	return(-1);
	}

int find_FKPno_chpx(U32 fc,U32 *plcfbteChpx,U32 chpintervals)
	{
	int j=1;
	while (j<=chpintervals)
		{
		if (fc < plcfbteChpx[j])
			{
			wvTrace(("CHP: it was less that it\n"));
			wvTrace(("CHP: so the FKP thatll have the limits of this is %d\n",plcfbteChpx[j+chpintervals]));
			return(plcfbteChpx[j+chpintervals]);
			break;
			}
		j++;
		}
	wvError(("CHP: no chp found, using last entry\n"));
	return(-1);
	}

U32 find_next_biggest_orequal_fc(U32 charindex,U32 pageindex, FILE *in, U16 *location,long *pos)
	{
	U8 crun;
	int i;
	U32 *rgfc;
	U32 rval=0xffffffffL;
	if (pos != NULL)
		*pos = ftell(in);

	if (pageindex != 0xffffffffL)
		{
		wvTrace(("smallest seek to %x\n",(512*pageindex)+511));
		if ((mainend < (512*pageindex)+511) || (0 != fseek(in,(512*pageindex)+511,SEEK_SET)))
			wvError(("ran too far forward on smallest seek 1 to %x, bloody semi corrupt file\n",(512*pageindex)+511));
		else
			{
			crun = getc(in);
			wvTrace(("there are %d paragraphs/char runs in this zone at %x, pageindex %d",crun,(512*pageindex),pageindex));
			rgfc = (U32 *) malloc(sizeof(U32)*(crun+1));
			if (rgfc== NULL)
				{
				wvError(("no memory argh\n"));
				exit(-1);
				}
			fseek(in,(512*pageindex),SEEK_SET);
			for(i=0;i<crun+1;i++)
				rgfc[i] = read_32ubit(in);
			wvTrace((" charindex is %x\n",charindex));
			i=crun-1;
			rval = rgfc[0];
			while (i >= 0)
				{
				if (charindex >=  rgfc[i])
					{
					*location = i;
					rval = rgfc[i];
					break;
					}
				i--;		
				}

			free(rgfc);
			}
		}
	if (pos != NULL)
		{
		fseek(in,*pos,SEEK_SET);
		wvTrace(("papx: reset stream\n"));
		}
	return(rval); 
	}

U32 find_next_biggest_fc(U32 charindex,U32 pageindex, FILE *in, U16 *location,long *pos)
	{
	U8 crun;
	int i;
	U32 *rgfc;
	U32 rval=0xffffffffL;
	if (pos != NULL)
		*pos = ftell(in);

	if (pageindex != 0xffffffffL)
		{
		wvTrace(("smallest seek to %x\n",(512*pageindex)+511));
		if ((mainend < (512*pageindex)+511) || (0 != fseek(in,(512*pageindex)+511,SEEK_SET)))
			wvError(("ran too far forward on smallest seek 1 to %x, bloody semi corrupt file\n",(512*pageindex)+511));
		else
			{
			crun = getc(in);
			wvTrace(("there are %d paragraphs/char runs in this zone at %x, pageindex %d",crun,(512*pageindex),pageindex));
			rgfc = (U32 *) malloc(sizeof(U32)*(crun+1));
			if (rgfc== NULL)
				{
				wvError(("no memory argh\n"));
				exit(-1);
				}
			fseek(in,(512*pageindex),SEEK_SET);
			for(i=0;i<crun+1;i++)
				rgfc[i] = read_32ubit(in);
			wvTrace((" charindex is %x\n",charindex));
			i=crun-1;
			rval = rgfc[0];
			while (i >= 0)
				{
				if (charindex >  rgfc[i])
					{
					*location = i;
					rval = rgfc[i];
					break;
					}
				i--;		
				}

			free(rgfc);
			}
		}
	if (pos != NULL)
		{
		fseek(in,*pos,SEEK_SET);
		wvTrace(("papx: reset stream\n"));
		}
	return(rval); 
	}

U32 find_next_smallest_fc(U32 charindex,U32 pageindex, FILE *in, S16 *location,long *pos)
	{
	U8 crun,i;
	U32 *rgfc;
	U32 rval=0xffffffffL;
	if (pos != NULL)
		*pos = ftell(in);

	*location=-1; /*guilty until declared innocent*/


	if (pageindex != 0xffffffffL)
		{
		wvTrace(("smallest seek to %x\n",(512*pageindex)+511));
		if ((mainend < (512*pageindex)+511) || (0 != fseek(in,(512*pageindex)+511,SEEK_SET)))
			wvError(("ran too far forward on smallest seek 1 to %x, bloody semi corrupt file\n",(512*pageindex)+511));
		else
			{
			crun = getc(in);
			wvTrace(("there are %d paragraphs/char runs in this zone at %x, pageindex %d",crun,(512*pageindex),pageindex));
			rgfc = (U32 *) malloc(sizeof(U32)*(crun+1));
			if (rgfc== NULL)
				{
				wvError(("no memory argh\n"));
				exit(-1);
				}
			fseek(in,(512*pageindex),SEEK_SET);
			for(i=0;i<crun+1;i++)
				rgfc[i] = read_32ubit(in);
			i=0;
			while (i<crun)
				{
				if (charindex <  rgfc[i+1])
					{
					wvTrace(("looking for BX %ld, charindex is %ld (%x),  rgfc is %ld (%x),\n",i,charindex,charindex, rgfc[i+1],rgfc[i+1]));
					*location = i;
					rval = rgfc[i+1];
					break;
					}
				i++;		
				}

			free(rgfc);
			}
		}
	if (pos != NULL)
		{
		fseek(in,*pos,SEEK_SET);
		wvTrace(("papx: reset stream\n"));
		}
	return(rval); 
	}


chp * get_complex_chp(U32 fc,U32 *plcfbteChpx,U16 i,U16 nopieces,U32 chpintervals,U32 *rgfc,FILE *main,U32 *avalrgfc,U32 *thenextone,style *sheet,U16 istd)
	{
	chp *achp=NULL;
	int tempfck;
	S16 location;
	long passposition;
	U32 nextfc=0;
	U32 tempfc;

	/*we have the chp's fc*/

	/*
	Using the FC, search the FCs FKP for the largest FC less 
	than the character's FC
	*/

	if (fc & 0x40000000UL)
		{
		fc = fc & 0xbfffffffUL;
		fc = fc/2;
		}

	/*finding the correct FKP for this fc*/
	/*
	The CHPX FKP stored in that page is fetched and then the rgfc in the 
	FKP is searched to locate the bounds of the run of
	exception text that encompasses the character.
	*/
	tempfck = find_FKPno_chpx(fc,plcfbteChpx,chpintervals);

	/*now attempt to find the end of the run of text*/
	if (tempfck != -1)
		{
		nextfc = find_next_smallest_fc(fc,tempfck,main,&location,&passposition);
		wvTrace(("CHP: in this FKP the next fc larger than (%x) is (%x)\n",fc,nextfc));
		wvTrace(("CHP: the end of this piece is (%x)\n",avalrgfc[i]+rgfc[i+1]-rgfc[i]));
		}

	/*
	The CHPX for that run is then located within the FKP,
	and the CHPX is applied to the contents of the local CHP
	*/
	achp = get_chp(tempfck,main,NULL,nextfc-1,&tempfc,sheet,istd);
	*thenextone = nextfc;
	return(achp);
	}


pap * get_complex_pap(U32 fc,U32 *plcfbtePapx,U16 i,U16 nopieces,U32 intervals,U32 *rgfc,FILE *main,U32 *avalrgfc,U32 *thenextone,U32 *paraendfc,int *paraendpiece,style *sheet,list_info *a_list_info)
	{
	pap *apap=NULL;
	int tempfck;
	S16 location;
	U16 j;
	long passposition;
	U32 fc2;
	U32 nextfc=0;
	U32 tempfc;
/*
	get_para_bounds(i,fc,rgfc,avalrgfc,nopieces,plcfbtePapx,intervals,main);
*/
	if (fc & 0x40000000UL)
		{
		fc = fc & 0xbfffffffUL;
		fc = fc/2;
		}

	*paraendpiece = i;


	/*first get the piece that this char is from*/
	/*ive been running the current fc through this processing, but maybe
	i should be taking something else*/
	

	/*
	if we are not at the beginning of a paragraph get the 
	bounds of this paragraph, using the end mark of the paragraph
	look for the char after it, and then look at fkp.rgbx[i - 1] 
	to get the PAPX (last non-fastsaved one), add the SPRMS 
	that would be got
	from the clxt == 1 stuff
	*/

	/*
	Using the FC, search the FCs FKP for the smalled FC greater
	than the character's FC
	*/

	/*finding the correct FKP for this fc*/
	tempfck = find_FKPno_papx(fc,plcfbtePapx,intervals);

	/*get the end fc of the piece*/
	
	fc2 = avalrgfc[i];
    if (fc2 & 0x40000000UL)
		{
        fc2 = fc2 & 0xbfffffffUL;
        fc2 = fc2/2;
		fc2 += (rgfc[i+1]-rgfc[i]);
        }
	else
		fc2 +=2*(rgfc[i+1]-rgfc[i]);
		


	/*now attempt to find the end of the paragraph*/
	if (tempfck != -1)
		{
		nextfc = find_next_smallest_fc(fc,tempfck,main,&location,&passposition);
		}
	else
		{
		wvError(("for some reason this fc is outside the limits of the fkp itself\n"));
		/*this nextfc must be > fc2*/
		nextfc = fc2+1;
		}
	
	if ( nextfc <= (fc2) )
		{
		error(erroroutput,"not doing para end search\n");
		error(erroroutput,"the end of the paragraph that describes this piece is at (%x)",nextfc-1);
		if (paraendfc != NULL)
			*paraendfc=nextfc;
		/*this means that if we grab the PAP of this char that we have got the pap for the piece*/
		*paraendpiece = i;
		if (tempfck != -1)
			apap = get_pap(tempfck,main,nextfc-1,&tempfc,sheet,a_list_info);
		/*in this case the paragraph ends on nextfc*/
		*thenextone=nextfc;
		}
	else if(nextfc == -1)
		{
		nextfc = fc;
		error(erroroutput,"BUGGING OUT\n");
		}
	else
		{
		error(erroroutput,"PAP: doing para end search\n");
		/*
		in this case the paragraph will have to be looked for again
		when this piece comes to an end ?
		*/
		*thenextone = fc2;

		/*
		we go through the pieces looking for one that has a 
		end of paragraph mark in it
		
		It's possible to check if a piece contains a paragraph
		mark by using the FC of the beginning of the piece to 
		search in the FKPs for the smallest FC in the FKP
		rgfc that is greater than the FC of the beginning of the piece
		*/
		for (j=i+1;j < nopieces;j++)
			{
			fc = avalrgfc[j];
			if (fc & 0x40000000UL)
					{
					fc = fc & 0xbfffffffUL;
					fc = fc/2;
					}
			error(erroroutput,"fc pulled out here is (%x) (%x) %d\n",fc,avalrgfc[j],j);
			tempfck = find_FKPno_papx(fc,plcfbtePapx,intervals);
			if (tempfck != -1)
				{
				nextfc = find_next_smallest_fc(fc,tempfck,main,&location,&passposition);
				error(erroroutput,"the next one is (%x)\n",nextfc);
				error(erroroutput,"less than aval is (%x),and end is (%x)",fc,fc+rgfc[j+1]-rgfc[j]);
				error(erroroutput,"avals are %x and %x\n",nextfc-1,fc+rgfc[j+1]-rgfc[j]);
			/*
				if ((nextfc <= (fc+rgfc[j+1]-rgfc[j])) || ((nextfc-1 >= fc+rgfc[j+1]-rgfc[j]) && (j==i+1)))
			*/
			/*
				if ((nextfc <= (fc+rgfc[j+1]-rgfc[j])) || ((nextfc-1 == fc+rgfc[j+1]-rgfc[j]) && (j==i+1)))
			*/
			/*
				if (nextfc <= (fc+rgfc[j+1]-rgfc[j])) 
			*/
			
				fc2 = avalrgfc[j];
				if (fc2 & 0x40000000UL)
					{
					fc2 = fc2 & 0xbfffffffUL;
					fc2 = fc2/2;
					fc2 +=rgfc[j+1]-rgfc[j];
					}
				else
					fc2 += 2*(rgfc[j+1]-rgfc[j]);
				error(erroroutput,"fc2 is %x\n",fc2);
			

				if (nextfc <= fc2 )
					{
					/*well this is the end then*/
					error(erroroutput,"the end of the paragraph that describes this piece is at (%x), after trying further",nextfc-1);
					*paraendpiece = j; 
					if (paraendfc != NULL)
						*paraendfc=nextfc;

					error(erroroutput,"the pieceend is %d\n",*paraendpiece);
					if (tempfck != -1)
						apap = get_pap(tempfck,main,nextfc-1,&tempfc,sheet,a_list_info);
					break;
					}
				else
					error(erroroutput,"next cycle nopieces is %d,j is %d\n",nopieces,j);
				}
			}
		}	
/*
	*thenextone = nextfc;
*/
		
	if (apap != NULL)
		error(erroroutput,"PAP: istd is %ld\n",apap->istd);
	return(apap);
	}
