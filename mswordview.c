/*
Released under GPL, written by Caolan.McNamara@ul.ie.

Copyright (C) 1998,1999 
	Caolan McNamara

Real Life: Caolan McNamara           *  Doing: MSc in HCI
Work: Caolan.McNamara@ul.ie          *  Phone: +353-61-202699
URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
How would you have done it?
*/

/*
this software no longer requires laola
*/

/*

this code is often all over the shop, being more of an organic entity
that a carefully planed piece of code, so no laughing there at the back!

and send me patches by all means, but think carefully before sending me
a patch that doesnt fix a bug or add a feature but instead just changes
the style of coding, i.e no more thousand line patches that fix my 
indentation. (i like it this way)

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include "config.h"
#include "wv.h"
#include "roman.h"
#include "utf.h"
	

/*begin new spid handling*/
fsp_list *afsp_list=NULL;
fbse_list *afbse_list=NULL;
/*end new spid handling*/

extern int nostyles;
extern ATRD *key_atrd;
extern DOP dop;

/*ok i got impatient*/
char *title=NULL;
document_style *doc_style=NULL;
element_style ele_style[3];
obj_by_spid *picblips=NULL;

/*table magic*/
int *tablevals=NULL;
int cno=0;
int cunit=1;
/*end table*/

int notablewidth=0;

int list_author_key=0;

int nofontfaces=0;
int riskbadole=0;
int NORMAL=20;
int inunderline = 0;
int revision=0;
DTTM revisiontime;
STTBF revisions;
int inbold = 0;
int incenter= 0;
int inrightjust= 0;
int inblink= 0;
int inah1= -1;
char *endthing=NULL;
int inaheaderfooter=0;
int initalic = 0;
int inafont= 0;
int pagenumber=1;
int sectionpagenumber=1;
int sectionno=1;
int insuper = 0;
int insub = 0;
int inatable=0;
float tabsize=-1;
int cellempty=1;
int inarow=0;
int inacell=0;
int inalist = 0;
int doannos=1;
int dorevs=1;

int revisiontag=0;

int noheaders=0;

int deferrednewpage=0;

S16 lastcellwidth[65];
S16 lastrowlen=0;

int colcount;
int rowcount;

U32 mainend;
U32 lastlistid =0;
int listvalue[9];
int currentfontsize;
int currentfontcode=-1;

sep *currentsep=NULL;

int newline=1;
int inacolor;
char incolor[8]="";
char backgroundcolor[8]="";

U8 chps=0;

int padding=0;
int verpadding=0;
int ignoreheadings=0;
long int cp=0;
long int realcp=0;
static U8 header;
int breakcount=0;
int footnotehack=0;
int instrike=0;
int indstrike=0;

char *symbolurl=NULL;
char *imagesurl=NULL;
char *imagesdir=NULL;
char *wingdingurl=NULL;
char *patternurl=NULL;
char *outputfilename=NULL;
char *errorfilename=NULL;
char *filename=NULL;
char *configfile=NULL;
char *truetypedir=NULL;
FILE *outputfile=NULL;
FILE *erroroutput=NULL;

void cleanupglobals(void)
	{
	obj_by_spid *freeme=NULL;

	wvReleaseSTTBF(&revisions);

	if ((outputfile!=NULL) && (outputfile!=stdout))
		fclose(outputfile);

	if ((erroroutput!=NULL) && (erroroutput!=stderr))
		fclose(erroroutput);

	if (picblips != NULL)
		{
		freeme = picblips;
		picblips = picblips->next;
		if (freeme->filename != NULL)
			free(freeme->filename);
		free(freeme);
		}
	
	if (symbolurl!=NULL)
		free(symbolurl);
	if (imagesurl!=NULL)
		free(imagesurl);
	if (imagesdir!=NULL)
		free(imagesdir);
	if (wingdingurl!=NULL)
		free(wingdingurl);
	if (configfile !=NULL)
		free(configfile);
	if (patternurl!=NULL)
		free(patternurl);
	if (outputfilename!=NULL)
		free(outputfilename);
	if (errorfilename!=NULL)
		free(errorfilename);
	if (filename!=NULL)
		free(filename);
	if (key_atrd != NULL)
		free(key_atrd);
	}

int decode_word8(FILE *mainfd,FILE *tablefd0,FILE *tablefd1,FILE *data,int core)
	{
	FIB fib;
	U32 i,j,k;
	
	int ret;
	textportions portions;
	U32 *plcfbtePapx;
	U32 *plcfbteChpx;
	U32 chpintervals;
	U32 intervals;
	obj_by_spid *freeme=NULL;
	list_def *free_def,*free_def2;

	FFN_STTBF ffn_sttbf;
	
	field_info *all_fields[5];
	field_info main_fields;
	field_info header_fields;
	field_info footnote_fields;
	field_info annotation_fields;
	field_info endnote_fields;
	/*begin annotations*/
	Xst *freegroup;
	/*end annotations*/

	/*begin Document Properties*/
	/*end Document Properties*/

	/*begin section table*/
	/*end section table*/

	/*begin string table*/
	STTBF anSttbfAssoc;
	/*end string table*/

	fsp_list *tfsp_list;
	fbse_list *tfbse_list;
	fopte_list *tfopte_list;
	
	list_info a_list_info;
	style *masterstylesheet=NULL;

	FILE *tablefd;

	/*begin config stuff*/
	FILE *in=NULL;
	config_style *in_style=NULL;
	config_style *temp_styles;
	/*end config stuff*/

	int reason;

	if (configfile != NULL)
		{
		in = fopen(configfile,"rb");
		if (in == NULL)
			fprintf(erroroutput,"Sorry, couldn't open config file %s, using default configfile\n",configfile);
		}

	if (in == NULL)
		{
		in = fopen(CONFIGFILE,"rb");
		if (in == NULL)
			fprintf(erroroutput,"Sorry, couldn't open config file %s\nnot a problem using internal defaults.\n",CONFIGFILE);
		}

	if (mainfd == NULL)
		{
		fprintf(erroroutput,"There was no document stream, this is probably not a word file at all\n");
		return(10);
		}


	if (outputfilename == NULL)
		{
		outputfilename = (char *)malloc(strlen(filename) + strlen(".html") + 1);
		if (outputfilename == NULL)
			{
			fprintf(erroroutput,"no mem\n");
			return(-1);
			}
		sprintf(outputfilename,"%s%s",filename,".html");
		}

	if (strcmp(outputfilename,"-"))
		{
		outputfile = fopen(outputfilename,"w");
		if (outputfile == NULL)
			{
			fprintf(erroroutput,"couldnt open %s for writing\n",outputfilename);
			return(-1);
			}
		}
	else
		outputfile = stdout;
	
	
	
	fseek(mainfd,0,SEEK_END);
	mainend = ftell(mainfd);
	error(erroroutput,"the end of the stream is %x\n",mainend);
	fseek(mainfd,0,SEEK_SET);
	wvGetFIB(&fib,mainfd);
	
	ret = wvQuerySupported(&fib,&reason);
	if (ret) 
		{
		wvError("%s",wvReason(reason));
		return(ret);
		}

	/*do config file*/
	ele_style[BOLD].begin = "<b>";
	ele_style[BOLD].end = "</b>";
	ele_style[ITALIC].begin = "<i>";
	ele_style[ITALIC].end = "</i>";
	ele_style[FONT].begin = "<font [face=\"$fontface\"] [color=\"$color\"] [size=\"$size\"]>";
	ele_style[FONT].end = "</font>";
	if (in != NULL)
		Parse(in,&in_style,&doc_style,ele_style);
	/*end do config file*/

	tablefd = wvWhichTableStream(&fib,tablefd0,tablefd1);

	error(erroroutput,"first char is at %ld (%x)\n",fib.fcMin,fib.fcMin);
	error(erroroutput,"last char is at %ld (%x)\n",fib.fcMac,fib.fcMac);
	portions.fcMin = fib.fcMin;
	portions.fcMac = fib.fcMac;

	portions.ccpText = fib.ccpText;
	error(erroroutput,"the main doc text is of size %ld\n",portions.ccpText);
	portions.ccpFtn = fib.ccpFtn;
	error(erroroutput,"the footer text is of size %ld\n",portions.ccpFtn);
	portions.ccpHdd = fib.ccpHdd;
	error(erroroutput,"the header text is of size %ld\n",portions.ccpHdd);
	portions.ccpAtn = fib.ccpAtn;
	error(erroroutput,"the annotation text is size %ld\n",portions.ccpAtn);
	portions.ccpEdn = fib.ccpEdn;
	
	/*attempt to list all paragraph bounds*/

	error(erroroutput,"\n page of first different char type %d\nnumber of different character types %d\n",fib.pnChpFirst,fib.cpnBteChp);
	error(erroroutput,"\n page of first different para type %d\nnumber of different para types %d\n",fib.pnPapFirst,fib.cpnBtePap);
	error(erroroutput,"\n page of first different lvc type %d\nnumber of different lvc types %d\n",fib.pnLvcFirst,fib.cpnBteLvc);
	error(erroroutput,"the new stsh is (%x), len (%x)\n",fib.fcStshf,fib.lcbStshf);

	masterstylesheet = decode_stylesheet(tablefd,fib.fcStshf,fib.lcbStshf,in_style);

	error(erroroutput,"footnote: table offset of frd thingies (%x) of len %d\n",fib.fcPlcffndRef,fib.lcbPlcffndRef);
	error(erroroutput,"there are %d footnotes\n",(fib.lcbPlcffndRef-4)/6);
	error(erroroutput,"footnote: table offset for footnote text (%x) of len %d\n",fib.fcPlcffndTxt,fib.lcbPlcffndTxt);

	portions.list_foot_no = 0;
	portions.auto_foot=1;
	portions.last_foot=0;

	if (0 != wvGetFRD_PLCF(&portions.fndFRD,&portions.fndRef,&portions.fndref_no,fib.fcPlcffndRef,fib.lcbPlcffndRef,tablefd))
		return(-1);

	if (0 != wvGetEmpty_PLCF(&portions.fndTxt,&portions.fndtxt_no,fib.fcPlcffndTxt,fib.lcbPlcffndTxt,tablefd))
		return(-1);

	portions.list_anno_no = 0;
	portions.last_anno=0;

	error(erroroutput,"annotation offset is %x, len is %d\n",fib.fcPlcfandRef,fib.lcbPlcfandRef);

	if (0 != wvGetATRD_PLCF(&portions.the_atrd,&portions.andRef,&portions.andref_no,fib.fcPlcfandRef,fib.lcbPlcfandRef,tablefd))
		return(-1);

	if (0 != wvGetEmpty_PLCF(&portions.andTxt,&portions.andtxt_no,fib.fcPlcfandTxt,fib.lcbPlcfandTxt,tablefd))
		return(-1);

	error(erroroutput,"section: table offset for section table (%x) of len %d\n",fib.fcPlcfsed,fib.lcbPlcfsed);

	if (0 != wvGetSED_PLCF(&portions.section_fcs,&portions.section_cps,&portions.section_nos,fib.fcPlcfsed,fib.lcbPlcfsed,tablefd))
			return(-1);
	
	portions.fcPlcfhdd = fib.fcPlcfhdd;
	portions.lcbPlcfhdd = fib.lcbPlcfhdd;
	/*these point to the header/footer information thing*/

	error(erroroutput,"header in table offset of (%x), len is %d\n",portions.fcPlcfhdd,portions.lcbPlcfhdd);
	error(erroroutput,"\nlocation of char description in table stream is %x\nsize is %ld\n",fib.fcPlcfbteChpx,fib.lcbPlcfbteChpx);
	
	/*
	if (0 != wvGetBTE_PLCF(&RplcfbteChpx,&plcfbteChpx,&chpintervals,fib.fcPlcfbteChpx,fib.lcbPlcfbteChpx,tablefd))
	*/
	if (0 != wvGetEmpty_PLCF(&plcfbteChpx,&chpintervals,fib.fcPlcfbteChpx,fib.lcbPlcfbteChpx,tablefd))
		return(-1);
	chpintervals = (chpintervals-1)/2;
	
	error(erroroutput,"there are %d charrun intervals ? ending at ",chpintervals);
	for (i=1;i<chpintervals+1;i++)
		error(erroroutput,"%d (%d)", plcfbteChpx[i],plcfbteChpx[i+chpintervals]);
	error(erroroutput,"\n");


	error(erroroutput,"\nlocation of para description in table stream is %ld\nsize is %ld\n",fib.fcPlcfbtePapx,fib.lcbPlcfbtePapx);
	/*go to location in table stream, */
	/*i believe that this is just an array of longs(4 bytes blocks)
	 */
	 /*
	if (0 != wvGetBTE_PLCF(&RplcfbtePapx,&plcfbtePapx,&intervals,fib.fcPlcfbtePapx,fib.lcbPlcfbtePapx,tablefd))
	*/
	if (0 != wvGetEmpty_PLCF(&plcfbtePapx,&intervals,fib.fcPlcfbtePapx,fib.lcbPlcfbtePapx,tablefd))
		return(-1);
	intervals = (intervals-1)/2;

	error(erroroutput,"there are %d pragraph intervals ? ending at ",intervals);
	for (i=1;i<intervals+1;i++)
		error(erroroutput,"%d %x (%d)", plcfbtePapx[i], plcfbtePapx[i],plcfbtePapx[i+intervals]);
	error(erroroutput,"\n");

	wvGetFFN_STTBF(&ffn_sttbf,fib.fcSttbfffn,fib.lcbSttbfffn,tablefd);

	/*determine field plc*/
	error(erroroutput,"in table stream field plc is %ld, and len is %ld\n",fib.fcPlcffldMom,fib.lcbPlcffldMom);

	if (0 != wvGetFLD_PLCF(&main_fields.flds,&main_fields.cps,&main_fields.no,fib.fcPlcffldMom,fib.lcbPlcffldMom,tablefd))
			return(-1);

	error(erroroutput,"in table stream field header plc is (%x), and len is %ld\n",fib.fcPlcffldHdr,fib.lcbPlcffldHdr);

	if (0 != wvGetFLD_PLCF(&header_fields.flds,&header_fields.cps,&header_fields.no,fib.fcPlcffldHdr,fib.lcbPlcffldHdr,tablefd))
            return(-1);

	error(erroroutput,"in table stream field footnote plc is (%x), and len is %ld\n",fib.fcPlcffldFtn,fib.lcbPlcffldFtn);
	error(erroroutput,"in table stream field annotation plc is (%x), and len is %ld\n",fib.fcPlcffldAtn,fib.lcbPlcffldAtn);

	if (0 != wvGetFLD_PLCF(&footnote_fields.flds,&footnote_fields.cps,&footnote_fields.no,fib.fcPlcffldFtn,fib.lcbPlcffldFtn,tablefd))
            return(-1);

	if (0 != wvGetFLD_PLCF(&annotation_fields.flds,&annotation_fields.cps,&annotation_fields.no,fib.fcPlcffldAtn,fib.lcbPlcffldAtn,tablefd))
            return(-1);

	wvGetSTTBF(&portions.bookmarks,fib.fcSttbfbkmk,fib.lcbSttbfbkmk,tablefd);
	wvGetBKF_PLCF(&portions.l_bookmarks.bookmark_b_bkfs,&portions.l_bookmarks.bookmark_b_cps,&portions.l_bookmarks.bookmark_b_no,fib.fcPlcfbkl,fib.lcbPlcfbkl,tablefd);
	wvGetEmpty_PLCF(&portions.l_bookmarks.bookmark_e_cps,&portions.l_bookmarks.bookmark_e_no,fib.fcPlcfbkl,fib.lcbPlcfbkl,tablefd);

	wvGetDOP(&dop,fib.fcDop,fib.lcbDop,tablefd);	/*reference dop through the dop_ functions*/
	error(erroroutput,"dop fc is %x, len is %d\n",fib.fcDop,fib.lcbDop);

	wvGetSTTBF(&anSttbfAssoc,fib.fcSttbfAssoc,fib.lcbSttbfAssoc,tablefd);

	title = wvGetTitle(&anSttbfAssoc);
	fprintf(stderr,"title is %s\n",title);

#if 0
	/*filthy temp hack*/
	if (anSttbfAssoc.nostrings >= 3)
		{
		if (anSttbfAssoc.extendedflag == 0xFFFF)
			{
			if (anSttbfAssoc.u16strings[ibstAssocTitle] != NULL)	
				{
				i=0;
				while(anSttbfAssoc.u16strings[ibstAssocTitle][i] != 0)
					{
					title[i] = anSttbfAssoc.u16strings[ibstAssocTitle][i];
					i++;
					}
				title[i] = '\0';
				}
			}
		else
			{
			if (anSttbfAssoc.s8strings[ibstAssocTitle] != NULL)
				strncpy(title,(char *)anSttbfAssoc.s8strings[ibstAssocTitle],1024);
			}
		}
#endif
	/*complex info bit*/
	error(erroroutput,"complex bit begins at %X, and it %d long",fib.fcClx,fib.lcbClx);
	error(erroroutput,"fcGrpXstAtnOwners %x, lcbGrpXstAtnOwners %d\n",fib.fcGrpXstAtnOwners,fib.lcbGrpXstAtnOwners);

	wvGetXst(&portions.authors,fib.fcGrpXstAtnOwners,fib.lcbGrpXstAtnOwners,tablefd);
	wvGetSTTBF(&portions.annotations,fib.fcSttbfAtnbkmk,fib.lcbSttbfAtnbkmk,tablefd);
	wvGetBKF_PLCF(&portions.a_bookmarks.bookmark_b_bkfs,&portions.a_bookmarks.bookmark_b_cps,&portions.a_bookmarks.bookmark_b_no,fib.fcPlcfbkl,fib.lcbPlcfbkl,tablefd);
	wvGetEmpty_PLCF(&portions.a_bookmarks.bookmark_e_cps,&portions.a_bookmarks.bookmark_e_no,fib.fcPlcfbkl,fib.lcbPlcfbkl,tablefd);
	if (portions.authors != NULL)
        {
        key_atrd = (ATRD *) malloc(sizeof(ATRD) * portions.authors->noofstrings);
        if (key_atrd == NULL)
            {
            fprintf(erroroutput,"mem alloc error\n");
            return(-1);
            }
        for (i=0;i<portions.annotations.nostrings;i++)
            {
            key_atrd->ibst = i;
            key_atrd->xstUsrInitl[0] = 0;
            }
        }

	error(erroroutput,"error: pictures offset %x len %d\n",fib.fcPlcspaMom,fib.lcbPlcspaMom);

	wvGetFSPA_PLCF(&portions.fspas,&portions.officedrawcps,&portions.noofficedraw,fib.fcPlcspaMom,fib.lcbPlcspaMom,tablefd);

	error(erroroutput,"endnote: table offset of frd thingies (%x) of len %d\n",fib.fcPlcfendRef,fib.lcbPlcfendRef);

	portions.list_end_no = 0;
	portions.auto_end=1;

	if (0 != wvGetFRD_PLCF(&portions.endFRD,&portions.endRef,&portions.endref_no,fib.fcPlcfendRef,fib.lcbPlcfendRef,tablefd))
		return(-1);
	if (portions.endref_no > 0)
		{
		portions.endTrueFRD = (S16 *) malloc( portions.endref_no * sizeof(S16));
		for(i=0;i<portions.endref_no;i++)
			portions.endTrueFRD[i]=portions.endFRD[i].frd;
		}
	else
		portions.endTrueFRD = NULL;

	error(erroroutput,"endnote: table offset for endnote text (%x) of len %d\n",fib.fcPlcfendTxt,fib.lcbPlcfendTxt);

	wvGetEmpty_PLCF(&portions.endTxt ,&portions.endtxt_no,fib.fcPlcfendTxt,fib.lcbPlcfendTxt,tablefd);

	if (0 != wvGetFLD_PLCF(&endnote_fields.flds,&endnote_fields.cps,&endnote_fields.no,fib.fcPlcffldEdn,fib.lcbPlcffldEdn,tablefd))
   		return(-1);

	/*end endnote*/

	all_fields[0] = &main_fields;
	all_fields[1] = &header_fields;
	all_fields[2] = &footnote_fields;
	all_fields[3] = &annotation_fields;
	all_fields[4] = &endnote_fields;
	
	error(erroroutput,"fcDggInfo is %x and len is %x\n",fib.fcDggInfo,fib.lcbDggInfo);

	portions.ablipdata = NULL;

	afsp_list = wvParseEscher(&afbse_list,fib.fcDggInfo,fib.lcbDggInfo,tablefd,mainfd);

	/*begin revision authors tables*/
	wvGetSTTBF(&revisions,fib.fcSttbfRMark,fib.lcbSttbfRMark,tablefd);
	/*end revision authors tables*/

	get_table_info(tablefd,&a_list_info,fib.fcSttbFnm,fib.lcbSttbFnm,fib.fcPlcfLst,fib.lcbPlcfLst,fib.fcPlfLfo,fib.lcbPlfLfo,masterstylesheet);

	error(erroroutput,"\n-----text------ %x\n",1024);
	if ((core) && ((doc_style == NULL) || (doc_style->begin == NULL)))
		fprintf(outputfile,"<html>\n");

	error(erroroutput,"endpoint for standard is %ld\n",fib.fcMac);


	fseek(tablefd,portions.fcPlcfhdd,SEEK_SET);
	portions.headercpno = portions.lcbPlcfhdd/4;
	portions.headercplist = NULL;
	if (portions.headercpno > 0)
		{
		error(erroroutput,"head no is %d\n",portions.headercpno);
		portions.headercplist = (U32 *)malloc(sizeof(U32) * portions.headercpno);
		if (portions.headercplist == NULL)
			{
			error(erroroutput,"feck no mem\n");
			return(-1);
			}

		for (i=0;i<portions.headercpno;i++)
			{
			portions.headercplist[i]= read_32ubit(tablefd);
			error(erroroutput,"header fc ? is %x -> %x  %d",portions.headercplist[i],portions.fcMin+portions.ccpText+portions.ccpFtn+portions.headercplist[i],i);
			switch (i)
				{
				case 6:
					error(erroroutput,"even header\n");
					break;
				case 7:
					error(erroroutput,"odd header\n");
					break;
				case 8:
					error(erroroutput,"even footer\n");
					break;
				case 9:
					error(erroroutput,"odd footer\n");
					break;
				case 10:
					error(erroroutput,"page 1 special header\n");
					break;
				case 11:
					error(erroroutput,"page 1 special footer\n");
					break;
				case 12:
					error(erroroutput,"even header, section 2?\n");
					break;
				case 13:
					error(erroroutput,"odd header, section 2?\n");
					break;
				default:
					error(erroroutput,"\n");
					break;
				}
			}
		}

	
	if (fib.fComplex)
		{
		error(erroroutput,"complex writing complex table looking in %ld (len %ld)\n",fib.fcClx,fib.lcbClx);
		/*i believe that the intervals and plcfbtePapx are valid entities for
		fastsaved docs as well (i bloody hope so)*/
		fprintf(outputfile,"<!--complex document-->\n");
		error(erroroutput,"main ends at %x\n",portions.ccpText);
		wvDecodeComplex(&fib,mainfd,tablefd,data);

		fseek(mainfd,1024,SEEK_SET);
		decode_clx(0,0,portions.ccpText,tablefd,mainfd,data,fib.fcClx,fib.lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,&a_list_info,masterstylesheet,&portions,&ffn_sttbf,0);
		}
	else
		{
		error(erroroutput,"decoding simple\n");
		fprintf(outputfile,"<!--noncomplex document-->\n");
		wvDecodeSimple(&fib,NULL,mainfd,tablefd,data);

		fseek(mainfd,1024,SEEK_SET);
		decode_simple(mainfd,tablefd,data,fib.fcClx,fib.fcMin,fib.fcMac,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,&a_list_info,masterstylesheet,&portions,&ffn_sttbf,0);
		}


	if ( (list_author_key) && (key_atrd!=NULL) && (doannos))
		{
		fprintf(outputfile,"<p>\n");
		fprintf(outputfile,"<table border=1>\n");
		fprintf(outputfile,"<tr><td colspan=2><b>Annotation Author Key</b></td></tr>");
		fprintf(outputfile,"<tr><td>Initials</td><td>Full Name</td></tr>");

		for (i=0;i<portions.authors->noofstrings;i++)
			{
			fprintf(outputfile,"<tr><td>");

			j = key_atrd[i].xstUsrInitl[0];
  			for (j=1;j<key_atrd[i].xstUsrInitl[0]+1;j++)
                    {
                    /*warning despite the possibility of being 16 bit nos ive done this*/
                    fprintf(outputfile,"%c",key_atrd[i].xstUsrInitl[j]);
                    }
			j = 0;
			fprintf(outputfile,"</td><td>");
			freegroup = portions.authors;
			while ((j<key_atrd[i].ibst) && (freegroup != NULL))
				{
				freegroup = freegroup->next;
				j++;
				}
			k=0;	

			if (freegroup->u16string != NULL)
				{
				while(freegroup->u16string[k] != '\0')
						{
						/*warning despite the possibility of being 16 bit nos ive done this*/
						fprintf(outputfile,"%c",freegroup->u16string[k]);
						k++;
						}
				}
			fprintf(outputfile,"</td></tr>");
			}
		fprintf(outputfile,"</table>\n");
		}

	wvReleaseFFN_STTBF(&ffn_sttbf);
	wvReleaseSTTBF(&anSttbfAssoc);
	wvFree(title);

	while (afbse_list != NULL)
		{
		tfbse_list = afbse_list;
		afbse_list = afbse_list->next;
		free(tfbse_list);
		}

	while (afsp_list != NULL)
		{
		while (afsp_list->afopte_list != NULL)
			{
			tfopte_list = afsp_list->afopte_list;
			afsp_list->afopte_list = afsp_list->afopte_list->next;
			free(tfopte_list);
			}
		tfsp_list = afsp_list;
		afsp_list = afsp_list->next;
		free(tfsp_list);
		}

	while (in_style != NULL)
		{
		temp_styles = in_style;
		if (temp_styles->begin!=NULL)
			free(temp_styles->begin);
		if (temp_styles->end!=NULL)
			free(temp_styles->end);
		if (temp_styles->Default!=NULL)
			free(temp_styles->Default);
		if (temp_styles->bold!=NULL)
			free(temp_styles->bold);
		if (temp_styles->font!=NULL)
			free(temp_styles->font);
		if (temp_styles->italic!=NULL)
			free(temp_styles->italic);
		if (temp_styles->name != NULL)
			free(temp_styles->name);
		if (temp_styles->prespace != NULL)
			free(temp_styles->prespace);
		if (temp_styles->postspace != NULL)
			free(temp_styles->postspace);
		in_style = in_style->next;
		free(temp_styles);
		}

	if (main_fields.cps != NULL)
		free(main_fields.cps);
	if (main_fields.flds != NULL)
		free(main_fields.flds);
	if (header_fields.cps != NULL)
		free(header_fields.cps);
	if (header_fields.flds != NULL)
		free(header_fields.flds);
	if (footnote_fields.cps != NULL)
		free(footnote_fields.cps);
	if (footnote_fields.flds != NULL)
		free(footnote_fields.flds);
	if (annotation_fields.cps != NULL)
		free(annotation_fields.cps);
	if (annotation_fields.flds != NULL)
		free(annotation_fields.flds);
	if (endnote_fields.cps != NULL)
		free(endnote_fields.cps);
	if (endnote_fields.flds != NULL)
		free(endnote_fields.flds);




	if (a_list_info.array != NULL)
		free(a_list_info.array);
	if (a_list_info.o_lst_ids != NULL)
		free(a_list_info.o_lst_ids);
	if (a_list_info.level != NULL)
		free(a_list_info.level);
	if (a_list_info.lstarray != NULL)
		free(a_list_info.lstarray);
	if (a_list_info.lst_ids != NULL)
		free(a_list_info.lst_ids);

	if (a_list_info.current_index_nos != NULL)
		{
		for (i=0;i<a_list_info.nooflsts+1;i++)
			free(a_list_info.current_index_nos[i]);
		free(a_list_info.current_index_nos);
		}

	if (a_list_info.o_list_def != NULL)
		{
		for (i=0;i<a_list_info.nooflfos;i++)
			{
			if (a_list_info.overridecount[i] > 0)
				{
				free_def = &(a_list_info.o_list_def[i]);
				if (free_def->list_string != NULL)
					free(free_def->list_string);
				free_def = free_def->sub_def_list;
				while (free_def != NULL)
					{
					free_def2 = free_def;
					free_def = free_def->sub_def_list;
					if (free_def2->list_string != NULL)
						free(free_def2->list_string);
					free(free_def2);
					}
				}
			}
		free(a_list_info.o_list_def);
		}       

	if (a_list_info.overridecount != NULL)
		free(a_list_info.overridecount);

	if (a_list_info.a_list_def != NULL)
		{
		for (i=0;i<a_list_info.nooflsts;i++)
			{
			free_def = &(a_list_info.a_list_def[i]);
			if (free_def->list_string != NULL)
				free(free_def->list_string);
			free_def = free_def->sub_def_list;
			while (free_def != NULL)
				{
				free_def2 = free_def;
				free_def = free_def->sub_def_list;
				if (free_def2->list_string != NULL)
					free(free_def2->list_string);
				free(free_def2);
				}
			}
		free(a_list_info.a_list_def);
		}

	
	if (masterstylesheet != NULL)
		{
		wvReleaseSTSH(&masterstylesheet[0].stsh);
		for(i=0;i<nostyles;i++)
			if (masterstylesheet[i].name != NULL)
				free(masterstylesheet[i].name);
		free(masterstylesheet);
		}
	
	wvFree(plcfbtePapx);
	wvFree(plcfbteChpx);

	wvFreeXst(&portions.authors);

	while (portions.ablipdata != NULL)
		{
		freeme = portions.ablipdata;
		portions.ablipdata = portions.ablipdata->next;
		free(freeme);
		}

	wvFree(portions.endRef);
	wvFree(portions.endFRD);
	wvFree(portions.endTrueFRD);
	wvFree(portions.endTxt);
	wvFree(portions.fndRef);
	wvFree(portions.fndFRD);
	wvFree(portions.fndTxt);
	wvFree(portions.andTxt);
	wvFree(portions.andRef);
	wvFree(portions.the_atrd);
	wvFree(portions.a_bookmarks.bookmark_b_cps);
	wvFree(portions.a_bookmarks.bookmark_b_bkfs);
	wvFree(portions.a_bookmarks.bookmark_e_cps);

	wvReleaseSTTBF(&portions.annotations);
		
	if (portions.headercplist != NULL)
		free(portions.headercplist);

	wvFree(portions.officedrawcps);
	wvFree(portions.fspas);

	if (portions.section_cps != NULL)
		free(portions.section_cps);
	if (portions.section_fcs != NULL)
		free(portions.section_fcs);

	wvFree(portions.l_bookmarks.bookmark_b_cps);
	wvFree(portions.l_bookmarks.bookmark_b_bkfs);
	wvFree(portions.l_bookmarks.bookmark_e_cps);

	wvReleaseSTTBF(&portions.bookmarks);
	return(0);
	}

