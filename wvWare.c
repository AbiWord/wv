#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include "getopt.h"

#define VERSION "0.6.5"

/* strdup isn't declared in <string.h> for `gcc -ansi'; declare it here */
extern char* strdup (const char*);

/*
Released under GPL, written by Caolan.McNamara@ul.ie.

Copyright (C) 1998,1999 
	Caolan McNamara

Real Life: Caolan McNamara           *  Doing: MSc in HCI
Work: Caolan.McNamara@ul.ie          *  Phone: +353-86-8790257
URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
How would you have done it?
*/

/*
returns 1 for not an ole doc
2 ole but not word doc
-1 for an error of some unknown kind
0 on success
*/

char *config="wvHtml.xml";

int myelehandler(wvParseStruct *ps,wvTag tag, void *props, int dirty);
int mydochandler(wvParseStruct *ps,wvTag tag);
int myCharProc(wvParseStruct *ps,U16 eachchar,U8 chartype,U16 lid);
int mySpecCharProc(wvParseStruct *ps,U16 eachchar,CHP *achp);

FILE *wvOpenConfig(char *config);


int HandleBitmap(char *name,BitmapBlip *bitmap);
int HandleMetafile(char *name,MetaFileBlip *bitmap);

/* should really be a config.h decl for having strdup, but... */
#ifdef __MWERKS__
char *strdup(const char *text)
{
    char *buf;
    size_t len;

    len = strlen(text) + 1;
    buf = (char *) wvMalloc(len);
    memcpy(buf, text, len);

    return buf;
}


#endif

char *wvHtmlGraphic(wvParseStruct *ps,Blip *blip)
	{
	static int i;
	char buffer[10];
	char *name;
	FILE *fd;
	char test[3];
	sprintf(buffer,"%d",i++);
	name = strdup(ps->filename);
	wvTrace(("name is %s\n",name));
	remove_suffix (name, ".doc");
	if (ps->dir != NULL)
		{
		char *tempa,*tempb;
		tempb = strdup(ps->dir);
		tempa = base_name(name);
		wvAppendStr(&tempb,"/");
		wvAppendStr(&tempb,tempa);
		wvFree(name);
		name = tempb;
		}

	wvAppendStr(&name,buffer);
	/* 
	temp hack to test older included bmps in word 6 and 7,
	should be wrapped in a modern escher strucure before getting
	to here, and then handled as normal
	*/
	wvTrace(("type is %d\n",blip->type));
	switch(blip->type)
		{
		case msoblipJPEG:
        case msoblipDIB:
        case msoblipPNG:
			fd = (FILE *)(blip->blip.bitmap.m_pvBits);
			test[2] = '\0';
			test[0] = getc(fd);
			test[1] = getc(fd);
			rewind(fd);
			if (!(strcmp(test,"BM")))
				{
				wvAppendStr(&name,".bmp");
				if (0 != HandleBitmap(name,&blip->blip.bitmap))
					return(NULL);
				remove_suffix (name, ".bmp");
				bmptopng(name);
				wvAppendStr(&name,".png");
				return(name);
				}
		default:
			break;
		}

	switch(blip->type)
        {
        case msoblipWMF:
			wvAppendStr(&name,".wmf");
			if (0 != HandleMetafile(name,&blip->blip.metafile))
				return(NULL);
			break;
        case msoblipEMF:
			wvAppendStr(&name,".emf");
			if (0 != HandleMetafile(name,&blip->blip.metafile))
				return(NULL);
			break;
        case msoblipPICT:
			wvAppendStr(&name,".pict");
			if (0 != HandleMetafile(name,&blip->blip.metafile))
				return(NULL);
            break;
        case msoblipJPEG:
			wvAppendStr(&name,".jpg");
			if (0 != HandleBitmap(name,&blip->blip.bitmap))
				return(NULL);
			break;
        case msoblipDIB:
			wvAppendStr(&name,".dib");
			if (0 != HandleBitmap(name,&blip->blip.bitmap))
				return(NULL);
			break;
        case msoblipPNG:
			wvAppendStr(&name,".png");
			if (0 != HandleBitmap(name,&blip->blip.bitmap))
				return(NULL);
            break;
        }
	return(name);
printf("<%s>\n", name);
	}


