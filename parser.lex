%{
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include <string.h>
#include <ctype.h>
#include "wv.h"

int noconfig_styles=0;
extern char *charset;
extern char *title;
extern char *wv_version;

char *remove_slashes(char *in);

enum tokens {ENDOFINPUT=0, ELEMENTtoken,STYLEtoken,LEFTBRACEtoken,RIGHTBRACEtoken,
	STARTtoken,ENDtoken,PRESPACEtoken,POSTSPACEtoken,BOLDtoken,
	ITALICtoken,FONTtoken,DEFAULTtoken,IDENTIFIERtoken,DOCUMENTtoken,VTWIPStoken,HTWIPStoken};

%}
whitespace          [ \t\n]
myalpha               [[:alnum:]><{}\/\[\]=]
/*
myalpha               [[:alnum:]><{}/\[\]=]
*/
%s instring
%%
<INITIAL>\"     {  /* start of string */
         BEGIN instring;
         yymore();
        }
<instring>\\\"    {
         yymore();
		}
<instring>\"    {  /* end of string */
         BEGIN INITIAL;
		 return(IDENTIFIERtoken);
        }
<instring>.     {
         yymore();
        }
<instring>\n    {
		yymore();
        }
<INITIAL>"#".*                          ;
<INITIAL>{whitespace}+                  ;
<INITIAL>VTWIPS							return(VTWIPStoken);
<INITIAL>HTWIPS							return(HTWIPStoken);
<INITIAL>Document						return(DOCUMENTtoken);
<INITIAL>Style							return(STYLEtoken);
<INITIAL>Start							return(STARTtoken);
<INITIAL>End							return(ENDtoken);
<INITIAL>PreSpace						return(PRESPACEtoken);
<INITIAL>PostSpace						return(POSTSPACEtoken);
<INITIAL>Element						return(ELEMENTtoken);
<INITIAL>"{"                 			return(LEFTBRACEtoken);
<INITIAL>"}"                 			return(RIGHTBRACEtoken);
<INITIAL>"Bold"                 		return(BOLDtoken);
<INITIAL>"Italic"                 		return(ITALICtoken);
<INITIAL>"Font"                 		return(FONTtoken);
<INITIAL>"Default"                 		return(DEFAULTtoken);
<INITIAL>{myalpha}{myalpha}*            return(IDENTIFIERtoken);
%%

#if 0
int main(void)
	{
	FILE *in = fopen("config-mswordview","rb");
	config_style *in_style=NULL;
	config_style *ctemp;
	Parse(in,&in_style);
	ctemp = in_style;
	while(ctemp != NULL)
		{
		error(stderr,"the style name is %s, the beginning is %s, the end is %s\n",ctemp->name,ctemp->begin,ctemp->end);
		ctemp = ctemp->next;
		}
	}
#endif

int lennum(int num)
	{
	char buffer[128];
	sprintf(buffer,"%d",num);
	return(strlen(buffer));
	}

char *remove_slashes(char *in)
	{
	int i,j=0,len;
	int slash=0;
	char *temp;
	len = strlen(in);
	temp = malloc(strlen(in)+1);
	for (i=0;i<len;i++)
		{
		if (in[i] != '\\')
			{
			temp[j] = in[i];
			slash=0;
			j++;
			}
		else if (slash)
			{
			temp[j] = in[i];
			slash=0;
			j++;
			}
		else
			slash++;
		}
	temp[j] = '\0';
	strcpy(in,temp);
	free(temp);
	return(in);
	}

