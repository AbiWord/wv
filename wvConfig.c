#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "wvinternal.h"
#include "xmlparse.h"

extern char *wv_version;

TokenTable s_Tokens[] =
{
    {   "document",      TT_DOCUMENT     	},
    {   "begin",         TT_BEGIN        	},
    {   "end",           TT_END          	},
    {   "title",         TT_TITLE        	},
    {   "paragraph",     TT_PARA         	},
    {   "charset",       TT_CHARSET      	},
    {   "version",       TT_VERSION      	},
    {   "justification", TT_JUSTIFICATION	},
    {   "just",       	 TT_JUST     	 	},
    {   "left",       	 TT_LEFT     	 	},
    {   "right",       	 TT_RIGHT     	 	},
    {   "center",        TT_CENTER     	 	},
    {   "block",       	 TT_BLOCK     	 	},
    {   "asian",       	 TT_ASIAN     	 	},
    {   "section",       TT_SECTION     	},
	{	"bold",			 TT_BOLD			},
	{	"character",	 TT_CHAR			},
	{	"bold.begin",	 TT_BOLDB			},
	{	"bold.end",	   	 TT_BOLDE			},
	{	"italic",		 TT_ITALIC			},
	{	"italic.begin",	 TT_ITALICB			},
	{	"italic.end",	 TT_ITALICE,		},
    {   "*",             TT_OTHER        	} /* must be last */
};

#if 0
int main(void)
	{
	state_data myhandle;

	wvInitStateData(&myhandle);
	wvParseConfig(&myhandle,stdin);
	wvReleaseStateData(&myhandle);
	}
#endif


void wvInitStateData(state_data *data)
	{
	int i;
	data->fp=NULL;
	data->currentele=NULL;
	data->current=NULL;
	data->currentlen=0;
	for(i=0;i<TokenTableSize;i++)
		{
		data->elements[i].nostr=0;
		data->elements[i].str=NULL;
		}

	}

void wvListStateData(state_data *data)
	{
	int i,j;
	for(i=0;i<TokenTableSize;i++)
		{
		for(j=0;j<data->elements[i].nostr;j++)
			{
			if (data->elements[i].str[j])
				wvTrace(("listing->element %s\n",data->elements[i].str[j]));
			}
		}

	}

void wvReleaseStateData(state_data *data)
	{
	int i,k;
	if (data->fp) fclose(data->fp);
	for(i=0;i<TokenTableSize;i++)
	    {
		for(k=0;k<data->elements[i].nostr;k++)
			wvFree(data->elements[i].str[k]);
		wvFree(data->elements[i].str);
		}
	}

unsigned int s_mapNameToToken(const char* name)
	{
	unsigned int k;
    for (k=0; k<TokenTableSize; k++)
		{
        if (s_Tokens[k].m_name[0] == '*')
            return k;
        else if (!(strcasecmp(s_Tokens[k].m_name,name)))
            return k;
		}
    return 0;
	}

void exstartElement(void *userData, const char *name, const char **atts)
	{
	unsigned int tokenIndex;
	expand_data *mydata = (expand_data *)userData;
	char *text,*str;
	const char *ctext;
	static int bold,italic;

	tokenIndex = s_mapNameToToken(name);
	wvTrace(("name = %s tokenIndex = %d\n", name, tokenIndex));
	switch (s_Tokens[tokenIndex].m_type)
		{
		case TT_TITLE:
			text = wvGetTitle(mydata->anSttbfAssoc);
			wvAppendStr(&mydata->retstring,text);
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_CHARSET:
			wvTrace(("the charset is %d\n",mydata->charset));
			ctext = wvGetCharset(mydata->charset);
			wvAppendStr(&mydata->retstring,ctext);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_VERSION:
			wvTrace(("the version is %s\n",wv_version));
			wvAppendStr(&mydata->retstring,wv_version);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_JUST:
		    wvTrace(("just is %d\n",((PAP*)(mydata->props))->jc));
			wvTrace(("str is %s\n",mydata->sd->elements[TT_JUSTIFICATION].str[0]));
			text = (char *)malloc(strlen(mydata->sd->elements[TT_JUSTIFICATION].str[((PAP*)(mydata->props))->jc])+1);
			wvTrace(("the just is %d\n",((PAP*)(mydata->props))->jc));
			strcpy(text,mydata->sd->elements[TT_JUSTIFICATION].str[((PAP*)(mydata->props))->jc]);
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BOLDB:
		    wvTrace(("bold is %d\n",((CHP*)(mydata->props))->fBold));
			wvTrace(("str is %s\n",mydata->sd->elements[TT_BOLD].str[0]));
			if ( (((CHP*)(mydata->props))->fBold) && (bold == 0) )
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_BOLD].str[0])+1);
				strcpy(text,mydata->sd->elements[TT_BOLD].str[0]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				bold=1;
				}
			break;
		case TT_ITALICB:
		    wvTrace(("italic is %d\n",((CHP*)(mydata->props))->fItalic));
			wvTrace(("str is %s\n",mydata->sd->elements[TT_ITALIC].str[0]));
			if ( (((CHP*)(mydata->props))->fItalic) && (italic == 0) )
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ITALIC].str[0])+1);
				strcpy(text,mydata->sd->elements[TT_ITALIC].str[0]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				italic=1;
				}
			break;
		case TT_ITALICE:
		    wvTrace(("italic is %d\n",((CHP*)(mydata->props))->fItalic));
			wvTrace(("str is %s\n",mydata->sd->elements[TT_ITALIC].str[0]));
			/*
			if ( (!((CHP*)(mydata->props))->fItalic) && (italic == 1) )
			*/
			if (italic == 1)
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ITALIC].str[1])+1);
				strcpy(text,mydata->sd->elements[TT_ITALIC].str[1]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				italic=0;
				}
			break;
		case TT_BOLDE:
		    wvTrace(("bold is %d\n",((CHP*)(mydata->props))->fBold));
			wvTrace(("str is %s\n",mydata->sd->elements[TT_BOLD].str[0]));
			/*
			if ( (!((CHP*)(mydata->props))->fBold) && (bold == 1) )
			*/
			if (bold == 1)
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_BOLD].str[1])+1);
				strcpy(text,mydata->sd->elements[TT_BOLD].str[1]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				bold=0;
				}
			break;
		}
	}