int HandleBitmap(char *name,BitmapBlip *bitmap)
	{
	int c;
	FILE *fd;
	fd = fopen(name,"wb");
	if (fd == NULL)
		{
		wvError(("Cannot open %s for writing:%s\n",name,strerror(errno)));
		return(-1);
		}
	while (EOF != (c = getc((FILE *)(bitmap->m_pvBits))))
		fputc(c,fd);
	fclose(fd);
	wvTrace(("Name is %s\n",name));
	return(0);
	}


int HandleMetafile(char *name,MetaFileBlip *bitmap)
	{
	int c;
	FILE *fd;
	fd = fopen(name,"wb");
	if (fd == NULL)
		{
		wvError(("Cannot open %s for writing:%s\n",name,strerror(errno)));
		return(-1);
		}
	while (EOF != (c = getc((FILE *)(bitmap->m_pvBits))))
		fputc(c,fd);
	fclose(fd);
	wvTrace(("Name is %s\n",name));
	return(0);
	}

static void do_version(void)
{
  /* todo: initialize this in a configure script */
  printf("wvWare %s\n", VERSION);
}

static void do_help( void )
	{
	  do_version();
	  printf("Usage: wvWare [OPTION...] filename.doc\n");
	  printf("\nCommon Options:\n");
	  printf("  -x --config=config.xml\tSpecify an output filter to use\n");
	  printf("  -c --charset=charset\t\tSpecify an iconv charset encoding\n");
	  printf("  -p --password=password\tSpecify password for encrypted\n\t\t\t\tWord Documents\n");
	  printf("  -d --dir=dir\t\t\tDIR\n");
	  printf("  -v --version\t\t\tPrint wvWare's version number\n");
	  printf("  -? --help\t\t\tPrint this help message\n");
	  printf("\nwvWare is a suite of applications that converts Microsoft Word Documents\n");
	  printf("(versions 2,5,6,7,8,9) into more \"useful\" formats such as HTML, LaTeX,\n");
	  printf("ABW, WML, Text, etc... wvWare is also a library which can be used by\n");
	  printf("other applications to import (and soon export) Word documents.\n\n");
	  printf("Authors:\nDom Lachowicz (dominicl@seas.upenn.edu)\n");
	  printf("Caolan McNamara (original author)\nVisit http://www.wvware.com\n");
	}

char *charset=NULL;

