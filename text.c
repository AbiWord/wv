#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "iconv.h"
#include "wv.h"

extern TokenTable s_Tokens[];

int (*charhandler)(wvParseStruct *ps,U16 eachchar,U8 chartype,U16 lid)=NULL;
int (*scharhandler)(wvParseStruct *ps,U16 eachchar,CHP *achp)=NULL;
int (*elehandler)(wvParseStruct *ps,wvTag tag, void *props, int dirty)=NULL;
int (*dochandler)(wvParseStruct *ps,wvTag tag)=NULL;

int wvOutputTextChar(U16 eachchar,U8 chartype,U8 *state,wvParseStruct *ps, CHP *achp)
	{
	U16 lid;
	/* testing adding a language */
    lid = achp->lidDefault;
	if (lid == 0x400)
		lid = ps->fib.lid;
	/* end testing adding a language */

	
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
			return( (*charhandler)(ps,eachchar,chartype,lid) );
		}
	wvError(("No CharHandler registered, programmer error\n"));
	return(0);
	}

void wvOutputHtmlChar(U16 eachchar,U8 chartype,char *outputtype,U16 lid)
	{
	if (chartype)
		wvHandleCodePage(eachchar,outputtype,lid);
	else
		wvOutputFromUnicode(eachchar,outputtype);
	}


char *wvLIDToCodePageConverter(U16 lid)
	{
	switch(lid)
		{
		case 0x0401:	/*Arabic*/
			return("CP1256");
		case 0x0402:	/*Bulgarian*/
			return("CP1251");
		case 0x0403:	/*Catalan*/
			return("CP1252");
		case 0x0404:	/*Traditional Chinese*/
			return("CP950");
		case 0x0804:	/*Simplified Chinese*/
			return("CP936");
		case 0x0405:	/*Czech*/
			return("CP1250");
		case 0x0406:	/*Danish*/
			return("CP1252");
		case 0x0407:	/*German*/
			return("CP1252");
		case 0x0807:	/*Swiss German*/
			return("CP1252");
		case 0x0408:	/*Greek*/
			return("CP1253");
		case 0x0409:	/*U.S. English*/
			return("CP1252");
		case 0x0809:	/*U.K. English*/
			return("CP1252");
		case 0x0c09:	/*Australian English*/
			return("CP1252");
		case 0x040a:	/*Castilian Spanish*/
			return("CP1252");
		case 0x080a:	/*Mexican Spanish*/
			return("CP1252");
		case 0x040b:	/*Finnish*/
			return("CP1252");
		case 0x040c:	/*French*/
			return("CP1252");
		case 0x080c:	/*Belgian French*/
			return("CP1252");
		case 0x0c0c:	/*Canadian French*/
			return("CP1252");
		case 0x100c:	/*Swiss French*/
			return("CP1252");
		case 0x040d:	/*Hebrew*/
			return("CP1255");
		case 0x040e:	/*Hungarian*/
			return("CP1250");
		case 0x040f:	/*Icelandic*/
			return("CP1252");
		case 0x0410:	/*Italian*/
			return("CP1252");
		case 0x0810:	/*Swiss Italian*/
			return("CP1252");
		case 0x0411:	/*Japanese*/
			return("CP932");
		case 0x0412:	/*Korean*/
			return("CP949");
		case 0x0413:	/*Dutch*/
			return("CP1252");
		case 0x0813:	/*Belgian Dutch*/
			return("CP1252");
		case 0x0414:	/*Norwegian - Bokmal*/
			return("CP1252");
		case 0x0814:	/*Norwegian - Nynorsk*/
			return("CP1252");
		case 0x0415:	/*Polish*/
			return("CP1250");
		case 0x0416:	/*Brazilian Portuguese*/
			return("CP1252");
		case 0x0816:	/*Portuguese*/
			return("CP1252");
		case 0x0417:	/*Rhaeto-Romanic*/
			return("CP1252");	/* ? */
		case 0x0418:	/*Romanian*/
			return("CP1250");
		case 0x0419:	/*Russian*/
			return("CP1251");
		case 0x041a:	/*Croato-Serbian*/
			return("CP1250");	/* ? */
		case 0x081a:	/*(Latin)*/
			return("CP1252");
		case 0x041b:	/*Serbo-Croatian*/
			return("CP1250");	/* ? */
		case 0x041c:	/*(Cyrillic)*/
			return("CP1251");
		case 0x041d:	/*Slovak*/
			return("CP1250");
		case 0x041e:	/*Albanian*/
			return("CP1250");
		case 0x041f:	/*Swedish*/
			return("CP1252");
		case 0x0420:	/*Thai*/
			return("CP874");
		case 0x0421:	/*Turkish*/
			return("CP1254");
		case 0x0422:	/*Urdu*/
			return("CP1256");
		case 0x0423:	/*Bahasa*/
			return("CP1256");
		case 0x0424:	/*Ukrainian*/
			return("CP1251");
		case 0x0425:	/*Byelorussian*/
			return("CP1251");
		case 0x0426:	/*Slovenian*/
			return("CP1250");
		case 0x0427:	/*Estonian*/
			return("CP1257");
		case 0x0429:	/*Latvian*/
			return("CP1257");
		case 0x042D:	/*Lithuanian*/
			return("CP1257");
		case 0x042F:	/*Farsi*/
			return("CP1256");
		case 0x0436:	/*Basque*/
			return("CP1252");
		case 0x043E:	/*Macedonian*/
			return("CP1251");
		};
	return("CP1252");
	}

