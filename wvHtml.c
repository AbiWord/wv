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

int myelehandler(wvParseStruct *ps,wvTag tag, void *props);
int mydochandler(wvParseStruct *ps,wvTag tag);
int myCharProc(wvParseStruct *ps,U16 eachchar,U8 chartype);

void usage( void )
	{
	printf("Usage: wvHtml [--charset charset] filename.doc\n");
	exit(-1);
	}

U16 charset=0xffff;

int main(int argc,char **argv)
	{
	FILE *input;
	int ret;
	state_data myhandle;
	expand_data expandhandle;
	wvParseStruct ps;
	int c,index=0;
	static struct option long_options[] =
        {
        { "charset",1,0,'c'},
        { 0,      0, 0, '0' },
        };

	wvInitError();

	if (argc < 2) 
		usage();

	 while (1)
        { 
        c = getopt_long (argc, argv, "c:", long_options, &index);
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
			default:
                usage();
                break;
			}
		}

	input = fopen(argv[optind],"rb");

	ret = wvInitParser(&ps,input);
	if (ret)
		{
		wvError(("startup error\n"));
		return(-1);
		}

	wvSetElementHandler(myelehandler);
	wvSetDocumentHandler(mydochandler);
	wvSetCharHandler(myCharProc);

	wvInitStateData(&myhandle);
    myhandle.fp = fopen("wvHtml.xml","rb");
    if (myhandle.fp == NULL)
        wvError(("config file not found\n"));
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

int myelehandler(wvParseStruct *ps,wvTag tag, void *props)
    {
    expand_data *data = (expand_data *)ps->userData;
    data->anSttbfAssoc = &ps->anSttbfAssoc;
	data->lfo = ps->lfo;
	if (charset == 0xffff)
    	data->charset = wvAutoCharset(&ps->clx);
	else
		data->charset = charset;
    data->props = props;

    switch (tag)
        {
        case PARABEGIN:
            wvBeginPara(data);
            break;
        case PARAEND:
            wvEndCharProp(data);	/* danger will break in the future */
            wvEndPara(data);
            break;
        case CHARPROPBEGIN:
            wvBeginCharProp(data);
            break;
        case CHARPROPEND:
            wvEndCharProp(data);
            break;
		case SECTIONBEGIN:
			break;
		case SECTIONEND:
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
	data->lfo = ps->lfo;
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


int myCharProc(wvParseStruct *ps,U16 eachchar,U8 chartype)
	{
	if (charset != 0xffff)
		wvOutputHtmlChar(eachchar,chartype,charset);
	else
		wvOutputHtmlChar(eachchar,chartype,wvAutoCharset(&ps->clx));
	return(0);
	}