char *expand_element(char *in, char *fontface, char *color, char *size)
	{
	int optional=0;
	unsigned char *test,*oldtest;
	unsigned char *lb,*rb;
	char buffer[4096];
	char *variable;
	char *replacement=NULL;
	unsigned char *begin=(unsigned char *)malloc(strlen(in)+1);
	strcpy(begin,in);

	error(stderr,"incoming is %s\n",in);

	test = oldtest = strchr(begin,'$');
	lb = strchr(begin,'[');
	rb = strchr(begin,']');
	if ((lb < test) && (rb > test))
		{
		error(stderr,"1 OPTIONAL ELEMENT\n");
		optional=1;
		}
	while (test != NULL)
		{
		replacement=NULL;
		variable = buffer;
		test++;
		while(legal_variable_char(*test))
			*variable++ = *test++;
		*variable= '\0';

		ms_strlower(buffer);

		if (!(strcmp(buffer,"fontface")))
			{
			if (fontface != NULL)
				{
				replacement = (char *)malloc(
					strlen(begin)+1-strlen(buffer)+strlen(fontface));
				}
			else
				{
				replacement = (char *)malloc(
					strlen(begin)+1-strlen(buffer));
				}
			if (optional)
				{
				lb = strchr(begin,'[');
				rb = strchr(begin,']');
				strncpy(replacement,begin,lb-begin);

				if (fontface != NULL)
					{
					strncpy(replacement+(lb-begin),begin+(lb-begin)+1,oldtest-lb);
					strcpy(replacement+(oldtest-begin)-1,fontface);
					strncpy(replacement+(oldtest-begin)-1+strlen(fontface),test,rb-test);
					strcpy(replacement+(oldtest-begin)-1+strlen(fontface)+(rb-test),rb+1);
					}
				else
					strcpy(replacement+(lb-begin),rb+1);
				}
			else
				{
				strncpy(replacement,begin,oldtest-begin);
				strcpy(replacement+(oldtest-begin),fontface);
				strcpy(replacement+(oldtest-begin)+strlen(fontface),
					begin+(oldtest-begin)+(test-oldtest));
				}
			error(stderr,"this replacement is %s\n",replacement);
			}
		else if (!(strcmp(buffer,"color")))
			{
			if (color != NULL)
				{
				replacement = (char *)malloc(
					strlen(begin)+1-strlen(buffer)+strlen(color));
				}
			else
				{
				replacement = (char *)malloc(
					strlen(begin)+1-strlen(buffer));
				}
			if (optional)
				{
				lb = strchr(begin,'[');
				rb = strchr(begin,']');
				strncpy(replacement,begin,lb-begin);

				if (color != NULL)
					{
					strncpy(replacement+(lb-begin),begin+(lb-begin)+1,oldtest-lb);
					strcpy(replacement+(oldtest-begin)-1,color);
					strncpy(replacement+(oldtest-begin)-1+strlen(color),test,rb-test);
					strcpy(replacement+(oldtest-begin)-1+strlen(color)+(rb-test),rb+1);
					}
				else
					strcpy(replacement+(lb-begin),rb+1);
				}
			else
				{
				strncpy(replacement,begin,oldtest-begin);
				strcpy(replacement+(oldtest-begin),color);
				strcpy(replacement+(oldtest-begin)+strlen(color),
					begin+(oldtest-begin)+(test-oldtest));
				}
			error(stderr,"this replacement is %s\n",replacement);
			}
		else if (!(strcmp(buffer,"size")))
			{
			if (size != NULL)
				{
				replacement = (char *)malloc(
					strlen(begin)+1-strlen(buffer)+strlen(size));
				}
			else
				{
				replacement = (char *)malloc(
					strlen(begin)+1-strlen(buffer));
				}
			if (optional)
				{
				lb = strchr(begin,'[');
				rb = strchr(begin,']');
				strncpy(replacement,begin,lb-begin);

				if (size!= NULL)
					{
					strncpy(replacement+(lb-begin),begin+(lb-begin)+1,oldtest-lb);
					strcpy(replacement+(oldtest-begin)-1,size);
					strncpy(replacement+(oldtest-begin)-1+strlen(size),test,rb-test);
					strcpy(replacement+(oldtest-begin)-1+strlen(size)+(rb-test),rb+1);
					}
				else
					strcpy(replacement+(lb-begin),rb+1);
				}
			else
				{
				strncpy(replacement,begin,oldtest-begin);
				strcpy(replacement+(oldtest-begin),size);
				strcpy(replacement+(oldtest-begin)+strlen(size),
					begin+(oldtest-begin)+(test-oldtest));
				}
			error(stderr,"this replacement is %s\n",replacement);
			}
		else
			{
			fprintf(stderr,"unrecognized variable type1 $%s\n",buffer);
			begin++;
			}

		if (NULL != replacement)
			{
			free(begin);
			begin = replacement;
			}

		test = oldtest = strchr(begin,'$');
		lb = strchr(begin,'[');
		rb = strchr(begin,']');
		if ((lb < test) && (rb > test))
			error(stderr,"2 OPTIONAL ELEMENT\n");
		}
	remove_slashes(begin);
	return(begin);
	}

