%{
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include <string.h>
#include "mswordview.h"

int noconfig_styles=0;

enum tokens {ENDOFINPUT=0, ELEMENTtoken,STYLEtoken,LEFTBRACEtoken,RIGHTBRACEtoken,
	STARTtoken,ENDtoken,PRESPACEtoken,POSTSPACEtoken,BOLDtoken,
	ITALICtoken,FONTtoken,DEFAULTtoken,IDENTIFIERtoken};

%}
whitespace          [ \t\n]
alpha               [[:alnum:]><{}/]
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
         printf("Error, new line in string\n");
         BEGIN INITIAL;
        }
<INITIAL>"#".*                          ;
<INITIAL>{whitespace}+                  ;
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
<INITIAL>{alpha}{alpha}*             	return(IDENTIFIERtoken);
%%

int main(void)
	{
	FILE *in = fopen("testconfig","rb");
	config_style *in_style=NULL;
	config_style *ctemp;
	Parse(in,&in_style);
	ctemp = in_style;
	while(ctemp != NULL)
		{
		fprintf(stderr,"the style name is %s, the beginning is %s, the end is %s\n",ctemp->name,ctemp->begin,ctemp->end);
		ctemp = ctemp->next;
		}
	}

int lennum(int num)
	{
	char buffer[128];
	sprintf(buffer,"%d",num);
	return(strlen(buffer));
	}

char *expand_variables(char *in, pap *apap)
	{
	int hack=0;
	char *test,*oldtest,*temp;
	char buffer[4096];
	char *variable;
	char *replacement=NULL;
	char *begin=(char *)malloc(strlen(in)+1);
	strcpy(begin,in);

	test = oldtest = strchr(begin,'$');

	while (test != NULL)
		{
		replacement=NULL;
		variable = buffer;
		test++;
		while(legal_variable_char(*test))
			*variable++ = *test++;
		*variable= '\0';

		if (!(strcmp(buffer,"patterndir")))
			{
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+strlen(patterndir()));
			strncpy(replacement,begin,oldtest-begin);
			strcpy(replacement+(oldtest-begin),patterndir());
			strcpy(replacement+(oldtest-begin)+strlen(patterndir()),
				begin+(oldtest-begin)+(test-oldtest));
			fprintf(stderr,"replacement is %s\n",replacement);
			}
		else if (!(strcmp(buffer,"pixelsafter")))
			{
			if (!apap)
				{
				fprintf(stderr,"invalid pap for variable request pixelsafter\n");
				apap = (pap *)malloc(sizeof(pap));
				apap->dyaAfter = 0;
				apap->dyaBefore = 0;
				}
				
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+lennum(apap->dyaAfter/TWIRPS_PER_V_PIXEL));
			strncpy(replacement,begin,oldtest-begin);
			
			sprintf(replacement+(oldtest-begin),"%d",apap->dyaAfter/TWIRPS_PER_V_PIXEL);
			
			strcpy(replacement+(oldtest-begin)+lennum(apap->dyaAfter/TWIRPS_PER_V_PIXEL),
				begin+(oldtest-begin)+(test-oldtest));
			fprintf(stderr,"replacement is %s\n",replacement);

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
				}
				
			replacement = (char *)malloc(
				strlen(begin)+1-strlen(buffer)+lennum(apap->dyaBefore/TWIRPS_PER_V_PIXEL));
			strncpy(replacement,begin,oldtest-begin);
			
			sprintf(replacement+(oldtest-begin),"%d",apap->dyaBefore/TWIRPS_PER_V_PIXEL);
			
			strcpy(replacement+(oldtest-begin)+lennum(apap->dyaBefore/TWIRPS_PER_V_PIXEL),
				begin+(oldtest-begin)+(test-oldtest));
			fprintf(stderr,"replacement is %s\n",replacement);

			if (hack)
				free(apap);
			}
		else
			fprintf(stderr,"unrecognized variable $%s\n",buffer);

		if (NULL != replacement)
			{
			free(begin);
			begin = replacement;
			}

		test = oldtest = strchr(begin,'$');
		}
	return(begin);
	}