U32 get_fc_from_cp(U32 acp,U32 *rgfc,U32 *avalrgfc,int nopieces)
	{
	int i=0;
	U32 distance;
	U32 fc2;

	/*given a cp find the piece*/
	while(i<nopieces)
        {
        if (rgfc[i+1] >= acp)
            {
			distance = acp-rgfc[i];	
			error(erroroutput,"cp distance is %d\n",distance);
			if (avalrgfc[i] & 0x40000000UL)
				{
				fc2 = avalrgfc[i];
				fc2 = fc2 & 0xbfffffffUL;
				fc2 = fc2/2;
				return(fc2+distance);
				}
			else
				return(avalrgfc[i]+(2*distance));
            }
        i++;
        }
	return(0);
	}

int decode_simple_endnote(FILE *mainfd,FILE *tablefd,FILE *data,sep *asep,U32 fcClx,U32 fcMin,U32 fcMac,U32 intervals,U32 chpintervals,U32 *plcfbtePapx,U32 *plcfbteChpx,field_info *all_fields[4],list_info *a_list_info,style *sheet,textportions *portions,FFN_STTBF *ffn_sttbf,int flag)
	{
	U32 headerpos;
	U32 oldccpText;
	U32 begin=0;
	U32 len=0;
	int tempcp;
	int i;
	
	field_info *tempfields;
	chp achp;
	int nopieces;
	U32 *avalrgfc;
	U32 *rgfc;
	U16 *sprm;
	U32 clxcount=0;
	U32 fcvalue;
	pap indentpap;
	int anyendnotes=0;
	/*
	char roman[81];
	*/
	
	flushbreaks(0);
	init_pap(&indentpap);
	do_indent(&indentpap);

	if (noheaders)
		return(0);

	if (header == 0)
		{
		error(erroroutput,"doing endnotes page number is %d\n",pagenumber);

		for (i=0;i<portions->list_end_no;i++)
			{
			decode_endnote(&begin,&len,portions,i);

			if ((begin != -1) && (len != -1))
				{
				if (anyendnotes==0)
					fprintf(outputfile,"\n<br><img src=\"%s/endnotebegin.gif\"><br>\n",patterndir());
				anyendnotes=1;
				init_chp(&achp);
				decode_s_chp(&achp,ffn_sttbf,sheet);
				error(erroroutput,"beginning endnote\n");
				fprintf(outputfile,"<p>");
				/*test*/
				fprintf(outputfile,"<font color=#330099>");
				inafont=1;
				inacolor=1;
				strcpy(incolor,"#330099");
				/*end test*/
				if (!insuper)
					{
					fprintf(outputfile,"<sup>");
					insuper=2;
					}
				fprintf(outputfile,"<a name=\"end%d\">",i);

				if (portions->endFRD[i].frd > 0)
					{
					error(erroroutput,"FRD is %d, end is %d, i is %d\n",portions->endTrueFRD[i],portions->auto_end,i);
					decode_list_nfc(portions->endTrueFRD[i],dop.new_nfcEdnRef);
					fprintf(outputfile,"</A>");
					if (insuper==2)
						{
						fprintf(outputfile,"</sup>");
						insuper=0;
						}
					}
				else
					footnotehack=1;
					
				/*dont handling custom footnotes this way*/

				headerpos = ftell(mainfd);
				oldccpText = portions->ccpText;
				header++;

				fseek(tablefd,fcClx,SEEK_SET);
				getc(tablefd);
				nopieces = get_piecetable(tablefd,&rgfc,&avalrgfc,&sprm,&clxcount);
				
				fcvalue = get_fc_from_cp(oldccpText+portions->ccpFtn+portions->ccpHdd+portions->ccpAtn+begin,rgfc,avalrgfc,nopieces);

				free(rgfc);
				free(avalrgfc);
				free(sprm);
				error(erroroutput,"footer fcvalue is %x, old one would be %x\n",fcvalue,portions->fcMin+oldccpText+begin);
				tempcp=cp;
				portions->ccpText = len;
				cp=begin;
				tempfields = all_fields[0];
				all_fields[0] = all_fields[4];
				realcp = oldccpText+portions->ccpFtn+portions->ccpHdd+portions->ccpAtn;
				inaheaderfooter=1;
				/*temp line*/
				fseek(mainfd,fcvalue,SEEK_SET);
				decode_simple(mainfd,tablefd,data,fcClx,fcvalue,fcMac,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,2);
				inaheaderfooter=0;
				if (footnotehack == 1)
					{
					fprintf(outputfile,"</A>");
					footnotehack=0;
					}
				all_fields[0] = tempfields;
				cp=tempcp;
				realcp = tempcp;
				fseek(mainfd,headerpos,SEEK_SET);
				portions->ccpText = oldccpText;
				header--;
				error(erroroutput,"ending endnote\n");
				}
			}
		
		portions->list_end_no=0; /*ready for next section endnotes*/
		portions->auto_end=1;

		
		
		init_chp(&achp);
		decode_s_chp(&achp,ffn_sttbf,sheet);
		}
	return(anyendnotes);
	}


int decode_simple_footer(FILE *mainfd,FILE *tablefd,FILE *data,sep *asep,U32 fcClx,U32 fcMin,U32 fcMac,U32 intervals,U32 chpintervals,U32 *plcfbtePapx,U32 *plcfbteChpx,field_info *all_fields[4],list_info *a_list_info,style *sheet,textportions *portions,FFN_STTBF *ffn_sttbf,int flag)
	{
	U32 headerpos;
	U32 oldccpText;
	U32 begin=0;
	U32 len=0;
	int tempcp;
	int i,j;
	int footnoteflag=0;
	int annotationflag=0;
	field_info *tempfields;
	chp achp;
	int nopieces;
	U32 *avalrgfc;
	U32 *rgfc;
	U16 *sprm;
	U32 clxcount=0;
	U32 fcvalue;
	pap indentpap;
	int ret=0;

	flushbreaks(0);
	init_pap(&indentpap);
	do_indent(&indentpap);

	if (noheaders)
		return(ret);
	
	if (header == 0)
		{
		error(erroroutput,"doing footer page number is %d\n",pagenumber);

		for (i=portions->last_foot;i<portions->list_foot_no+portions->last_foot;i++)
			{
			decode_footnote(&begin,&len,portions,i);

			if ((begin != -1) && (len != -1))
				{
				init_chp(&achp);
				decode_s_chp(&achp,ffn_sttbf,sheet);
				if (footnoteflag == 0)
					{
					fprintf(outputfile,"\n<br><img src=\"%s/footnotebegin.gif\"><br>\n",patterndir());
					footnoteflag=1;
					}
				error(erroroutput,"beginning footnote\n");
				fprintf(outputfile,"<p>");
				/*test*/
				fprintf(outputfile,"<font color=#330099>");
				inafont=1;
				inacolor=1;
				strcpy(incolor,"#330099");
				/*end test*/
				if (!insuper)
					{
					fprintf(outputfile,"<sup>");
					insuper=2;
					}
				fprintf(outputfile,"<a name=\"foot%d\">",i);

				if (portions->fndFRD[i].frd > 0)
					{
					decode_list_nfc(portions->fndFRD[i].frd,dop.new_nfcFtnRef);
					fprintf(outputfile,"</A>");
					if (insuper==2)
						{
						fprintf(outputfile,"</sup>");
						insuper=0;
						}
					}
				else
					footnotehack=1;
				/*dont handling custom footnotes this way*/

				headerpos = ftell(mainfd);
				oldccpText = portions->ccpText;
				header++;

				fseek(tablefd,fcClx,SEEK_SET);
				getc(tablefd);
				nopieces = get_piecetable(tablefd,&rgfc,&avalrgfc,&sprm,&clxcount);
				
				fcvalue = get_fc_from_cp(oldccpText+begin,rgfc,avalrgfc,nopieces);

				free(rgfc);
				free(avalrgfc);
				free(sprm);
				/*			
				fseek(mainfd,portions->fcMin+oldccpText+begin,SEEK_SET);
				*/
				error(erroroutput,"footer fcvalue is %x, old one would be %x\n",fcvalue,portions->fcMin+oldccpText+begin);
				tempcp=cp;
				portions->ccpText = len;
				cp=begin;
				tempfields = all_fields[0];
				all_fields[0] = all_fields[2];
				realcp = oldccpText+begin;
				inaheaderfooter=1;
				/*temp line*/
				fseek(mainfd,fcvalue,SEEK_SET);
				decode_simple(mainfd,tablefd,data,fcClx,fcvalue,fcMac,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,2);
				inaheaderfooter=0;
				if (footnotehack == 1)
					{
					fprintf(outputfile,"</A>");
					footnotehack=0;
					}
				all_fields[0] = tempfields;
				cp=tempcp;
				realcp = tempcp;
				fseek(mainfd,headerpos,SEEK_SET);
				portions->ccpText = oldccpText;
				header--;
				error(erroroutput,"ending footnote\n");
				}
			}

		portions->last_foot+=portions->list_foot_no;
		portions->list_foot_no=0; /*ready for next page footnotes*/
		portions->auto_foot=1;
		if (footnoteflag != 0)
			{
			fprintf(outputfile,"\n<br><img src=\"%s/footnoteend.gif\"><br>\n",patterndir());
			}
			
		for (i=portions->last_anno;i<portions->list_anno_no+portions->last_anno;i++)
			{
			decode_footanno(&begin,&len,portions,i);
			
			init_chp(&achp);
			decode_s_chp(&achp,ffn_sttbf,sheet);

			if ((begin != -1) && (len != -1))
				{
				if (annotationflag == 0)
					{
					fprintf(outputfile,"\n<br><img src=\"%s/commentbegin.gif\"><br>\n",patterndir());
					annotationflag=1;
					}
				error(erroroutput,"beginning annotation\n");
				/*test*/
				fprintf(outputfile,"<font color=#ff7777>");
				inafont=1;
				inacolor=1;
				strcpy(incolor,"#ff7777");
				/*end test*/
				if (!insuper)
					fprintf(outputfile,"<sup>");
				/*
				fprintf(outputfile,"<a name=\"anno%d\">",i);
				*/
				fprintf(outputfile,"<a name=\"");
				j = portions->the_atrd[i].xstUsrInitl[0];
				for (j=1;j<portions->the_atrd[i].xstUsrInitl[0]+1;j++)
                    {
                    /*warning despite the possibility of being 16 bit nos ive done this*/
                    fprintf(outputfile,"%c",portions->the_atrd[i].xstUsrInitl[j]);
					}
				fprintf(outputfile,"%d",i);
				fprintf(outputfile,"\">[");
				list_author_key=1;
				j = portions->the_atrd[i].xstUsrInitl[0];
				for (j=1;j<portions->the_atrd[i].xstUsrInitl[0]+1;j++)
                    {
                    /*warning despite the possibility of being 16 bit nos ive done this*/
                    fprintf(outputfile,"%c",portions->the_atrd[i].xstUsrInitl[j]);
					}
				fprintf(outputfile,"%d]",i+1);
				/*
				fprintf(outputfile,"anno%d",i);
				*/
				fprintf(outputfile,"</a>");
				if (!insuper)
					fprintf(outputfile,"</sup>");

				headerpos = ftell(mainfd);
				oldccpText = portions->ccpText;
				header++;

				fseek(tablefd,fcClx,SEEK_SET);
				getc(tablefd);
				nopieces = get_piecetable(tablefd,&rgfc,&avalrgfc,&sprm,&clxcount);
				
				fcvalue = get_fc_from_cp(oldccpText+portions->ccpFtn+portions->ccpHdd+begin,rgfc,avalrgfc,nopieces);

				free(rgfc);
				free(avalrgfc);
				free(sprm);
				fseek(mainfd,fcvalue,SEEK_SET);
				tempcp=cp;
				portions->ccpText = len;
				cp=begin;
				tempfields = all_fields[0];
				all_fields[0] = all_fields[3];
				realcp = oldccpText+portions->ccpFtn+portions->ccpHdd+begin;
				for(j=0;j<all_fields[3]->no;j++)
					{
					if (cp == all_fields[3]->cps[j])
						{
						if (all_fields[3]->flds[j].var1.flt == 19)
							all_fields[3]->flds[j].var1.flt =0;
						}
					}
				decode_simple(mainfd,tablefd,data,fcClx,fcvalue,fcMac,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,3);
				all_fields[0] = tempfields;
				cp=tempcp;
				realcp = tempcp;
				fseek(mainfd,headerpos,SEEK_SET);
				portions->ccpText = oldccpText;
				header--;
				error(erroroutput,"ending annotation\n");
				}
			}

		if (annotationflag)
			fprintf(outputfile,"\n<br><img src=\"%s/commentend.gif\"><br>\n",patterndir());
			
		portions->last_anno+=portions->list_anno_no;
		portions->list_anno_no=0; /*ready for next page footnotes*/
		
		decode_footer(&begin,&len,portions,asep);
		if ((begin != -1) && (len != -1))
			{
			init_chp(&achp);
			decode_s_chp(&achp,ffn_sttbf,sheet);
			headerpos = ftell(mainfd);
			oldccpText = portions->ccpText;
			header++;


			fseek(tablefd,fcClx,SEEK_SET);
			getc(tablefd);
			nopieces = get_piecetable(tablefd,&rgfc,&avalrgfc,&sprm,&clxcount);

			
			fcvalue = get_fc_from_cp(portions->ccpFtn+oldccpText+begin,rgfc,avalrgfc,nopieces);

			free(rgfc);
			free(avalrgfc);
			free(sprm);
			error(erroroutput,"standard footer, fcvalue is %x, old method %d\n",fcvalue,portions->fcMin+portions->ccpFtn+oldccpText+begin);
			
		/*	
			fseek(mainfd,portions->fcMin+portions->ccpFtn+oldccpText+begin,SEEK_SET);
		*/
	
			fseek(mainfd,fcvalue,SEEK_SET);
	
			tempcp=cp;
			portions->ccpText = len;
			cp=begin;
			tempfields=all_fields[0];
			all_fields[0] = all_fields[1];
			realcp = oldccpText+portions->ccpFtn+begin;
			
			decode_simple(mainfd,tablefd,data,fcClx,fcvalue,fcMac,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,1);
			
			all_fields[0] = tempfields;
			cp=tempcp;
			realcp = tempcp;
			fseek(mainfd,headerpos,SEEK_SET);
			portions->ccpText = oldccpText;
			header--;
			error(erroroutput,"ending footer\n");
			}

		ret = 1;
		pagenumber++;
		sectionpagenumber++;
		init_chp(&achp);
		decode_s_chp(&achp,ffn_sttbf,sheet);
		}
	return(ret);
	}

