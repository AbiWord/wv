#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "wv.h"

static TokenTable s_Tokens[] =
{
	{"TIME", FC_TIME},
	{"\\@", FC_DateTimePicture}
};

static unsigned int s_mapNameToToken(const char* name)
    {
    unsigned int k;
    for (k=0; k<FieldCodeTableSize; k++)
        {
        if (s_Tokens[k].m_name[0] == '*')
            return k;
        else if (!(strcasecmp(s_Tokens[k].m_name,name)))
            return k;
        }
    return 0;
    }


int lookahead(char *token,char test1, char test2)
	{
	int ret=0;
	while ((*token == test1) || (*token == test2))
		{
		token++;
		ret++;
		}
	return(ret);	
	}

int wvHandleDateTimePicture(char *retstring,size_t max,char *token,time_t *mytime)
	{
	int no;
	struct tm *current;
	char timestr[4096];
	char temp[64];
	timestr[0] = '\0';

	fprintf(stderr,"token is %s\n",token);
	if (!token) return;
	while(*token)
		{
		switch(*token)
			{
			case 'A':
				if ((strlen(token) > 5) && (0 == strncmp(token,"AM/PM",5)))
					{
					strcat(timestr,"%p");
					token+=5;
					}
				break;
			case 'a':
				if ((strlen(token) > 5) && (0 == strncmp(token,"AM/PM",5)))
					{
					strcat(timestr,"%P");
					token+=5;
					}
				break;
			case 'M':
				no = lookahead(token,'M','M');
				token+=(no-1);
				switch(no)
					{
					case 1:
						sprintf(temp,"%d",current->tm_mon);
						strcat(timestr,temp);
						break;
					case 2:
						strcat(timestr,"%m");
						break;
					case 3:
						strcat(timestr,"%b");
						break;
					default:
						strcat(timestr,"%B");
						break;
					}
				break;
			case 's':
			case 'S':
				no = lookahead(token,'s','S');
				token+=(no-1);
				switch(no)
					{
					case 1:
					default:
						strcat(timestr,"%S");
						break;
					}
				break;
			case 'd':
			case 'D':
				no = lookahead(token,'d','D');
				token+=(no-1);
				switch(no)
					{
					case 1:
						sprintf(temp,"%d",current->tm_wday);
						strcat(timestr,temp);
						break;
					case 2:
						strcat(timestr,"%d");
						break;
					case 3:
						strcat(timestr,"%a");
						break;
					default:
						strcat(timestr,"%A");
						break;
					}
				break;
			case 'y':
			case 'Y':
				no = lookahead(token,'y','Y');
				token+=(no-1);
				switch(no)
					{
					case 2:
						strcat(timestr,"%y");
						break;
					default:
						strcat(timestr,"%Y");
						break;
					}
				break;
			case 'h':
				no = lookahead(token,'h','h');
				token+=(no-1);
				switch(no)
					{
					case 1:
						sprintf(temp,"%d",current->tm_hour%12);
						strcat(timestr,temp);
						break;
					default:
						strcat(timestr,"%I");
						break;
					}
				break;
			case 'H':
				no = lookahead(token,'H','H');
				token+=(no-1);
				switch(no)
					{
					case 1:
						sprintf(temp,"%d",current->tm_hour);
						strcat(timestr,temp);
						break;
					default:
						strcat(timestr,"%H");
						break;
					}
				break;
			case 'm':
				no = lookahead(token,'m','m');
				token+=(no-1);
				switch(no)
					{
					case 1:
						sprintf(temp,"%d",current->tm_min);
						strcat(timestr,temp);
						break;
					default:
						strcat(timestr,"%M");
						break;
					}
				break;
			case '\"':
				break;
			case '`':
				break;
			default:
				temp[0] = *token;
				strcat(timestr,temp);
				break;
			}
		token++;
		}
	current = localtime(mytime);
	return(strftime(retstring,max,timestr,current));
	}

	
void wvHandleField(char *command,char *argument)
	{
	int a=0;
	unsigned int tokenIndex;
	char *token;
	char datestr[4096];
	time_t mytime;
	
	if (*command!= 0x13)
		{
		wvError(("field did not begin with 0x13\n"));
		return;
		}
	strtok(command,"\t, ");
	while(token = strtok(NULL,"\t, "))
		{
		tokenIndex = s_mapNameToToken(token);
		switch (s_Tokens[tokenIndex].m_type)
			{
			case FC_TIME:
				wvError(("time token\n"));
				time(&mytime);
				break;
			case FC_DateTimePicture:
				wvError(("DateTimePicture\n"));
				token = strtok(NULL,"\"\"");
				if (wvHandleDateTimePicture(datestr,4096,token,&mytime))
					printf("%s",datestr);
				else
					wvError(("date and time field function returned nothing\n"));
				break;
			default:
				break;
			}
		}
	
	if (token = strtok(argument,"\x14,\x15"))
		fprintf(stderr,"argument would have been %s\n",token);
	}