char *argument(void)
	{
	char *test;
	char *begin;
	int i;
	begin = yytext;
	if (*begin == '\"')
		begin++;
	if (begin[strlen(begin)-1] == '\"')
		begin[strlen(begin)-1] = '\0';
    while ((isspace(*begin)) && (yytext+strlen(yytext)-begin > 0))
    	begin++;
    for (i=0;i<strlen(begin);i++)
    	begin[i] = tolower(begin[i]);
	return(begin);
	}

int Parse (FILE * in,config_style **in_style)
	{
	config_style *current;
	config_style *temp;
	config_style *start=NULL;
	enum tokens CurrentToken;
	char *begin="test";
	int i;
	yyin = in;
	do
		{
		CurrentToken = yylex();
		switch (CurrentToken)
			{
			case STYLEtoken:
				fprintf(stderr,"Style Token named ");
				noconfig_styles++;
				CurrentToken = yylex ();
				begin = argument();
				temp = (config_style *)malloc(sizeof(config_style));
				if (start == NULL)
					{
					start = temp;
					current = temp;
					fprintf(stderr,"first time\n");
					}
				else
					{
					current->next = temp;
					current = current->next;
					fprintf(stderr,"next time\n");
					}
				current->next = NULL;
				current->begin=NULL;
				current->end=NULL;
				current->Default=NULL;
				current->bold=NULL;
				current->font=NULL;
				current->italic=NULL;
				current->name = (char *)malloc(strlen(begin)+1);
				strcpy(current->name,begin);
				fprintf(stderr,"%s\n",current->name);
				CurrentToken = yylex ();
				if (CurrentToken != LEFTBRACEtoken)
					fprintf(stderr,"hmm, got it wrogn, got %s\n",yytext);
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
							fprintf(stderr,"start string is %s\n",current->begin);
							break;
						case ENDtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->end = (char *)malloc(strlen(begin)+1);
							strcpy(current->end,begin);
							fprintf(stderr,"end string is %s\n",current->end);
							break;
						case BOLDtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->bold = (char *)malloc(strlen(begin)+1);
							strcpy(current->bold,begin);
							fprintf(stderr,"bold string is %s\n",current->bold);
							break;
						case ITALICtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->italic = (char *)malloc(strlen(begin)+1);
							strcpy(current->italic,begin);
							fprintf(stderr,"italic string is %s\n",current->italic);
							break;
						case FONTtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->font = (char *)malloc(strlen(begin)+1);
							strcpy(current->font,begin);
							fprintf(stderr,"font string is %s\n",current->font);
							break;
						case DEFAULTtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->Default = (char *)malloc(strlen(begin)+1);
							strcpy(current->Default,begin);
							fprintf(stderr,"default string is %s\n",current->Default);
							break;
						case PRESPACEtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->prespace = (char *)malloc(strlen(begin)+1);
							strcpy(current->prespace,begin);
							fprintf(stderr,"prespace string is %s\n",current->prespace);
							break;
						case POSTSPACEtoken:
							CurrentToken = yylex ();
							begin = argument();
							current->postspace = (char *)malloc(strlen(begin)+1);
							strcpy(current->postspace,begin);
							fprintf(stderr,"portspace string is %s\n",current->postspace);
							break;
						default:
							break;
						}
					}
				while(CurrentToken != RIGHTBRACEtoken);
				break;
			case ELEMENTtoken:
				fprintf(stderr,"Element Token named ");
				CurrentToken = yylex ();
				begin = argument();
				fprintf(stderr,"%s\n",begin);
				CurrentToken = yylex ();
				if (CurrentToken != LEFTBRACEtoken)
					fprintf(stderr,"hmm, got it wrogn\n");
				do
					{
					CurrentToken = yylex ();
					fprintf(stderr,"parsing style code %s\n",yytext);
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