void decode_simple_header(FILE *mainfd,FILE *tablefd,FILE *data,sep *asep,U32 fcClx,U32 fcMin,U32 fcMac,U32 intervals,U32 chpintervals,U32 *plcfbtePapx,U32 *plcfbteChpx,field_info *all_fields[5],list_info *a_list_info,style *sheet,textportions *portions,FFN_STTBF *ffn_sttbf,int flag)
	{
	U32 headerpos;
	U32 oldccpText;
	U32 begin=0;
	U32 len=0;
	int tempcp;
	int temprealcp;
	field_info *tempfields;
	int i;
	int nopieces;
	U32 *avalrgfc=NULL;
	U32 *rgfc=NULL;
	U16 *sprm=NULL;
	U32 fcvalue;
	U32 clxcount=0;

	if (noheaders)
		return;
	

	if (header == 0)
		{
		error(erroroutput,"doing header\n");
		for (i=0;i<1;i++)
			{
			if (i==0)
				decode_header(&begin,&len,portions,asep);
#if 0
			/*
			im ignoring this for now, i think this is
			a header textbox, so ill hold off on this 
			until i get it right
			*/
			else
				decode_header2(&begin,&len,portions);
#endif
				
			if ((begin != -1) && (len != -1))
				{
				
				fseek(tablefd,fcClx,SEEK_SET);
				getc(tablefd);
				nopieces = get_piecetable(tablefd,&rgfc,&avalrgfc,&sprm,&clxcount);
				

				headerpos = ftell(mainfd);
				oldccpText = portions->ccpText;
				header++;

				fcvalue = get_fc_from_cp(portions->ccpFtn+oldccpText+begin,rgfc,avalrgfc,nopieces);
				
				fseek(mainfd,fcvalue,SEEK_SET);

				free(avalrgfc);
				free(rgfc);
				free(sprm);

				tempcp=cp;
				portions->ccpText = len;
				cp=begin;
				tempfields = all_fields[0];
				all_fields[0] = all_fields[1];
				error(erroroutput,"overall header begin is %x\n",portions->fcMin+oldccpText+portions->ccpFtn+begin);
				error(erroroutput,"fcvalue is %x\n, ordinary seek is %x",fcvalue,portions->fcMin+portions->ccpFtn+oldccpText+begin);
				temprealcp = realcp;
				realcp = oldccpText+portions->ccpFtn+begin;
				inaheaderfooter=1;
				decode_simple(mainfd,tablefd,data,fcClx,fcvalue,fcMac,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,1);
				inaheaderfooter=0;
				all_fields[0] = tempfields;
				cp=tempcp;
				realcp = temprealcp;
				error(erroroutput,"after header realcp is %x\n",realcp);
				fseek(mainfd,headerpos,SEEK_SET);
				portions->ccpText = oldccpText;
				header--;
				error(erroroutput,"ending header\n");
				}
			}

		}
	}

void get_next_f_ref(textportions *portions,signed long *nextfootnote)
	{
	int i;
	if (portions->fndref_no > 0)
		{
		/*then there might be footnotes to look out for*/
		for(i=0;i<portions->fndref_no;i++)
			{
			error(erroroutput,"footnote blah %d, %ld, %ld",portions->fndFRD[i].frd,*nextfootnote,portions->fndRef[i]);
			if ( (portions->fndFRD[i].frd <= 0) && (*nextfootnote < portions->fndRef[i]) )
				{
				/*not automatically numbered, so we have to look out for it manually*/
				*nextfootnote = portions->fndRef[i];
				error(erroroutput,"the next footnote is at cp %x\n",*nextfootnote);
				break;
				}
			}
		
		}
	}

void get_next_e_ref(textportions *portions,signed long *nextendnote)
	{
	int i;
	if (portions->endref_no > 0)
		{
		/*then there might be endnotes to look out for*/
		for(i=0;i<portions->endref_no;i++)
			{
			error(erroroutput,"endnote blah %d, %ld, %ld",portions->endFRD[i].frd,*nextendnote,portions->endRef[i]);
			if ( (*nextendnote) < portions->endRef[i] )
				{
				/*not automatically numbered, so we have to look out for it manually*/
				*nextendnote = portions->endRef[i];
				error(erroroutput,"the next endnote is at cp %x\n",*nextendnote);
				break;
				}
			}
		
		}
	}

/* 
how does this work
we seek to the beginning of the text, we loop for a count of charaters that is stored in the fib.

the piecetable divides the text up into various sections, we keep track of our location vs
the next entry in that table, when we reach that location, we seek to the position that
the table tells us to go.

there are special cases for coming to the end of a section, and for the beginning and ends of
pages. for the purposes of headers and footers etc.
*/
void decode_simple(FILE *mainfd,FILE *tablefd,FILE *data,U32 fcclx,U32 fcmin,U32 fcmac,U32 intervals,U32 chpintervals,U32 *plcfbtepapx,U32 *plcfbtechpx,field_info *all_fields[5],list_info *a_list_info,style *sheet,textportions *portions,FFN_STTBF *ffn_sttbf,int flag)
	{
	
	U32 nextfc=0;
	U32 nextchpfc=0;
	U32 lastfc=0;
	U32 lastchpfc=0;
	U32 internalcp;
	U32 limitcp;
	pap *apap;
	chp *achp;
	sep *asep=NULL;
	sep *tempsep=NULL;
	pap *temppap = NULL;
	chp *tempchp = NULL;
	int i;
	int letter;
	int tempfck;
	int newpage=1;
	U32 sepxfc;
	
	int tablemagic;

	U32 *rgfc=NULL;
	U32 *avalrgfc=NULL;
	
	U16 *sprm=NULL;
	U32 clxcount=0;
	int nopieces;
	signed long nextfootnote=0;
	signed long nextendnote=0;
	U32 nextbookmark_b=realcp; /*was 0*/
	U32 nextbookmark_e=realcp; /*was 0*/
	U32 nextannotation_b=realcp; /*was 0*/
	U32 nextannotation_e=realcp; /*was 0*/

	U32 untilfc=0;

	int flag_8_16=0;

	int d_count=0;

	U32 nextpiece=0;

	int issection=1;

	U32 sepcp;
	int charsetflag=0;


	apap = NULL;
	achp = NULL;
	nextfc=fcmin;
	nextchpfc=fcmin;



	/* go to the text starting position*/
	fseek(tablefd,fcclx,SEEK_SET);
		
	/*
	hmm, simple files can be start off in 8 bit and go to 16
	bit and vice versa, is so the piecetable can be used to determine these
	limits
	*/
	getc(tablefd);
	nopieces = get_piecetable(tablefd,&rgfc,&avalrgfc,&sprm,&clxcount);
	if (nopieces > 1)
		error(erroroutput,"ah shit!, buggering microsoft over-complex \
			toolkits!!!!, theres no call for this dumb *piece* technology \
			anyway\n");

	
	/*
	right so this is to guess (over safely) whether to go into unicode mode or 
	not for the full html document, if i get it wrong then ill get away with 
	it anyway
	*/
	if (header==0)
		charsetflag = do_output_start(avalrgfc,nopieces,doc_style);

	/*
	now we have a problem as some simple (hah!!!) docs can go from 16 bit text 
	to 8 bit and vice versa depending on the piece table, so keep an eye on that
	so as to know how to twiddle the flags
	*/
	query_piece_cp(rgfc,avalrgfc,nopieces,realcp,&nextpiece,&flag_8_16);

	if  ( (flag < 2) && (flag >0) )
		{
		untilfc=0xffffffffL;  /*max it out for header/footers and rely on doubled 0x0d's*/
		limitcp=0xffffffffL;
		}
	else
		limitcp=portions->ccpText;

	/*portions->cpptext gives the amount of chars to handle*/
	i=fcmin;
	internalcp=0;

	while(internalcp < limitcp)
		{
		/*
		if we have come to the end of the current piece, 
		then find and go to the next piece
		*/
		if (nextpiece == realcp)
			i = nextfc = query_piece_cp_seek(rgfc,avalrgfc,nopieces,realcp,
				&nextpiece,&flag_8_16,mainfd);

		/*get the sep*/
		if (flag == 0)
			if (issection)
				{
				error(erroroutput,"getting sep, %x, piece is %d\n",realcp,i);
				sepxfc = find_FC_sepx(realcp,&sepcp,portions);
				tempsep = asep;
				asep = get_sep(sepxfc,mainfd);
				if (dop.epc == 0) 
					if (decode_simple_endnote(mainfd,tablefd,data,asep,fcclx,fcmin,fcmac,intervals,chpintervals,plcfbtepapx,plcfbtechpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,flag))
						fprintf(outputfile,"\n<br><img src=\"%s/endnoteend.gif\"><br>\n",patterndir());
					
				if (pagenumber!=1)
					sectionbreak(asep);

				if (asep != NULL)
					{
					if (tempsep != NULL)
						free(tempsep);
					tempsep=NULL;
					}
				else
					{
					asep = tempsep;
					tempsep=NULL;
					}
					
				issection=0;
				}
		
		/*
		go down through all the text, when we hit a new page spit out the 
		header
		*/
		error(erroroutput,"fc is %ld (%x)\n",i,i);
		if (newpage)
			{
			if (!inatable)
				{
				newpage=0;
				decode_simple_header(mainfd,tablefd,data,asep,fcclx,fcmin,fcmac,intervals,chpintervals,plcfbtepapx,plcfbtechpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,flag);
				error(erroroutput,"end: flag is %d\n",flag);
				/*force new paragraph and chp check*/
				nextfc=i;
				}
			else
				deferrednewpage=1;
			}


		/*get the pap*/
		if (i == nextfc) 
			{
			lastfc = i;
			temppap = apap;
			tempfck = find_FKPno_papx(i,plcfbtepapx,intervals);
			if (tempfck != -1)
				{
				apap = get_pap(tempfck,mainfd,i,&nextfc,sheet,a_list_info);
				}
			
			if (apap == NULL)
				{
				error(erroroutput,"found no pap, reverting to previous\n");
				apap = temppap;
				temppap=NULL;
				}

			/*if table set search for tap*/
			if (apap->fInTable)
				fill_table_info(apap,i,plcfbtepapx,intervals,mainfd,sheet,
					a_list_info);
			}

		/* get the chp, using the paps istd for a basis*/
		if ((i == nextchpfc) || (lastfc == i)) 
			{
			lastchpfc=i;
			error(erroroutput,"looking for chp\n");
			tempchp = achp;
			tempfck = find_FKPno_chpx(i,plcfbtechpx,chpintervals);
			if (tempfck != -1)
				achp = get_chp(tempfck,mainfd,data,i,&nextchpfc,sheet,apap->istd);

			if (achp == NULL)
				{
				error(erroroutput,"found no chp, reverting to previous\n");
				achp = tempchp;
				}
			else if (tempchp != NULL)
				{
				free(tempchp);
				tempchp=NULL;
				}

			if (achp->color[0] == '\0')
				{
				if (flag == 1)
					{
					strcpy(achp->color,"#7f5555");
					error(erroroutput,"color set\n");
					}
				else if (flag == 2)
					{
					strcpy(achp->color,"#330099");
					error(erroroutput,"color set\n");
					}
				else if (flag == 3)
					{
					strcpy(achp->color,"#ff7777");
					error(erroroutput,"color set\n");
					}
				else 
					{
					error(erroroutput,"color not set\n");
					}
				}

			error(erroroutput,"the next char lim is %d (%x)\n",nextchpfc,nextchpfc);
	
			/*
			decode_e_list(apap,achp,a_list_info);
			*/
			decode_e_chp(achp);
			}
			
		/*after we have a new pap, do the special codes*/
		if (lastfc == i)
			{
			decode_e_specials(apap,achp,a_list_info);
			decode_e_table(apap,achp,a_list_info,0);

			if (temppap != NULL)
				{
				/*last gasp of the pap*/
				end_para(temppap,apap);
				free(temppap);
				temppap=NULL;
				}
			else
				end_para(NULL,apap);

			tablemagic = decode_s_table(apap,achp,a_list_info,0);
#if 0
			if (tablemagic == 1)
				{
				/*
				keep getting characters and paps until the
				table is ended
				*/
				tableindex = i;
				tablepap = NULL;
				if (tablevals != NULL)
					free(tablevals);
				cno=0;
				do
					{
					if (tablepap != NULL)
						free(tablepap);
					temptab = find_FKPno_papx(tableindex,plcfbtepapx,intervals);
					tablepap = get_pap(temptab,mainfd,tableindex,&tablenextfc,sheet,a_list_info);
					fprintf(stderr,"next t para is at %x\n",tablenextfc);
					if (tablepap->fInTable)
						{
						fprintf(stderr,"filling table\n");
						fill_table_info(tablepap,tableindex,plcfbtepapx,intervals,mainfd,sheet,
							a_list_info);
						decode_s_table(tablepap,achp,a_list_info,1);
						}
					fprintf(stderr,"loop\n");
					tableindex = tablenextfc;
					}
				while (tablepap->fInTable);
				fprintf(stderr,"table is finished\n");
				cunit = gcf_list(tablevals,cno);
				fprintf(stderr,"the basic table unit is %d\n",cunit);
				}
#endif		
			decode_s_specials(apap,achp,a_list_info);
			decode_s_chp(achp,ffn_sttbf,sheet);
			}

		if (lastchpfc == i) 
			{
			decode_s_chp(achp,ffn_sttbf,sheet);
			lastchpfc = 0;
			}


		if (flag_8_16)
			letter = read_16ubit(mainfd);
		else
			letter = getc(mainfd);


		if (lastfc == i)
			{
			if ( ((letter != 12) && (letter != 13))  )
				{
				error(erroroutput,"doing list and next letter is %c %x\n",letter,letter);
				/*
				decode_s_list(apap,achp,a_list_info,ffn_sttbf,DONTIGNORENUM,sheet);
				*/
				wvGetListInfo(apap,achp,a_list_info->lfo,
					a_list_info->lfolvl,a_list_info->lvl,
					a_list_info->nolfo,a_list_info->lst,
					a_list_info->noofLST,sheet,ffn_sttbf);
				}
			else
				error(erroroutput,"a special list\n");
				
			lastfc = 0;
			}

		if (flag)
			{
			if (letter == 13)
				{
				/*
				headers and footers seem to continue until 2 0x0d are reached, they dont
				appear after all to go until the len derived from the header table
				like i thought originally.
				*/
				d_count++;
				if (d_count == 2)
					break;
				}
			else
				d_count=0;
			}

		if ( (cp == nextfootnote) && (flag == 0) ) 
			{
			decode_f_reference(portions);
			get_next_f_ref(portions,&nextfootnote);
			}

		if ( (cp == nextendnote) && (flag == 0) ) 
			{
			decode_e_reference(portions);
			get_next_e_ref(portions,&nextendnote);
			}

		while ( (realcp == nextannotation_b) && (doannos==1) )
			nextannotation_b = decode_b_annotation(&(portions->a_bookmarks));		

		while (realcp == nextbookmark_b)
			nextbookmark_b = decode_b_bookmark(&(portions->l_bookmarks),&(portions->bookmarks));

		while (realcp == nextbookmark_e)
			nextbookmark_e = decode_e_bookmark(&(portions->l_bookmarks));

		while ( (realcp == nextannotation_e) && (doannos==1) )
			nextannotation_e = decode_e_annotation(&(portions->a_bookmarks));
		
		newpage = decode_letter(letter,flag_8_16,apap,achp,all_fields[0],mainfd,data,ffn_sttbf,a_list_info,portions,&issection,sheet);

		if (newpage)
			{
			error(erroroutput,"this one\n");
			/*if we are in a table defer the pagebreaking until after the end of the row, and then pagebreak*/

			if ((inatable)  && newpage == 2)
				{
				/*in this case we have ended a row, so we should take the opportunity to halt the table*/
				fprintf(outputfile,"\n</table>\n");
				inatable=0;
				newpage=1;
				}
			
			if ((!inatable) && (newpage == 1))
				{
				decode_simple_footer(mainfd,tablefd,data,asep,fcclx,fcmin,fcmac,intervals,chpintervals,plcfbtepapx,plcfbtechpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,flag);
				if (!issection)
					pagebreak();
					
				}
			else if (newpage == 1)
				deferrednewpage=1;
				
			}


		if (flag_8_16)
			i+=2;
		else
			i++;
		internalcp++;
		}

	if (apap == NULL)
		{
		apap = (pap *)malloc(sizeof(pap));
		if (apap == NULL)
			{
			fprintf(erroroutput,"no mem available!\n");
			return;
			}
		}
	init_pap(apap);
	/*
	decode_e_list(apap,achp,a_list_info);
	*/
	decode_e_specials(apap,achp,a_list_info);
	decode_e_table(apap,achp,a_list_info,0);

	if (!newpage)
		{
		error(erroroutput,"the other one\n");
		decode_simple_footer(mainfd,tablefd,data,asep,fcclx,fcmin,fcmac,intervals,chpintervals,plcfbtepapx,plcfbtechpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,flag);
		/*
		always do the endnotes at the end of the doc, even if some have been done earlier, there
		might be more left to do
		*/
		if (decode_simple_endnote(mainfd,tablefd,data,asep,fcclx,fcmin,fcmac,intervals,chpintervals,plcfbtepapx,plcfbtechpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,flag))
			fprintf(outputfile,"\n<br><img src=\"%s/endnoteend.gif\"><br>\n",patterndir());
		}

	if (rgfc != NULL)
		free(rgfc);	
	if (avalrgfc != NULL)
		free(avalrgfc);
	if (sprm != NULL)
		free(sprm);

	if (achp != NULL)
		free(achp);
	if (apap != NULL)
		free(apap);
	if (asep != NULL)
		free(asep);
	}


