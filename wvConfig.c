#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "wvinternal.h"
#include "xmlparse.h"

#define HANDLE_B_CHAR_ELE(a,b,c,d) \
wvTrace(("flag is %d\n",((CHP*)(mydata->props))->b)); \
wvTrace(("str is %s\n",mydata->sd->elements[a].str[0])); \
if ( (((CHP*)(mydata->props))->b == d) && (c == 0) ) \
	{ \
	text = (char *)malloc(strlen(mydata->sd->elements[a].str[0])+1); \
	strcpy(text,mydata->sd->elements[a].str[0]); \
	str = mydata->retstring; \
	wvExpand(mydata,text,strlen(text)); \
	wvAppendStr(&str,mydata->retstring); \
	wvFree(mydata->retstring); \
	mydata->retstring = str; \
	wvFree(text); \
	mydata->currentlen = strlen(mydata->retstring); \
	c=d; \
	}

#define HANDLE_E_CHAR_ELE(a,b,c,d) \
wvTrace(("flag is %d\n",((CHP*)(mydata->props))->b)); \
wvTrace(("str is %s\n",mydata->sd->elements[a].str[0])); \
/* \
if ( (!((CHP*)(mydata->props))->b) && (c != 0) ) \
*/ \
if (c == d) \
	{ \
	text = (char *)malloc(strlen(mydata->sd->elements[a].str[1])+1); \
	strcpy(text,mydata->sd->elements[a].str[1]); \
	str = mydata->retstring; \
	wvExpand(mydata,text,strlen(text)); \
	wvAppendStr(&str,mydata->retstring); \
	wvFree(mydata->retstring); \
	mydata->retstring = str; \
	wvFree(text); \
	mydata->currentlen = strlen(mydata->retstring); \
	c=0; \
	}


extern char *wv_version;

