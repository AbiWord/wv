#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

extern TokenTable s_Tokens[];

CharsetTable c_Tokens[] =
{
    { "utf-8",UTF8 },
    { "iso-5589-15",ISO_5589_15 },
    { "cp-1252",CP1252 }
};

const char *wvGetCharset(U16 charset)
    {
    unsigned int k;
    for (k=0; k<CharsetTableSize; k++)
        {
        if (c_Tokens[k].m_type == charset)
            return(c_Tokens[k].m_name);
        }
    return(NULL);
    }

void wvOutputTextChar(U16 eachchar,U8 chartype,U8 outputtype,U8 *state)
	{
	switch(eachchar)
		{
		case 11:
			printf("\n");
			return;
		case 45:
		case 31:
		case 30:
			printf("-");
			return;
		case 160:
			printf(" ");
			return;
		case 12:
		case 13:
		case 14:
		case 7:
			return;
		case 19:	/*field begin*/
			wvTrace("field begin\n");
			*state=1;
			return;
		case 20:	/*field middle*/
			*state=0;
			return;
		case 21:	/*field end*/
			*state=0;
			return;
		}

	if (*state)	
		return;
	if (chartype == CP1252)
		wvOutputFromCP1252(eachchar,outputtype);
	else
		wvOutputFromUnicode(eachchar,outputtype);
	}

void wvOutputHtmlChar(U16 eachchar,U8 chartype,U8 outputtype)
	{
	switch(eachchar)
		{
		case 11:
			printf("<br>");
			return;
		case 30:
		case 31:
		case 45:
			printf("-");
			return;
		case 160:
			printf("&nbsp;");
			return;
		case 12:
		case 13:
		case 14:
		case 7:
			return;
		case 34:
			printf("&quot;");
			return;
		case 38:
			printf("&amp;");
			return;
		case 60:
			printf("&lt;");
			return;
		case 62:
			printf("&gt;");
			return;
		}

	
	if (chartype == CP1252)
		{
		if (wvConvert1252ToHtml(eachchar))
			return;
		wvOutputFromCP1252(eachchar,outputtype);
		}
	else
		wvOutputFromUnicode(eachchar,outputtype);
	}

void wvOutputFromCP1252(U16 eachchar,U8 outputtype)
	{
	U16 temp16;
	U8 *str;
	switch(outputtype)
		{
		case UTF8:
			temp16 = wvConvert1252ToUnicode(eachchar);
			if (temp16 == 0xffff)
				{
				printf("?");
				return;
				}
			str = wvWideCharToMB(eachchar);
			printf("%s",str);
			wvFree(str);
			break;
		case ISO_5589_15:
			temp16 = wvConvert1252Toiso8859_15(eachchar);
			if (temp16 == 0xffff)
				{
				printf("?");
				return;
				}
			printf("%c",temp16);
			break;
		/*add your own charset here*/
		}
	}


void wvOutputFromUnicode(U16 eachchar,U8 outputtype)
	{
	U16 temp16;
	U8 *str;
	switch (outputtype)
		{
		case UTF8:
			str = wvWideCharToMB(eachchar);
			printf("%s",str);
			wvFree(str);
			break;
		case ISO_5589_15:
			temp16 = wvConvertUnicodeToiso8859_15(eachchar);
			if (temp16 == 0xffff)
				{
				printf("?");
				return;
				}
			printf("%c",temp16);
			break;
		/*add your own charset here*/
		}
	}


void wvBeginDocument(expand_data *data)
	{
	if ( (data->sd != NULL) && (data->sd->elements[TT_DOCUMENT].str[0] != NULL) )
		{
		wvTrace("doc begin is %s",data->sd->elements[TT_DOCUMENT].str[0]);
		wvExpand(data,data->sd->elements[TT_DOCUMENT].str[0],
			strlen(data->sd->elements[TT_DOCUMENT].str[0]));
		if (data->retstring)
			{
			wvTrace("doc begin is now %s",data->retstring);
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvEndDocument(expand_data *data)
	{
	if ( (data->sd!= NULL) && (data->sd->elements[TT_DOCUMENT].str[1]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_DOCUMENT].str[1],
		strlen(data->sd->elements[TT_DOCUMENT].str[1]));
		if (data->retstring)
			{
			wvTrace("doc end is now %s",data->retstring);
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvBeginPara(expand_data *data)
	{
	if ( (data->sd != NULL) && (data->sd->elements[TT_PARA].str[0]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_PARA].str[0],
		strlen(data->sd->elements[TT_PARA].str[0]));
		if (data->retstring)
			{
			wvTrace("para begin is now %s",data->retstring);
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvEndPara(expand_data *data)
	{
	if ( (data->sd != NULL) && (data->sd->elements[TT_PARA].str[1]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_PARA].str[1],
		strlen(data->sd->elements[TT_PARA].str[1]));
		if (data->retstring)
			{
			wvTrace("para end is now %s",data->retstring);
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}


/*

void wvSetCharHandler()
    {
	}


void wvPrint(char *fmt, ...)
	{
	char buffer[5];
	if (magic == NULL)
		wvRealPrint(fmt,...);
	else
		{
		sprintf(buffer,fmt,...);
		magic(buffer);
		}
	}

void wvRealPrint(char *fmt, ...)
    {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(wvwarn, fmt, argp);
    va_end(argp);
    }

*/
