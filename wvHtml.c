#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "wv.h"
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

int myelehandler(wvParseStruct *ps,wvTag tag, void *props, int dirty);
int mydochandler(wvParseStruct *ps,wvTag tag);
int myCharProc(wvParseStruct *ps,U16 eachchar,U8 chartype);
int mySpecCharProc(wvParseStruct *ps,U16 eachchar,CHP *achp);

FILE *wvOpenConfig(char *config);

void usage( void )
	{
	printf("Usage: wvHtml [--config config.xml] [--charset charset] [--password password] filename.doc\n");
	exit(-1);
	}

static U16 charset=0xffff;

int main(int argc,char **argv)
	{
	FILE *input;
	char *password=NULL;
	char *config=NULL;
	int ret;
	state_data myhandle;
	expand_data expandhandle;
	wvParseStruct ps;
	int c,index=0;
	static struct option long_options[] =
        {
        { "charset",1,0,'c'},
        { "config",1,0,'x'},
        { "password",1,0,'p'},
        { 0,      0, 0, '0' },
        };

	wvInitError();

	if (argc < 2) 
		usage();

	 while (1)
        { 
        c = getopt_long (argc, argv, "c:x:p:", long_options, &index);
        if (c == -1)
            break;
		switch(c)
			{
			case 'c':
				if (optarg)
					charset = wvLookupCharset(optarg);
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
			default:
                usage();
                break;
			}
		}

	input = fopen(argv[optind],"rb");

	ret = wvInitParser(&ps,input);

	if (ret == 4)
		{
		ret = 0;
		if (password == NULL)
			{
			wvError(("Password required, this is an encrypted document\n"));
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
	else if (ret == 7)
		{
		ret=0;
		if (password == NULL)
			{
			wvError(("Password required, this is an encrypted document\n"));
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
	data->lst = ps->lst;
	data->noofLST = &ps->noofLST;
	data->liststartnos = &ps->liststartnos;
	data->finallvl = &ps->finallvl;
	data->fib = &ps->fib;
	data->intable = &ps->intable;
	data->cellbounds = &ps->cellbounds;
	data->endcell = &ps->endcell;
	data->vmerges = &ps->vmerges;
	data->norows = &ps->norows;



	if (charset == 0xffff)
    	data->charset = wvAutoCharset(&ps->clx);
	else
		data->charset = charset;
    data->props = props;

    switch (tag)
        {
        case PARABEGIN:
			if (dirty) wvTrace(("unclean para\n"));
			ppap = (PAP *)data->props;
            wvBeginPara(data);
            break;
        case PARAEND:
            wvEndCharProp(data);	/* danger will break in the future */
            wvEndPara(data);
            break;
        case CHARPROPBEGIN:
			if (dirty) wvTrace(("unclean char\n"));
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
	data->lst = ps->lst;
	data->noofLST = &ps->noofLST;
	data->liststartnos = &ps->liststartnos;
	data->finallvl = &ps->finallvl;
	data->fib = &ps->fib;
	data->intable = &ps->intable;
	data->cellbounds = &ps->cellbounds;
	data->endcell = &ps->endcell;
	data->vmerges = &ps->vmerges;
	data->norows = &ps->norows;
	if (i==0)
		{
		data->whichcell=0;
		data->whichrow=0;
		data->asep = NULL;
		i++;
		}

	if (charset == 0xffff)
	    data->charset = wvAutoCharset(&ps->clx);
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


int mySpecCharProc(wvParseStruct *ps,U16 eachchar,CHP *achp)
	{
	wvTrace(("special char %d in stream\n",eachchar));
	switch(eachchar)
		{
		case 5:
			/* this should be handled by the COMMENTBEGIN and COMMENTEND events */
			return(0);
			break;
		case 1:
			wvError(("picture here\n"));
			printf(" [there should be a picture here] ");
			return(0);
		}
	return(0);
	}

	
int myCharProc(wvParseStruct *ps,U16 eachchar,U8 chartype)
	{
	static int state,i;
	switch(eachchar)
		{
		case 19:
			state=1;
			i=0;
			return(0);
			break;
		case 20:
		case 21:
			state=0;
			return(0);
			break;
		}
	if (state) 
		{
		/*fieldstring[i++] = eachchar;*/
		fieldCharProc(ps,eachchar,chartype);
		return(0);
		}
	if (charset != 0xffff)
		wvOutputHtmlChar(eachchar,chartype,charset);
	else
		wvOutputHtmlChar(eachchar,chartype,wvAutoCharset(&ps->clx));
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
