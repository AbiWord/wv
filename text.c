#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wv.h"

extern TokenTable s_Tokens[];

int (*charhandler)(wvParseStruct *ps,U16 eachchar,U8 chartype)=NULL;
int (*scharhandler)(wvParseStruct *ps,U16 eachchar,CHP *achp)=NULL;
int (*elehandler)(wvParseStruct *ps,wvTag tag, void *props, int dirty)=NULL;
int (*dochandler)(wvParseStruct *ps,wvTag tag)=NULL;


CharsetTable c_Tokens[] =
{
    { "utf-8",UTF8 },
    { "iso-5589-15",ISO_5589_15 },
    { "koi8-r",KOI8 },
	{ "tis-620", TIS620 }
	/* add your charset here */
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

U16 wvLookupCharset(char *optarg)
	{
	unsigned int k;
    for (k=0; k<CharsetTableSize; k++)
        {
        if (0 == strcasecmp(c_Tokens[k].m_name,optarg))
            return(c_Tokens[k].m_type);
        }
	wvError(("Unrecognized charset %s, resetting to defaults\n",optarg));
    return(0xffff);
	}

int wvOutputTextChar(U16 eachchar,U8 chartype,U16 outputtype,U8 *state,wvParseStruct *ps, CHP *achp)
	{
	if (achp->fSpec)
		{
	/*
	if the character is still one of the special ones then call this other 
	handler 
	instead
	*/
		if (scharhandler)
			return( (*scharhandler)(ps,eachchar,achp) );
		}	
	else 
		{
	/* Most Chars go through this baby */
		if (charhandler)
			return( (*charhandler)(ps,eachchar,chartype) );
		}

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
		case 0x92:
			printf("'");
			return(0);
		}

	if (*state)	
		return(0);
	if (chartype)
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
		case 0x96:
		case 0x2013:
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
		case 0x85:
		case 0x2026:
#if 0
/* this just looks awful in netscape 4.5, so im going to do a very foolish
thing and just put ... instead of this
*/
			printf("&#133;");	/*is there a proper html name for ... &ellipse; ?*/
#endif
			printf("...");	
			return;
		case 0x92:
		case 0x2019:
			printf("'");
			return;
		case 0x2215:
			printf("/");
			return;
		case 0x93:
		case 0x94:
			printf("\"");
			return;
		case 0xF8E7:	/* without this, things should work in theory, but not for me */
			printf("_");
			return;
		case 0x91:
		case 0x2018:
			printf("`");
			return;
		}

	
	if (chartype)
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
		case KOI8:
			wvError(("It is my belief that there is no russian word 97 documents in 8bit mode, if I am wrong then this is my mistake, please send me this document if it is actually in russian\n"));
			break;
		case TIS620:
			wvError(("It is my belief that there is no thai word 97 documents in 8bit mode, if I am wrong then this is my mistake, please send me this document if it is actually in russian\n"));
			break;
		/*add your own charset here*/
		}
	}


