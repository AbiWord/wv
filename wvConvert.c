#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "wvinternal.h"
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

int myelehandler(wvParseStruct *ps,wvTag tag,PAP *apap);
int mydochandler(wvParseStruct *ps,wvTag tag);

void Usage(void)
	{
	printf("Usage: wvConvert [-c config.xml] filename.doc\n");
	}

int main(int argc,char **argv)
	{
	FILE *input;
	int ret;
    state_data myhandle;
	expand_data expandhandle;
	wvParseStruct ps;

	if (argc < 2) 
		{
		Usage();
		return(-1);
		}
	
	input = fopen(argv[1],"rb");

	ret = wvInitParser(&ps,input);
	if (ret)
		{
		wvError("startup error with file %s\n",argv[1]);
		wvOLEFree();
		return(2);
		}
	
	wvSetElementHandler(myelehandler);
	wvSetDocumentHandler(mydochandler);
    
    wvInitStateData(&myhandle);
	myhandle.fp= fopen("wvConfig.xml","rb");
	if (myhandle.fp== NULL)
		wvError("config file not found\n");
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

int myelehandler(wvParseStruct *ps,wvTag tag,PAP *apap)
	{
	expand_data *data = (expand_data *)ps->userData;
	data->anSttbfAssoc = &ps->anSttbfAssoc;
	data->charset = wvAutoCharset(&ps->clx);
	data->apap = apap;

	switch (tag)
		{
		case PARABEGIN:	
			wvBeginPara(data);
			break;
		case PARAEND:
			wvEndPara(data);
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
