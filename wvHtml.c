#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
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


int main(int argc,char **argv)
	{
	FILE *mainfd;
	FILE *tablefd0;
	FILE *tablefd1;
	FILE *data;
	FILE *summary;
	char *buffer;
	char fileinbuf[1024];
	FILE *filein;
	FILE *input;
	int ret;
	state_data myhandle;


	if (argc < 2) 
		{
		printf("Usage: wvHtml filename.doc\n");
		return(-1);
		}
	input = fopen(argv[1],"rb");
	ret = wvOLEDecode(input,&mainfd,&tablefd0,&tablefd1,&data,&summary);
	if (ret)
		{
		wvError("Sorry main document stream couldnt be found in doc \n%s\n",argv[1]);
		wvError("if this *is* a word 8 file, it appears to be corrupt\n");
		wvError("remember, mswordview cannot handle rtf,word 6 or word 7 etc\n");

		buffer = (char *) malloc(strlen(argv[1]) +3 + strlen("file "));
		sprintf(buffer,"file \"%s\"",argv[1]);
		filein = popen(buffer,"r");
		if (filein != NULL)
			{
			wvError("for your information, the utility \n\"file %s\" reports ...\n\n",argv[1]);
			while (fgets(fileinbuf,1024,filein) != NULL)
				wvError("%s",fileinbuf);
			}
		free(buffer);
		return(1);
		}

	if (mainfd == NULL)
        {
        wvError("No WordDocument stream found, this is not a word document\n");
        wvError("use wvSummary to try and determine the type of file\n");
        return(2);
        }

	wvInitStateData(&myhandle);
    myhandle.fp = fopen("wvHtml.xml","rb");
    if (myhandle.fp == NULL)
        wvError("config file not found\n");
    else
		{
		wvTrace("x for FILE is %x\n",myhandle.fp);
        ret = wvParseConfig(&myhandle);
		}

	if (!ret)	
		ret = wvHtml(&myhandle,mainfd,tablefd0,tablefd1,data);
	wvReleaseStateData(&myhandle);

	if (ret == 2)
		return(2);
	else if (ret != 0)
		ret = -1;
	wvOLEFree();
	return(ret);
	}
