#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "wvinternal.h"
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
returns 1 for not an ole doc
2 ole but not word doc
-1 for an error of some unknown kind
0 on success
*/

void Usage(void)
	{
	printf("Usage: wvConvert [-c config.xml] filename.doc\n");
	}

int main(int argc,char **argv)
	{
	char *buffer;
	char fileinbuf[1024];
	FILE *filein;
	FILE *input;
	int ret;
    state_data myhandle;
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
		wvError("startup error\n");
		return(-1);
		}
    
    wvInitStateData(&myhandle);
	myhandle.fp= fopen("wvConfig.xml","rb");
	if (myhandle.fp== NULL)
		wvError("config file not found\n");
	else
    	ret = wvParseConfig(&myhandle);
	
	if (!ret)
		ret = wvText(&myhandle,&ps);

    wvReleaseStateData(&myhandle);
	if (ret == 2)
		return(2);
	else if (ret != 0)
		ret = -1;
	wvOLEFree();
	return(ret);
	}