void decode_header(U32 *begin,U32 *len,textportions *portions,sep *asep)
	{
	/*i dont know how these fields are working as of yet, the
	docs seem to be a bit dodgy*/
	int odd;
	int val,tval;
	*begin = 0xffffffffL;
	*len= 0xffffffffL;


	/*is this the first page of a section*/
	if ((sectionpagenumber==asep->pgnStart) && (asep->fTitlePage))
		{
		error(erroroutput,"checking for page 1 special header\n");
		if (portions->headercpno > 10)
			if (portions->headercplist[11] > portions->headercplist[10])
				{
				*begin = portions->headercplist[10];
				*len = portions->headercplist[11]-portions->headercplist[10];
				error(erroroutput,"header begin is %x end is %x\n",portions->headercplist[11],portions->headercplist[10]);
				return;
				}
		if (asep->fTitlePage)
			return;
				
		/*crazed bit of logic here :-)*/
		if (portions->headercpno > 13)
			if (portions->headercplist[portions->headercpno-1] > (portions->headercplist[10]+3))
				return;
		}
	
	/*if nothing was found continue, and pull out ordinary header*/

	if ((sectionpagenumber/2)*2 == sectionpagenumber)
		odd=0;
	else 
		odd=1;

	*len=0;


	/*if index 8 is larger than index 7 i think thats the boundaries
	of header for all pages*/

	tval = 6+(6*(sectionno-1)); /*hairy test*/

	val = tval;
	error(erroroutput,"header val is %d, odd is %d, pagenumber is %d\n",val,odd,pagenumber);

	if (!odd)
		{
		while ((*len==0) && (val>0))
			{
			if (portions->headercpno > val)
				{
				if (portions->headercplist[val+1] > portions->headercplist[val])
					{
					*begin = portions->headercplist[val];
					*len = portions->headercplist[val+1]-portions->headercplist[val];
					error(erroroutput,"header begin is %x end is %x\n",portions->headercplist[val],portions->headercplist[val+1]);
					}
				}
			else
				error(erroroutput,"well arse!!!, ive got the wrong idea about headers\n");
			val-=6;
			}
		}

	val=tval;
	
	if ((odd) || (*len == 0))
		{
		while ((*len==0) && (val>0))
			{
			if (portions->headercpno > val+1)
				{
				error(erroroutput,"val is %d headercplist[val+2] %x headercplist[val+1] %x\n",val,portions->headercplist[val+2],portions->headercplist[val+1]);
				if (portions->headercplist[val+2] > portions->headercplist[val+1])
					{
					*begin = portions->headercplist[val+1];
					*len = portions->headercplist[val+2]-portions->headercplist[val+1];
					error(erroroutput,"this header begin is %x end is %x\n",portions->headercplist[val+1],portions->headercplist[val+2]);
					}
				}
			else
				error(erroroutput,"well arse!!!, ive got the wrong idea about headers\n");
			val-=6;
			}
		}

	}

void decode_header2(U32 *begin,U32 *len,textportions *portions)
	{

	/*
	this is just a hacked together function, i have no basis for
	this at all except thats the output that word gets, dunno at all
	about this, im not seeing something obvious somewhere, hmm
	*/

	/*i dont know how these fields are working as of yet, the
	docs seem to be a bit dodgy*/
	*begin = 0xffffffffL;
	*len= 0xffffffffL;

	/*is this the first page of a section*/
	if (sectionpagenumber==1)
		return;
	
	/*if index 8 is larger than index 7 i think thats the boundaries
	of header for all pages*/
	if (portions->headercpno > 12)
		if (portions->headercplist[13] > portions->headercplist[12])
			{
			*begin = portions->headercplist[12];
			*len = portions->headercplist[13]-portions->headercplist[12];
			error(erroroutput,"header begin is %x end is %x\n",portions->headercplist[12],portions->headercplist[13]);
			}
	else
		error(erroroutput,"well arse!!!, ive got the wrong idea about headers\n");

	}


void decode_endnote(U32 *begin,U32 *len,textportions *portions,int i)
	{
	error(erroroutput,"endnote cp begin is %x end is %x\n",portions->endTxt[i],portions->endTxt[i+1]);
	*begin = portions->endTxt[i];
	*len = portions->endTxt[i+1]-*begin;
	}

void decode_footnote(U32 *begin,U32 *len,textportions *portions,int i)
	{
	error(erroroutput,"footnote cp begin is %x end is %x\n",portions->fndTxt[i],portions->fndTxt[i+1]);
	*begin = portions->fndTxt[i];
	*len = portions->fndTxt[i+1]-*begin;
	}

void decode_footanno(U32 *begin,U32 *len,textportions *portions,int i)
	{
	error(erroroutput,"annotation cp begin is %x end is %x\n",portions->andTxt[i],portions->andTxt[i+1]);
	*begin = portions->andTxt[i];
	*len = portions->andTxt[i+1]-*begin;
	}

void decode_footer(U32 *begin,U32 *len,textportions *portions,sep *asep)
	{
	/*i dont know how these fields are working as of yet, the
	docs seem to be a bit dodgy*/
	int odd;
	int val,tval;
	*begin = 0xffffffffL;
	*len= 0xffffffffL;
	
	error(erroroutput,"pagenumber is %d,sectionpagenumber is %d, pgnStart is %d, title is %d\n",pagenumber, sectionpagenumber,asep->pgnStart,asep->fTitlePage);

	if ((sectionpagenumber==asep->pgnStart) && (asep->fTitlePage))
		{
		error(erroroutput,"checking for page 1 special footer\n");
		if (portions->headercpno > 12)
			{
			if (portions->headercplist[12] > portions->headercplist[11])
				{
				*begin = portions->headercplist[11];
				*len = portions->headercplist[12]-portions->headercplist[11];
				fprintf(outputfile,"\n<hr width=\"50%%\">\n");
				error(erroroutput,"header begin is %x end is %x\n",portions->headercplist[12],portions->headercplist[11]);
				return;
				}
			}
		if (asep->fTitlePage)
			return;

		/*crazed bit of logic here :-)*/
		if (portions->headercpno > 14)
			if (portions->headercplist[portions->headercpno-1] > (portions->headercplist[11]+3))
				return;
		}

	/*else contunue down the wire*/
	/*
	if its an odd page look at that, return the appropiate portion,
	if that is 0 len then return the even*/
	/*if index 10 is larger than index 9 i think thats the boundaries
	of header for odd(or all) pages*/


	if ((sectionpagenumber/2)*2 == sectionpagenumber)
		odd=0;
	else 
		odd=1;

/*
	fprintf(outputfile,"\n<hr width=\"50%%\">\n");
*/

	*len=0;

	tval = 8+ ((sectionno-1)*6);

	error(erroroutput,"odd is %d, section no is %d, tval is %d, *len is %d\n",odd,sectionno,tval,*len);

	val=tval;
	if (!odd)
		{
		while ((*len == 0) && (val > 0))
			{
			if (portions->headercpno > val)
				{
				if (portions->headercplist[val+1] > portions->headercplist[val])
					{
					*begin = portions->headercplist[val];
					*len = portions->headercplist[val+1]-portions->headercplist[val];
					fprintf(outputfile,"\n<hr width=\"50%%\">\n");
					error(erroroutput,"even begin is %x end is %x\n",portions->headercplist[val],portions->headercplist[val+1]);
					}
				}
			else
				error(erroroutput,"well arse!!!, ive got the wrong idea about footers val %d\n",portions->headercpno);
			val-=6;
			}
		}

	val = tval;

	if ( (odd) || (*len == 0))
		{
		while ((*len == 0) && (val > 0))
			{
			if (portions->headercpno > val+1)
				{
				if (portions->headercplist[val+2] > portions->headercplist[val+1])
					{
					*begin = portions->headercplist[val+1];
					*len = portions->headercplist[val+2]-portions->headercplist[val+1];
					fprintf(outputfile,"\n<hr width=\"50%%\">\n");
					error(erroroutput,"odd begin is %x end is %x\n",portions->headercplist[val+1],portions->headercplist[val+2]);
					}
				}
			else
				error(erroroutput,"well arse!!!, ive got the wrong idea about footers val %d\n",portions->headercpno);
			val-=6;
			}
		}
		
	}

void decode_s_chp(chp *achp, FFN_STTBF *ffn_sttbf,style *sheet)
	{
	char *handle;
	char size[128];
	char fontname[1024];
	char color[16];
	char *size1=NULL;
	char *fontname1=NULL;
	char *color1=NULL;
	char *tempfont=NULL;
	error(erroroutput,"in start chp\n");
	if (chps)
		return;
	error(erroroutput,"still in start chp\n");
	error(erroroutput,"incolor is %s,font =%d, current font is %d, currentfont is %d, achpfont is %d\n", incolor,achp->fontsize,currentfontsize,currentfontcode,achp->ftcAscii);

	error(erroroutput,"bold coming in is %d\n",achp->fBold);

#if 0
	if (achp->fStrike)
		strcpy(achp->color,"#ed32ff");		/*i just ensure that strike through text becomes this horrible shade of pink*/
#endif
	if (achp->fDStrike)
		strcpy(achp->color,"#ff7332");		


	if ( (achp->fRMark) &&  (achp->underline) && (revision==0) )
		{
		fprintf(outputfile,"\n<img src=\"%s/bnewlytyped.jpg\">",patterndir());
		revisiontag = achp->ibstRMark;
		revisiontime = achp->dttmRMark;
		revision=1;
		}

	if ( (achp->fRMarkDel) &&  (achp->fStrike) && (revision==0) )
		{
		fprintf(outputfile,"\n<img src=\"%s/bdeleted.jpg\">",patterndir());
		revisiontag = achp->ibstRMarkDel;
		revisiontime = achp->dttmRMarkDel;
		revision=2;
		}

	if ( (achp->fPropRMark) &&  (doannos) && (revision==0) )
		{
		fprintf(outputfile,"\n<img src=\"%s/battributes.jpg\">",patterndir());
		revisiontag = achp->ibstPropRMark;
		revisiontime = achp->dttmPropRMark;
		revision=3;
		}
#if 0	
	if (achp->allowedfont && ((use_fontfacequery(achp) && (achp->ftcAscii != currentfontcode)) || (achp->fontsize!=currentfontsize) || (strcmp(achp->color,incolor)) /* ( (achp->color[0] != '\0') && (!inacolor))  || ( (achp->color[0] == '\0' ) && (inacolor))*/   ))
#endif
	
	if (allowedfont(sheet,achp->istd) && ((use_fontfacequery(achp) && (achp->ftcAscii != currentfontcode)) || (achp->fontsize!=currentfontsize) || (strcmp(achp->color,incolor)) /* ( (achp->color[0] != '\0') && (!inacolor))  || ( (achp->color[0] == '\0' ) && (inacolor))*/   ))
		{
		error(erroroutput,"b: font =%d %s\n", achp->fontsize,achp->color);
		if (inunderline)
			{
			fprintf(outputfile,"</u>");
			inunderline=0;
			}
		if (initalic)
			{
			fprintf(outputfile,"</i>");
			initalic=0;
			}
		if (inbold)
			{
			fprintf(outputfile,"%s",ele_style[BOLD].end);
			inbold=0;
			}
		if (inblink)
			{
			fprintf(outputfile,"</blink>");
			inblink=0;
			}
		if (inafont)
			{
			inacolor=0;
			incolor[0] = '\0';
			fprintf(outputfile,"%s",ele_style[FONT].end);
			currentfontsize=NORMAL;
			inafont=0;
			}

		if ( (achp->color[0] != '\0') || (achp->fontsize!=currentfontsize) || ((use_fontfacequery(achp) && (achp->ftcAscii != currentfontcode))))
			{
			if ( (((achp->fontsize/2)-NORMAL/2) != 0) && (achp->fontsize!=NORMAL))
				{
				sprintf(size,"%+d", (achp->fontsize/2)-NORMAL/2);
				size1=size;
				inafont=1;
				}

			if (achp->color[0] != '\0')
				{
				sprintf(color,"%s", achp->color);
				color1 = color;
				inafont=1;
				inacolor=1;
				}

			strcpy(incolor,achp->color);

			if (use_fontfacequery(achp))
				{
				/*do diggery pokery to get fontface*/
				tempfont = wvGetFontnameFromCode(ffn_sttbf,achp->ftcAscii);
				if (tempfont != NULL)
					{
					if (!strcmp(tempfont,"times new roman"))
						sprintf(fontname,"%s,%s",tempfont,"times");
					else if (!strcmp(tempfont,"courier new"))
						sprintf(fontname,"%s,%s",tempfont,"courier");
					else 
						sprintf(fontname,"%s,%s",tempfont,"helvetica");
					fontname1 = fontname;
					}
				inafont=1;
				}
			
			/*set currentfontface to this chps*/
			currentfontcode = achp->ftcAscii;
			handle = expand_element(ele_style[FONT].begin, fontname1, color1, size1);
			fprintf(outputfile,"%s",handle);
			free(handle);
			currentfontsize=achp->fontsize;
			wvFree(tempfont);
			}
		}


	if ((achp->supersubscript == 1) && (!insuper))
		{
		error(erroroutput,"superscript begins\n");
		fprintf(outputfile,"<sup>");
		insuper=1;
		}

	if ((achp->supersubscript == 2) && (!insub))
        {
        error(erroroutput,"subscript begins\n");
        fprintf(outputfile,"<sub>");
        insub=1;
        }

	error(erroroutput,"bold is %d\n",achp->fBold);

	if ((achp->sfxtText) && (inblink == 0))
		{
		fprintf(outputfile,"<blink>");
		inblink=1;
		}

	if ((achp->fBold) && (inbold == 0))
		{
		fprintf(outputfile,"%s",ele_style[BOLD].begin);
		inbold=1;
		}
	
	if ((achp->fItalic) && (initalic==0))
		{
		fprintf(outputfile,"%s",ele_style[ITALIC].begin);
		initalic=1;
		}


	if ((achp->underline) && (inunderline == 0))
		{
		fprintf(outputfile,"<u>");
		inunderline=1;
		}

	if ((achp->fStrike) && (instrike==0))
		{
		fprintf(outputfile,"<s>");
		instrike=1;
		}

	if ((achp->fDStrike) && (indstrike==0))
		{
		fprintf(outputfile,"<s>");
		indstrike=1;
		}
	}

void end_para(pap *apap,pap *newpap)
	{
	char *handle;
	error(erroroutput,"thisend of para\n");
		
	if (apap != NULL)
		if (apap->fInTable == 1) 
			return;
#if 0	/*brcBottom disabled for the moment*/

	if (apap != NULL)
		{
		height = (apap->brcBottom>>24)&0x1f;
		error(erroroutput,"-->border depth is %d\n",height);
		
		if (newpap != NULL)
			{
			if (apap->brcLeft == newpap->brcLeft)
				if (apap->brcRight == newpap->brcRight)
					if (apap->fInTable == newpap->fInTable)
						if (apap->dxaWidth == newpap->dxaWidth)
							height = (apap->brcBetween>>24)&0x1f;	
			}
		}
	
	if (height > 1)
		{
		if (flushbreaks(0))
			{
			error(erroroutput,"<!--new paragraph-->");
			do_indent(apap);
			}
		error(erroroutput,"apap height\n");
		fprintf(outputfile,"\n<img width=1 height=%d src=\"%s/clear.gif\"><br>\n",height*2,patterndir());
		}
#endif

	if (apap != NULL)
		{
		if (apap->dyaAfter > 0)
			{
			if (flushbreaks(0))
				{
				error(erroroutput,"<!--new paragraph-->");
				do_indent(apap);
				}
			if (apap->dyaAfter/TWIRPS_PER_V_PIXEL > 1)
				{
				error(erroroutput,"apap height\n");
				if (apap->postspace != NULL)
					{
					handle = expand_variables(apap->postspace,apap);
					fprintf(outputfile,"%s\n",handle);
					free(handle);
					}
				else
					{
					fprintf(outputfile,"\n<!--oldbottom--><img width=1 height=%d src=\"%s/clear.gif\"><br>\n",apap->dyaAfter/TWIRPS_PER_V_PIXEL,patterndir());
					}
				}
			}
		}


	if (flushbreaks(0))
		{
		error(erroroutput,"<!--new paragraph-->");
		do_indent(newpap);
		}
	fprintf(outputfile,"\n<!--newpara-->\n");

	if (newpap != NULL)
		{
		if (newpap->fInTable == 1)  
			return;

		if (newpap->dyaBefore > 0)
			{
			if (newpap->dyaBefore/TWIRPS_PER_V_PIXEL >1)
				{
				error(erroroutput,"newpap height\n");
				if (newpap->prespace != NULL)
					{
					handle = expand_variables(newpap->prespace,newpap);
					fprintf(outputfile,"%s\n",handle);
					free(handle);
					}
				else 
					fprintf(outputfile,"\n<!--newtop--><img width=1 height=%d src=\"%s/clear.gif\"><br>\n",newpap->dyaBefore/TWIRPS_PER_V_PIXEL,patterndir());
				}
			}
		}
	}