char *expand_variables(char *in, pap *apap)
	{
	int hack=0;
	unsigned char *test,*oldtest;
	char buffer[4096];
	char *variable;
	char *replacement=NULL,*newpos;
	unsigned char *begin=(unsigned char *)malloc(strlen(in)+1);
	strcpy(begin,in);

	newpos = begin;
	test = oldtest = strchr(newpos,'$');

	while (test != NULL)
		{
		replacement=NULL;
		variable = buffer;
		test++;
		while(legal_variable_char(*test))
			*variable++ = *test++;
		*variable= '\0';

		ms_strlower(buffer);

		if (!(strcmp(buffer,"patterndir")))
			{
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+strlen(patterndir()));
			strncpy(replacement,begin,oldtest-begin);
			strcpy(replacement+(oldtest-begin),patterndir());
			strcpy(replacement+(oldtest-begin)+strlen(patterndir()),
				begin+(oldtest-begin)+(test-oldtest));
			newpos = replacement+(oldtest-begin)+strlen(patterndir());
			error(stderr,"replacement is %s\n",replacement);
			}
		else if (!(strcmp(buffer,"pixelsafter")))
			{
			if (!apap)
				{
				fprintf(stderr,"invalid pap for variable request pixelsafter\n");
				apap = (pap *)malloc(sizeof(pap));
				apap->dyaAfter = 0;
				apap->dyaBefore = 0;
				hack=1;
				}
				
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+lennum(apap->dyaAfter/TWIRPS_PER_V_PIXEL));
			strncpy(replacement,begin,oldtest-begin);
			
			sprintf(replacement+(oldtest-begin),"%d",apap->dyaAfter/TWIRPS_PER_V_PIXEL);
			
			strcpy(replacement+(oldtest-begin)+lennum(apap->dyaAfter/TWIRPS_PER_V_PIXEL),
				begin+(oldtest-begin)+(test-oldtest));

			newpos = replacement+(oldtest-begin)+
				lennum(apap->dyaAfter/TWIRPS_PER_V_PIXEL);
			error(stderr,"replacement is %s\n",replacement);
			

			if (hack)
				free(apap);
			}
		else if (!(strcmp(buffer,"pixelsbefore")))
			{
			if (!apap)
				{
				fprintf(stderr,"invalid pap for variable request pixelsbefore\n");
				apap = (pap *)malloc(sizeof(pap));
				apap->dyaAfter = 0;
				hack=1;
				}
				
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+lennum(apap->dyaBefore/TWIRPS_PER_V_PIXEL));
			strncpy(replacement,begin,oldtest-begin);
			
			sprintf(replacement+(oldtest-begin),"%d",apap->dyaBefore/TWIRPS_PER_V_PIXEL);
			
			strcpy(replacement+(oldtest-begin)+
			lennum(apap->dyaBefore/TWIRPS_PER_V_PIXEL),
				begin+(oldtest-begin)+(test-oldtest));
			newpos = replacement+(oldtest-begin)+
				lennum(apap->dyaBefore/TWIRPS_PER_V_PIXEL);
			error(stderr,"replacement is %s\n",replacement);

			if (hack)
				free(apap);
			}
		else if (!(strcmp(buffer,"charset")))
			{
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+strlen(charset));
			strncpy(replacement,begin,oldtest-begin);

			sprintf(replacement+(oldtest-begin),"%s",charset);

			strcpy(replacement+(oldtest-begin)+strlen(charset),
			                begin+(oldtest-begin)+(test-oldtest));
			newpos = replacement+(oldtest-begin)+strlen(charset);
			error(stderr,"replacement is %s\n",replacement);
			}
		else if (!(strcmp(buffer,"title")))
			{
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+strlen(title));
			strncpy(replacement,begin,oldtest-begin);

			sprintf(replacement+(oldtest-begin),"%s",title);

			strcpy(replacement+(oldtest-begin)+strlen(title),
			                begin+(oldtest-begin)+(test-oldtest));
			newpos = replacement+(oldtest-begin)+strlen(title);
			error(stderr,"replacement is %s\n",replacement);
			}
		else if (!(strcmp(buffer,"version")))
			{
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+strlen(wv_version));
			strncpy(replacement,begin,oldtest-begin);

			sprintf(replacement+(oldtest-begin),"%s",wv_version);

			strcpy(replacement+(oldtest-begin)+strlen(wv_version),
			                begin+(oldtest-begin)+(test-oldtest));
			newpos = replacement+(oldtest-begin)+strlen(wv_version);
			error(stderr,"replacement is %s\n",replacement);
			}
		else
			{
			fprintf(stderr,"unrecognized variable type 2 $%s\n",buffer);
			begin++;
			}

		if (NULL != replacement)
			{
			free(begin);
			begin = replacement;
			}

		test = oldtest = strchr(newpos,'$');
		}
	remove_slashes(begin);
	return(begin);
	}