void startElement(void *userData, const char *name, const char **atts)
	{
	unsigned int tokenIndex,i;
	state_data *mydata = (state_data *)userData;

	tokenIndex = s_mapNameToToken(name);
	if (mydata->current != NULL)
		wvTrace(("current not NULL\n"));
	wvTrace(("element %s started\n",name));
	switch (s_Tokens[tokenIndex].m_type)
		{
		case TT_DOCUMENT:
		case TT_PARA:
		case TT_SECTION:
		case TT_BOLD:
		case TT_CHAR:
		case TT_ITALIC:
			mydata->elements[s_Tokens[tokenIndex].m_type].str = (char **)malloc(sizeof(char *)*2);
			mydata->elements[s_Tokens[tokenIndex].m_type].nostr=2;
			for(i=0;i<2;i++)
				mydata->elements[s_Tokens[tokenIndex].m_type].str[i] = NULL;
			mydata->currentele = &(mydata->elements[s_Tokens[tokenIndex].m_type]);
			break;
		case TT_JUSTIFICATION:
			mydata->elements[s_Tokens[tokenIndex].m_type].str = (char **)malloc(sizeof(char *)*5);
			mydata->elements[s_Tokens[tokenIndex].m_type].nostr=5;
			for(i=0;i<5;i++)
				mydata->elements[s_Tokens[tokenIndex].m_type].str[i] = NULL;
			mydata->currentele = &(mydata->elements[s_Tokens[tokenIndex].m_type]);
			break;
		case TT_BEGIN:
		case TT_LEFT:
			mydata->current = &(mydata->currentele->str[0]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this point the string is %s\n",*(mydata->current)));
			break;
		case TT_END:
		case TT_CENTER:
			mydata->current = &(mydata->currentele->str[1]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_RIGHT:
			mydata->current = &(mydata->currentele->str[2]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BLOCK:
			mydata->current = &(mydata->currentele->str[3]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ASIAN:
			mydata->current = &(mydata->currentele->str[4]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_TITLE:
			wvAppendStr(mydata->current,"<title/>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this other point the string is %s\n",*(mydata->current)));
			break;
		case TT_CHARSET:
			wvAppendStr(mydata->current,"<charset/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_VERSION:
			wvAppendStr(mydata->current,"<version/>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this other other point the string is %s\n",*(mydata->current)));
			break;
		case TT_ITALICB:
			wvAppendStr(mydata->current,"<italic.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this other other point the string is %s\n",*(mydata->current)));
			break;
		case TT_ITALICE:
			wvAppendStr(mydata->current,"<italic.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this other other point the string is %s\n",*(mydata->current)));
			break;
		case TT_BOLDB:
			wvAppendStr(mydata->current,"<bold.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this other other point the string is %s\n",*(mydata->current)));
			break;
		case TT_BOLDE:
			wvAppendStr(mydata->current,"<bold.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this other other point the string is %s\n",*(mydata->current)));
			break;
		case TT_JUST:
			wvAppendStr(mydata->current,"<just/>");
			mydata->currentlen = strlen(*(mydata->current));
			wvTrace(("At this other other other point the string is %s\n",*(mydata->current)));
			break;
		}
	}