void decode_e_chp(chp *achp)
	{
	int colorflag=0;
	U16 *letter16;
	char *letter8;
	error(erroroutput,"in end chp\n");
	if (chps)
		return;
#if 0
	if (achp->fStrike)
		strcpy(achp->color,"#ed32ff");		/*i just ensure that strike through text becomes this horrible shade of pink*/
#endif
	if (achp->fDStrike)
		strcpy(achp->color,"#ff7332");		

	if ((achp->fDStrike==0) && (indstrike==1))
		{
		fprintf(outputfile,"</s>");
		indstrike=0;
		}

	if ((achp->fStrike==0) && (instrike==1))
		{
		fprintf(outputfile,"</s>");
		instrike=0;
		}

	if ((inunderline== 1) && (achp->underline == 0))
		{
		inunderline=0;
		fprintf(outputfile,"</U>");
		}


	if ((initalic == 1) && (achp->fItalic == 0))
		{
		if (inunderline== 1)
			{
			inunderline=0;
			fprintf(outputfile,"</U>");
			}
		initalic=0;
		fprintf(outputfile,"</I>");
		}

	if ((inbold==1) && (!achp->fBold))
		{
		if (inunderline== 1)
			{
			inunderline=0;
			fprintf(outputfile,"</U>");
			}
		if (initalic == 1)
			{
			initalic=0;
			fprintf(outputfile,"</I>");
			}
		inbold=0;
		fprintf(outputfile,"%s",ele_style[BOLD].end);
		}

	if ((inblink ==1) && (!achp->sfxtText))
		{
		if (inunderline== 1)
			{
			inunderline=0;
			fprintf(outputfile,"</U>");
			}
		if (initalic == 1)
			{
			initalic=0;
			fprintf(outputfile,"</I>");
			}
		if (inbold== 1)
			{
			inbold=0;
			fprintf(outputfile,"%s",ele_style[BOLD].end);
			}
		fprintf(outputfile,"</BLINK>");
		}

	if ((insuper==1) && (achp->supersubscript != 1))
		{
		insuper=0;
		if (footnotehack==1)
			{
			fprintf(outputfile,"</a>");
			footnotehack=0;
			}
		fprintf(outputfile,"</SUP>");
		}

	if ((insub) && (achp->supersubscript != 2))
		{
		insub=0;
		fprintf(outputfile,"</SUB>");
		}


	error(erroroutput,"the color is %s, %s\n",achp->color,incolor);
/*
	if ((inacolor) && (achp->color[0] == '\0'))
		colorflag=1;
	else*/
	if (strcmp(achp->color,incolor)) 
		colorflag=1;
		/*
	else if (achp->color[0] != '\0')
		if (incolor && (0 != strcmp(achp->color,incolor)) ) 
			colorflag=1;
		*/

	error(erroroutput,"inafont is %d\n,currentfontcode is %d\n,achpfntcode is %d, color flag iss %d\n",inafont,currentfontcode,achp->ftcAscii,colorflag);

	if ( ((inafont) && (achp->fontsize!=currentfontsize)) || colorflag || (currentfontcode != achp->ftcAscii))
		{
		error(erroroutput,"font ended\n");
		if (inunderline== 1)
			{
			inunderline=0;
			fprintf(outputfile,"</U>");
			}
		if (initalic == 1)
			{
			initalic=0;
			fprintf(outputfile,"</I>");
			}
		if (inbold == 1)
			{
			inbold=0;
			fprintf(outputfile,"%s",ele_style[BOLD].end);
			}
		if (inblink == 1)
			{
			inblink=0;
			fprintf(outputfile,"</BLINK>");
			}
		if (inafont)
			{
			fprintf(outputfile,"%s",ele_style[FONT].end);
			incolor[0] = '\0';
			currentfontcode=-1;
			}
		inafont =0;
		inacolor=0;
		currentfontsize=NORMAL;
		}
	error(erroroutput,"left end chp sucessfully\n");

	if ( (achp->fPropRMark== 0) &&  (revision == 3) )
		{
		fprintf(outputfile,"\n<img src=\"%s/eattributes.jpg\">",patterndir());
		fprintf(outputfile,"<font color=\"yellow\"><u><sup>");
		if (revisiontag < revisions.nostrings)
			{
			if (revisions.extendedflag == 0xFFFF)
				{
				letter16 = revisions.u16strings[revisiontag];

				while (*letter16 != '\0')
					fprintf(outputfile,"%c",*letter16++);
				}
			else
				{
				letter8 = revisions.s8strings[revisiontag];

				while (*letter8 != '\0')
					fprintf(outputfile,"%c",*letter8++);
				}
			}
		fprintf(outputfile," (%d:%d  %s %d/%d/%d)",revisiontime.hr,revisiontime.mint,notoday(revisiontime.wdy),revisiontime.dom,revisiontime.mon,1900+revisiontime.yr);
		fprintf(outputfile,"</sup></u></font>\n");
		revisiontag = 0;
		revision=0;
		}

	if ( (achp->fRMarkDel == 0) &&  (revision == 2) )
		{
		fprintf(outputfile,"\n<img src=\"%s/edeleted.jpg\">",patterndir());
		fprintf(outputfile,"<font color=\"yellow\"><u><sup>");
		if (revisions.extendedflag == 0xFFFF)
			{
			letter16 = revisions.u16strings[revisiontag];
			while (*letter16 != '\0')
				fprintf(outputfile,"%c",*letter16++);
			}
		else
			{
			letter8 = revisions.s8strings[revisiontag];
			while (*letter8 != '\0')
				fprintf(outputfile,"%c",*letter8++);
			}
		fprintf(outputfile," (%d:%d  %s %d/%d/%d)",revisiontime.hr,revisiontime.mint,notoday(revisiontime.wdy),revisiontime.dom,revisiontime.mon,1900+revisiontime.yr);
		fprintf(outputfile,"</sup></u></font>\n");
		revisiontag = 0;
		revision=0;
		}

	if ( (achp->fRMark == 0) && (revision == 1) )
		{
		fprintf(outputfile,"\n<img src=\"%s/enewlytyped.jpg\">",patterndir());
		fprintf(outputfile,"<font color=\"yellow\"><u><sup>");
		if (revisions.extendedflag == 0xFFFF)
			{
			letter16 = revisions.u16strings[revisiontag];
			while (*letter16 != '\0')
				fprintf(outputfile,"%c",*letter16++);
			}
		else
			{
			letter8 = revisions.s8strings[revisiontag];
			while (*letter8 != '\0')
				fprintf(outputfile,"%c",*letter8++);
			}
		fprintf(outputfile," (%d:%d  %s %d/%d/%d)",revisiontime.hr,revisiontime.mint,notoday(revisiontime.wdy),revisiontime.dom,revisiontime.mon,1900+revisiontime.yr);
		fprintf(outputfile,"</sup></u></font>\n");
		revisiontag = 0;
		revision=0;
		}
	
	}


/*
letter logic:
	in comes letter, check the font stuff, if its some fonts it appears
	to be fSpec by default, then run through the fSpec code , then
	if it gets through that then if its a special font decode the char
	otherwise run it through the ordinary letter handling
*/

int decode_letter(int letter,int flag,pap *apap, chp * achp,field_info *magic_fields,FILE *main,FILE *data,FFN_STTBF *ffn_sttbf,list_info *a_list_info,textportions *portions, int *issection,style *sheet)
	{
	int ret=0;
	int i=0,j;
	static U8 fieldwas=0xff;
	static int fieldeater=0;
	static long int swallowcp1=0xffffffffL,swallowcp2=0xffffffffL;
	static int spacecount;
	static int tabstop;
	static int silent=0;
	float tabbing;
	char *fontstr;
	time_t timep;
	struct tm *times;
	char date[1024];
	char *fontname=NULL;
	int fSpecflag=0;
	U16 temp=0;
	fbse_list *pfbse_list;
	
	static int fieldparse=0;
	U16 *array;
	U16 *array2;
	U16 *deleteme=NULL;

	char target[7];
	int len;

	chp tempchp;

	int temp2;

	PICF apicf;

	cellempty--;

	if (insuper==2)
		insuper=1;

	switch (fieldparse)	
		{
		case 37:
			realcp++;
			cp++;
			array = decode_crosslink(letter,&swallowcp1,&swallowcp2);
			if (array != NULL)
				{
				fprintf(outputfile,"<a href=\"#");
				array2 = array;
				while (*array2 != '\0')
					{ 
					/*warning despite the possibility of being 16 bit nos ive done this*/
					fprintf(outputfile,"%c",*array2); 
					array2++;
					}
				fprintf(outputfile,"\">*</a>");
				fflush(outputfile);
				error(erroroutput,"end of the mallocing\n");
				free(array);
				fieldparse=0;
				}
			return(0);
			break;
		case 88:
			array = decode_hyperlink(letter,&swallowcp1,&swallowcp2,&deleteme);		
			realcp++;
			cp++;

			if (array == NULL)		/*if theres no letters for the rest of this guy to parse*/
				return(0);
			else	
				{
				fieldparse=0;
				array2 = array;
				if (array2[0] != '\"')
					fprintf(outputfile,"\"");
				
				while (*array2 != '\0')
					{
					/*warning despite the possibility of being 16 bit nos ive done this*/
					fprintf(outputfile,"%c",*array2); 
					array2++;
					}
				array2--;
				
				if (*array2 != '\"')
					fprintf(outputfile,"\"");

				fprintf(outputfile,">");

				
				if (deleteme != NULL)
					free(deleteme);
					
				return(0);
				}
			break;
		default:
			break;
		}

	if  (letter != 13 || achp->fontcode !=0 ) 
		{
		if (flushbreaks(0))
			{
			error(erroroutput,"<!--new paragraph-->");
			do_indent(apap);
			}
		}

	if (ffn_sttbf->nostrings > 0)
		{
		if ( (achp->fontcode !=0) && (letter > 34) ) 
			{
			error(erroroutput,"asked for nonstandard font %d\n",achp->fontcode);
			fontstr = wvGetFontnameFromCode(ffn_sttbf,achp->fontcode);
			if (fontstr == NULL)
				wvWarning("test fontstr is NULL\n");

			if (fontstr)
				{
				if (!(strcmp("Symbol",fontstr)))
					{
					error(erroroutput,"must use symbol font for this letter\n");
					if (decode_symbol(achp->fontspec))
						{
						cp++;
						realcp++;
						return(ret);
						}
					}
				else if (!(strcmp("Wingdings",fontstr)))
					{
					error(erroroutput,"must use wingding font for this letter %c\n",letter);
					if (decode_wingding(achp->fontspec))
						{
						cp++;
						realcp++;
						return(ret);
						}
					}
				}
			wvFree(fontstr);
			}
		else 
			{
			fontstr = wvGetFontnameFromCode(ffn_sttbf,achp->ftcAscii);
			if (fontstr == NULL)
				wvWarning("test fontstr is NULL\n");

			if ( (fontstr) && (letter > 34) ) 
				{
				if (!(strcmp("Symbol",fontstr)))
					{
					error(erroroutput,"must use symbol font for this letter here\n");
					fontname=fontstr;
					if (achp->fSpec==0)
						{
						achp->fSpec=1;
						fSpecflag=1;
						}
					}
				else if (!(strcmp("Wingdings",fontstr)))
					{
					error(erroroutput,"must use wingding font for this letter here %d fontcode is %d\n",letter,achp->fontcode);
					fontname=fontstr;
					if (achp->fSpec==0)
						{
						achp->fSpec=1;
						fSpecflag=1;
						}
					}
				}

			}
		}

	error(erroroutput,"cp is %x (%x) Trealcp is %x\n",cp,letter,realcp);
	if ( ( (fieldeater == 2) && (letter != 0x20) ) || ( (fieldeater > 0) && (letter == 0x20) ))
		{
		fieldeater--;
		if (fieldeater == 0)
			silent=0;
		}
#if 0
	else if ((cp > swallowcp1) && (cp < swallowcp2))
		{
		error(erroroutput,"swallowing %c\n",letter);
		}
#endif
	else
		{
		if ((letter != 32) && (spacecount == 1))
			{
			if (achp->underline == 2)
				{
				temp = achp->underline;
				achp->underline=0;
				decode_e_chp(achp);
				decode_s_chp(achp,ffn_sttbf,sheet);
				}
			oprintf(silent," ");
			if (temp == 2)
				{
				achp->underline=temp;
				decode_e_chp(achp);
				decode_s_chp(achp,ffn_sttbf,sheet);
				}
			spacecount=0;
			tabstop++;
			}
		else if (letter != 32) 
			{
			if (i>0)
				{
				if (achp->underline == 2)
					{
					temp = achp->underline;
					achp->underline=0;
					decode_e_chp(achp);
					decode_s_chp(achp,ffn_sttbf,sheet);
					}
				for (i=0;i<spacecount;i++)
					{
					if (padding < 3)
						oprintf(silent,"&nbsp;");
					tabstop++;
					}
				if (temp == 2)
					{
					achp->underline=temp;
					decode_e_chp(achp);
					decode_s_chp(achp,ffn_sttbf,sheet);
					}
				}
			spacecount=0;
			}


		tabstop++;

		if (achp->fSpec==1)
			{
			switch (letter)
				{
				case 0:
					fprintf(outputfile,"%d",pagenumber);
					error(erroroutput,"pagenumber is now %d\n",pagenumber);
					break;
				case 1:
					if ((achp->fData == 0) && (achp->fOle2 == 0))
						{
						error(erroroutput,"0x01 handle picture\n");
						error(erroroutput,"IN THIS CASE data offset is %x, ole2 is %x\n",achp->fcPic,achp->fOle2);
						wvGetPICF(&apicf,data,achp->fcPic);
						if (apicf.rgb != NULL)
							{
							outputimgsrc(apicf.rgb,apicf.dxaGoal,apicf.dyaGoal);
							free(apicf.rgb);
							}
						}
					break;
				case 2:
					if (inaheaderfooter!=1)
						{
						error(erroroutput,"INSERT REF\n");
						decode_f_reference(portions);
						}
					break;
				case 3:
				case 4:
					error(erroroutput,"do these matter in html mode\n");
					break;
				case 5:
					error(erroroutput,"INSERT ANNOTATION\n");
					if (doannos)
						decode_annotation(portions);
					break;
				case 6:
					error(erroroutput,"line no\n");
					fprintf(outputfile,"UNKNOWN LINE NO\n");
					break;
				case 7:	
					error(erroroutput,"what is this !, pen windowsn\n");
					break;
				case 8:	
					if (afsp_list != NULL)
						{
						error(erroroutput,"office draw thing, must get spid\n");
						error(erroroutput,"SPID is %x\n",wvGetSPIDfromCP(cp,portions));
						pfbse_list = wvGetSPID(wvGetSPIDfromCP(cp,portions),afsp_list,afbse_list);
						if (pfbse_list != NULL)
							{
							error(erroroutput,"the file to link in is %s\n",pfbse_list->filename);
							outputimgsrc(pfbse_list->filename,0,0);
							}
						}
					break;
				case 10:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%a, %b ",times);
					error(erroroutput,"output date as %s\n",date);
					fprintf(outputfile,"%s",date);
					fprintf(outputfile,"%d",times->tm_mday);
					strftime(date,1024,",%Y",times);
					error(erroroutput,"output date as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 30:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%x",times);
					error(erroroutput,"output date as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 11:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%H:%M:%S",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 28:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%X",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 12:
					fprintf(outputfile,"--%d--\n",sectionno);
					error(erroroutput,"did section no\n");
					break;
				case 14:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%a",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 15:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%A",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 16:
					timep = time(NULL);
					times = localtime(&timep);
					/*
					strftime(date,1024,"%d",times);
					error(erroroutput,"output time as %s\n",date);
					*/
					fprintf(outputfile,"%d",times->tm_mday);
					break;
				case 22:
					timep = time(NULL);
					times = localtime(&timep);
					/*
					strftime(date,1024,"%H",times);
					error(erroroutput,"output time as %s\n",date);
					*/
					fprintf(outputfile,"%d",times->tm_hour);
					break;
				case 23:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%H",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 24:
					timep = time(NULL);
					times = localtime(&timep);
					/*
					strftime(date,1024,"%M",times);
					error(erroroutput,"output time as %s\n",date);
					*/
					fprintf(outputfile,"%d",times->tm_min);
					break;
				case 25:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%M",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 26:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%S",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 27:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%p",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 29:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%B ",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					fprintf(outputfile,"%d",times->tm_mday);
					strftime(date,1024,", %Y",times);
					fprintf(outputfile,"%s",date);
					break;
				case 33:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%m",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 34:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%Y",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 35:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%y",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 36:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%b",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 37:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%B",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 38:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%H:%M",times);
					error(erroroutput,"output time as %s\n",date);
					fprintf(outputfile,"%s",date);
					break;
				case 39:
					timep = time(NULL);
					times = localtime(&timep);
					strftime(date,1024,"%A, %B, ",times);
					fprintf(outputfile,"%s",date);
					fprintf(outputfile,"%d, ",times->tm_mday);
					strftime(date,1024,"%Y",times);
					fprintf(outputfile,"%s",date);
					break;
				case 41:
					error(erroroutput,"print merge ?\n");
					break;
				case 19:
					error(erroroutput,"\nfield begins\n");
					init_chp(&tempchp);
					chpsoff();
					decode_field(main,magic_fields,&cp,&fieldwas,&swallowcp1,&swallowcp2);
					error(erroroutput,"cp vals are %x and %x\n",swallowcp1,swallowcp2);		/*if theres no letters for the rest of this guy to parse*/

					switch (fieldwas)
						{
						case 37:
							if ((swallowcp1 != 0xffffffffL) && (swallowcp2 != 0xffffffffL))
								fieldparse=37;
							silent=1;
							break;
						case 88:
							if ((swallowcp1 != 0xffffffffL) && (swallowcp2 != 0xffffffffL))
								fieldparse=88;
							break;
						default:
							silent=1;
							break;
						}
					break;
				case 21:
					error(erroroutput,"\nfield ends\n");
					decode_field(main,magic_fields,&cp,&fieldwas,&swallowcp1,&swallowcp2);
					chpson();
					silent=0;
					fieldparse=0;
					break;
				case 20:
					error(erroroutput,"\n field separator\n");
					decode_field(main,magic_fields,&cp,&fieldwas,&swallowcp1,&swallowcp2);
					if ( (fieldwas == 10) || (fieldwas == 12) || (fieldwas == 68) || (fieldwas == 17) || (fieldwas == 29) || (fieldwas == 13) || (fieldwas == 88) || (fieldwas == 3))
						{
						silent=0;
						chpson();
						}
					break;
				default:
					if (fontname==NULL)
						{
						if (flag)
							{
							len = our_wctomb(target,letter);
							error(erroroutput,"letter3: %x %d",letter,letter);
							/*
							expand this in the ranges that we will have to handle 
							ourselves, just for my own benefit for now, i might be
							able to build a table with some word macros and luck*/
							if (letter == 0x2122)
								decode_symbol(0xf0e4);
							else if (letter != 0)
								for(i=0;i<len;i++)
									{
									if ( (achp->fSmallCaps || achp->fCaps)  && use_fontfacequery(achp) && (len == 1) )
										{
										if  ( isupper(target[i]) && achp->fSmallCaps)
											{
											temp = achp->fontsize;
											achp->fontsize+=2;
											decode_e_chp(achp);
											decode_s_chp(achp,ffn_sttbf,sheet);
											achp->fontsize = temp;
											}
										target[i] = toupper(target[i]);
										}
									oprintf(silent,"%c",target[i]);

									error(erroroutput,"letter2: %c, silent is %d",target[i],silent);
									}
							else
								error(erroroutput,"given 0 as a letter !\n");
							}
						else
							{
							if (letter == 0xae)
								decode_symbol(61666);
							else if (letter != 0)
								{
								if ( ( achp->fSmallCaps || achp->fCaps) && use_fontfacequery(achp) )
									{
									if  ( letter && achp->fSmallCaps)
											{
											temp = achp->fontsize;
											achp->fontsize+=2;
											decode_e_chp(achp);
											decode_s_chp(achp,ffn_sttbf,sheet);
											achp->fontsize = temp;
											}
									letter = toupper(letter);
									}
								oprintf(silent,"%c",letter);
								}
							error(erroroutput,"letter2: %c %d %x, silent is %d",letter,letter,letter,silent);
							}
						}
					else
						{
						if (!(strcmp("Wingdings",fontname)))
							decode_wingding(letter);
						else if (!(strcmp("Symbol",fontname)))
							decode_symbol(letter);
						realcp++;
						cp++;
						if (fSpecflag)
							achp->fSpec=0;
						wvFree(fontname);
						return(ret);
						}
					break;
				}
			}
		else
			{
			switch(letter)
				{
				case 13:
					error(erroroutput,"\n<!--paragraph end-->\n");
					if (!silent)
						{
						breakcount++;

						if (cellempty == 0)
							cellempty++;
						
						tabstop=0;
						newline=1;
						}
					break;
				case 11:
					error(erroroutput,"\n(--line break--)\n");
					oprintf(silent,"<BR>\n");
					tabstop=0;
					break;
				case 45:
					error(erroroutput,"-");
					oprintf(silent,"-"); 
					break;
				case 32:
					spacecount++;
					tabstop--;
					if (cellempty == 0)
						cellempty++;
					break;
				case 31:
					error(erroroutput,"\n(-nonrequired hyphen- ?)\n");
					oprintf(silent,"-");
					break;
				case 30:
					error(erroroutput,"\n(-nonbreaking hyphen- ?)\n");
					oprintf(silent,"-");
					break;
				case 160:
					error(erroroutput,"\n(-non breaking space)\n");
					oprintf(silent,"&nbsp;");
					break;
				case 12:
					error(erroroutput,"\npage break (maybe section) at %x\n",cp);
					for (i=0;i<portions->section_nos+1;i++)
						{
						if (cp+1 == portions->section_cps[i])
							{
							/*sectionpagenumber=1;*/
							sectionno++;
							*issection=1;
							}
						}
					ret=1;
					tabstop=0;
					break;
				case 14:
					error(erroroutput,"\ncolumn break\n");
					columnbreak();
					break;
				case 9:
					tabstop--;
					error(erroroutput,"\ntab\n");
					tabbing = ((float)tabstop)/8;
					error(erroroutput,"tabbing is %f, from %d\n",tabbing,tabstop);
					error(erroroutput,"tabsize is %f\n",tabsize);
					if (tabbing == tabstop/8)
						{
						if ( (padding == 0) || (padding == 3))
							fprintf(outputfile,"<img height=1 width=%d src=\"%s/clear.gif\">",(int)(8*tabsize),patterndir());
						else if ( (padding == 1) || (padding == 4))
							for(j=0;j<(int)tabsize;j++)
								oprintf(silent,"&nbsp;");
						else if ( (padding == 5) || (padding == 2))
							oprintf(silent," ");
						tabstop = tabstop+8;
						}
					else 
						{
						temp2 = (int)((8-((tabbing - (tabstop/8))*8))+1);
					    error(erroroutput,"temp2 is %d\n",temp2);
						if ( (padding == 1) || (padding == 4))
							{
							for(i=0;i< (8-((tabbing - (tabstop/8))*8))+1;i++)
									for(j=0;j<(int)(tabsize/8);j++)
										oprintf(silent,"&nbsp;");
							}
						else if ( (padding == 0) || (padding == 3))
							fprintf(outputfile,"<img height=1 width=%d src=\"%s/clear.gif\">",(int)(temp2*tabsize),patterndir());
						else if ((padding == 5) || (padding == 2))
							oprintf(silent," ");
						tabstop = (int)(tabstop+(8-(tabbing - (tabstop/8))*8));
						}
					/*oprintf(silent,"<TAB>");*/
					break;
				case 19:
					error(erroroutput,"\nfield begins\n");
					init_chp(&tempchp);
					chpsoff();
					decode_field(main,magic_fields,&cp,&fieldwas,&swallowcp1,&swallowcp2);
					error(erroroutput,"cp vals are %x and %x\n",swallowcp1,swallowcp2);
					switch (fieldwas)
						{
						case 37:
							if ((swallowcp1 != 0xffffffffL) && (swallowcp2 != 0xffffffffL))
								fieldparse=37;
							break;
						case 88:
							if ((swallowcp1 != 0xffffffffL) && (swallowcp2 != 0xffffffffL))
								fieldparse=88;
							break;
						default:
							silent=1;
							break;
						}
					break;
				case 21:
					error(erroroutput,"\nfield ends\n");
					decode_field(main,magic_fields,&cp,&fieldwas,&swallowcp1,&swallowcp2);
					chpson();
					silent=0;
					fieldparse=0;
					break;
				case 20:
					error(erroroutput,"\n field separator\n");
					decode_field(main,magic_fields,&cp,&fieldwas,&swallowcp1,&swallowcp2);
					if ( (fieldwas == 10) || (fieldwas == 12) || (fieldwas == 68) || (fieldwas == 17) || (fieldwas == 29) || (fieldwas == 13) || (fieldwas == 88) || (fieldwas == 3))
						{
						silent=0;
						chpson();
						}
					break;
				case '<':
					oprintf(silent,"&lt;");
					break;
				case '>':
					oprintf(silent,"&gt;");
					break;
				case 7:
					if (cellempty == 0)
						cellempty++;
					apap->tableflag=1;
					decode_e_specials(apap,achp,a_list_info);
					if (decode_e_table(apap,achp,a_list_info,0)==1)
						{
						if (deferrednewpage)
							{
							deferrednewpage=0;
							ret=2;
							}
						}
					error(erroroutput,"\n");
					newline=1;
					break;
				case 0x96:
				case 0x2013:
					oprintf(silent,"-");
					break;
				case 145:
				case 146:
					oprintf(silent,"'");
					break;
				case 132:
				case 147:
					error(erroroutput,"begin quot\n");
					oprintf(silent,"&quot;");
					break;
				case 148:
					error(erroroutput,"end quot\n");
					oprintf(silent,"&quot;");
					break;
				case 0x85:
					if (flag)
						letter = 0x2026;
					else
						{
						wvFree(fontname);
						fontname = (char *)malloc(strlen("Symbol")+1);
						strcpy(fontname,"Symbol");
						letter = 0xf0bc;
						/*156.gif*/
						}
					/*deliberate fallthrough*/
				default:
					if (fontname==NULL)
						{
						if (flag)
							{
							len = our_wctomb(target,letter);
							error(erroroutput,"letter3: %x %d",letter,letter);
							/*
							expand this in the ranges that we will have to handle 
							ourselves, just for my own benefit for now, i might be
							able to build a table with some word macros and luck*/
							if (letter == 0x2122)
								decode_symbol(0xf0e4);
							else if (letter != 0)
								for(i=0;i<len;i++)
									{
									if ( (achp->fSmallCaps || achp->fCaps) && use_fontfacequery(achp) && (len == 1) )
										{
										if  ( isupper(target[i]) && achp->fSmallCaps)
												{
												temp = achp->fontsize;
												achp->fontsize+=2;
												decode_e_chp(achp);
												decode_s_chp(achp,ffn_sttbf,sheet);
												achp->fontsize = temp;
												}
										target[i] = toupper(target[i]);
										}
									oprintf(silent,"%c",target[i]);
									error(erroroutput,"letter2: %c, silent is %d",target[i],silent);
									}
							else
								error(erroroutput,"given 0 as a letter !\n");
							}
						else
							{
							if (letter == 0xae)
								decode_symbol(61666);
							else if (letter != 0)
								{
								if ( (achp->fSmallCaps || achp->fCaps) && (use_fontfacequery(achp)) )
									{
									if  ( isupper(letter) && achp->fSmallCaps)
											{
											temp = achp->fontsize;
											achp->fontsize+=2;
											decode_e_chp(achp);
											decode_s_chp(achp,ffn_sttbf,sheet);
											achp->fontsize = temp;
											}
									letter = toupper(letter);
									}
								oprintf(silent,"%c",letter);
								}
							else
								error(erroroutput,"silly 0 found\n");
							error(erroroutput,"letter2: %c %d %x silent is %d",letter,letter,letter,silent);
							}
						}
					else
						{
						if (!(strcmp("Wingdings",fontname)))
							decode_wingding(letter);
						else if (!(strcmp("Symbol",fontname)))
							decode_symbol(letter);
						realcp++;
						cp++;
						if (fSpecflag)
							achp->fSpec=0;
						wvFree(fontname);
						return(ret);
						}
					break;
				}
				
			}
		}
	cp++;
	realcp++;
	return(ret);
	}


