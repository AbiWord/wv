#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

extern FILE *erroroutput;
extern FILE *outputfile;
char *charset;
extern char *wv_version;


int do_output_start(U32 *avalrgfc,int nopieces,document_style *doc_style)
	{
	int charsetflag=0;
	int i=0;
	char *astring;

	/*
    right so this is to guess (over safely) whether to go into unicode mode or 
	not for the full html document, if i get it wrong then well ill get away 
	with it wih utf-8 encoding coz i believe as ms only uses 8 bit for west 
	european languages as far as i can tell

	If a piece's file location & 0x40000000 is true, then the chars at that 
	location are 8 bit ones, the default is that they are 16 bits ones, we 
	search to see if any piece contains 16 bit chars, if there is any, then we 
	set this html file to be in the utf-8 charset. The problem is that in 
	fastsaved mode, all the pieces are in 16bit, even if the language is ascii 
	based, so for the most part html files are output with a utf-8 heading, 
	even if it turns out afterwards that this is unneeded.
    */

	while(i<nopieces)
		{
		if (!(avalrgfc[i] & 0x40000000UL))
			charsetflag=1;
		i++;
		}

	if (charsetflag == 0)
		charset="iso-8859-1";
	else
		charset="utf-8";

	if ((doc_style == NULL) || (doc_style->begin == NULL))
		fprintf(outputfile,"\n<head>\n<META HTTP-EQUIV=\"Content-Type\" \
		CONTENT=\"text/html;charset=%s\">\n</head>\n<body>\n",charset);
	else
		{
		astring = expand_variables(doc_style->begin,NULL);
		fprintf(outputfile,"\n%s\n",astring);
		free(astring);
		}

	return(charsetflag);
	}

void do_output_end(document_style *doc_style,int core,int tail)
	{
	char *astring;

	if ((doc_style == NULL) || (doc_style->end == NULL))
		{
		if (tail)
			{
			fprintf(outputfile,"<hr><p>\nDocument converted from word 8 by \n\
			<a href=\"http://www.csn.ul.ie/~caolan/docs/MSWordView.html\">\
			MSWordView</a> (%s)<br>\n",wv_version); 
			fprintf(outputfile,"MSWordView written by <a href=\"\
			mailto:Caolan.McNamara@ul.ie\">Caolan McNamara</a>\n</body>\n");
			}
		if (core)
			fprintf(outputfile,"\n</html>\n");
		}
	else
		{
		astring = expand_variables(doc_style->end,NULL);
		fprintf(outputfile,"\n%s\n",astring);
		free(astring);
		}
	}