void endElement(void *userData, const char *name)
	{
	state_data *mydata = (state_data *)userData;
	unsigned int tokenIndex;

	tokenIndex = s_mapNameToToken(name);
	switch (s_Tokens[tokenIndex].m_type)
		{
		case TT_BEGIN:
		case TT_END:
		case TT_LEFT:
		case TT_RIGHT:
		case TT_CENTER:
		case TT_BLOCK:
		case TT_ASIAN:
			wvAppendStr(mydata->current,"</begin>");
			wvTrace(("When we finish the str is %s\n",*(mydata->current)));
			mydata->currentlen=0;
			mydata->current=NULL;
			break;
		case TT_TITLE:
		case TT_CHARSET:
		case TT_VERSION:
		case TT_JUST:
		case TT_BOLDB:
		case TT_BOLDE:
		case TT_ITALICB:
		case TT_ITALICE:
			break;
		default:
			mydata->currentlen=0;
			mydata->current=NULL;
			break;
		}
	wvTrace(("ele ended\n"));
	}

void exendElement(void *userData, const char *name)
	{
	/*
	expand_data *mydata = (expand_data *)userData;
	*/
	unsigned int tokenIndex;

	tokenIndex = s_mapNameToToken(name);
	switch (s_Tokens[tokenIndex].m_type)
		{
		case TT_TITLE:
		case TT_CHARSET:
		case TT_VERSION:
			break;
		default:
			break;
		}
	wvTrace(("ele ended\n"));
	}

void charData(void* userData, const XML_Char *s, int len)
	{
	int i;

	state_data *mydata = (state_data *)userData;
	if ((len > 0) && (mydata->current != NULL))
		*(mydata->current) = (char *)realloc(*(mydata->current),len+mydata->currentlen+1);
	else
		return;

	for(i=0;i<len;i++)
		{
		if (mydata->current != NULL)
			{
			switch (s[i])
				{
				case '&':
					wvAppendStr(mydata->current,"&amp;");
					mydata->currentlen+=strlen("&amp;")-1;
					break;
				case '<':
					wvAppendStr(mydata->current,"&lt;");
					mydata->currentlen+=strlen("&lt;")-1;
					break;
				case '>':
					wvAppendStr(mydata->current,"&gt;");
					mydata->currentlen+=strlen("&gt;")-1;
					break;
				case '"':
					wvAppendStr(mydata->current,"&quot;");
					mydata->currentlen+=strlen("&quot;")-1;
					break;
				default:
					(*(mydata->current))[i+mydata->currentlen] = s[i];
					break;
				}
			}
		}
	if (mydata->current != NULL)
		{
		(*(mydata->current))[i+mydata->currentlen] = '\0';
		mydata->currentlen+=len;
		}
	wvTrace(("chardata str is %s\n",*(mydata->current)));
	}

void excharData(void* userData, const XML_Char *s, int len)
	{
	int i;

	expand_data *mydata = (expand_data *)userData;
	if (len > 0) 
		mydata->retstring = (char *)realloc(mydata->retstring,len+mydata->currentlen+1);
	else
		return;

	for(i=0;i<len;i++)
		{
		wvTrace(("exchar->%c ",s[i]));
		if (mydata->retstring!= NULL)
			mydata->retstring[i+mydata->currentlen] = s[i];
		}
	if (mydata->retstring != NULL)
		{
		mydata->retstring[i+mydata->currentlen] = '\0';
		mydata->currentlen+=len;
		}
	}

int wvParseConfig(state_data *myhandle)
	{
	char buf[BUFSIZ];
	XML_Parser parser = XML_ParserCreate(NULL);
	int done;
	size_t len;

	XML_SetUserData(parser, myhandle);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, charData);

	if (myhandle->fp == NULL)
		{
		wvError(("how can this happen\n"));
		exit(-1);
		}

	do {
		wvTrace(("loop in\n"));
		len = fread(buf, 1, sizeof(buf), myhandle->fp);
		wvTrace(("loop out\n"));
		done = len < sizeof(buf);
		if (!XML_Parse(parser, buf, len, done)) 
			{
			wvError(("%s at line %d\n",
				XML_ErrorString(XML_GetErrorCode(parser)),
				XML_GetCurrentLineNumber(parser)));
			return(1);
			}
	} while (!done);
	XML_ParserFree(parser);

	wvListStateData(myhandle);

	return 0;
	}


void wvInitExpandData(expand_data *data)
	{
	data->retstring=NULL;
	data->currentlen=0;
	}

int wvExpand(expand_data *myhandle,char *buf,int len)
	{
	XML_Parser parser = XML_ParserCreate(NULL);

	wvTrace(("expanding string %s\n",buf));

	XML_SetUserData(parser, myhandle);
	XML_SetElementHandler(parser, exstartElement, exendElement);
	XML_SetCharacterDataHandler(parser, excharData);
	wvInitExpandData(myhandle);

	if (!XML_Parse(parser, buf, len, 1)) 
		{
		wvError(("%s at line %d\n",
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser)));
		return 1;
		}

	XML_ParserFree(parser);

	return 0;
	}