int decode_clx_endnote(U32 *rgfc,sep *asep,int nopieces,U32 startpiece,U32 begincp,U32 endcp,FILE *in,FILE *main,FILE *data,U32 fcClx,U32 lcbClx,U32 intervals,U32 chpintervals,U32 *plcfbtePapx,U32 *plcfbteChpx,field_info *all_fields[5],list_info *a_list_info,style *sheet,textportions *portions,FFN_STTBF *ffn_sttbf,int headerfooterflag)
	{
	U32 oldtablepos;
	U32 oldmainpos;
	U32 tempcp;
	U32 begin=0;
	U32 len=0;
	int k;
	field_info *tempfields;
	int i;
	chp achp;
	pap indentpap;
	int anyendnotes=0;
	/*
	char roman[81];
	*/

	flushbreaks(0);
	init_pap(&indentpap);
	do_indent(&indentpap);

	if (noheaders)
		return(anyendnotes);

	error(erroroutput,"list_end_no is %d\n",portions->list_end_no);

	if (header == 0)
		{
		for (i=0;i<portions->list_end_no;i++)
			{
			decode_endnote(&begin,&len,portions,i);

			if ((begin != -1) && (len != -1))
				{
				if (anyendnotes==0)
					fprintf(outputfile,"\n<br><img src=\"%s/endnotebegin.gif\"><br>\n",patterndir());
				anyendnotes++;
				init_chp(&achp);
                decode_s_chp(&achp,ffn_sttbf,sheet);


				error(erroroutput,"beginning endnote\n");
				/*test*/
				fprintf(outputfile,"<p>");
				fprintf(outputfile,"<font color=\"#330099\">");
				inafont=1;
				inacolor=1;
				strcpy(incolor,"#330099");
				/*end test*/
				if (!insuper)
					{
					fprintf(outputfile,"<sup>");
					insuper=2;
					}
				fprintf(outputfile,"<a name=\"end%d\">",i);

				if (portions->endFRD[i].frd > 0)
					{
					decode_list_nfc(portions->endTrueFRD[i],dop.new_nfcEdnRef);
					fprintf(outputfile,"</A>");
					if (insuper == 2)
						{
						fprintf(outputfile,"</sup>");
						insuper=0;
						}
					}
				else
					 footnotehack=1;


				oldmainpos = ftell(main);
				oldtablepos = ftell(in);
				header++;

				k=0;
				while (k< nopieces)
					{
					if (portions->ccpText+portions->ccpFtn+portions->ccpHdd+portions->ccpAtn+begin < rgfc[k]) 
						{
						error(erroroutput,"piece for footer is %d (%x) (%x)\n",k-1,rgfc[k-1],portions->ccpText+begin);
						break;
						}
					k++;
					}
				tempcp=cp;
				cp=begin;
				error(erroroutput,"the difference is %d\n",(portions->ccpText+begin)-rgfc[k-1]);
				cp = rgfc[k-1];
				error(erroroutput,"endnote len is %d\n",len);
				error(erroroutput,"endnote cp is %x\n",cp);
				tempfields = all_fields[0];
				all_fields[0] = all_fields[4];
				begin = portions->ccpText+portions->ccpFtn+portions->ccpHdd+portions->ccpAtn+begin;
				error(erroroutput,"endnote begin cp is %d, begin is %d, end is %d\n",cp,begin,begin+len);
				decode_clx(k-1,begin,begin+len,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,1);
				if (footnotehack == 1)
                    {
                    fprintf(outputfile,"</A>");
                    footnotehack=0;
                    }
				all_fields[0] = tempfields;
				cp=tempcp;
				fseek(in,oldtablepos,SEEK_SET);
				fseek(main,oldmainpos,SEEK_SET);
				header--;
				}
			}
		portions->list_end_no=0; /*ready for next section endnotes*/
		error(erroroutput,"ending endnote\n");

		init_chp(&achp);
        decode_s_chp(&achp,ffn_sttbf,sheet);
		}
	return(anyendnotes);
	}

void decode_clx_footer(U32 *rgfc,sep *asep,int nopieces,U32 startpiece,U32 begincp,U32 endcp,FILE *in,FILE *main,FILE *data,U32 fcClx,U32 lcbClx,U32 intervals,U32 chpintervals,U32 *plcfbtePapx,U32 *plcfbteChpx,field_info *all_fields[5],list_info *a_list_info,style *sheet,textportions *portions,FFN_STTBF *ffn_sttbf,int headerfooterflag)
	{
	U32 oldtablepos;
	U32 oldmainpos;
	U32 tempcp;
	U32 begin=0;
	U32 len=0;
	int k;
	field_info *tempfields;
	int i,j;
	int footnoteflag=0;
	int annotationflag=0;
	chp achp;
	pap indentpap;

	flushbreaks(0);
	init_pap(&indentpap);
	do_indent(&indentpap);

	if (noheaders)
		return;

	if (header == 0)
		{
		for (i=portions->last_foot;i<portions->list_foot_no+portions->last_foot;i++)
			{
			decode_footnote(&begin,&len,portions,i);

			if ((begin != -1) && (len != -1))
				{
				init_chp(&achp);
                decode_s_chp(&achp,ffn_sttbf,sheet);

				if (footnoteflag == 0)
					{
					fprintf(outputfile,"\n<br><img src=\"%s/footnotebegin.gif\"><br>\n",patterndir());
					footnoteflag=1;
					}
				error(erroroutput,"beginning footnote\n");
				/*test*/
				fprintf(outputfile,"<p>");
				fprintf(outputfile,"<font color=\"#330099\">");
				inafont=1;
				inacolor=1;
				strcpy(incolor,"#330099");
				/*end test*/
				if (!insuper)
					{
					fprintf(outputfile,"<sup>");
					insuper=2;
					}
					/*fish*/
				fprintf(outputfile,"<a name=\"foot%d\">",i);
					/*
				fprintf(outputfile,"<a name=\"foot%d\">",portions->list_footnotes[i]);
					*/

				if (portions->fndFRD[i].frd > 0)
					{
					/*
					fprintf(outputfile,"%d",portions->fndFRD[i].frd);
					*/
					decode_list_nfc(portions->fndFRD[i].frd,dop.new_nfcFtnRef);
					fprintf(outputfile,"</A>");
					if (insuper == 2)
						{
						fprintf(outputfile,"</Sup>");
						insuper=0;
						}
					}
				else
					 footnotehack=1;


				oldmainpos = ftell(main);
				oldtablepos = ftell(in);
				header++;

				k=0;
				while (k< nopieces)
					{
					if (portions->ccpText+begin < rgfc[k]) 
						{
						error(erroroutput,"piece for footer is %d (%x) (%x)\n",k-1,rgfc[k-1],portions->ccpText+begin);
						break;
						}
					k++;
					}
				tempcp=cp;
				cp=begin;
				error(erroroutput,"the difference is %d\n",(portions->ccpText+begin)-rgfc[k-1]);
				/*
				cp = cp-((portions->ccpText+begin)-rgfc[k-1]);
				*/
				cp = rgfc[k-1];
				error(erroroutput,"footer len is %d\n",len);
				error(erroroutput,"footer cp is %x\n",cp);
				tempfields = all_fields[0];
				all_fields[0] = all_fields[2];
				begin = portions->ccpText+begin;
				decode_clx(k-1,begin,begin+len,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,2);
				if (footnotehack == 1)
                    {
                    fprintf(outputfile,"</A>");
                    footnotehack=0;
                    }
				all_fields[0] = tempfields;
				cp=tempcp;
				fseek(in,oldtablepos,SEEK_SET);
				fseek(main,oldmainpos,SEEK_SET);
				header--;
				}
			}
		portions->last_foot+=portions->list_foot_no;
		portions->list_foot_no=0; /*ready for next page footnotes*/
		portions->auto_foot=1;

		if (footnoteflag != 0)
			{
			fprintf(outputfile,"\n<br><img src=\"%s/footnoteend.gif\"><br>\n",patterndir());
			}

/*begin annotation*/
		for (i=portions->last_anno;i<portions->list_anno_no+portions->last_anno;i++)
			{
			decode_footanno(&begin,&len,portions,i);

			init_chp(&achp);
            decode_s_chp(&achp,ffn_sttbf,sheet);

			if ((begin != -1) && (len != -1))
				{
				if (annotationflag == 0)
					{
					fprintf(outputfile,"\n<br><img src=\"%s/commentbegin.gif\"><br>\n",patterndir());
					annotationflag=1;
					}
				error(erroroutput,"beginning annotation\n");
				/*test*/
                fprintf(outputfile,"<font color=#ff7777>");
                inafont=1;
                inacolor=1;
                strcpy(incolor,"#ff7777");
                /*end test*/

				if (!insuper)
					fprintf(outputfile,"<sup>");

				/*
				fprintf(outputfile,"<a name=\"anno%d\">",i);
				*/
				fprintf(outputfile,"<a name=\"");
				j = portions->the_atrd[i].xstUsrInitl[0];
				for (j=1;j<portions->the_atrd[i].xstUsrInitl[0]+1;j++)
                    {
                    /*warning despite the possibility of being 16 bit nos ive done this*/
                    fprintf(outputfile,"%c",portions->the_atrd[i].xstUsrInitl[j]);
					}
				fprintf(outputfile,"%d",i);
				fprintf(outputfile,"\">[");
				list_author_key=1;
				j = portions->the_atrd[i].xstUsrInitl[0];
				for (j=1;j<portions->the_atrd[i].xstUsrInitl[0]+1;j++)
                    {
                    /*warning despite the possibility of being 16 bit nos ive done this*/
                    fprintf(outputfile,"%c",portions->the_atrd[i].xstUsrInitl[j]);
					}
				fprintf(outputfile,"%d]",i+1);
				/*
				fprintf(outputfile,"anno%d",i);
				*/

				fprintf(outputfile,"</a>");
				if (!insuper)
					fprintf(outputfile,"</sup>");

				oldmainpos = ftell(main);
				oldtablepos = ftell(in);
				header++;

				k=0;
				while (k< nopieces)
					{
					if (portions->ccpText+portions->ccpFtn + portions->ccpHdd+begin < rgfc[k]) 
						{
						error(erroroutput,"piece for footer is %d (%x) (%x)\n",k-1,rgfc[k-1],portions->ccpText+portions->ccpFtn + portions->ccpHdd+begin);
						break;
						}
					k++;
					}
				tempcp=cp;
				cp=begin;
				error(erroroutput,"the difference is %d\n",(portions->ccpText+portions->ccpFtn + portions->ccpHdd+begin)-rgfc[k-1]);
				cp = cp-((portions->ccpText+portions->ccpFtn + portions->ccpHdd+begin)-rgfc[k-1]);
				error(erroroutput,"footer len is %d\n",len);
				error(erroroutput,"footer cp is %x\n",cp);
				tempfields = all_fields[0];
				all_fields[0] = all_fields[3];
				for(j=0;j<all_fields[3]->no;j++)
					{
					if (begin == all_fields[3]->cps[j])
						{
						if (all_fields[3]->flds[j].var1.flt == 19)
							all_fields[3]->flds[j].var1.flt =0;
						}
					}
				decode_clx(k-1,begin,begin+len,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,3);
				all_fields[0] = tempfields;
				cp=tempcp;
				fseek(in,oldtablepos,SEEK_SET);
				fseek(main,oldmainpos,SEEK_SET);
				header--;
				}
			}
		if (annotationflag)
			fprintf(outputfile,"\n<br><img src=\"%s/commentend.gif\"><br>\n",patterndir());
			
		portions->last_anno+=portions->list_anno_no;
		portions->list_anno_no=0; /*ready for next page footnotes*/
/*end annotation*/		
		decode_footer(&begin,&len,portions,asep);
		if ((begin != -1) && (len != -1))
			{
			init_chp(&achp);
            decode_s_chp(&achp,ffn_sttbf,sheet);

			header++;
			error(erroroutput,"doing footer\n");

			oldmainpos = ftell(main);
			oldtablepos = ftell(in);
			error(erroroutput,"decoding footer clx 1\n");
			error(erroroutput,"the cp of begin of the header is probably cps (%x) to (%x)", portions->ccpText,portions->ccpText+portions->ccpFtn+portions->ccpHdd);
			k=0;
			while (k< nopieces)
				{
				if (portions->ccpText+portions->ccpFtn+begin < rgfc[k]) 
					{
					error(erroroutput,"piece for this footer is %d (%x) (%x)\n",k-1,rgfc[k-1],portions->ccpText+portions->ccpFtn+begin);
					break;
					}
				k++;
				}
			tempcp=cp;
			cp=begin;
			error(erroroutput,"the difference is %d\n",(portions->ccpText+portions->ccpFtn+begin)-rgfc[k-1]);
			cp = cp-((portions->ccpText+portions->ccpFtn+begin)-rgfc[k-1]);
			error(erroroutput,"footer len is %d\n",len);
			error(erroroutput,"footer cp is %x\n",cp);
			tempfields = all_fields[0];
			all_fields[0] = all_fields[1];
			inaheaderfooter=1;
			decode_clx(k-1,begin,begin+len,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,1);
			inaheaderfooter=0;
			all_fields[0] = tempfields;
			cp = tempcp;
			realcp = tempcp;
			fseek(in,oldtablepos,SEEK_SET);
			fseek(main,oldmainpos,SEEK_SET);
			header--;
			}
		error(erroroutput,"ending footer\n");
		pagenumber++;
		sectionpagenumber++;
		init_chp(&achp);
        decode_s_chp(&achp,ffn_sttbf,sheet);
		}
	}