int main(int argc,char **argv)
	{
	FILE *input;
	char *password=NULL;
	char *dir=NULL;
	int ret;
	state_data myhandle;
	expand_data expandhandle;
	wvParseStruct ps;
	int c,index=0;
	static struct option long_options[] =
        {
        {"charset",1,0,'c'},
        {"config",1,0,'x'},
        {"password",1,0,'p'},
        {"dir",1,0,'d'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
        {0,0,0,0}
        };

	wvInitError();

	if (argc < 2)
	  {
		do_help();
		exit(-1);
	  }

	 while (1)
        { 
        c = getopt_long (argc, argv, "?vc:x:p:d:", long_options, &index);
        if (c == -1)
            break;
		switch(c)
			{
			case '?':
			  do_help();
			  return 0;
			case 'v':
			  do_version();
			  return 0;
			case 'c':
				if (optarg)
					charset = optarg;
				else
					wvError(("No argument given to charset"));
				break;
			case 'x':
				if (optarg)
					config = optarg;
				else
					wvError(("No config file given to config option"));
				break;
			case 'p':
				if (optarg)
					password = optarg;
				else
					wvError(("No password given to password option"));
				break;
			case 'd':
				if (optarg)
					dir = optarg;
				else
					wvError(("No directory given to dir option"));
				break;
			default:
			  do_help();
			  return -1;
			}
		}

	if (optind >= argc)
		{
		fprintf(stderr,"No file name given to open\n");
		return(-1);
		}

	input = fopen(argv[optind],"rb");
	if (!input)
		{
		fprintf(stderr,"Failed to open %s: %s\n",argv[optind],strerror(errno));
		return(-1);
		}
	fclose(input);

	ret = wvInitParser(&ps,argv[optind]);
	ps.filename = argv[optind];
	ps.dir = dir;

	if (ret & 0x8000)  /* Password protected? */
		{
		if ( (ret & 0x7fff) == WORD8)
			{
			ret = 0;
			if (password == NULL)
				{
				fprintf(stderr,"Password required, this is an encrypted document\n");
				return(-1);
				}
			else
				{
				wvSetPassword(password,&ps);
				if (wvDecrypt97(&ps))
					{
					wvError(("Incorrect Password\n"));
					return(-1);
					}
				}
			}
		else if ( ( (ret & 0x7fff) == WORD7) || ( (ret & 0x7fff) == WORD6))
			{
			ret=0;
			if (password == NULL)
				{
				fprintf(stderr,"Password required, this is an encrypted document\n");
				return(-1);
				}
			else
				{
				wvSetPassword(password,&ps);
				if (wvDecrypt95(&ps))
					{
					wvError(("Incorrect Password\n"));
					return(-1);
					}
				}
			}
		}

	if (ret)
		{
		wvError(("startup error\n"));
		wvOLEFree();
		return(-1);
		}

	wvSetElementHandler(myelehandler);
	wvSetDocumentHandler(mydochandler);
	wvSetCharHandler(myCharProc);
	wvSetSpecialCharHandler(mySpecCharProc);

	wvInitStateData(&myhandle);
    myhandle.fp = wvOpenConfig(config);
    if (myhandle.fp == NULL)
		{
        wvError(("config file not found\n"));
		return(-1);
		}
    else
		{
		wvTrace(("x for FILE is %x\n",myhandle.fp));
        ret = wvParseConfig(&myhandle);
		}

	if (!ret)	
		{
		expandhandle.sd = &myhandle;
		ps.userData = &expandhandle;
		ret = wvHtml(&ps);
		}
	wvReleaseStateData(&myhandle);

	if (ret == 2)
		return(2);
	else if (ret != 0)
		ret = -1;
	wvOLEFree();
	return(ret);
	}

int myelehandler(wvParseStruct *ps,wvTag tag, void *props, int dirty)
    {
	static PAP *ppap;
    expand_data *data = (expand_data *)ps->userData;
    data->anSttbfAssoc = &ps->anSttbfAssoc;
	data->lfo = &ps->lfo;
	data->lfolvl = ps->lfolvl;
	data->lvl = ps->lvl;
	data->nolfo = &ps->nolfo;
	data->nooflvl = &ps->nooflvl;
	data->stsh = &ps->stsh;
	data->lst = &ps->lst;
	data->noofLST = &ps->noofLST;
	data->liststartnos = &ps->liststartnos;
	data->listnfcs = &ps->listnfcs;
	data->finallvl = &ps->finallvl;
	data->fib = &ps->fib;
	data->dop = &ps->dop;
	data->intable = &ps->intable;
	data->cellbounds = &ps->cellbounds;
	data->nocellbounds = &ps->nocellbounds;
	data->endcell = &ps->endcell;
	data->vmerges = &ps->vmerges;
	data->norows = &ps->norows;
	data->nextpap = &ps->nextpap;
	if (charset == NULL)
		{
    	data->charset = wvAutoCharset(ps);
		charset=data->charset;
		}
	else
		data->charset = charset;
    data->props = props;

    switch (tag)
        {
        case PARABEGIN:
			{
            S16 tilfo=0;
            /* test begin */
            if (*(data->endcell))
                {
                tilfo = ((PAP *)(data->props))->ilfo;
                ((PAP *)(data->props))->ilfo=0;
                }
            /* test end */
			ppap = (PAP *)data->props;
			wvTrace(("fore back is %d %d\n",((PAP *)(data->props))->shd.icoFore,((PAP *)(data->props))->shd.icoBack));
            wvBeginPara(data);
			if (tilfo)
                ((PAP *)(data->props))->ilfo=tilfo;
            }
            break;
        case PARAEND:
			{
			S16 tilfo=0;
            /* test begin */
            if (*(data->endcell))
                {
                tilfo = ((PAP *)(data->props))->ilfo;
                ((PAP *)(data->props))->ilfo=0;
                }
            /* test end */
            wvEndCharProp(data);	/* danger will break in the future */
            wvEndPara(data);
			if (tilfo)
                ((PAP *)(data->props))->ilfo=tilfo;
			wvCopyPAP(&data->lastpap,(PAP*)(data->props));
			}
            break;
        case CHARPROPBEGIN:
            wvBeginCharProp(data,ppap);
            break;
        case CHARPROPEND:
            wvEndCharProp(data);
            break;
		case SECTIONBEGIN:
			wvBeginSection(data);
			break;
		case SECTIONEND:
			wvEndSection(data);
			break;
		case COMMENTBEGIN:
			wvBeginComment(data);
			break;
		case COMMENTEND:
			wvEndComment(data);
			break;
        default:
            break;
        }
    return(0);
    }

int mydochandler(wvParseStruct *ps,wvTag tag)
    {
	static int i;
    expand_data *data = (expand_data *)ps->userData;
    data->anSttbfAssoc = &ps->anSttbfAssoc;
	data->lfo = &ps->lfo;
	data->lfolvl = ps->lfolvl;
	data->lvl = ps->lvl;
	data->nolfo = &ps->nolfo;
	data->nooflvl = &ps->nooflvl;
	data->stsh = &ps->stsh;
	data->lst = &ps->lst;
	data->noofLST = &ps->noofLST;
	data->liststartnos = &ps->liststartnos;
	data->listnfcs = &ps->listnfcs;
	data->finallvl = &ps->finallvl;
	data->fib = &ps->fib;
	data->dop = &ps->dop;
	data->intable = &ps->intable;
	data->cellbounds = &ps->cellbounds;
	data->nocellbounds = &ps->nocellbounds;
	data->endcell = &ps->endcell;
	data->vmerges = &ps->vmerges;
	data->norows = &ps->norows;
	if (i==0)
		{
		wvSetEntityConverter(data);
		data->filename = ps->filename;
		data->whichcell=0;
		data->whichrow=0;
		data->asep = NULL;
		i++;
		wvInitPAP(&data->lastpap);
		data->nextpap=NULL;
		data->ps=ps;
		}

	if (charset == NULL)
		{
	    data->charset = wvAutoCharset(ps);
		charset = data->charset;
		}
	else
		data->charset = charset;

    switch (tag)
        {
        case DOCBEGIN:
            wvBeginDocument(data);
            break;
        case DOCEND:
            wvEndDocument(data);
            break;
        default:
            break;
        }
    return(0);
    }

void wvStrangeNoGraphicData(char *config, int graphicstype)
	{
	wvError(("Strange No Graphic Data in the 0x01/0x08 graphic\n"));

	if ( (strstr(config, "wvLaTeX.xml") != NULL) 
	  || (strstr(config, "wvCleanLaTeX.xml") != NULL) )
		printf("\n\\resizebox*{\\baselineskip}{!}{\\includegraphics{placeholder.eps}}\
 		  \n-- %#.2x graphic: StrangeNoGraphicData --", 
			graphicstype);
	else
		printf("<img alt=\"%#.2x graphic\" src=\"%s\"><br>", graphicstype,
			"StrangeNoGraphicData");
	return;
	}

void wvPrintGraphics(char *config, int graphicstype, int width, int height,
		char *source)
	{
	if ( (strstr(config, "wvLaTeX.xml") != NULL) 
	  || (strstr(config, "wvCleanLaTeX.xml") != NULL) )
	      {
		remove_suffix (source, ".wmf");
		remove_suffix (source, ".pict");
		remove_suffix (source, ".png");
		remove_suffix (source, ".jpg");
		/* 
		Output to real file name. Conversion to .eps must be done manually for now 
		*/
		printf("\n\\resizebox{%dpt}{%dpt}\
		  {\\includegraphics{%s.eps}}\
		  \n-- %#.2x graphic -- \n",
			width, height, source, graphicstype);
	      }
	else
		printf("<img width=\"%d\" height=\"%d\" alt=\"%#.2x graphic\" src=\"%s\"><br>",
		width, height, graphicstype, source);
	return;
	}

int mySpecCharProc(wvParseStruct *ps,U16 eachchar,CHP *achp)
	{
	static int message;
	PICF picf;
	FSPA *fspa;
	expand_data *data = (expand_data *)ps->userData;

	switch(eachchar)
		{
		case 19:
			wvError(("field began\n"));
			ps->fieldstate++;
			ps->fieldmiddle=0;
			fieldCharProc(ps,eachchar,0,0x400);	/* temp */
			return(0);
			break;
		case 20:
			wvTrace(("field middle\n"));
			if (achp->fOle2)
				{
				/*pps_entry *test;*/
				wvError(("this field has an associated embedded object of id %x\n",achp->fcPic_fcObj_lTagObj));
				/*test = wvFindObject(achp->fcPic_fcObj_lTagObj);
				if (test)
					wvError(("data can be found in object entry named %s\n",test->name));
				*/}
			fieldCharProc(ps,eachchar,0,0x400);	/* temp */
			ps->fieldmiddle=1;
			return(0);
			break;
		case 21:
			wvTrace(("field end\n"));
			ps->fieldstate--;
			ps->fieldmiddle=0;
			fieldCharProc(ps,eachchar,0,0x400);	/* temp */
			return(0);
			break;
		}

	if (ps->fieldstate)
		{
		if (fieldCharProc(ps,eachchar,0,0x400))
			return(0);
		}

	switch(eachchar)
		{
		case 0x05:
			/* this should be handled by the COMMENTBEGIN and COMMENTEND events */
			return(0);
			break;
		case 0x01:
			{
			wvStream *f;
			Blip blip;
			char *name;
			long p = wvStream_tell(ps->data);
			wvError(("picture 0x01 here, at offset %x in Data Stream, obj is %d, ole is %d\n",achp->fcPic_fcObj_lTagObj,achp->fObj,achp->fOle2));
			if (achp->fOle2)
				exit(139);
			wvStream_goto(ps->data,achp->fcPic_fcObj_lTagObj);
			wvGetPICF(wvQuerySupported(&ps->fib,NULL),&picf,ps->data);
			f = picf.rgb;
			if (wv0x01(&blip,f,picf.lcb-picf.cbHeader))
				{
				wvTrace(("Here\n"));
				name = wvHtmlGraphic(ps,&blip);
				wvPrintGraphics(config, 0x01, 
				(int)wvTwipsToHPixels(picf.dxaGoal),
				(int)wvTwipsToVPixels(picf.dyaGoal),
				name);
				wvFree(name);
				}
			else
				wvStrangeNoGraphicData(config, 0x01);
			wvStream_goto(ps->data,p);
			return(0);
			}
		case 0x08:
			{
			Blip blip;
			char *name;
			if (wvQuerySupported(&ps->fib,NULL) == WORD8)
				{
				if(ps->nooffspa>0) 
					{
					fspa = wvGetFSPAFromCP(ps->currentcp,ps->fspa,ps->fspapos,ps->nooffspa);

					if(!fspa)
					  {
					    wvError(("No fspa! Insanity abounds!\n"));
					    return 0;
					  }

					data->props = fspa;
					if (wv0x08(&blip,fspa->spid,ps))
						{
						wvTrace(("Here\n"));
						name = wvHtmlGraphic(ps,&blip);
						wvPrintGraphics(config, 0x08,
						(int)wvTwipsToHPixels(fspa->xaRight-fspa->xaLeft),
						(int)wvTwipsToVPixels(fspa->yaBottom-fspa->yaTop),
						name);
						wvFree(name);
						}
					else
						wvStrangeNoGraphicData(config, 0x08);
					}
				else
					{
					wvError(("nooffspa was <=0!  Ignoring.\n"));
					}
				}
			else
				{
				FDOA *fdoa;
				wvError(("pre word8 0x08 graphic, unsupported at the moment\n"));
				fdoa = wvGetFDOAFromCP(ps->currentcp,ps->fdoa,ps->fdoapos,ps->nooffdoa);
				data->props = fdoa;
				}





#if 0
			if ( (fspa) && (data->sd != NULL) && (data->sd->elements[TT_PICTURE].str) && (data->sd->elements[TT_PICTURE].str[0] != NULL) )
				{
				wvExpand(data,data->sd->elements[TT_PICTURE].str[0],
					strlen(data->sd->elements[TT_PICTURE].str[0]));
				if (data->retstring)
					{
					wvTrace(("picture string is now %s",data->retstring));
					printf("%s",data->retstring);
					wvFree(data->retstring);
					}
				}
#endif
			return(0);
			}
		case 0x28:
			{
			U16 symbol[6] = {'S','y','m','b','o','l'};
			U16 wingdings[9] = {'W','i','n','g','d','i','n','g','s'};
			wvTrace(("no of strings %d %d\n",ps->fonts.nostrings,achp->ftcSym));
			if (0 == memcmp(symbol,ps->fonts.ffn[achp->ftcSym].xszFfn,12))
				{
				if ( (!message) && (strcasecmp("UTF-8",charset)) )
					{
					wvWarning("Symbol font detected (too late sorry!), rerun wvHtml with option --charset utf-8\n\
option to support correct symbol font conversion to a viewable format.\n");
					message++;
					}
				wvTrace(("symbol char %d %x %c, using font %d %s\n",achp->xchSym,achp->xchSym,achp->xchSym,achp->ftcSym,wvWideStrToMB(ps->fonts.ffn[achp->ftcSym].xszFfn) ));
				wvTrace(("symbol char ends up as a unicode %x\n",wvConvertSymbolToUnicode(achp->xchSym-61440)));
				wvOutputFromUnicode(wvConvertSymbolToUnicode(achp->xchSym-61440),charset);
				return(0);
				}
			else if (0 == memcmp(wingdings,ps->fonts.ffn[achp->ftcSym].xszFfn,18))
				{
				if (!message)
					{
					wvError(("I have yet to do a wingdings to unicode mapping table, if you know of one tell me\n"));
					message++;
					}
				}
			else
				{
				if (!message)
					{
					char *fontname = wvWideStrToMB(ps->fonts.ffn[achp->ftcSym].xszFfn);
					wvError(("Special font %s, i need a mapping table to unicode for this\n",fontname));
					wvFree(fontname);
					printf("*");
					}
				return(0);
				}
			}
		default:
			return(0);
		}



	return(0);
	}

	
int myCharProc(wvParseStruct *ps,U16 eachchar,U8 chartype,U16 lid)
	{
	switch(eachchar)
		{
		case 19:
			wvTrace(("field began\n"));
			ps->fieldstate++;
			ps->fieldmiddle=0;
			fieldCharProc(ps,eachchar,chartype,lid);	/* temp */
			return(0);
			break;
		case 20:
			wvTrace(("field middle\n"));
			fieldCharProc(ps,eachchar,chartype,lid);
			ps->fieldmiddle=1;
			return(0);
			break;
		case 21:
			wvTrace(("field began\n"));
			ps->fieldmiddle=0;
			ps->fieldstate--;
			fieldCharProc(ps,eachchar,chartype,lid);	/* temp */
			return(0);
			break;
		case 0x08:
			wvError(("hmm did we loose the fSpec flag ?, this is possibly a bug\n"));
			break;
		}

	if (ps->fieldstate)
		{
		if (fieldCharProc(ps,eachchar,chartype,lid))
			return(0);
		}

	wvTrace(("charset is %s, lid is %x, type is %d, char is %x\n",charset,lid,chartype,eachchar));

	if ( (chartype) && (wvQuerySupported(&ps->fib,NULL) == WORD8) )
		wvTrace(("lid is %x\n",lid));
	
	if (charset != NULL)
		wvOutputHtmlChar(eachchar,chartype,charset,lid);
	else
		wvOutputHtmlChar(eachchar,chartype,wvAutoCharset(ps),lid);
	return(0);
	}


FILE *wvOpenConfig(char *config)
	{
	FILE *tmp;
	int i=0;
	if (config == NULL)
		config = "wvHtml.xml";
	else
		i=1;
    tmp = fopen(config,"rb");
	if (tmp == NULL)
		{
		if (i) wvError(("Attempt to open %s failed, using %s\n",config,HTMLCONFIG));
		config = HTMLCONFIG;
	    tmp = fopen(config,"rb");
		}
	return(tmp);
	}