TokenTable s_Tokens[] =
{
    {   "begin",         TT_BEGIN        	},
    {   "end",           TT_END          	},
    {   "title",         TT_TITLE        	},
    {   "charset",       TT_CHARSET      	},
    {   "version",       TT_VERSION      	},

    {   "document",      TT_DOCUMENT     	},
	 {   "section",       TT_SECTION     	},
      {   "paragraph",     TT_PARA         	},
       {   "block",       	 TT_BLOCK     	 	},
        {   "justification", TT_JUSTIFICATION	},
         {   "just",       	 TT_JUST     	 	},
          {   "left",       	 TT_LEFT     	 	},
          {   "right",       	 TT_RIGHT     	 	},
          {   "center",        TT_CENTER     	 	},
          {   "asian",       	 TT_ASIAN     	 	},
	   {	"character",	 TT_CHAR			},
	    {	"bold",			 TT_BOLD			},
	     {	"bold.begin",	 TT_BOLDB			},
	     {	"bold.end",	   	 TT_BOLDE			},
	    {	"italic",		 TT_ITALIC			},
	     {	"italic.begin",	 TT_ITALICB			},
	     {	"italic.end",	 TT_ITALICE,		},
	    {	"strike",		 TT_STRIKE			},
	     { 	"strike.begin",	 TT_STRIKEB			},
	     {	"strike.end",	 TT_STRIKEE,		},
	    {	"rmarkdel",		 TT_RMarkDel,		},
	     {	"rmarkdel.begin",TT_RMarkDelB,		},
	     {	"rmarkdel.end",	 TT_RMarkDelE,		},
		{	"outline",		 TT_OUTLINE,		},
		 {	"outline.begin", TT_OUTLINEB,		},
		 {	"outline.end",	 TT_OUTLINEE,		},
		{	"smallcaps",	 TT_SMALLCAPS,		},
		 {	"smallcaps.begin",TT_SMALLCAPSB,	},
		 {	"smallcaps.end", TT_SMALLCAPSE,		},
		{	"caps",	 		 TT_CAPS,			},
		 {	"caps.begin",	 TT_CAPSB,			},
		 {	"caps.end", 	 TT_CAPSE,	 		},
		{	"vanish",	 	 TT_VANISH,			},
		 {	"vanish.begin",	 TT_VANISHB,		},
		 {	"vanish.end", 	 TT_VANISHE,	 	},
		{	"rmark",	 	 TT_RMark,			},
		 {	"rmark.begin",	 TT_RMarkB,			},
		 {	"rmark.end", 	 TT_RMarkE,		 	},
		{	"shadow",	 	 TT_SHADOW,			},
		 {	"shadow.begin",	 TT_SHADOWB,		},
		 {	"shadow.end", 	 TT_SHADOWE,		},
		{	"lowercase",	 TT_LOWERCASE,		},
		 {	"lowercase.begin",TT_LOWERCASEB,	},
		 {	"lowercase.end", TT_LOWERCASEE,		},
		{	"emboss",	 	 TT_EMBOSS,			},
		 {	"emboss.begin",	 TT_EMBOSSB,		},
		 {	"emboss.end", 	 TT_EMBOSSE,		},
		{	"imprint",	 	 TT_IMPRINT,		},
		 {	"imprint.begin", TT_IMPRINTB,		},
		 {	"imprint.end", 	 TT_IMPRINTE,		},
		{	"dstrike",	 	 TT_DSTRIKE,		},
		 {	"dstrike.begin", TT_DSTRIKEB,		},
		 {	"dstrike.end", 	 TT_DSTRIKEE,		},
		{	"super",	 	 TT_SUPER,			},
		 {	"super.begin", 	 TT_SUPERB,			},
		 {	"super.end", 	 TT_SUPERE,			},
		{	"sub",	 	 	 TT_SUB,			},
		 {	"sub.begin", 	 TT_SUBB,			},
		 {	"sub.end", 	 	 TT_SUBE,			},
		{	"single",	 	 TT_SINGLE,			},
		 {	"single.begin",  TT_SINGLEB,		},
		 {	"single.end", 	 TT_SINGLEE,		},
		{	"word",	 	 	 TT_WORD,			},
		 {	"word.begin", 	 TT_WORDB,			},
		 {	"word.end", 	 TT_WORDE,			},
		{	"double",	 	 TT_DOUBLE,			},
		 {	"double.begin",  TT_DOUBLEB,		},
		 {	"double.end", 	 TT_DOUBLEE,		},
		{	"dotted",	 	 TT_DOTTED,			},
		 {	"dotted.begin",  TT_DOTTEDB,		},
		 {	"dotted.end", 	 TT_DOTTEDE,		},
		{	"hidden",	 	 TT_HIDDEN,			},
		 {	"hidden.begin",  TT_HIDDENB,		},
		 {	"hidden.end", 	 TT_HIDDENE,		},
		{	"thick",	 	 TT_THICK,			},
		 {	"thick.begin", 	 TT_THICKB,			},
		 {	"thick.end", 	 TT_THICKE,			},
		{	"dash",	 	 	 TT_DASH,			},
		 {	"dash.begin", 	 TT_DASHB,			},
		 {	"dash.end", 	 TT_DASHE,			},
		{	"dot",	 	 	 TT_DOT,			},
		 {	"dot.begin", 	 TT_DOTB,			},
		 {	"dot.end", 	 	 TT_DOTE,			},
		{	"dotdash",	 	 TT_DOTDASH,		},
		 {	"dotdash.begin", TT_DOTDASHB,		},
		 {	"dotdash.end", 	 TT_DOTDASHE,		},
		{	"dotdotdash",	 TT_DOTDOTDASH,		},
		 {	"dotdotdash.begin",TT_DOTDOTDASHB,	},
		 {	"dotdotdash.end",TT_DOTDOTDASHE,	},
		{	"wave",	 	 	 TT_WAVE,			},
		 {	"wave.begin", 	 TT_WAVEB,			},
		 {	"wave.end", 	 TT_WAVEE,			},
		{	"black",	 	 TT_BLACK,			},
		 {	"black.begin", 	 TT_BLACKB,			},
		 {	"black.end", 	 TT_BLACKE,			},
		{	"blue",	 	 	 TT_BLUE,			},
		 {	"blue.begin", 	 TT_BLUEB,			},
		 {	"blue.end", 	 TT_BLUEE,			},
		{	"cyan",	 	 	 TT_CYAN,			},
		 {	"cyan.begin", 	 TT_CYANB,			},
		 {	"cyan.end", 	 TT_CYANE,			},
		{	"green",	 	 TT_GREEN,			},
		 {	"green.begin", 	 TT_GREENB,			},
		 {	"green.end", 	 TT_GREENE,			},
		{	"magenta",	 	 TT_MAGENTA,		},
		 {	"magenta.begin", TT_MAGENTAB,		},
		 {	"magenta.end", 	 TT_MAGENTAE,		},
		{	"red",	 	 	 TT_RED,			},
		 {	"red.begin", 	 TT_REDB,			},
		 {	"red.end", 	 	 TT_REDE,			},
		{	"yellow",	 	 TT_YELLOW,			},
		 {	"yellow.begin",  TT_YELLOWB,		},
		 {	"yellow.end", 	 TT_YELLOWE,		},
		{	"white",	 	 TT_WHITE,			},
		 {	"white.begin", 	 TT_WHITEB,			},
		 {	"white.end", 	 TT_WHITEE,			},
		{	"dkblue",	 	 TT_DKBLUE,			},
		 {	"dkblue.begin",  TT_DKBLUEB,		},
		 {	"dkblue.end", 	 TT_DKBLUEE,		},
		{	"dkcyan",	 	 TT_DKCYAN,			},
		 {	"dkcyan.begin",  TT_DKCYANB,		},
		 {	"dkcyan.end", 	 TT_DKCYANE,		},
		{	"dkgreen",	 	 TT_DKGREEN,		},
		 {	"dkgreen.begin", TT_DKGREENB,		},
		 {	"dkgreen.end", 	 TT_DKGREENE,		},
		{	"dkmagenta",	 TT_DKMAGENTA,		},
		 {	"dkmagenta.begin",TT_DKMAGENTAB,	},
		 {	"dkmagenta.end", TT_DKMAGENTAE,		},
		{	"dkred",	 	 TT_DKRED,			},
		 {	"dkred.begin", 	 TT_DKREDB,			},
		 {	"dkred.end", 	 TT_DKREDE,			},
		{	"dkyellow",	 	 TT_DKYELLOW,		},
		 {	"dkyellow.begin",TT_DKYELLOWB,		},
		 {	"dkyellow.end",  TT_DKYELLOWE,		},
		{	"dkgray",	 	 TT_DKGRAY,			},
		 {	"dkgray.begin",  TT_DKGRAYB,		},
		 {	"dkgray.end", 	 TT_DKGRAYE,		},
		{	"ltgray",	 	 TT_LTGRAY,			},
		 {	"ltgray.begin",  TT_LTGRAYB,		},
		 {	"ltgray.end", 	 TT_LTGRAYE,		},
		{	"fontstr",	 	 TT_FONTSTR,		},
		 {	"fontstr.begin", TT_FONTSTRB,		},
		 {	"fontstr.end", 	 TT_FONTSTRE,		},
		{	"color",	 	 TT_COLOR,			},
		 {	"color.begin",	 TT_COLORB,			},
		 {	"color.end", 	 TT_COLORE,			},
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
	static int bold,italic,strike,outline,smallcaps,caps,vanish,underline,
	shadow,lowercase,emboss,imprint,dstrike,iss,kul,color,fontstr;

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
		case TT_COLORB:
			wvTrace(("color is %d\n",((CHP*)(mydata->props))->ico)); 
			wvTrace(("str is %s\n",mydata->sd->elements[TT_COLOR].str[0]));
			text = (char *)malloc(strlen(mydata->sd->elements[TT_COLOR].str[((CHP*)(mydata->props))->ico])+1);
			wvTrace(("the just is %d\n",((CHP*)(mydata->props))->ico));
			strcpy(text,mydata->sd->elements[TT_COLOR].str[((CHP*)(mydata->props))->ico]);
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
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
			HANDLE_B_CHAR_ELE(TT_BOLD,fBold,bold,1)
			break;
		case TT_RMarkDelB:
			HANDLE_B_CHAR_ELE(TT_RMarkDel,fRMarkDel,strike,1)
			break;
		case TT_OUTLINEB:
			HANDLE_B_CHAR_ELE(TT_OUTLINE,fOutline,outline,1)
			break;
		case TT_STRIKEB:
			HANDLE_B_CHAR_ELE(TT_STRIKE,fStrike,strike,1)
			break;
		case TT_ITALICB:
			HANDLE_B_CHAR_ELE(TT_ITALIC,fItalic,italic,1)
			break;
		case TT_RMarkDelE:
			HANDLE_E_CHAR_ELE(TT_RMarkDel,fRMarkDel,strike,1)
			break;
		case TT_OUTLINEE:
			HANDLE_E_CHAR_ELE(TT_OUTLINE,fOutline,outline,1)
			break;
		case TT_STRIKEE:
			HANDLE_E_CHAR_ELE(TT_STRIKE,fStrike,strike,1)
			break;
		case TT_ITALICE:
			HANDLE_E_CHAR_ELE(TT_ITALIC,fItalic,italic,1)
			break;
		case TT_BOLDE:
			HANDLE_E_CHAR_ELE(TT_BOLD,fBold,bold,1)
			break;
		case TT_SMALLCAPSB:
			HANDLE_B_CHAR_ELE(TT_SMALLCAPS,fSmallCaps,smallcaps,1)
			break;
		case TT_SMALLCAPSE:
			HANDLE_E_CHAR_ELE(TT_SMALLCAPS,fSmallCaps,smallcaps,1)
			break;
		case TT_CAPSB:
			HANDLE_B_CHAR_ELE(TT_CAPS,fCaps,caps,1)
			break;
		case TT_CAPSE:
			HANDLE_E_CHAR_ELE(TT_CAPS,fCaps,caps,1)
			break;
		case TT_VANISHB:
			HANDLE_B_CHAR_ELE(TT_VANISH,fVanish,vanish,1)
			break;
		case TT_VANISHE:
			HANDLE_E_CHAR_ELE(TT_VANISH,fVanish,vanish,1)
			break;
		case TT_RMarkB:
			HANDLE_B_CHAR_ELE(TT_RMark,fRMark,underline,1)
			break;
		case TT_RMarkE:
			HANDLE_E_CHAR_ELE(TT_RMark,fRMark,underline,1)
			break;
		case TT_SHADOWB:
			HANDLE_B_CHAR_ELE(TT_SHADOW,fShadow,shadow,1)
			break;
		case TT_SHADOWE:
			HANDLE_E_CHAR_ELE(TT_SHADOW,fShadow,shadow,1)
			break;
		case TT_LOWERCASEB:
			HANDLE_B_CHAR_ELE(TT_LOWERCASE,fLowerCase,lowercase,1)
			break;
		case TT_LOWERCASEE:
			HANDLE_E_CHAR_ELE(TT_LOWERCASE,fLowerCase,lowercase,1)
			break;
		case TT_EMBOSSB:
			HANDLE_B_CHAR_ELE(TT_EMBOSS,fEmboss,emboss,1)
			break;
		case TT_EMBOSSE:
			HANDLE_E_CHAR_ELE(TT_EMBOSS,fEmboss,emboss,1)
			break;
		case TT_IMPRINTB:
			HANDLE_B_CHAR_ELE(TT_IMPRINT,fImprint,imprint,1)
			break;
		case TT_IMPRINTE:
			HANDLE_E_CHAR_ELE(TT_IMPRINT,fImprint,imprint,1)
			break;
		case TT_DSTRIKEB:
			HANDLE_B_CHAR_ELE(TT_DSTRIKE,fDStrike,dstrike,1)
			break;
		case TT_DSTRIKEE:
			HANDLE_E_CHAR_ELE(TT_DSTRIKE,fDStrike,dstrike,1)
			break;
		case TT_SUPERB:
		case TT_SUBB:
			HANDLE_B_CHAR_ELE(s_Tokens[tokenIndex].m_type-1,iss,iss,(s_Tokens[tokenIndex].m_type-TT_SUPERB)/3+1)
			break;
		case TT_SUPERE:
		case TT_SUBE:
			HANDLE_E_CHAR_ELE(s_Tokens[tokenIndex].m_type-2,iss,iss,(s_Tokens[tokenIndex].m_type-TT_SUPERE)/3+1)
			break;

		case TT_SINGLEB:
		case TT_WORDB:
		case TT_DOUBLEB:
		case TT_DOTTEDB:
		case TT_HIDDENB:
		case TT_THICKB:
		case TT_DASHB:
		case TT_DOTB:
		case TT_DOTDASHB:
		case TT_DOTDOTDASHB:
		case TT_WAVEB:
			HANDLE_B_CHAR_ELE(s_Tokens[tokenIndex].m_type-1,kul,kul,(s_Tokens[tokenIndex].m_type-TT_SINGLEB)/3+1)
			break;

		case TT_BLACKB:
		case TT_BLUEB:
		case TT_CYANB:
		case TT_GREENB:
		case TT_MAGENTAB:
		case TT_REDB:
		case TT_YELLOWB:
		case TT_WHITEB:
		case TT_DKBLUEB:
		case TT_DKCYANB:
		case TT_DKGREENB:
		case TT_DKMAGENTAB:
		case TT_DKREDB:
		case TT_DKYELLOWB:
		case TT_DKGRAYB:
		case TT_LTGRAYB:
			HANDLE_B_CHAR_ELE(s_Tokens[tokenIndex].m_type-1,ico,color,(s_Tokens[tokenIndex].m_type-TT_BLACKB)/3+1)
			break;


		case TT_SINGLEE:
		case TT_WORDE:
		case TT_DOUBLEE:
		case TT_DOTTEDE:
		case TT_HIDDENE:
		case TT_THICKE:
		case TT_DASHE:
		case TT_DOTE:
		case TT_DOTDASHE:
		case TT_DOTDOTDASHE:
		case TT_WAVEE:
			HANDLE_E_CHAR_ELE(s_Tokens[tokenIndex].m_type-2,kul,kul,(s_Tokens[tokenIndex].m_type-TT_SINGLEE)/3+1)
			break;

		case TT_BLACKE:
		case TT_BLUEE:
		case TT_CYANE:
		case TT_GREENE:
		case TT_MAGENTAE:
		case TT_REDE:
		case TT_YELLOWE:
		case TT_WHITEE:
		case TT_DKBLUEE:
		case TT_DKCYANE:
		case TT_DKGREENE:
		case TT_DKMAGENTAE:
		case TT_DKREDE:
		case TT_DKYELLOWE:
		case TT_DKGRAYE:
		case TT_LTGRAYE:
			HANDLE_E_CHAR_ELE(s_Tokens[tokenIndex].m_type-2,ico,color,(s_Tokens[tokenIndex].m_type-TT_BLACKE)/3+1)
			break;

		case TT_FONTSTRB:
			wvTrace(("flag is %d\n",((CHP*)(mydata->props))->ico)); 
			wvTrace(("str is %s\n",mydata->sd->elements[TT_FONTSTR].str[0])); 
			if ( (((CHP*)(mydata->props))->ico) && (fontstr == 0) ) 
				{ 
				text = (char *)malloc(strlen(mydata->sd->elements[TT_FONTSTR].str[0])+1); 
				strcpy(text,mydata->sd->elements[TT_FONTSTR].str[0]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring); 
				fontstr=1; 
				}
			
			break;
		case TT_FONTSTRE:
			wvTrace(("flag is %d\n",((CHP*)(mydata->props))->ico)); 
			wvTrace(("str is %s\n",mydata->sd->elements[TT_FONTSTR].str[0])); 
			if (fontstr) 
				{ 
				text = (char *)malloc(strlen(mydata->sd->elements[TT_FONTSTR].str[1])+1); 
				strcpy(text,mydata->sd->elements[TT_FONTSTR].str[1]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring); 
				fontstr=0; 
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
		case TT_STRIKE:
		case TT_RMarkDel:
		case TT_OUTLINE:
		case TT_SMALLCAPS:
		case TT_CAPS:
		case TT_VANISH:
		case TT_RMark:
		case TT_SHADOW:
		case TT_LOWERCASE:
		case TT_EMBOSS:
		case TT_IMPRINT:
		case TT_DSTRIKE:
		case TT_SUPER:
		case TT_SUB:
		case TT_SINGLE:
		case TT_WORD:
		case TT_DOUBLE:
		case TT_DOTTED:
		case TT_HIDDEN:
		case TT_THICK:
		case TT_DASH:
		case TT_DOT:
		case TT_DOTDASH:
		case TT_DOTDOTDASH:
		case TT_WAVE:
		case TT_BLACK:
		case TT_BLUE:
		case TT_CYAN:
		case TT_GREEN:
		case TT_MAGENTA:
		case TT_RED:
		case TT_YELLOW:
		case TT_WHITE:
		case TT_DKBLUE:
		case TT_DKCYAN:
		case TT_DKGREEN:
		case TT_DKMAGENTA:
		case TT_DKRED:
		case TT_DKYELLOW:
		case TT_DKGRAY:
		case TT_LTGRAY:
		case TT_FONTSTR:
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
		case TT_COLOR:
			mydata->elements[s_Tokens[tokenIndex].m_type].str = (char **)malloc(sizeof(char *)*16);
			mydata->elements[s_Tokens[tokenIndex].m_type].nostr=16;
			for(i=0;i<16;i++)
				mydata->elements[s_Tokens[tokenIndex].m_type].str[i] = NULL;
			mydata->currentele = &(mydata->elements[s_Tokens[tokenIndex].m_type]);
			break;
		case TT_BEGIN:
		case TT_LEFT:
			mydata->current = &(mydata->currentele->str[0]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
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
			break;
		case TT_CHARSET:
			wvAppendStr(mydata->current,"<charset/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_VERSION:
			wvAppendStr(mydata->current,"<version/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_STRIKEB:
			wvAppendStr(mydata->current,"<strike.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_OUTLINEB:
			wvAppendStr(mydata->current,"<outline.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_RMarkDelB:
			wvAppendStr(mydata->current,"<rmarkdel.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ITALICB:
			wvAppendStr(mydata->current,"<italic.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ITALICE:
			wvAppendStr(mydata->current,"<italic.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_OUTLINEE:
			wvAppendStr(mydata->current,"<outline.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_STRIKEE:
			wvAppendStr(mydata->current,"<strike.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_RMarkDelE:
			wvAppendStr(mydata->current,"<rmarkdel.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BOLDB:
			wvAppendStr(mydata->current,"<bold.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BOLDE:
			wvAppendStr(mydata->current,"<bold.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SMALLCAPSB:
			wvAppendStr(mydata->current,"<smallcaps.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SMALLCAPSE:
			wvAppendStr(mydata->current,"<smallcaps.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_CAPSB:
			wvAppendStr(mydata->current,"<caps.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_CAPSE:
			wvAppendStr(mydata->current,"<caps.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_VANISHB:
			wvAppendStr(mydata->current,"<vanish.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_VANISHE:
			wvAppendStr(mydata->current,"<vanish.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_RMarkB:
			wvAppendStr(mydata->current,"<rmark.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_RMarkE:
			wvAppendStr(mydata->current,"<rmark.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SHADOWB:
			wvAppendStr(mydata->current,"<shadow.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SHADOWE:
			wvAppendStr(mydata->current,"<shadow.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_LOWERCASEB:
			wvAppendStr(mydata->current,"<lowercase.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_LOWERCASEE:
			wvAppendStr(mydata->current,"<lowercase.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_EMBOSSB:
			wvAppendStr(mydata->current,"<emboss.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_EMBOSSE:
			wvAppendStr(mydata->current,"<emboss.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_IMPRINTB:
			wvAppendStr(mydata->current,"<imprint.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_IMPRINTE:
			wvAppendStr(mydata->current,"<imprint.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DSTRIKEB:
			wvAppendStr(mydata->current,"<dstrike.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DSTRIKEE:
			wvAppendStr(mydata->current,"<dstrike.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SUPERB:
			wvAppendStr(mydata->current,"<super.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SUPERE:
			wvAppendStr(mydata->current,"<super.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SUBB:
			wvAppendStr(mydata->current,"<sub.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SUBE:
			wvAppendStr(mydata->current,"<sub.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_JUST:
			wvAppendStr(mydata->current,"<just/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_COLORB:
			wvAppendStr(mydata->current,"<color.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SINGLEB:
			wvAppendStr(mydata->current,"<single.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WORDB:
			wvAppendStr(mydata->current,"<word.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOUBLEB:
			wvAppendStr(mydata->current,"<double.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTTEDB:
			wvAppendStr(mydata->current,"<dotted.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_HIDDENB:
			wvAppendStr(mydata->current,"<hidden.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_THICKB:
			wvAppendStr(mydata->current,"<thick.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DASHB:
			wvAppendStr(mydata->current,"<dash.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTB:
			wvAppendStr(mydata->current,"<dot.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDASHB:
			wvAppendStr(mydata->current,"<dotdash.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDOTDASHB:
			wvAppendStr(mydata->current,"<dotdotdash.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WAVEB:
			wvAppendStr(mydata->current,"<wave.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SINGLEE:
			wvAppendStr(mydata->current,"<single.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WORDE:
			wvAppendStr(mydata->current,"<word.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOUBLEE:
			wvAppendStr(mydata->current,"<double.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTTEDE:
			wvAppendStr(mydata->current,"<dotted.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_HIDDENE:
			wvAppendStr(mydata->current,"<hidden.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_THICKE:
			wvAppendStr(mydata->current,"<thick.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DASHE:
			wvAppendStr(mydata->current,"<dash.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTE:
			wvAppendStr(mydata->current,"<dot.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDASHE:
			wvAppendStr(mydata->current,"<dotdash.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDOTDASHE:
			wvAppendStr(mydata->current,"<dotdotdash.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WAVEE:
			wvAppendStr(mydata->current,"<wave.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_BLACKB:
			wvAppendStr(mydata->current,"<black.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BLUEB:
			wvAppendStr(mydata->current,"<blue.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_CYANB:
			wvAppendStr(mydata->current,"<cyan.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_GREENB:
			wvAppendStr(mydata->current,"<green.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_MAGENTAB:
			wvAppendStr(mydata->current,"<magenta.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_REDB:
			wvAppendStr(mydata->current,"<red.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_YELLOWB:
			wvAppendStr(mydata->current,"<yellow.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WHITEB:
			wvAppendStr(mydata->current,"<white.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKBLUEB:
			wvAppendStr(mydata->current,"<dkblue.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKCYANB:
			wvAppendStr(mydata->current,"<dkcyan.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKGREENB:
			wvAppendStr(mydata->current,"<dkgreen.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKMAGENTAB:
			wvAppendStr(mydata->current,"<dkmagenta.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKREDB:
			wvAppendStr(mydata->current,"<dkred.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKYELLOWB:
			wvAppendStr(mydata->current,"<dkyellow.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKGRAYB:
			wvAppendStr(mydata->current,"<dkgray.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_LTGRAYB:
			wvAppendStr(mydata->current,"<ltgray.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_BLACKE:
			wvAppendStr(mydata->current,"<black.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BLUEE:
			wvAppendStr(mydata->current,"<blue.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_CYANE:
			wvAppendStr(mydata->current,"<cyan.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_GREENE:
			wvAppendStr(mydata->current,"<green.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_MAGENTAE:
			wvAppendStr(mydata->current,"<magenta.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_REDE:
			wvAppendStr(mydata->current,"<red.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_YELLOWE:
			wvAppendStr(mydata->current,"<yellow.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WHITEE:
			wvAppendStr(mydata->current,"<white.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKBLUEE:
			wvAppendStr(mydata->current,"<dkblue.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKCYANE:
			wvAppendStr(mydata->current,"<dkcyan.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKGREENE:
			wvAppendStr(mydata->current,"<dkgreen.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKMAGENTAE:
			wvAppendStr(mydata->current,"<dkmagenta.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKREDE:
			wvAppendStr(mydata->current,"<dkred.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKYELLOWE:
			wvAppendStr(mydata->current,"<dkyellow.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DKGRAYE:
			wvAppendStr(mydata->current,"<dkgray.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_LTGRAYE:
			wvAppendStr(mydata->current,"<ltgray.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_FONTSTRB:
			wvAppendStr(mydata->current,"<fontstr.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_FONTSTRE:
			wvAppendStr(mydata->current,"<fontstr.end/>");
			mydata->currentlen = strlen(*(mydata->current));
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
		case TT_STRIKEB:
		case TT_STRIKEE:
		case TT_RMarkDelB:
		case TT_RMarkDelE:
		case TT_OUTLINEB:
		case TT_OUTLINEE:
		case TT_SMALLCAPSB:
		case TT_SMALLCAPSE:
		case TT_CAPSB:
		case TT_CAPSE:
		case TT_VANISHB:
		case TT_VANISHE:
		case TT_RMarkB:
		case TT_RMarkE:
		case TT_SHADOWB:
		case TT_SHADOWE:
		case TT_LOWERCASEB:
		case TT_LOWERCASEE:
		case TT_EMBOSSB:
		case TT_EMBOSSE:
		case TT_IMPRINTB:
		case TT_IMPRINTE:
		case TT_DSTRIKEB:
		case TT_DSTRIKEE:
		case TT_SUPERB:
		case TT_SUPERE:
		case TT_SUBB:
		case TT_SUBE:
		case TT_SINGLEB:
		case TT_WORDB:
		case TT_DOUBLEB:
		case TT_DOTTEDB:
		case TT_HIDDENB:
		case TT_THICKB:
		case TT_DASHB:
		case TT_DOTB:
		case TT_DOTDASHB:
		case TT_DOTDOTDASHB:
		case TT_WAVEB:
		case TT_SINGLEE:
		case TT_WORDE:
		case TT_DOUBLEE:
		case TT_DOTTEDE:
		case TT_HIDDENE:
		case TT_THICKE:
		case TT_DASHE:
		case TT_DOTE:
		case TT_DOTDASHE:
		case TT_DOTDOTDASHE:
		case TT_WAVEE:
		case TT_BLACKB:
		case TT_BLUEB:
		case TT_CYANB:
		case TT_GREENB:
		case TT_MAGENTAB:
		case TT_REDB:
		case TT_YELLOWB:
		case TT_WHITEB:
		case TT_DKBLUEB:
		case TT_DKCYANB:
		case TT_DKGREENB:
		case TT_DKMAGENTAB:
		case TT_DKREDB:
		case TT_DKYELLOWB:
		case TT_DKGRAYB:
		case TT_LTGRAYB:
		case TT_BLACKE:
		case TT_BLUEE:
		case TT_CYANE:
		case TT_GREENE:
		case TT_MAGENTAE:
		case TT_REDE:
		case TT_YELLOWE:
		case TT_WHITEE:
		case TT_DKBLUEE:
		case TT_DKCYANE:
		case TT_DKGREENE:
		case TT_DKMAGENTAE:
		case TT_DKREDE:
		case TT_DKYELLOWE:
		case TT_DKGRAYE:
		case TT_LTGRAYE:
		case TT_FONTSTRB:
		case TT_FONTSTRE:
		case TT_COLORB:
		case TT_COLORE:
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
		wvError(("how can this happen, i bet you added TT_ lines to wv.h and didn't recompile wvHtml.o etc\n"));
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