void decode_clx_header(U32 *rgfc,sep *asep,int nopieces,U32 startpiece,U32 begincp,U32 endcp,FILE *in,FILE *main,FILE *data,U32 fcClx,U32 lcbClx,U32 intervals,U32 chpintervals,U32 *plcfbtePapx,U32 *plcfbteChpx,field_info *all_fields[5],list_info *a_list_info,style *sheet,textportions *portions,FFN_STTBF *ffn_sttbf,int headerfooterflag)
	{
	U32 oldtablepos;
	U32 oldmainpos;
	U32 tempcp;
	U32 begin=0;
	U32 len=0;
	int k;
	field_info *tempfields;

	if (noheaders)
		return;

	if (header == 0)
		{
		decode_header(&begin,&len,portions,asep);
		if ((begin != -1) && (len != -1))
			{
			header++;
			oldmainpos = ftell(main);
			oldtablepos = ftell(in);
			error(erroroutput,"decoding header\n");
			error(erroroutput,"the cp of begin of the header is probably cps (%x) to (%x)", portions->ccpText+portions->ccpFtn+begin,portions->ccpText+portions->ccpFtn+begin+len);
			k=0;
			while (k< nopieces)
				{
				if (portions->ccpText+portions->ccpFtn+begin < rgfc[k]) 
					{
					error(erroroutput,"piece for header is %d (%x) (%x)\n",k-1,rgfc[k-1],portions->ccpText+portions->ccpFtn+begin);
					break;
					}
				k++;
				}
			tempcp=cp;
			cp=begin;
			cp = rgfc[k-1];
			error(erroroutput,"header cp is %x\n",cp);
			tempfields = all_fields[0];
			all_fields[0] = all_fields[1];
			begin = portions->ccpText+portions->ccpFtn+begin;
			inaheaderfooter=1;
			decode_clx(k-1,begin,begin+len,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,1);
			inaheaderfooter=0;
			all_fields[0] = tempfields;
			cp = tempcp;
			fseek(in,oldtablepos,SEEK_SET);
			fseek(main,oldmainpos,SEEK_SET);
			header--;
			}
		}
	}

int find_piece_cp(U32 sepcp,U32  *rgfc,int nopieces)
	{
	int i;
	error(erroroutput,"find_piece_cp-> %x,\n",sepcp);
	for (i=0;i<nopieces;i++)
		{
		if (sepcp <= rgfc[i])
			{
			error(erroroutput,"sep piece is %d cp is %x, other is %x\n",i,sepcp,rgfc[i]);
			return(i);
			}
		}
	return(-1);
	}

void decode_clx(U32 startpiece,U32 begincp,U32 endcp,FILE *in,FILE *main,FILE *data,U32 fcClx,U32 lcbClx,U32 intervals,U32 chpintervals,U32 *plcfbtePapx,U32 *plcfbteChpx,field_info *all_fields[5],list_info *a_list_info,style * sheet,textportions *portions, FFN_STTBF *ffn_sttbf,int headerfooterflag)
	{
	int paraendpiece;
	int paraendpiece2;
	int seppiece;
	int index=0;
	pap *apap=NULL;
	pap *tappap=NULL;
	pap *temppap=NULL;
	chp *achp=NULL;
	pap fakepap;
	chp fakechp;
	sep fakesep;
	long pos = ftell(in);
	long mainpos = ftell(main);
	int i;
	int k;
	U32 tapfc1=0;
	U32 tapfc2=0;
	
	U8 clxt;
	U16 cb;
	U16 *sprm=NULL;
	
	int j=0;
	int nopieces=0;
	U32 *rgfc=NULL;
	U32 *avalrgfc=NULL;
	int letter;
	int hack=0;
	U32 nextfc=0;
	U32 chpnextfc;
	U32 clxcount=0;
	tSprm asprmlist;
	tSprm *psprmlist=&asprmlist;
	tSprm *freesprm;
	U32 lastfc=0;
	U32 lastchpfc=0;
	static int metadone=0;

	signed long nextfootnote=0;
	signed long nextendnote=0;
	U32 nextbookmark_b=cp; /*was 0*/
	U32 nextbookmark_e=cp; /*was 0*/
	U32 nextannotation_b=cp; /*was 0*/
	U32 nextannotation_e=cp; /*was 0*/

	U32 sepxfc;
	U32 sepcp;
	sep *asep=NULL;
	sep *tempsep=NULL;

	int newpage=1;

	int notfinished=1;
	int fccount=0;
	int paraflag=0;
	U32 paraendfc=1;

	int d_count=0;

	int issection=1;

	long seek_val=0;

	fseek(in,fcClx,SEEK_SET);
	error(erroroutput,"seeking table to (%x) len %d\n",fcClx,lcbClx);
	error(erroroutput,"begincp is %x\n",begincp);
	while ((clxcount < lcbClx) && (notfinished))
		{
		clxt = getc(in);
		clxcount++;
		error(erroroutput,"clxt is %d\n",clxt);

		psprmlist->list=NULL;
		psprmlist->next=NULL;
		psprmlist->len=0;

		if (clxt == 1)
			{
			/*make a list of of these gprrls*/
			/*decode them when fetched in clxt==2*/
			/*use bitfields to skip ones i dont care about*/
			error(erroroutput,"contains  grpprl\n");
			cb = read_16ubit(in);
			error(erroroutput,"cb is %d\n",cb);
			psprmlist->list = (U8 *) malloc(cb);
			if (psprmlist->list == NULL)
				{
				fprintf(erroroutput,"mem barfoid\n");
				exit(-1);
				}

			clxcount+=(cb+2);
			psprmlist->len=cb;
			error(erroroutput,"here come the gpprl\n");
			for(i=0;i<cb;i++)
				{
				psprmlist->list[i] = getc(in);
				error(erroroutput,"%x",psprmlist->list[i]);
				}
			psprmlist->next = (tSprm *) malloc(sizeof(tSprm));
			if (psprmlist->next == NULL)
				{
				fprintf(erroroutput,"mem barfoid\n");
				exit(-1);
				}
			psprmlist = psprmlist->next;
			index++;
			error(erroroutput,"on gp no %d\n",index);
			}
		else if (clxt == 2)
			{
			index =0;
			error(erroroutput,"contains  Plcfpcd\n");
			
			nopieces = get_piecetable(in,&rgfc,&avalrgfc,&sprm,&clxcount);


			/*guess based on first piece whether to go utf 8 or not*/
			/*
			actually doing utf8 all the time on complex docs as it
			turns out not to be such a good test :-(
			*/

			if ( (header == 0) && (metadone == 0) )
				{
				/*
				fprintf(outputfile,"\n<head>\n<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;charset=utf-8\">\n</head>\n");
				fprintf(outputfile,"<body>\n");
				*/
				do_output_start(avalrgfc,nopieces,doc_style);
				metadone=1;
				}

			for(i=startpiece/*start piece*/;i<nopieces;i++)
				{

				if ((header == 0) && (metadone ==0))
					{
					fprintf(outputfile,"<body>\n");
					metadone=1;
					}

				if (notfinished == 0)
					break;

				/*get the sep*/
				if (headerfooterflag == 0)
					if (issection)
						{
						error(erroroutput,"getting sep, %x, piece is %d\n",cp,i);
						sepxfc = find_FC_sepx(cp,&sepcp,portions);
						tempsep = asep;
						asep = get_sep(sepxfc,main);
						seppiece = find_piece_cp(sepcp,rgfc,nopieces);
						init_chp(&fakechp);
						decode_gpprls(&fakepap,&fakechp,asep,sprm,seppiece,&asprmlist,sheet);
						if (dop.epc == 0)
							if (decode_clx_endnote(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag))
								fprintf(outputfile,"\n<br><img src=\"%s/endnoteend.gif\"><br>\n",patterndir());
						if ((pagenumber != 1) && (newpage))
							sectionbreak(asep);

						if (asep != NULL)
							{
							if (tempsep != NULL)
								free(tempsep);
							tempsep=NULL;
							}
						else
							{
							asep=tempsep;
							tempsep=NULL;
							}
						issection=0;
						}

				if (newpage)
					{
					if (!inatable)
						{
						newpage =0;
						decode_clx_header(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag);
						}
					else
						deferrednewpage=1;
					}			

				/*push this val back as its something else*/
				if (avalrgfc[i] & 0x40000000UL)
					{
					seek_val = avalrgfc[i] & 0xbfffffffUL;
					seek_val = seek_val/2;
					error(erroroutput,"tricky ? seek val (%x)\n",seek_val);
					error(erroroutput,"seeking main to (the FC %x) for len %d\n",seek_val,rgfc[i+1]-rgfc[i]);
					hack=1;
					}
				else
					{
					error(erroroutput,"seeking main to (the FC %x) for len %d piece %d\n",seek_val,2*(rgfc[i+1]-rgfc[i]),i);
					seek_val = avalrgfc[i];
					hack=0;
					}

				/*sometimes corrupt files have a few offsets in the file that are *past* the end of the file, so ive stuck 
				this in, which will sort them out for now*/
				if ((mainend < seek_val) || (0 != fseek(main,seek_val,SEEK_SET)))
					{
					error(erroroutput,"overran the end of the file !!\n");
					continue;
					}

				error(erroroutput,"piece entry fc is %x sprm is %x len is %d\n",seek_val,sprm[i],rgfc[i+1]-rgfc[i]);

				error(erroroutput,"1 getting pap HH nopieces is %d, current piece is %d\n",nopieces,i);
				temppap = apap;
				apap = NULL;
				error(erroroutput,"fccount is %x, paraendfc is %x\n",fccount,paraendfc);
				if (fccount == paraendfc-1)
					paraflag=1;
				apap = get_complex_pap(seek_val,plcfbtePapx,i,nopieces,intervals,rgfc,main,avalrgfc,&nextfc,&paraendfc,&paraendpiece,sheet,a_list_info);
				fflush(main);
				error(erroroutput," the next para is at (%x)\n",nextfc);
				if (apap == NULL)
					{
					error(erroroutput,"we failed in out attempt to get a paragraph\n");
					if (temppap == NULL)
						{
						apap = (pap *) malloc(sizeof(pap));
						if (apap == NULL)
							{
							fprintf(erroroutput,"no mem aieee\n");
							exit(-1);
							}
						init_pap(apap);
						}
					else
						{
						error(erroroutput,"using previous pap\n");
						apap = temppap;
						temppap = NULL;
						}
					}
				else
					{
					if (paraflag)
						{
						end_para(temppap,apap);
						paraflag=0;
						}
					if (temppap != NULL)
						free(temppap);
					temppap = NULL;
					}

				/*begin tappap search*/
				/*if we're in a table search for the one with fTtp*/
			
				decode_gpprls(apap,&fakechp,&fakesep,sprm,paraendpiece,&asprmlist,sheet);

				if (apap->fInTable)
					{
					k = i;
					do
						{
						tapfc1 = avalrgfc[k];
						error(erroroutput,"1. we search for the tap here\n");
						error(erroroutput,"tappap nexts are %x, k is %x\n",tapfc1,k);
						do
							{
							if (tappap != NULL)
								{
								free(tappap);
								tappap=NULL;
								}
							tapfc2 = tapfc1;
							tappap = get_complex_pap(tapfc1,plcfbtePapx,k,nopieces,intervals,rgfc,main,avalrgfc,&tapfc1,NULL,&paraendpiece2,sheet,a_list_info);
							if (tappap != NULL)
								decode_gpprls(tappap,&fakechp,&fakesep,sprm,paraendpiece2,&asprmlist,sheet);
							}
						while( (tappap != NULL) && (!tappap->fTtp)  && (tapfc1 != tapfc2)) ;
						k++;
						}
					while ( (tappap != NULL) && (!tappap->fTtp)  && (k<nopieces) );
					/*
					at this stage tappap has the correct row structure stored in it
					that we want apap to have
					*/
					if (tappap != NULL)
						{
						copy_tap(&(apap->ourtap),&(tappap->ourtap));
						error(erroroutput,"finished search for the tap here\n");
						error(erroroutput,"no of cells is %d, %d\n",apap->ourtap.cell_no,tappap->ourtap.cell_no);
						free(tappap);
						tappap=NULL;
						}
					}
				/*end tappap search*/
				
				error(erroroutput,"AT THIS POINT %d\n",apap->istd);
		
				if (achp != NULL)
					free(achp);
				achp = get_complex_chp(seek_val,plcfbteChpx,i,nopieces,chpintervals,rgfc,main,avalrgfc,&chpnextfc,sheet,apap->istd);
				error(erroroutput," CHP: the next para is at (%x)\n",chpnextfc);
				if (achp == NULL)
					{
					error(erroroutput,"we failed in out attempt to get a chp\n");
					achp = (chp *) malloc(sizeof(chp));
					if (achp == NULL)
						{
						fprintf(erroroutput,"no mem aieee\n");
						exit(-1);
						}
					init_chp(achp);

					}

				if (achp->color[0] == '\0')
					{
					if (headerfooterflag == 1)
						strcpy(achp->color,"#7f5555");
					else if (headerfooterflag == 2)
						strcpy(achp->color,"#330099");
					else if (headerfooterflag == 3)
						strcpy(achp->color,"#ff7777");
					}

				/*
				the gpprl that id added is the one that belongs to the 
				piece that has the paragraph mark
				*/
				
				error(erroroutput,"decoding gpprl with (%x) index %d\n",sprm[paraendpiece],paraendpiece);
				fakepap.istd = apap->istd;
				decode_gpprls(&fakepap,achp,&fakesep,sprm,paraendpiece,&asprmlist,sheet);
				error(erroroutput,"1) ilfo is %d\n",apap->ilfo);
				/*
				decode_e_list(apap,achp,a_list_info);
				*/
				decode_e_chp(achp);
				decode_e_specials(apap,achp,a_list_info);
				decode_e_table(apap,achp,a_list_info,0);
				decode_s_table(apap,achp,a_list_info,0);
				decode_s_specials(apap,achp,a_list_info);
				decode_s_chp(achp,ffn_sttbf,sheet);

				error(erroroutput,"2) ilfo is %d\n",apap->ilfo);
				j=0;

				/*this are the count of elements (can be both 8 and 16 bit)*/
				fccount=seek_val-1;
				error(erroroutput,"fcount is %x pos is %x\n",fccount,ftell(main));
				/*this is the actual file position, always in bytes*/
				while(j<rgfc[i+1]-rgfc[i])
					{
					/*get the sep*/
					if (headerfooterflag == 0)
						if (issection)
							{
							error(erroroutput,"getting sep, %x, piece is %d\n",cp,i);
							sepxfc = find_FC_sepx(cp,&sepcp,portions);
							tempsep=NULL;
							asep = get_sep(sepxfc,main);
							seppiece = find_piece_cp(sepcp,rgfc,nopieces);
							decode_gpprls(&fakepap,&fakechp,asep,sprm,seppiece,&asprmlist,sheet);
							if (dop.epc == 0)
								if (decode_clx_endnote(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag))
									fprintf(outputfile,"\n<br><img src=\"%s/endnoteend.gif\"><br>\n",patterndir());
							if ((pagenumber != 1) && (newpage))
								sectionbreak(asep);
							issection=0;

							if (asep != NULL)
								{
								if (tempsep != NULL)
									free(tempsep);
								tempsep=NULL;
								}
							else
								{
								asep=tempsep;
								tempsep=NULL;
								}
							}
					
					if (newpage)
						{
						if (header == 0)
							{
							if (!inatable)
								{
								newpage =0;
								decode_clx_header(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag);
								}
							else
								deferrednewpage=1;
							}
						}			

					if (!hack)
						{
						letter = read_16ubit(main);
						fccount+=2;	
						}
					else
						{
						letter = getc(main);
						fccount++;
						}

					if (j==0) 
						{
						if ( (letter != 12) && (letter!=13) )
							{
							/*
							decode_s_list(apap,achp,a_list_info,ffn_sttbf,DONTIGNORENUM,sheet);
							*/
							wvGetListInfo(apap,achp,a_list_info->lfo,
								a_list_info->lfolvl,a_list_info->lvl,
								a_list_info->nolfo,a_list_info->lst,
								a_list_info->noofLST,sheet,ffn_sttbf);
							error(erroroutput,"finished list hwew letter was %d %x\n",letter,letter);
							}
						else
							{
							error(erroroutput,"finished list hwew letter was %d %x\n",letter,letter);
							}
						}


					if (fccount >= nextfc)
						{
						lastfc = nextfc;
						error(erroroutput,"the fc is (%x) and the first fc after end of para is (%x)",fccount,nextfc);
						error(erroroutput,"so im looking for the pap again\n");
						error(erroroutput,"2 getting pap HH, jc was %d\n",apap->justify);
						if (apap != NULL)
							{
							temppap = apap;
							apap = NULL;
							}
						if (fccount == paraendfc-1)
							paraflag=1;
						apap = get_complex_pap(fccount,plcfbtePapx,i,nopieces,intervals,rgfc,main,avalrgfc,&nextfc,&paraendfc,&paraendpiece,sheet,a_list_info);
						fflush(main);
						error(erroroutput,"the nextfc for pap one is at %x\n",nextfc);

						if (apap == NULL)
							{
							error(erroroutput,"we failed in out attempt to get a paragraph\n");
							if (temppap == NULL)
								{
								apap = (pap *) malloc(sizeof(pap));
								if (apap == NULL)
									{
									fprintf(erroroutput,"no mem aieee\n");
									exit(-1);
									}
								init_pap(apap);
								}
							else
								{
								apap = temppap;
								temppap = NULL;
								error(erroroutput,"using previous pap, jc is now %d\n",apap->justify);
								}
							}
						else
							{
							/*begin tappap search*/
							/*if we're in a table search for the one with fTtp*/
							decode_gpprls(apap,&fakechp,&fakesep,sprm,paraendpiece,&asprmlist,sheet);
							if (apap->fInTable)
								{
								k = i;
								do
									{
									if (k==i)
										tapfc1 = fccount;
									else
										tapfc1 = avalrgfc[k];
									error(erroroutput,"2. we search for the tap here\n");
									error(erroroutput,"tappap nexts are %x, piece is %x\n",tapfc1,k);
									do
										{
										if (tappap != NULL)
											{
											free(tappap);
											tappap=NULL;
											}
										tapfc2 = tapfc1;
										error(stderr,"table trying %x\n",tapfc1);
										tappap = get_complex_pap(tapfc1,plcfbtePapx,k,nopieces,intervals,rgfc,main,avalrgfc,&tapfc1,NULL,&paraendpiece2,sheet,a_list_info);
										if (tappap != NULL)
											decode_gpprls(tappap,&fakechp,&fakesep,sprm,paraendpiece2,&asprmlist,sheet);
										}
									while( (tappap != NULL) && (!tappap->fTtp) && (tapfc1 != tapfc2)) ;
									k++;
									}
								while ( (tappap != NULL) && (!tappap->fTtp) && (k<nopieces) );
								/*
								at this stage tappap has the correct row structure stored in it
								that we want apap to have
								*/
								if (tappap != NULL)
									{
									copy_tap(&(apap->ourtap),&(tappap->ourtap));
									error(erroroutput,"finished 2nd search for the tap here\n");
									error(erroroutput,"no of cells is %d, %d\n",apap->ourtap.cell_no,tappap->ourtap.cell_no);
									free(tappap);
									tappap=NULL;
									}
								else
									{
									error(erroroutput,"finished 2nd search for the tap here in error\n");
									}
								}
							/*end tappap search*/

							/*last gasp of the pap*/
							if (paraflag)
								{
								end_para(temppap,apap);
								paraflag=0;
								}
							
							/*remove unneeded pap*/
							if (temppap != NULL)
								free(temppap);
								
							temppap = NULL;
							}

						}


					if ((fccount >= chpnextfc)  || (fccount >= lastfc))
						{
						lastchpfc=1;
						if (achp != NULL)
							{
							free(achp);
							achp=NULL;
							}
						achp = get_complex_chp(fccount,plcfbteChpx,i,nopieces,chpintervals,rgfc,main,avalrgfc,&chpnextfc,sheet,apap->istd);
						error(erroroutput," CHP: the next para is at (%x)\n",chpnextfc);

						if (achp == NULL)
							error(erroroutput,"we failed in out attempt to get a chp\n");
						else
							{
							if (achp->color[0] == '\0')
								{
								if (headerfooterflag == 1)
									strcpy(achp->color,"#7f5555");
								else if (headerfooterflag == 2)
									strcpy(achp->color,"#330099");
								else if (headerfooterflag == 3)
									strcpy(achp->color,"#ff7777");
								}
		
							error(erroroutput,"before gprl chp fontcode is %d\n",achp->fontcode);
							decode_gpprls(&fakepap,achp,&fakesep,sprm,paraendpiece,&asprmlist,sheet);
							error(erroroutput,"after gprl chp fontcode is %d\n",achp->fontcode);
							/*
							decode_e_list(apap,achp,a_list_info);
							*/
							decode_e_chp(achp);
							error(erroroutput,"after decode chp fontcode is %d\n",achp->fontcode);
							}
						}

					if (fccount >= lastfc) 
						{
						decode_e_specials(apap,achp,a_list_info);
						decode_e_table(apap,achp,a_list_info,0);
						decode_s_table(apap,achp,a_list_info,0);
						decode_s_specials(apap,achp,a_list_info);
						decode_s_chp(achp,ffn_sttbf,sheet);
						lastfc=0xffffffffL;
						}

					if (lastchpfc) 
						{
						lastchpfc=0xffffffffL;
						decode_s_chp(achp,ffn_sttbf,sheet);
						if ( ((letter != 12) && (letter != 13)) )
							{
							wvGetListInfo(apap,achp,a_list_info->lfo,
								a_list_info->lfolvl,a_list_info->lvl,
								a_list_info->nolfo,a_list_info->lst,
								a_list_info->noofLST,sheet,ffn_sttbf);
							/*
							decode_s_list(apap,achp,a_list_info,ffn_sttbf,DONTIGNORENUM,sheet);
							*/
							error(erroroutput,"list started following letter is %d\n",letter);
							}
						else 
							error(erroroutput,"list started following letter is %d\n",letter);
						}


					if (cp >= begincp)
						{
						if ((headerfooterflag >0) && (headerfooterflag < 2))
							{
							if (letter == 13)
								{
								/*
								headers and footers seem to continue until 2 0x0d are reached, they dont
								appear after all to go until the len derived from the header table
								like i thought originally.
								*/
								d_count++;
								if (d_count == 2)
									{
									notfinished=0;
									break;
									}
								}
							else
								d_count=0;
							}
						
						error(erroroutput,"decoding letter: %c, fc is %x, nextfc is %x\n",letter,fccount,nextfc);

						if ( (realcp == nextfootnote) && (headerfooterflag == 0) ) 
							{
							decode_f_reference(portions);
							get_next_f_ref(portions,&nextfootnote);
							}
						
						if ( (realcp == nextendnote) && (headerfooterflag == 0) ) 
							{
							decode_e_reference(portions);
							get_next_e_ref(portions,&nextendnote);
							}

						while ( (realcp == nextannotation_b) && (doannos==1) )
							nextannotation_b = decode_b_annotation(&(portions->a_bookmarks));		

						while (realcp == nextbookmark_b) 
							nextbookmark_b = decode_b_bookmark(&(portions->l_bookmarks),&(portions->bookmarks));

						while (realcp == nextbookmark_e) 
							nextbookmark_e = decode_e_bookmark(&(portions->l_bookmarks));

						while ( (realcp == nextannotation_e) && (doannos==1) )
							nextannotation_e = decode_e_annotation(&(portions->a_bookmarks));

						if (hack)
							newpage = decode_letter(letter,1,apap,achp,all_fields[0],main,data,ffn_sttbf,a_list_info,portions,&issection,sheet);
						else
							newpage = decode_letter(letter,1,apap,achp,all_fields[0],main,data,ffn_sttbf,a_list_info,portions,&issection,sheet);

						error(erroroutput,"W:sync, actuallt at %x, fccount is %x and aval is %xthough\n",ftell(main),fccount,fccount);
						}
					else
						{
						realcp++;
						cp++;
						}


					/*if newpage then decode header*/
					if (newpage)
						{

						if ((inatable)  && newpage == 2)
							{
							/*in this case we have ended a row, so we should take the opportunity to halt the table*/
							fprintf(outputfile,"\n</table>\n");
							inatable=0;
							newpage=1;
							}
						
						error(erroroutput,"clx footer\n");

						if ((!inatable) && (newpage == 1))
							{
							decode_clx_footer(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag);
							if (!issection)
								pagebreak();
							}	
						else if (newpage == 1)
							deferrednewpage=1;
						}

			


					/*get the sep*/
					if (headerfooterflag == 0)
						if (issection)
							{
							error(erroroutput,"getting sep, %x\n",cp);
							sepxfc = find_FC_sepx(cp,&sepcp,portions);
							asep = get_sep(sepxfc,main);
							seppiece = find_piece_cp(sepcp,rgfc,nopieces);
							decode_gpprls(&fakepap,&fakechp,asep,sprm,seppiece,&asprmlist,sheet);
							if (dop.epc == 0)
								if (decode_clx_endnote(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag))
									fprintf(outputfile,"\n<br><img src=\"%s/endnoteend.gif\"><br>\n",patterndir());
							if ((pagenumber != 1) && (newpage))
								sectionbreak(asep);
							issection=0;

							if (asep!= NULL)
								{
								if (tempsep != NULL)
									free(tempsep);
								tempsep=NULL;
								}
							else
								{
								asep=tempsep;
								tempsep=NULL;
								}
							}

					if (cp >= endcp)
						{
						error(erroroutput,"reached end of header or main, headerfootflag is %d\n",headerfooterflag);
							notfinished=0;
							error(erroroutput,"end of header or main break\n");
							break;
						}

					j++;
					}/*end for/while j*/
					
				error(erroroutput,"here after header ?\n");

				if (achp != NULL)
					{
					free(achp);
					achp = NULL;
					}
				}
			}
		else
			fprintf(erroroutput,"some kind of error occured, byte count off\n");
		}


	if (apap == NULL)
		{
		apap = (pap *)malloc(sizeof(pap));
		if (apap == NULL)
			return;
		}
#if 0	
	apap->istd = 0;
	apap->ilvl=-1;
	apap->tableflag=0;
#endif
	init_pap(apap);
	/*
	decode_e_list(apap,achp,a_list_info);
	*/
	decode_e_specials(apap,achp,a_list_info);
	decode_e_table(apap,achp,a_list_info,0);
	if (apap !=NULL)
		free(apap);
	if (achp !=NULL)
		free(achp);

	if (!newpage)
		{
		error(erroroutput,"clx footer 2\n");
		decode_clx_footer(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag);
		/*
		always do the endnotes at the end of the doc, even if some have been done earlier, there
		might be more left to do
		*/
		if (decode_clx_endnote(rgfc,asep,nopieces,startpiece,begincp,endcp,in,main,data,fcClx,lcbClx,intervals,chpintervals,plcfbtePapx,plcfbteChpx,all_fields,a_list_info,sheet,portions,ffn_sttbf,headerfooterflag))
			fprintf(outputfile,"\n<br><img src=\"%s/endnoteend.gif\"><br>\n",patterndir());
		}

	if (rgfc != NULL)
		free(rgfc);
	if (avalrgfc != NULL)
		free(avalrgfc);
	if (sprm != NULL)
		free(sprm);

	if (asep != NULL)
		free(asep);

	psprmlist = &asprmlist;
	if (psprmlist->list != NULL)
			free(psprmlist->list);
	psprmlist = psprmlist->next;
	while (psprmlist != NULL)
		{
		freesprm = psprmlist;
		psprmlist = psprmlist->next;
		if (freesprm->list != NULL)
			free(freesprm->list);
		free(freesprm);
		}

	fseek(in,pos,SEEK_SET);
	fseek(main,mainpos,SEEK_SET);
	}



		
