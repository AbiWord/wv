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
2 ole but not supported word doc
-1 for an error of some unknown kind
0 on success
*/

int myelehandler(wvParseStruct *ps,wvTag tag, void *props,int dirty);
int mydochandler(wvParseStruct *ps,wvTag tag);
FILE *wvOpenConfig(char *config);

void usage(void)
	{
	printf("Usage: wvConvert [--config config.xml] [--password password] filename.doc\n");
	exit(-1);
	}

int main(int argc,char **argv)
	{
	char *config=NULL;
	char *password=NULL;
	FILE *input;
	int ret;
    state_data myhandle;
	expand_data expandhandle;
	wvParseStruct ps;
	int c,index=0;
	static struct option long_options[] =
        {
        { "config",1,0,'x'},
        { "password",1,0,'p'},
        { 0,0,0,0 }
        };

	wvInitError();

	if (argc < 2) 
		usage();

	while(1)
		{
		c = getopt_long (argc, argv, "x:p:", long_options, &index);
		if (c == -1)
			break;
		switch(c)
			{
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
		wvError(("startup error with file %s\n",argv[1]));
		wvOLEFree();
		return(2);
		}
	
	wvSetElementHandler(myelehandler);
	wvSetDocumentHandler(mydochandler);
    
    wvInitStateData(&myhandle);

	myhandle.fp = wvOpenConfig(config);

	if (myhandle.fp== NULL)
		wvError(("config file not found\n"));
	else
    	ret = wvParseConfig(&myhandle);

	if (!ret)
		{
		expandhandle.sd = &myhandle;
		ps.userData = &expandhandle;
		ret = wvText(&ps);
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
	data->finallvl = &ps->finallvl;
    data->fib = &ps->fib;
	data->intable = &ps->intable;
	data->cellbounds = &ps->cellbounds;
	data->nocellbounds = &ps->nocellbounds;
	data->endcell = &ps->endcell;
	data->vmerges = &ps->vmerges;
	data->norows = &ps->norows;
	data->nextpap = &ps->nextpap;
	data->charset = wvAutoCharset(&ps->clx);
	data->props = props;

	switch (tag)
		{
		case PARABEGIN:	
			wvBeginPara(data);
			break;
		case PARAEND:
			wvEndPara(data);
			wvCopyPAP(&data->lastpap, (PAP*)(data->props));
			break;
		case SECTIONBEGIN:
			wvBeginSection(data);
			break;
		case SECTIONEND:
			wvEndSection(data);
			break;
		case CHARPROPBEGIN:
			wvBeginCharProp(data,NULL);
		    break;
		case CHARPROPEND:
			wvEndCharProp(data);
			break;
		default:
			break;
		}
	return(0);
	}

int mydochandler(wvParseStruct *ps,wvTag tag)
	{
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
	data->finallvl = &ps->finallvl;
    data->fib = &ps->fib;
	data->intable = &ps->intable;
	data->cellbounds = &ps->cellbounds;
	data->nocellbounds = &ps->nocellbounds;
	data->endcell = &ps->endcell;
	data->vmerges = &ps->vmerges;
	data->norows = &ps->norows;

	data->charset = wvAutoCharset(&ps->clx);

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

FILE *wvOpenConfig(char *config)
    {
    FILE *tmp;
    int i=0;
    if (config == NULL)
        config = "wvConfig.xml";
    else
        i=1;
    tmp = fopen(config,"rb");
    if (tmp == NULL)
        {
        if (i) wvError(("Attempt to open %s failed, using %s\n",config,HTMLCONFIG));
        config = XMLCONFIG;
        tmp = fopen(config,"rb");
        }
    return(tmp);
    }