void wvOutputFromUnicode(U16 eachchar,U8 outputtype)
	{
	U16 temp16;
	U8 temp8;
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
		case KOI8:
			temp16 = wvConvertUnicodeToKOI8_R(eachchar);
			if (temp16 == 0xffff)
				{
				printf("?");
				return;
				}
			temp8 = (U8)temp16;	/*whistle innocently*/
			printf("%c",temp8);
			break;
		case TIS620:
			temp16 = wvConvertUnicodeToTIS620(eachchar);
			if (temp16 == 0xffff)
				{
				printf("?");
				return;
				}
			temp8 = (U8)temp16;	/*whistle innocently*/
			printf("%c",temp8);
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
	PAP apap;
	/*
	just for html mode, as this is designed for, I always have an empty
	para end just to close off any open lists
	*/
	wvInitPAP(&apap);
	data->props=(void *)&apap;
	wvEndPara(data);

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

int wvHandleElement(wvParseStruct *ps,wvTag tag, void *props, int dirty)
	{
	if (elehandler)
		return( (*elehandler)(ps, tag, props, dirty) );
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
	if (data != NULL)
		data->asep = (SEP *)data->props;
	
	if ( (data != NULL) &&  (data->sd != NULL) && (data->sd->elements[TT_SECTION].str != NULL) && (data->sd->elements[TT_SECTION].str[0]!= NULL) )
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


int wvIsEmptyPara(PAP *apap,expand_data *data,int inc)
	{
	/* 
	if we are a end of table para then i consist of nothing that is of
	any use for beginning of a para
	*/
	if (apap == NULL)
		return(0);
	
	if (apap->fTtp == 1) 
		return(1);

	/* 
	if i consist of a vertically merged cell that is not the top one, then
	also i am of no use
	*/
	if (apap->fInTable == 1) 
		{
		wvTrace(("This Para is in cell %d %d\n",data->whichrow,data->whichcell));
		wvTrace(("%d\n",(*data->vmerges)[data->whichrow][data->whichcell]));
		if (*data->vmerges)
			if ((*data->vmerges)[data->whichrow][data->whichcell] == 0)
				{
				wvTrace(("Skipping the next paragraph\n"));
				if (inc) data->whichcell++;
				return(1);
				}
		}
	return(0);
	}
	
void wvBeginComment(expand_data *data)
	{
	if (data != NULL)
		{
		wvTrace(("comment beginning\n"));
		if ( (data->sd != NULL) && (data->sd->elements[TT_COMMENT].str) && (data->sd->elements[TT_COMMENT].str[0] != NULL) )
			{
			wvExpand(data,data->sd->elements[TT_COMMENT].str[0],strlen(data->sd->elements[TT_COMMENT].str[0]));
			if (data->retstring)
				{
				printf("%s",data->retstring);
				wvFree(data->retstring);
				}
			}
		}
	}

void wvEndComment(expand_data *data)
	{
	if ( (data->sd != NULL) && (data->sd->elements[TT_COMMENT].str) && (data->sd->elements[TT_COMMENT].str[1]!= NULL) )
		{
		wvTrace(("comment ending\n"));
		wvExpand(data,data->sd->elements[TT_COMMENT].str[1],strlen(data->sd->elements[TT_COMMENT].str[1]));
		if (data->retstring)
			{
			wvTrace(("comment end is now %s",data->retstring));
			printf("%s",data->retstring);
			wvFree(data->retstring);
			}
		}
	}

void wvBeginPara(expand_data *data)
	{
	if (wvIsEmptyPara((PAP *)data->props,data,1))
		return;

	if (data != NULL)
		{
		wvTrace(("para of style %d beginning\n",((PAP *)(data->props))->istd ));
		if ( (data->sd != NULL) && (data->sd->elements[TT_PARA].str) && (data->sd->elements[TT_PARA].str[0]!= NULL) )
			{
			wvExpand(data,data->sd->elements[TT_PARA].str[0],strlen(data->sd->elements[TT_PARA].str[0]));
			if (data->retstring)
				{
				printf("%s",data->retstring);
				wvFree(data->retstring);
				}
			}
		}
	wvTrace(("This Para is out cell %d %d \n",data->whichrow,data->whichcell));
	}

void wvEndPara(expand_data *data)
	{
	if ( (data->sd != NULL) && (data->sd->elements[TT_PARA].str) && (data->sd->elements[TT_PARA].str[1]!= NULL) )
		{
		wvExpand(data,data->sd->elements[TT_PARA].str[1],strlen(data->sd->elements[TT_PARA].str[1]));
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
	if ( (data != NULL) && (data->sd != NULL) && (data->sd->elements[TT_SECTION].str != NULL) && (data->sd->elements[TT_SECTION].str[1]!= NULL) )
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

void wvBeginCharProp(expand_data *data,PAP *apap)
	{
	CHP *achp;

	if (wvIsEmptyPara(apap,data,0))
		return;
	
	achp = (CHP*)data->props;
	wvTrace(("beginning character run\n"));
	if (achp->ico) { wvTrace(("color is %d\n",achp->ico)); }

	if ( (data != NULL) &&  (data->sd != NULL) &&  (data->sd->elements[TT_CHAR].str) && (data->sd->elements[TT_CHAR].str[0]!= NULL) )
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
	}

void wvEndCharProp(expand_data *data)
	{
	wvTrace(("ending character run\n"));
	if ( (data->sd != NULL) && (data->sd->elements[TT_CHAR].str) && (data->sd->elements[TT_CHAR].str[1]!= NULL) )
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

void wvSetSpecialCharHandler(int (*proc)(wvParseStruct *,U16,CHP *))
    {
	scharhandler = proc;
	}

void wvSetElementHandler(int (*proc)(wvParseStruct *,wvTag , void *, int))
    {
	elehandler = proc;
	}

void wvSetDocumentHandler(int (*proc)(wvParseStruct *,wvTag))
	{
	dochandler = proc;
	}