int fieldCharProc(wvParseStruct *ps,U16 eachchar,U8 chartype)
	{
	static char command[4096];
	static char argument[4096];
	static char *which;
	static int i;

	if (eachchar == 0x13)
		{
		which = command;
		command[0] = '\0';
		argument[0] = '\0';
		i=0;
		}
	else if (eachchar == 0x14)
		{
		command[i] = '\0';
		which = argument;
		i=0;
		}
	which[i] = eachchar;
	i++;
	if (eachchar == 0x15)
		{
		which[i] = '\0';
		fprintf(stderr,"field str is %s\n",argument);
		fprintf(stderr,"command str is %s\n",command);
		wvHandleField(command,argument);
		}
	return(0);
	}












































#if 0
void decode_field(FILE *main,field_info *magic_fields,long *cp,U8 *fieldwas,unsigned long *swallowcp1,unsigned long *swallowcp2)
	{
	time_t timep;
	struct tm *times;
	char date[1024];
	/*
	called at the start of a field, find the field associated
	with that cp and decode meaning, process to end of field
	*/

	/*wont deal with enbedded fields yet\n*/
	int i=0;

	*swallowcp1 = -1;
	*swallowcp2 = -1;

	error(erroroutput,"decoding field with cp %x\n",*cp);

	if (magic_fields != NULL)
		{
		while(i<magic_fields->no+1)
			{
			error(erroroutput,"field entry says %d (%x)\n",i,magic_fields->cps[i]);
			if (magic_fields->cps[i] == *cp)
				{
				error(erroroutput,"found field entry at %d\n",i);
				error(erroroutput,"type is %d \n",magic_fields->flds[i].var1.ch);
				break;
				}
			i++;
			}

		switch(magic_fields->flds[i].var1.ch)
			{
			case 19:
				/*beggining a field*/
				*fieldwas = magic_fields->flds[i].var1.flt;
				switch (*fieldwas)
					{
					case 10:	/*style reference*/
					case 12: /*sequence mark, make a hyperlink later on*/
					case 68:
						break;
					case 88:
						error(erroroutput,"HYPERLINK\n");
						/*
						fprintf(outputfile,"<a href=\"");
						*/
						fprintf(outputfile,"<a href=");
						*swallowcp1 = magic_fields->cps[i];	/*beginning of field*/
						*swallowcp2 = magic_fields->cps[i+1]-1; /*end of the first part*/
						break;
					case 33:
						if ((currentsep != NULL) && (inaheaderfooter))
							{
							error(erroroutput,"soing nfc\n");
							error(erroroutput,"nfcPgn is %d\n",currentsep->nfcPgn);
							decode_list_nfc(sectionpagenumber,currentsep->nfcPgn);
							}
						else 
							fprintf(outputfile,"%d",sectionpagenumber);
						error(erroroutput,"sectionpagenumber is now %d, inaheaderfooter is %d\n",sectionpagenumber,inaheaderfooter);
						break;
					case 65:
						fprintf(outputfile,"%d",sectionno);
						error(erroroutput,"sectionno is now %d\n",sectionno);
						break;
					case 26:
						fprintf(outputfile,"unknown # of pages");
						break;
					case 31:
						timep = time(NULL);
						times = localtime(&timep);
						strftime(date,1024,"%x",times);
						error(erroroutput,"output date as %s\n",date);
						fprintf(outputfile,"%s",date);
						break;
					case 32:
						timep = time(NULL);
						times = localtime(&timep);
						strftime(date,1024,"%X",times);
						error(erroroutput,"output time as %s\n",date);
						fprintf(outputfile,"%s",date);
						break;
					case 29:
						error(erroroutput,"spitting out original filename\n");
						break;
					case 13:
						error(erroroutput,"spitting out toc\n");
						break;
					case 37:
						*swallowcp1 = magic_fields->cps[i];	/*beginning of field*/
						*swallowcp2 = magic_fields->cps[i+1]-1; /*end of the first part*/
					default:
						error(erroroutput,"unsupported field %d\n",*fieldwas);
						break;
					}
				break;
			case 20:
				switch (*fieldwas)
					{
					case 68:
					case 65:
					case 33:
					case 26:
					case 10:
					case 12:
						break;
					case 88:
						error(erroroutput,"HYPERLINK middle\n");
						/*
						fprintf(outputfile,"\">");
						*/
						fprintf(outputfile,">");
						break;
					case 29:
						error(erroroutput,"filename middle\n");
						break;
					case 13:
						error(erroroutput,"toc middle\n");
						break;
					default:
						error(erroroutput,"unsupported field %d\n",*fieldwas);
						break;
					}
				error(erroroutput,"field middle reached\n");
				break;
			case 21:
				switch (*fieldwas)
					{
					case 10:
					case 12:
					case 68:
					case 65:
					case 33:
					case 26:
					case 37:
						break;
					case 88:
						error(erroroutput,"HYPERLINK end\n");
						fprintf(outputfile,"</a>");
						break;
					default:
						error(erroroutput,"unsupported field %d\n",*fieldwas);
						break;
					}
				error(erroroutput,"field end reached\n");
				break;
			default:
				error(erroroutput,"field wierdness!!\n");
				break;
			}
		}
	}
#endif