int decode_ilfo(pap *retpap,chp *achp,list_info *a_list_info,style *sheet,FFN_STTBF *ffn_sttbf)
	{
	int j;

	/*
	wvGetListInfo(retpap,achp,a_list_info->lfo,a_list_info->lfolvl,a_list_info->lvl,a_list_info->nolfo,a_list_info->lst,a_list_info->noofLST,sheet,ffn_sttbf);
	*/
	
	/*
	else now we use our a_list_data to generate the paps list_data
	including the text to use, whether to prepend a index or not
	what format the index is in, and the start index
	*/
	/*
	first get the text, ) / . / string / whatever
	*/
	error(erroroutput,"ilfo %d of %d\n",retpap->ilfo,a_list_info->nooflfos);
	if (retpap->ilfo >= a_list_info->nooflfos)
		{
		if (retpap->ilfo != 2047)
			{
			error(erroroutput,"hmm problem\n");
			return(-1);
			}
		else
			error(erroroutput,"legacy anld\n");
		return(-1);
		}

	/*
	what should happen here, is after we get this base list data, 
	we add the overrides to a copy of our own
	*/

	retpap->list_data = &(a_list_info->o_list_def[retpap->ilfo]);
	
	retpap->list_data->id = a_list_info->lst_ids[retpap->ilfo];
	for (j=0;j<retpap->list_data->len;j++)
	            error(erroroutput,"--> %c",retpap->list_data->list_string[j]);


	error(erroroutput,"ilfo request is %d, the id in the lfo is %x",retpap->ilfo,a_list_info->lst_ids[retpap->ilfo]);
	j=0;
	while(j<a_list_info->nooflsts)
		{
		if (a_list_info->o_lst_ids[j] == retpap->list_data->id)
			{
			error(erroroutput,"and the returned index is %d\n",j);
			return(j);
			}
		j++;
		}

	return(0);
	}




/*
returns 0 for nothing special,
returns 1 if a first pass through a table is needed
*/

int decode_s_table(pap *apap,chp *achp,list_info *a_list_info,int silent)
	{
	int redotable=0,i;
	float width;
	int clearedspecials=0;
	chp reset;
	pap resetpap;
	int resetchp=0;
	int ret=0;

	int tablewidth;

	init_chp(&reset);
	init_pap(&resetpap);
	reset.fontcode=-2;
	reset.ftcAscii=currentfontcode+1;

	error(erroroutput,"decoding start specials istd is %d\n",apap->istd);


	if ((!(inatable)) && (apap->fInTable))
		{
		if (!resetchp)
			{
			decode_e_chp(&reset);
			resetchp=1;
			}
		if (!clearedspecials)
			{
			decode_e_specials(&resetpap,NULL,NULL);
			clearedspecials=1;
			}
		flushbreaks(0);
		do_indent(apap);
	
		if (notablewidth)	
			{
			if (!(silent)) fprintf(outputfile,"\n<table border=1>");
			}
		else	
			{
			if (!(silent)) fprintf(outputfile,"\n<table border=1 width=\"");
			}
		ret=1;
		/*search for tap*/
		rowcount=0;
		inatable=1;
		inarow=0;
		inacell=0;
		lastrowlen=apap->ourtap.cell_no;
		error(erroroutput,"lastrowlen is %d\n",lastrowlen);
		tablewidth=0;
		for (i=0;i<apap->ourtap.cell_no+1;i++)
			lastcellwidth[i] = apap->ourtap.cellwidth[i];
			
		tablewidth = lastcellwidth[apap->ourtap.cell_no] - lastcellwidth[0];
		error(erroroutput,"set table, tablewidth is twirps is %d\n",tablewidth);
		if (!notablewidth)
			if (!(silent)) fprintf(outputfile,"%d\">\n",tablewidth/TWIRPS_PER_H_PIXEL);
		}

	if ( ((inatable) && (apap->fInTable) && (!apap->fTtp)) || ((inatable) && (!inacell) && (inarow)) )
		{
		if ((inarow == 0) || (silent))
			{

			if (silent)
				cno = add_t(&tablevals,apap->ourtap.cellwidth,apap->ourtap.cell_no);
			
			if (lastrowlen != apap->ourtap.cell_no)
				{
				error(erroroutput,"well have to start a new table\n");
				redotable=1;
				}
			else 
				{
				for (i=0;i<apap->ourtap.cell_no+1;i++)
					{
					if (lastcellwidth[i] != apap->ourtap.cellwidth[i])
						redotable=1;
					}
				}

			if (redotable)
				{
				if (!clearedspecials)
					{
					/*decode_e_specials(&reset,achp,a_list_info);*/
					clearedspecials=1;
					}
				if (!resetchp)
					{
					decode_e_chp(&reset);
					resetchp=1;
					}
				if (!(silent)) fprintf(outputfile,"\n</table>\n");
#if 0
				flushbreaks(0);
#endif
				if (notablewidth)
					{
					if (!(silent)) fprintf(outputfile,"\n<table border=1>\n");
					}
				else
					{
					if (!(silent)) fprintf(outputfile,"\n<table border=1 width=\"");
/*begin paste*/
					tablewidth =  apap->ourtap.cellwidth[apap->ourtap.cell_no] -  apap->ourtap.cellwidth[0];
					if (!(silent)) fprintf(outputfile,"%d\">\n",tablewidth/TWIRPS_PER_H_PIXEL);
/*end paste*/
					}
					
				error(erroroutput,"redone table\n");
				/*no need to reset rowcount*/
				inatable=1;
				inarow=1;
				inacell=0;
				lastrowlen=apap->ourtap.cell_no;
				for (i=0;i<apap->ourtap.cell_no+1;i++)
					lastcellwidth[i] = apap->ourtap.cellwidth[i];

				}

			if (!clearedspecials)
				{
				/*decode_e_specials(&reset,achp,a_list_info);*/
				clearedspecials=1;
				}
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			if (!(silent)) fprintf(outputfile,"\n<tr>\n");
			rowcount++;
			colcount=0;
			inarow = 1;
			}
			
		if ( (inacell == 0) && (colcount < apap->ourtap.cell_no) )
			{
			width = (apap->ourtap.cellwidth[colcount+1]-apap->ourtap.cellwidth[colcount])*100;
			width = rint(width/apap->ourtap.tablewidth);
			if (notablewidth)
				{
				if (!(silent)) fprintf(outputfile,"\n<td valign=\"top\" "); 
				}
			else
				{
				if (!(silent)) fprintf(outputfile,"\n<td valign=\"top\" width=\"%.0f%%\" ",width); 
				}
			if (apap->ourtap.rowheight != 0)
				if (!(silent)) fprintf(outputfile,"height=\"%d\"",abs(apap->ourtap.rowheight)/TWIRPS_PER_V_PIXEL); 
			chpson();

			if (!(silent)) output_tablebg(apap);

			cellempty=1;
			inacell=1;
			colcount++;
			}
		else if ( (inacell == 0) && (apap->ourtap.cell_no == 0) )
			{
			error(erroroutput,"dont have information as to width of cells\n");
#if 0
			if (!resetchp)
                {
                decode_e_chp(&reset);
                resetchp=1;
                }
#endif
            if (!(silent)) fprintf(outputfile,"\n<td valign=\"top\" ");
			if (apap->ourtap.rowheight != 0)
				if (!(silent)) fprintf(outputfile,"height=\"%d\"",abs(apap->ourtap.rowheight)/TWIRPS_PER_V_PIXEL); 
			chpson();

			if (!(silent)) output_tablebg(apap);

            cellempty=1;
            inacell=1;
            colcount++;
			}

		check_auto_color(achp);
			
		/*weve come into another cell*/
		}
	return(ret);
	}

void decode_s_specials(pap *apap,chp *achp,list_info *a_list_info)
	{
	int resetchp=0;
	chp reset;
	init_chp(&reset);
	
	reset.fontcode=-2;

#if 0
	at this stage if there was a style in the config file, then the
	pap when it reaches here will have its internals modified to reflect
	the config file.
	
#endif

	error(erroroutput,"decoding start specials istd is %d\n",apap->istd);

	do_indent(apap);

	if ((apap->justify == 1) && (incenter == 0))
		{
		incenter =1;
		fprintf(outputfile,"\n<CENTER>");
		error(erroroutput,"begin of center\n");
		}

	if ((apap->justify == 2) && (inrightjust == 0))
		{
		inrightjust =1;
		fprintf(outputfile,"\n<DIV align=right>");
		error(erroroutput,"begin of right\n");
		}

#if 0
	something like, if exist config file starting tag use it
	and bail out, otherwise continue on here.
#endif

	if (apap->begin != NULL)
		{
		if (apap->istd == inah1)
			return;
		if (!resetchp)
        	{
            decode_e_chp(&reset);
            resetchp=1;
            }
		fprintf(outputfile,"\n%s\n",apap->begin);
		inah1 = apap->istd;
		endthing =  apap->end;
		return;
		}
	else
		endthing=NULL;

	if (ignoreheadings == 0)
		{
		if ((apap->istd == 1) && (inah1 != 1))
			{
			inah1=1;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H1>");
			}

		if ((apap->istd == 2) && (inah1 != 2))
			{
			inah1=2;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H2>");
			}

		if ((apap->istd == 3) && (inah1 != 3))
			{
			inah1=3;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H3>");
			}

		if ((apap->istd == 4) && (inah1 != 4))
			{
			inah1=4;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H4>");
			}

		if ((apap->istd == 5) && (inah1 != 5))
			{
			inah1=5;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H5>");
			}

		if ((apap->istd == 6) && (inah1 != 6))
			{
			inah1=6;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H6>");
			}

		if ((apap->istd == 7) && (inah1 != 7))
			{
			inah1=7;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H7>");
			}

		if ((apap->istd == 8) && (inah1 != 8))
			{
			inah1=8;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H8>");
			}

		if ((apap->istd == 9) && (inah1 != 9))
			{
			inah1=9;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"<H9>");
			}
		}	

	error(erroroutput,"manually doing start chp\n");
	}

/*
returns 0 usually,
returns 1 if a row has ended,
returns 2 is the table has ended
*/

int decode_e_table(pap *apap,chp *achp,list_info *a_list_info, int silent)
	{
	int ret=0;
	pap reset;
	chp resetc;
	int clearedspecials=0;
	error(erroroutput,"decoding end table\n");

	init_pap(&reset);
	init_chp(&resetc);
	resetc.fontcode=-2;
	resetc.ftcAscii=currentfontcode+1;

	if ((apap->fInTable == 1) && (apap->fTtp == 1) && apap->tableflag)
		{
		error(erroroutput,"cell and row end\n");
		if (!(silent)) fprintf(outputfile,"\n</tr>\n");
		ret=1;
		inarow=0;
		inacell=0;
		apap->tableflag=0;
		}
	else if ((apap->fInTable == 1)  && (apap->fTtp != 1) && apap->tableflag )
		{
		if (!clearedspecials)
			{
			decode_e_chp(&resetc);
			decode_e_specials(&reset,achp,a_list_info);
			clearedspecials=1;
			}
		if (cellempty == 1)
			if (!(silent)) fprintf(outputfile,"&nbsp;");
		if (!(silent)) fprintf(outputfile,"\n</td>\n");
		backgroundcolor[0] = '\0';
		inacell=0;
		apap->tableflag=0;
		chpsoff();
		error(erroroutput,"cell end\n");
		}

	if ( (inatable) && (!apap->fInTable) && (!inarow) )
		{
		if (!(silent)) fprintf(outputfile,"\n</table>\n");
		lastrowlen=0;
		inatable=0;
		ret=2;
		chpson();
		}

	return(ret);
	}
void decode_e_specials(pap *apap,chp *achp,list_info *a_list_info)
	{
	int resetchp=0;
	chp reset;
	error(erroroutput,"decoding specials istd is %d\n",apap->istd);
	/*before a para ends, well reset all chars to original defaults*/

	init_chp(&reset);
	reset.fontcode=-2;

	error(erroroutput,"manual end chp\n");

	if (endthing != NULL)
		{
		if (apap->istd == inah1)
			return;
		if (!resetchp)
        	{
            decode_e_chp(&reset);
            resetchp=1;
            }
		fprintf(outputfile,"\n%s\n",endthing);
		inah1 = 0;
		}
	else
	if (ignoreheadings ==0)
		{
		if ((apap->istd != 9) && (inah1 == 9))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
				
			fprintf(outputfile,"</H9>\n");
			}
		else if ((apap->istd != 8) && (inah1 == 8))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H8>\n");
			}
		else if ((apap->istd != 7) && (inah1 == 7))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H7>\n");
			}
		else if ((apap->istd != 6) && (inah1 == 6))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H6>\n");
			}
		else if ((apap->istd != 5) && (inah1 == 5))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H5>\n");
			}
		else if ((apap->istd != 4) && (inah1 == 4))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H4>\n");
			}
		else if ((apap->istd != 3) && (inah1 == 3))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H3>\n");
			}
		else if ((apap->istd != 2) && (inah1 == 2))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H2>\n");
			}
		else if ((apap->istd != 1) && (inah1 == 1))
			{
			inah1=0;
			if (!resetchp)
				{
				decode_e_chp(&reset);
				resetchp=1;
				}
			fprintf(outputfile,"</H1>\n");
			}
		}

	error(erroroutput,"inah1 is now %d\n",inah1);
	if ((apap->justify != 2) && (inrightjust == 1))
		{
		inrightjust =0;
		error(erroroutput,"end of right\n");
		fprintf(outputfile,"\n</DIV>");
		}

	if ((apap->justify != 1) && (incenter == 1))
		{
		incenter =0;
		error(erroroutput,"end of center\n");
		fprintf(outputfile,"\n</CENTER>");
		}
#if 0
	do_indent(apap);
#endif
	}

void chpsoff()
	{
	error(erroroutput,"chps turned off\n");
	chps=1;
	}

void chpson()
	{
	error(erroroutput,"chps turned on\n");
	chps=0;
	}