void wvHandleCodePage(U16 eachchar,char *outputtype,U16 lid)
	{
	char f_code[33];            /* From CCSID                           */
	char t_code[33];            /* To CCSID                             */
	iconv_t iconv_handle;       /* Conversion Descriptor returned       */
								/* from iconv_open() function           */
	char *obuf;                 /* Buffer for converted characters      */
	char *p;
	size_t ibuflen;               /* Length of input buffer               */
	size_t obuflen;               /* Length of output buffer              */
	const char *ibuf;
	char *codepage;
	char buffer[1];
	char buffer2[2];

	buffer[0]=eachchar;
	ibuf = buffer;
	obuf = buffer2;

	codepage = wvLIDToCodePageConverter(lid);
	if (!(strcasecmp(codepage,"CP1252")))
		{
		if (wvConvert1252ToHtml(eachchar))
			return;
		}

	/* All reserved positions of from code (last 12 characters) and to code   */
	/* (last 19 characters) must be set to hexadecimal zeros.                 */

	memset(f_code,'\0',33);
	memset(t_code,'\0',33);

	strcpy(f_code,codepage);
	strcpy(t_code,"UCS-2");

	iconv_handle = iconv_open(t_code,f_code);
	if (iconv_handle == (iconv_t)-1)
		{
		wvError(("iconv_open fail: %d, cannot convert %s to unicode\n",errno,codepage));
		printf("?");
		return;
		}

	ibuflen = 1;
    obuflen = 2;
	p = obuf;
    iconv(iconv_handle, &ibuf, &ibuflen, &obuf, &obuflen);
    eachchar = (U8)*p++;
    eachchar = (eachchar << 8)&0xFF00;
    eachchar += (U8)*p;

	iconv_close(iconv_handle);
	wvOutputFromUnicode(eachchar,outputtype);
	}
	
void wvOutputFromUnicode(U16 eachchar,char *outputtype)
	{
	int i;
	U16 temp16;
	U8 temp8;
	char *str;
	char f_code[33];            /* From CCSID                           */
    char t_code[33];            /* To CCSID                             */
    iconv_t iconv_handle;       /* Conversion Descriptor returned       */
                                /* from iconv_open() function           */
    char *obuf;                 /* Buffer for converted characters      */
	char *p;
    size_t ibuflen;               /* Length of input buffer               */
    size_t obuflen;               /* Length of output buffer              */
	size_t len;
    const char *ibuf;
    char buffer[2];
    char buffer2[5];

    buffer[0]=(eachchar>>8)&0x00ff;
    buffer[1]=eachchar&0x00ff;
    ibuf = buffer;
    obuf = buffer2;

	if (wvConvertUnicodeToHtml(eachchar))
		return;

	 /* All reserved positions of from code (last 12 characters) and to code   */
    /* (last 19 characters) must be set to hexadecimal zeros.                 */

    memset(f_code,'\0',33);
    memset(t_code,'\0',33);

    strcpy(f_code,"UCS-2");
    strcpy(t_code,outputtype);

	iconv_handle = iconv_open(t_code,f_code);
    if (iconv_handle == (iconv_t)-1)
        {
        wvError(("iconv_open fail: %d, cannot convert %s to %s\n",errno,"UCS-2",outputtype));
        printf("?");
        return;
        }

	ibuflen = 2;
    obuflen = 5;
	p = obuf;
    len = obuflen;
    iconv(iconv_handle, &ibuf, &ibuflen, &obuf, &obuflen);
	len = len-obuflen;
    iconv_close(iconv_handle);

	for (i=0;i<len;i++)
		printf("%c",p[i]);
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

void wvSetCharHandler(int (*proc)(wvParseStruct *,U16,U8,U16))
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


int wvConvertUnicodeToHtml(U16 char16)
	{
	switch(char16)
		{
		case 11:
			printf("<br>");
			return(1);
		case 30:
		case 31:
		case 45:
		case 0x2013:
			printf("-");
			return(1);
		case 12:
		case 13:
		case 14:
		case 7:
			return(1);
		case 34:
			printf("&quot;");
			return(1);
		case 38:
			printf("&amp;");
			return(1);
		case 60:
			printf("&lt;");
			return(1);
		case 62:
			printf("&gt;");
			return(1);
		case 0x2026:
#if 0
/* 
this just looks awful in netscape 4.5, so im going to do a very foolish
thing and just put ... instead of this
*/
			printf("&#133;");	/*is there a proper html name for ... &ellipse; ?*/
#endif
			printf("...");	
			return(1);
		case 0x2019:
			printf("'");
			return(1);
		case 0x2215:
			printf("/");
			return(1);
		case 0xF8E7:	/* without this, things should work in theory, but not for me */
			printf("_");
			return(1);
		case 0x2018:
			printf("`");
			return(1);
		}
	return(0);
	}

	
