#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

extern TokenTable s_Tokens[];

int (*charhandler)(wvParseStruct *ps,U16 eachchar,U8 chartype)=NULL;
int (*elehandler)(wvParseStruct *ps,wvTag tag, void *props)=NULL;
int (*dochandler)(wvParseStruct *ps,wvTag tag)=NULL;


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

int wvOutputTextChar(U16 eachchar,U8 chartype,U8 outputtype,U8 *state,wvParseStruct *ps)
	{
	if (charhandler)
		return( (*charhandler)(ps,eachchar,chartype) );

	switch(eachchar)
		{
		case 11:
			printf("\n");
			return(0);
		case 45:
		case 31:
		case 30:
			printf("-");
			return(0);
		case 160:
			printf(" ");
			return(0);
		case 12:
		case 13:
		case 14:
		case 7:
			return(0);
		case 19:	/*field begin*/
			wvTrace(("field begin\n"));
			*state=1;
			return(0);
		case 20:	/*field middle*/
			*state=0;
			return(0);
		case 21:	/*field end*/
			*state=0;
			return(0);
		}

	if (*state)	
		return(0);
	if (chartype == CP1252)
		wvOutputFromCP1252(eachchar,outputtype);
	else
		wvOutputFromUnicode(eachchar,outputtype);
	return(0);
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
	char *str;
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
	char *str;
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
		wvTrace(("doc begin is %s",data->sd->elements[TT_DOCUMENT].str[0]));
		wvExpand(data,data->sd->elements[TT_DOCUMENT].str[0],
			strlen(data->sd->elements[TT_DOCUMENT].str[0]));
		if (data->retstring)
			{
			wvTrace(("doc begin is now %s",data->retstring));
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
			wvTrace(("doc end is now %s",data->retstring));
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

int wvHandleElement(wvParseStruct *ps,wvTag tag, void *props)
	{
	if (elehandler)
		return( (*elehandler)(ps, tag, props) );
	wvError(("unimplemented tag %d\n",tag));
	return(0);
	}

int wvHandleDocument(wvParseStruct *ps,wvTag tag)
	{
	if (dochandler)
		return( (*dochandler)(ps,tag) );
	wvError(("unimplemented tag %d\n",tag));
	return(0);
	}

void wvBeginSection(expand_data *data)
	{
	if ( (data != NULL) &&  (data->sd != NULL) && (data->sd->elements[TT_SECTION].str[0]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_SECTION].str[0],
		strlen(data->sd->elements[TT_SECTION].str[0]));
		if (data->retstring)
			{
			wvTrace(("para begin is now %s",data->retstring));
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvBeginPara(expand_data *data)
	{
	if ( (data != NULL) &&  (data->sd != NULL) && (data->sd->elements[TT_PARA].str[0]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_PARA].str[0],
		strlen(data->sd->elements[TT_PARA].str[0]));
		if (data->retstring)
			{
			wvTrace(("para begin is now %s",data->retstring));
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
			wvTrace(("para end is now %s",data->retstring));
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvEndSection(expand_data *data)
	{
	if ( (data->sd != NULL) && (data->sd->elements[TT_SECTION].str[1]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_SECTION].str[1],
		strlen(data->sd->elements[TT_SECTION].str[1]));
		if (data->retstring)
			{
			wvTrace(("para end is now %s",data->retstring));
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvBeginCharProp(expand_data *data)
	{
	CHP *achp;
	wvTrace(("beginning character run\n"));

	if ( (data != NULL) &&  (data->sd != NULL) && (data->sd->elements[TT_CHAR].str[0]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_CHAR].str[0],
		strlen(data->sd->elements[TT_CHAR].str[0]));
		if (data->retstring)
			{
			wvTrace(("char begin is now %s",data->retstring));
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
   
	/* some test examples */
	achp = (CHP*)data->props;
	if (achp->fBold) { wvTrace(("a BOLD character run\n")); }
	if (achp->fItalic) { wvTrace(("an ITALIC character run\n")); }
	if (achp->kul) { wvTrace(("some kind of UNDERLINED character run\n")); }
	}

void wvEndCharProp(expand_data *data)
	{
	wvTrace(("ending character run\n"));
	if ( (data->sd != NULL) && (data->sd->elements[TT_CHAR].str[1]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_CHAR].str[1],
		strlen(data->sd->elements[TT_CHAR].str[1]));
		if (data->retstring)
			{
			wvTrace(("char end is now %s",data->retstring));
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvSetCharHandler(int (*proc)(wvParseStruct *,U16,U8))
    {
	charhandler = proc;
	}

void wvSetElementHandler(int (*proc)(wvParseStruct *,wvTag , void *))
    {
	elehandler = proc;
	}

void wvSetDocumentHandler(int (*proc)(wvParseStruct *,wvTag))
	{
	dochandler = proc;
	}