char *argument(void)
	{
	char *begin;
	begin = yytext;
	if (*begin == '\"')
		begin++;
	if (begin[strlen(begin)-1] == '\"')
		begin[strlen(begin)-1] = '\0';
    while ((isspace(*begin)) && ((char *)yytext+strlen(yytext)-begin > 0))
    	begin++;
	return(begin);
	}

int Parse (FILE * in,config_style **in_style,document_style **doc_style,element_style *ele_style)
	{
	config_style *current;
	config_style *temp;
	config_style *start=NULL;
	element_style *currente;
	enum tokens CurrentToken;
	char *begin="test";
	yyin = in;
	do
		{
		CurrentToken = yylex();
		switch (CurrentToken)
			{
			case DOCUMENTtoken:
				error(stderr,"document begin\n");
				*doc_style = (document_style*)malloc(sizeof(document_style));
				(*doc_style)->begin = NULL;
				(*doc_style)->vtwips = 0;
				(*doc_style)->htwips = 0;
				(*doc_style)->end = NULL;
				CurrentToken = yylex ();
				if (CurrentToken != LEFTBRACEtoken)
					error(stderr,"hmm, got it wrogn, got %s\n",yytext);
				do
					{
					CurrentToken = yylex ();
					switch(CurrentToken)
						{
						case STARTtoken:
							CurrentToken = yylex ();
							begin = argument();
							(*doc_style)->begin = (char *)malloc(strlen(begin)+1);
							strcpy((*doc_style)->begin,begin);
							error(stderr,"start string is %s\n",(*doc_style)->begin);
							break;
						case VTWIPStoken:
							CurrentToken = yylex ();
							begin = argument();
							(*doc_style)->vtwips = atoi(begin);
							error(stderr,"vtwips value is %d\n",(*doc_style)->vtwips);
							break;
						case HTWIPStoken:
							CurrentToken = yylex ();
							begin = argument();
							(*doc_style)->htwips = atoi(begin);
							error(stderr,"htwips value is %d\n",(*doc_style)->htwips);
							break;
						case ENDtoken:
							CurrentToken = yylex ();
							begin = argument();
							(*doc_style)->end = (char *)malloc(strlen(begin)+1);
							strcpy((*doc_style)->end,begin);
							error(stderr,"end string is %s\n",(*doc_style)->end);
							break;
						default:
							break;
						}
					}
				while(CurrentToken != RIGHTBRACEtoken);
				error(stderr,"document end\n");
				break;
			case STYLEtoken:
				error(stderr,"Style Token named ");
				noconfig_styles++;
				CurrentToken = yylex ();
				begin = argument();
				temp = (config_style *)malloc(sizeof(config_style));
				if (start == NULL)
					{
					start = temp;
					current = temp;
					error(stderr,"first time\n");
					}
				else
					{
					current->next = temp;
					current = current->next;
					error(stderr,"next time\n");
					}
				current->next = NULL;
				current->begin=NULL;
				current->end=NULL;
				current->prespace=NULL;
				current->postspace=NULL;
				current->Default=NULL;
				current->bold=NULL;
				current->font=NULL;
				current->italic=NULL;
				current->name = (char *)malloc(strlen(begin)+1);
				strcpy(current->name,begin);
				error(stderr,"%s\n",current->name);
				CurrentToken = yylex ();
				if (CurrentToken != LEFTBRACEtoken)
					error(stderr,"hmm, got it wrogn, got %s\n",yytext);
				do
					{
					CurrentToken = yylex ();
					switch(CurrentToken)
						{
						case STARTtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->begin = (char *)malloc(strlen(begin)+1);
							strcpy(current->begin,begin);
							error(stderr,"start string is %s\n",current->begin);
							break;
						case ENDtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->end = (char *)malloc(strlen(begin)+1);
							strcpy(current->end,begin);
							error(stderr,"end string is %s\n",current->end);
							break;
						case BOLDtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->bold = (char *)malloc(strlen(begin)+1);
							strcpy(current->bold,begin);
							error(stderr,"bold string is %s\n",current->bold);
							break;
						case ITALICtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->italic = (char *)malloc(strlen(begin)+1);
							strcpy(current->italic,begin);
							error(stderr,"italic string is %s\n",current->italic);
							break;
						case FONTtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->font = (char *)malloc(strlen(begin)+1);
							strcpy(current->font,begin);
							error(stderr,"font string is %s\n",current->font);
							break;
						case DEFAULTtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->Default = (char *)malloc(strlen(begin)+1);
							strcpy(current->Default,begin);
							error(stderr,"default string is %s\n",current->Default);
							break;
						case PRESPACEtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->prespace = (char *)malloc(strlen(begin)+1);
							strcpy(current->prespace,begin);
							error(stderr,"prespace string is %s\n",current->prespace);
							break;
						case POSTSPACEtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->postspace = (char *)malloc(strlen(begin)+1);
							strcpy(current->postspace,begin);
							error(stderr,"portspace string is %s\n",current->postspace);
							break;
						default:
							break;
						}
					}
				while(CurrentToken != RIGHTBRACEtoken);
				break;
			case ELEMENTtoken:
				error(stderr,"Element Token named ");
				CurrentToken = yylex ();
				begin = argument();
				ms_strlower(begin);
				if (!(strcmp("bold",begin)))
					currente = &ele_style[BOLD];
				else if (!(strcmp("italic",begin)))
					currente = &ele_style[ITALIC];
				else if (!(strcmp("font",begin)))
					currente = &ele_style[FONT];
				else
					{
					fprintf(stderr,"sorry no element named %s\n",begin);
					break;
					}
				error(stderr,"%s\n",begin);
				CurrentToken = yylex ();
				if (CurrentToken != LEFTBRACEtoken)
					error(stderr,"hmm, got it wrogn\n");
				do
					{
					CurrentToken = yylex ();
					error(stderr,"parsing style code %s\n",yytext);
					switch(CurrentToken)
						{
						case STARTtoken:
							CurrentToken = yylex ();
							begin = argument();
							currente->begin = (char *)malloc(strlen(begin)+1);
							strcpy(currente->begin,begin);
							error(stderr,"start string is %s\n",currente->begin);
							break;
						case ENDtoken:
							CurrentToken = yylex ();
							begin = argument();
							currente->end = (char *)malloc(strlen(begin)+1);
							strcpy(currente->end,begin);
							error(stderr,"end string is %s\n",currente->end);
							break;
						default:
							break;
						}
					}
				while(CurrentToken != RIGHTBRACEtoken);

				break;

			}
		}
	while (CurrentToken != ENDOFINPUT);
	*in_style = start;
	}

#ifdef yywrap
#undef yywrap
#endif

int yywrap ()
    {
    return (1);
    }
