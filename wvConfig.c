#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wv.h"
#include "wvinternal.h"
#include "xmlparse.h"

#define HANDLE_B_PARA_ELE(a,b,c,d) \
wvTrace(("flag is %d\n",((PAP*)(mydata->props))->b)); \
wvTrace(("str is %s\n",mydata->sd->elements[a].str[0])); \
if ( (((PAP*)(mydata->props))->b == d) && (c == 0) ) \
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

#define HANDLE_E_PARA_ELE(a,b,c,d) \
wvTrace(("flag is %d\n",((PAP*)(mydata->props))->b)); \
wvTrace(("str is %s\n",mydata->sd->elements[a].str[0])); \
/* \
if ( (!((PAP*)(mydata->props))->b) && (c != 0) ) \
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
    {   "colspan",       TT_COLSPAN      	},
	{	"cellwidth",	 TT_CELLWIDTH		},
    {   "rowspan",       TT_ROWSPAN      	},
    {   "cellbgcolor",   TT_CELLBGCOLOR     },
    {   "parabgcolor",   TT_PARABGCOLOR     },
    {   "parafgcolor",   TT_PARAFGCOLOR     },
    {   "tablerelwidth", TT_TABLERELWIDTH	},
	{	"style",		 TT_STYLE			},
	{	"comment",		 TT_COMMENT			},
	{	"ibstanno",		 TT_IBSTANNO		},
	{	"xstUsrInitl",	 TT_xstUsrInitl		},
	{	"mmParaBefore",	 TT_mmParaBefore	},
	{	"mmParaAfter",	 TT_mmParaAfter		},
	{	"mmParaLeft",	 TT_mmParaLeft		},
	{	"mmParaRight",	 TT_mmParaRight		},
	{	"mmParaLeft1",	 TT_mmParaLeft1		},
	{	"bordertopstyle",TT_BORDERTopSTYLE	},
	{	"bordertopcolor",TT_BORDERTopCOLOR	},
	{	"borderleftstyle",TT_BORDERLeftSTYLE	},
	{	"borderleftcolor",TT_BORDERLeftCOLOR	},
	{	"borderrightstyle",TT_BORDERRightSTYLE},
	{	"borderrightcolor",TT_BORDERRightCOLOR},
	{	"borderbottomstyle",TT_BORDERBottomSTYLE},
	{	"borderbottomcolor",TT_BORDERBottomCOLOR},
	{	"mmPadTop",			TT_mmPadTop		},
	{	"mmPadRight",		TT_mmPadRight	},
	{	"mmPadBottom",		TT_mmPadBottom	},
	{	"mmPadLeft",		TT_mmPadLeft	},
	{	"mmLineHeight",		TT_mmLineHeight},

    {   "document",      TT_DOCUMENT     	},
	 {	 "picture",		  TT_PICTURE		},
	  {	  "pixPicWidth",	TT_pixPicWidth	},
	  {	  "pixPicHeight",	TT_pixPicHeight	},
	  {	  "htmlAlignGuess",	TT_htmlAlignGuess },
	  {	  "htmlNextLineGuess",TT_htmlNextLineGuess},
	 {   "section",       TT_SECTION     	},
      {   "paragraph",     TT_PARA         	},
	   {  "table",			TT_TABLE		},
	    {  "table.begin",		TT_TABLEB		},
	    {  "table.end",			TT_TABLEE		},
	   {  "row",			TT_ROW		},
	    {  "row.begin",		TT_ROWB		},
	    {  "row.end",			TT_ROWE		},
	   {  "cell",			TT_CELL		},
	    {  "cell.begin",		TT_CELLB		},
	    {  "cell.end",			TT_CELLE		},
	   {   "tableoverrides",	TT_TABLEOVERRIDES	},
		{	"ParaBefore",	 TT_ParaBefore	},
		{	"ParaAfter",	 TT_ParaAfter		},
		{	"ParaLeft",	 	TT_ParaLeft		},
		{	"ParaRight",	 TT_ParaRight		},
		{	"ParaLeft1",	 TT_ParaLeft1		},
		
       {   "block",       	 TT_BLOCK     	 	},
        {   "justification", TT_JUSTIFICATION	},
         {   "just",       	 TT_JUST     	 	},
          {   "left",       	 TT_LEFT     	 	},
          {   "right",       	 TT_RIGHT     	 	},
          {   "center",        TT_CENTER     	 	},
          {   "asian",       	 TT_ASIAN     	 	},
		 {	"margin",		TT_MARGIN			},
		  {	"paramargin",	TT_PARAMARGIN		},
		 {	"nfc",	 		TT_nfc					},
		 {	"start",		TT_START},
		 {	"numbering",	 TT_numbering		},
		  {	"arabic",	 		TT_Arabic},
		  {	"upperroman",	 	TT_UpperRoman},
		  {	"lowerroman",	 	TT_LowerRoman},
		  {	"uppercasen",	 	TT_UpperCaseN},
		  {	"lowercasen",	 	TT_LowerCaseN},
	   {	"text",	 		 TT_TEXT,			},
		{	"text.begin",	 TT_TEXTB,			},
		{	"text.end", 	 TT_TEXTE,			},
	   {	"olist",	 		 TT_OLIST,			},
		{	"olist.begin",	 TT_OLISTB,			},
		{	"olist.end", 	 TT_OLISTE,			},
	   {	"ulist",	 	 TT_ULIST,			},
		{	"ulist.begin",	 TT_ULISTB,			},
		{	"ulist.end", 	 TT_ULISTE,			},
	   {	"entry",	 	 TT_ENTRY,			},
		{	"entry.begin",	 TT_ENTRYB,			},
		{	"entry.end", 	 TT_ENTRYE,			},
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
		{	"singleu",	 	 TT_SINGLEU,		},
		 {	"singleu.begin", TT_SINGLEUB,		},
		 {	"singleu.end", 	 TT_SINGLEUE,		},
		{	"wordu",	 	 TT_WORDU,			},
		 {	"wordu.begin", 	 TT_WORDUB,			},
		 {	"wordu.end", 	 TT_WORDUE,			},
		{	"doubleu",	 	 TT_DOUBLEU,			},
		 {	"doubleu.begin", TT_DOUBLEUB,		},
		 {	"doubleu.end", 	 TT_DOUBLEUE,		},
		{	"dottedu",	 	 TT_DOTTEDU,			},
		 {	"dottedu.begin", TT_DOTTEDUB,		},
		 {	"dottedu.end", 	 TT_DOTTEDUE,		},
		{	"hiddenu",	 	 TT_HIDDENU,			},
		 {	"hiddenu.begin", TT_HIDDENUB,		},
		 {	"hiddenu.end", 	 TT_HIDDENUE,		},
		{	"thicku",	 	 TT_THICKU,			},
		 {	"thicku.begin",  TT_THICKUB,			},
		 {	"thicku.end", 	 TT_THICKUE,			},
		{	"dashu",	 	 TT_DASHU,			},
		 {	"dashu.begin", 	 TT_DASHUB,			},
		 {	"dashu.end", 	 TT_DASHUE,			},
		{	"dotu",	 	 	 TT_DOTU,			},
		 {	"dotu.begin", 	 TT_DOTUB,			},
		 {	"dotu.end", 	 TT_DOTUE,			},
		{	"dotdashu",	 	 TT_DOTDASHU,		},
		 {	"dotdashu.begin",TT_DOTDASHUB,		},
		 {	"dotdashu.end",  TT_DOTDASHUE,		},
		{	"dotdotdashu",	 TT_DOTDOTDASHU,		},
		 {	"dotdotdashu.begin",TT_DOTDOTDASHUB,	},
		 {	"dotdotdashu.end",TT_DOTDOTDASHUE,	},
		{	"waveu",	 	 TT_WAVEU,			},
		 {	"waveu.begin", 	 TT_WAVEUB,			},
		 {	"waveu.end", 	 TT_WAVEUE,			},
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
		{	"ibstrmark",	 TT_ibstRMark,		},
		{	"ibstrmarkdel",	 TT_ibstRMarkDel,	},
		{	"dttmRMark",	 TT_dttmRMark,		},
		{	"dttmRMarkDel",	 TT_dttmRMarkDel,	},
		{	"proprmark",	 TT_PropRMark,		},
		 {	"proprmark.begin",TT_PropRMarkB,	},
		 {	"proprmark.end", TT_PropRMarkE,		},
		{	"ibstPropRMark", TT_ibstPropRMark,	},
		{	"dttmPropRMark", TT_dttmPropRMark,	},
		{	"LasVegas",	 	 TT_LasVegas,		},
		 {	"LasVegas.begin",TT_LasVegasB,		},
		 {	"LasVegas.end",  TT_LasVegasE,		},
		{	"BackgroundBlink",TT_BackgroundBlink,},
		 {	"BackgroundBlink.begin",TT_BackgroundBlinkB,},
		 {	"BackgroundBlink.end", TT_BackgroundBlinkE,},
		{	"SparkleText",	 TT_SparkleText,	},
		 {	"SparkleText.begin",TT_SparkleTextB,},
		 {	"SparkleText.end",TT_SparkleTextE,	},
		{	"MarchingAnts",	 TT_MarchingAnts,	},
		 {	"MarchingAnts.begin",TT_MarchingAntsB,},
		 {	"MarchingAnts.end",TT_MarchingAntsE,},
		{	"MarchingRedAnts",TT_MarchingRedAnts,},
		 {	"MarchingRedAnts.begin",TT_MarchingRedAntsB,},
		 {	"MarchingRedAnts.end",TT_MarchingRedAntsE,},
		{	"Shimmer",	 	 TT_Shimmer,		},
		 {	"Shimmer.begin", TT_ShimmerB,		},
		 {	"Shimmer.end",   TT_ShimmerE,		},
		{	"ANIMATION",	 TT_ANIMATION,		},
		 {	"ANIMATION.begin",TT_ANIMATIONB,	},
		 {	"ANIMATION.end", TT_ANIMATIONE,		},
		{	"DispFldRMark",	 TT_DispFldRMark,	},
		 {	"DispFldRMark.begin",TT_DispFldRMarkB,},
		 {	"DispFldRMark.end", TT_DispFldRMarkE,},
		{	"ibstDispFldRMark", TT_ibstDispFldRMark,},
		{	"dttmDispFldRMark", TT_dttmDispFldRMark,},
		{	"xstDispFldRMark",	TT_xstDispFldRMark,},
		{	"border",			TT_BORDER,			},
		{	"noned",			TT_NONED,			},
		{	"singled",			TT_SINGLED,			},
		{	"thickd",			TT_THICKD,			},
		{	"doubled",			TT_DOUBLED,			},
		{	"number4d",			TT_NUMBER4D,		},
		{	"hairlined",		TT_HAIRLINED,		},
		{	"dotd",				TT_DOTD,			},
		{	"dashlargegapd",	TT_DASHLARGEGAPD,	},
		{	"dotdashd",			TT_DOTDASHD,		},
		{	"dotdotdashd",		TT_DOTDOTDASHD,		},
		{	"tripled",			TT_TRIPLED,			},
		{	"thin-thicksmallgapd",			TT_thin_thicksmallgapD,			},
		{	"thick-thinsmallgapd",			TT_thick_thinsmallgapD,			},
		{	"thin-thick-thinsmallgapd",		TT_thin_thick_thinsmallgapD,			},
		{	"thin-thickmediumgapd",			TT_thin_thickmediumgapD,			},
		{	"thick-thinmediumgapd",			TT_thick_thinmediumgapD,			},
		{	"thin-thick-thinmediumgapd",	TT_thin_thick_thinmediumgapD,			},
		{	"thin-thicklargegapd",			TT_thin_thicklargegapD,			},
		{	"thick-thinlargegapd",			TT_thick_thinlargegapD,			},
		{	"thin-thick-thinlargegapd",		TT_thin_thick_thinlargegapD,			},
		{	"waved",			TT_WAVED,			},
		{	"doublewaved",			TT_DOUBLEWAVED,			},
		{	"dashsmallgapd",			TT_DASHSMALLGAPD,			},
		{	"dashdotstrokedd",			TT_DASHDOTSTROKEDD,			},
		{	"emboss3Dd",			TT_EMBOSS3DD,			},
		{	"engrave3Dd",			TT_ENGRAVE3DD,			},
		{	"defaultd",			TT_DEFAULTD,			},
    {   "*",             TT_OTHER        	} /* must be last */
};

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
	static int bold,italic,strike,outline,smallcaps,caps,vanish,
	shadow,lowercase,emboss,imprint,dstrike,iss,kul,color,fontstr,proprmark,
	animation,delete,added,FldRMark,ilfo,ilvl=-1,ulist,olist,fintable,fttp=1,
	table,txt;
	char buffer[64];
	static LVL lvl;
	static U32 lastid=0;
	static LFO *retlfo;
	U32 k;
	int i, j;


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
		case TT_ROWSPAN:
			wvTrace(("This Para is here cell %d %d %d\n",
			mydata->whichrow,
			mydata->whichcell,
			(*mydata->vmerges)[mydata->whichrow][mydata->whichcell]));
			sprintf(buffer,"%d",(*mydata->vmerges)[mydata->whichrow][mydata->whichcell]);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_TABLERELWIDTH:
			{
			S16 width = ((PAP *)(mydata->props))->ptap.rgdxaCenter[((PAP *)(mydata->props))->ptap.itcMac] -
			((PAP *)(mydata->props))->ptap.rgdxaCenter[0];
			sprintf(buffer,"%.2f%%",wvRelativeWidth(width,mydata->asep));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			}
			break;
		case TT_CELLBGCOLOR:
			k = wvCellBgColor(mydata->whichrow,mydata->whichcell,((PAP *)(mydata->props))->ptap.itcMac,*(mydata->norows),&(((PAP *)(mydata->props))->ptap.tlp)); 
			if (k==0) k = 8;
			text = (char *)malloc(strlen(mydata->sd->elements[TT_BLACK+((k-1)*3)].str[0])+1); 
			strcpy(text,mydata->sd->elements[TT_BLACK+((k-1)*3)].str[0]); 
			str = mydata->retstring; 
			wvExpand(mydata,text,strlen(text)); 
			wvAppendStr(&str,mydata->retstring); 
			wvFree(mydata->retstring); 
			mydata->retstring = str; 
			wvFree(text); 
			mydata->currentlen = strlen(mydata->retstring); 
			break;
		case TT_PARABGCOLOR:
			k = ((PAP *)(mydata->props))->shd.icoBack;
			if (k==0) k = 8;
			text = (char *)malloc(strlen(mydata->sd->elements[TT_BLACK+((k-1)*3)].str[0])+1); 
			strcpy(text,mydata->sd->elements[TT_BLACK+((k-1)*3)].str[0]); 
			str = mydata->retstring; 
			wvExpand(mydata,text,strlen(text)); 
			wvAppendStr(&str,mydata->retstring); 
			wvFree(mydata->retstring); 
			mydata->retstring = str; 
			wvFree(text); 
			mydata->currentlen = strlen(mydata->retstring); 
			break;
		case TT_PARAFGCOLOR:
			k = ((PAP *)(mydata->props))->shd.icoFore;
			if (k==0) k = 1;
			text = (char *)malloc(strlen(mydata->sd->elements[TT_BLACK+((k-1)*3)].str[0])+1); 
			strcpy(text,mydata->sd->elements[TT_BLACK+((k-1)*3)].str[0]); 
			str = mydata->retstring; 
			wvExpand(mydata,text,strlen(text)); 
			wvAppendStr(&str,mydata->retstring); 
			wvFree(mydata->retstring); 
			mydata->retstring = str; 
			wvFree(text); 
			mydata->currentlen = strlen(mydata->retstring); 
			break;
		case TT_CELLWIDTH:
			{
			float pc;
			long over;
			long under;

			over = (((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell+1]-((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell]);
			over *=100;
			under = (((PAP*)(mydata->props))->ptap.rgdxaCenter[((PAP*)(mydata->props))->ptap.itcMac] - ((PAP*)(mydata->props))->ptap.rgdxaCenter[0]);
			
			pc = (float)over/under;

			sprintf(buffer,"%.2f%%",pc);
			
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			}
			break;
		case TT_COLSPAN:
			if (((PAP*)(mydata->props))->fInTable)
				{
				wvTrace(("this cell is %d, pos %d ends %d\n",mydata->whichcell,((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell],((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell+1]));
				i=0;
				while ( 
				(i <= itcMax) && 
#if 0
				(i <= ((PAP*)(mydata->props))->ptap.itcMac+1) && 
#endif
				(0 == cellCompEQ( &((*mydata->cellbounds)[i]), &(((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell])))
				)
					i++;

				j=i;
				while( 
				(j <= itcMax) && 
#if 0
				(j <= ((PAP*)(mydata->props))->ptap.itcMac+1) && 
#endif
				(0 == cellCompEQ( &((*mydata->cellbounds)[j]), &(((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell+1]))) 
				)
					{
					wvTrace(("j is %d %d %d\n",j,(*mydata->cellbounds)[j],((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell+1]));
					j++;
					}

				sprintf(buffer,"%d",j-i);
				wvAppendStr(&mydata->retstring,buffer);
				mydata->currentlen = strlen(mydata->retstring);
				}
			break;
		case TT_VERSION:
			wvTrace(("the version is %s\n",wv_version));
			wvAppendStr(&mydata->retstring,wv_version);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_COLORB:
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
		case TT_PARAMARGIN:
			if (fintable)
				{
				/* 
				if there are any table overrides check to make sure all are filled in,
				and if all are set to 0
				*/
				if (
				(mydata->sd->elements[TT_TABLEOVERRIDES].str) &&
				(mydata->sd->elements[TT_TABLEOVERRIDES].str[0]) &&
				(mydata->sd->elements[TT_TABLEOVERRIDES].str[1]) &&
				(mydata->sd->elements[TT_TABLEOVERRIDES].str[2]) &&
				(mydata->sd->elements[TT_TABLEOVERRIDES].str[3])
				)
					{
					if (
					(atoi(mydata->sd->elements[TT_TABLEOVERRIDES].str[0]) == 0) &&
					(atoi(mydata->sd->elements[TT_TABLEOVERRIDES].str[1]) == 0) &&
					(atoi(mydata->sd->elements[TT_TABLEOVERRIDES].str[2]) == 0) &&
					(atoi(mydata->sd->elements[TT_TABLEOVERRIDES].str[3]) == 0)
					)
						{
						wvTrace(("The table has overrides such that paramargin is to be ignored\n"));
						break;
						}
					}
				}

			if (
			((PAP*)(mydata->props))->dyaBefore ||
			((PAP*)(mydata->props))->dyaAfter ||
			((PAP*)(mydata->props))->dxaLeft ||
			((PAP*)(mydata->props))->dxaRight
				)
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_MARGIN].str[0])+1);
				strcpy(text,mydata->sd->elements[TT_MARGIN].str[0]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				}
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
		case TT_BORDERTopSTYLE:
			if (isPAPConform(&mydata->lastpap,(PAP*)(mydata->props)))
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcBetween.brcType])+1);
				strcpy(text,mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcBetween.brcType]);
				}
			else
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcTop.brcType])+1);
				strcpy(text,mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcTop.brcType]);
				}
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BORDERLeftSTYLE:
			text = (char *)malloc(strlen(mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcLeft.brcType])+1);
			strcpy(text,mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcLeft.brcType]);
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BORDERRightSTYLE:
			text = (char *)malloc(strlen(mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcRight.brcType])+1);
			strcpy(text,mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcRight.brcType]);
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BORDERBottomSTYLE:
			if (isPAPConform(mydata->nextpap,(PAP*)(mydata->props)))
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcBetween.brcType])+1);
				strcpy(text,mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcBetween.brcType]);
				}
			else
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcBottom.brcType])+1);
				strcpy(text,mydata->sd->elements[TT_BORDER].str[((PAP*)(mydata->props))->brcBottom.brcType]);
				}
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BORDERTopCOLOR:
			if (isPAPConform(&mydata->lastpap,(PAP*)(mydata->props)))
				{
				if (((PAP*)(mydata->props))->brcBetween.ico==0)
					{
					/* temp fix this in a while */
					((PAP*)(mydata->props))->brcBetween.ico++;
					}
				text = (char *)malloc(strlen(
				mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcBetween.ico-1)*3)].str[0]
				)+1);
				strcpy(text,mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcBetween.ico-1)*3)].str[0]);
				}
			else
				{
				if (((PAP*)(mydata->props))->brcTop.ico==0)
					{
					/* temp fix this in a while */
					((PAP*)(mydata->props))->brcTop.ico++;
					}
				text = (char *)malloc(strlen(
				mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcTop.ico-1)*3)].str[0]
				)+1);
				strcpy(text,mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcTop.ico-1)*3)].str[0]);
				}
			
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BORDERLeftCOLOR:
			if (((PAP*)(mydata->props))->brcLeft.ico==0)
				{
				/* temp fix this in a while */
				((PAP*)(mydata->props))->brcLeft.ico++;
				}
			text = (char *)malloc(strlen(
			mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcLeft.ico-1)*3)].str[0]
			)+1);
			strcpy(text,mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcLeft.ico-1)*3)].str[0]);
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BORDERRightCOLOR:
			if (((PAP*)(mydata->props))->brcRight.ico==0)
				{
				/* temp fix this in a while */
				((PAP*)(mydata->props))->brcRight.ico++;
				}
			text = (char *)malloc(strlen(
			mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcRight.ico-1)*3)].str[0]
			)+1);
			strcpy(text,mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcRight.ico-1)*3)].str[0]);
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_BORDERBottomCOLOR:
			if (isPAPConform(mydata->nextpap,(PAP*)(mydata->props)))
				{
				if (((PAP*)(mydata->props))->brcBetween.ico==0)
					{
					/* temp fix this in a while */
					((PAP*)(mydata->props))->brcBetween.ico++;
					}
				text = (char *)malloc(strlen(
				mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcBetween.ico-1)*3)].str[0]
				)+1);
				strcpy(text,mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcBetween.ico-1)*3)].str[0]);
				}
			else
				{
				if (((PAP*)(mydata->props))->brcBottom.ico==0)
					{
					/* temp fix this in a while */
					((PAP*)(mydata->props))->brcBottom.ico++;
					}
				text = (char *)malloc(strlen(
				mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcBottom.ico-1)*3)].str[0]
				)+1);
				strcpy(text,mydata->sd->elements[TT_BLACK+((((PAP*)(mydata->props))->brcBottom.ico-1)*3)].str[0]);
				}
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_START:
			sprintf(buffer,"%d",(*mydata->liststartnos)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl]);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_nfc:
		    wvTrace(("nfc is %d\n",lvl.lvlf.nfc));
			if (lvl.lvlf.nfc < 5)
				{
				wvTrace(("str is %s\n",mydata->sd->elements[TT_numbering].str[lvl.lvlf.nfc]));
				text = (char *)malloc(strlen(mydata->sd->elements[TT_numbering].str[lvl.lvlf.nfc])+1);
				strcpy(text,mydata->sd->elements[TT_numbering].str[lvl.lvlf.nfc]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				}
			break;
		case TT_ULISTE:
			if ( (ilfo != 0) && (ilfo != ((PAP*)(mydata->props))->ilfo) )
				{
				if (ulist)
					{
					while (ilvl >= 0)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_ULIST].str[1]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_ULIST].str[1])+1);
						strcpy(text,mydata->sd->elements[TT_ULIST].str[1]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						ilvl--;
						}
					ilfo = 0;
					ilvl = -1;
					wvReleaseLVL(&lvl);
					wvInitLVL(&lvl);
					ulist=0;
					}
				}
			break;
		case TT_ULISTB:
		    wvTrace(("ilfo is %d\n",((PAP*)(mydata->props))->ilfo));
			if (wvIsListEntry((PAP*)(mydata->props),wvQuerySupported(mydata->fib,NULL)))
				{
				wvReleaseLVL(&lvl);
				wvInitLVL(&lvl);
				if (wvGetListEntryInfo(mydata->finallvl,mydata->liststartnos,&lvl,&retlfo,(PAP*)(mydata->props),mydata->lfo,mydata->lfolvl,mydata->lvl,mydata->nolfo, mydata->lst, *mydata->noofLST,wvQuerySupported(mydata->fib,NULL)))
					{
					wvError(("aborted list entry, more work needed obviously\n"));
					return;
					}
				else
					{
					U32 i;
					wvTrace(("start number is %d, type is %d\n",lvl.lvlf.iStartAt,lvl.lvlf.nfc));
					if ((*mydata->liststartnos)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl] == 0xffffffffL) 
						{
						(*mydata->liststartnos)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl] = lvl.lvlf.iStartAt;
						wvTrace(("start number set to %d\n",(*mydata->liststartnos)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl]));
						wvCopyLVL(&((*mydata->finallvl)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl]),&lvl);
						}
					for (i=0;i<*(mydata->nolfo)*9;i++)
						{
						if ( (i%9 > ((PAP*)(mydata->props))->ilvl) && ((*mydata->finallvl)[i].lvlf.fNoRestart == 0) )
							(*mydata->liststartnos)[i] = (*mydata->finallvl)[i].lvlf.iStartAt;
						}
					}

				if (((PAP*)(mydata->props))->ilvl > ilvl)
					while (ilvl < ((PAP*)(mydata->props))->ilvl)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_ULIST].str[0]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_ULIST].str[0])+1);
						strcpy(text,mydata->sd->elements[TT_ULIST].str[0]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						ilvl++;
						}
				else if (((PAP*)(mydata->props))->ilvl < ilvl)
					while (ilvl > ((PAP*)(mydata->props))->ilvl)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_ULIST].str[1]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_ULIST].str[1])+1);
						strcpy(text,mydata->sd->elements[TT_ULIST].str[1]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						ilvl--;
						}
				ilfo = ((PAP*)(mydata->props))->ilfo;
				ilvl = ((PAP*)(mydata->props))->ilvl;
				lastid = retlfo->lsid;
				ulist=1;
				wvTrace(("start number still set to %d\n",(*mydata->liststartnos)[(ilfo-1)*9+ilvl]));
				}
			break;
		case TT_OLISTE:
		    wvTrace(("ilfo 1 is %d\n",((PAP*)(mydata->props))->ilfo));
		    wvTrace(("ilfo 2 is %d\n",ilfo));

			if (wvIsListEntry((PAP*)(mydata->props),wvQuerySupported(mydata->fib,NULL)))
				{
				wvReleaseLVL(&lvl);
				wvInitLVL(&lvl);
				wvGetListEntryInfo(mydata->finallvl,mydata->liststartnos,&lvl,&retlfo,(PAP*)(mydata->props),mydata->lfo,mydata->lfolvl,mydata->lvl,mydata->nolfo, mydata->lst, *mydata->noofLST,wvQuerySupported(mydata->fib,NULL));
				}
			
			if ( (ilfo != 0) && (ilfo != ((PAP*)(mydata->props))->ilfo) )
				{
				if (olist)
					{
					while (ilvl >= 0)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_OLIST].str[1]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_OLIST].str[1])+1);
						strcpy(text,mydata->sd->elements[TT_OLIST].str[1]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						ilvl--;
						}
					ilfo = 0;
					ilvl = -1;
					olist=0;
					wvReleaseLVL(&lvl);
					wvInitLVL(&lvl);
					}
				}
			break;
		case TT_TEXTB:
			text = (char *)malloc(strlen(mydata->sd->elements[TT_TEXT].str[0])+1);
			strcpy(text,mydata->sd->elements[TT_TEXT].str[0]);
			str = mydata->retstring;
			wvExpand(mydata,text,strlen(text));
			wvAppendStr(&str,mydata->retstring);
			wvFree(mydata->retstring);
			mydata->retstring = str;
			wvFree(text);
			mydata->currentlen = strlen(mydata->retstring);
			txt=1;
			break;
		case TT_TEXTE:
			if (txt)
				{
				text = (char *)malloc(strlen(mydata->sd->elements[TT_TEXT].str[1])+1);
				strcpy(text,mydata->sd->elements[TT_TEXT].str[1]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				txt=0;
				}
			break;
		case TT_OLISTB:
		    wvTrace(("ilfo is %d\n",((PAP*)(mydata->props))->ilfo));
			if (wvIsListEntry((PAP*)(mydata->props),wvQuerySupported(mydata->fib,NULL)))
				{
				wvReleaseLVL(&lvl);
				wvInitLVL(&lvl);
				if (wvGetListEntryInfo(mydata->finallvl,mydata->liststartnos,&lvl,&retlfo,(PAP*)(mydata->props),mydata->lfo,mydata->lfolvl,mydata->lvl,mydata->nolfo, mydata->lst, *mydata->noofLST,wvQuerySupported(mydata->fib,NULL)))
					{
					wvError(("aborted list entry, more work needed obviously\n"));
					return;
					}
				else
					{
					U32 i;
					wvTrace(("start number is %d, type is %d\n",lvl.lvlf.iStartAt,lvl.lvlf.nfc));
					wvTrace(("lfo is %d, ilvi is %d\n",((PAP*)(mydata->props))->ilfo,((PAP*)(mydata->props))->ilvl));


					if ((*mydata->liststartnos)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl] == 0xffffffffL) 
						{
						(*mydata->liststartnos)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl] = lvl.lvlf.iStartAt;
						wvCopyLVL(&((*mydata->finallvl)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl]),&lvl);
						}

					for (i=0;i<*(mydata->nolfo)*9;i++)
						{
						wvTrace(("new list, restart is %d\n",(*mydata->finallvl)[i].lvlf.fNoRestart));
						if ( (i%9 > ((PAP*)(mydata->props))->ilvl) && ((*mydata->finallvl)[i].lvlf.fNoRestart == 0) )
							(*mydata->liststartnos)[i] = (*mydata->finallvl)[i].lvlf.iStartAt;
						}

					if ((lvl.numbertext == NULL) || (lvl.numbertext[0] <= 1))
						{
						return;
						}

					}


				if ( (((PAP*)(mydata->props))->ilvl == ilvl) && (retlfo->lsid != lastid) )
					{
					if (ulist)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_ULIST].str[1]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_ULIST].str[1])+1);
						strcpy(text,mydata->sd->elements[TT_ULIST].str[1]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						}
					else if (olist)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_OLIST].str[1]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_OLIST].str[1])+1);
						strcpy(text,mydata->sd->elements[TT_OLIST].str[1]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						}
					ilvl--;
					}
				
				if (((PAP*)(mydata->props))->ilvl > ilvl)
					while (ilvl < ((PAP*)(mydata->props))->ilvl)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_OLIST].str[0]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_OLIST].str[0])+1);
						strcpy(text,mydata->sd->elements[TT_OLIST].str[0]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						ilvl++;
						}
				else if (((PAP*)(mydata->props))->ilvl < ilvl)
					while (ilvl > ((PAP*)(mydata->props))->ilvl)
						{
						wvTrace(("str is %s\n",mydata->sd->elements[TT_OLIST].str[1]));
						text = (char *)malloc(strlen(mydata->sd->elements[TT_OLIST].str[1])+1);
						strcpy(text,mydata->sd->elements[TT_OLIST].str[1]);
						str = mydata->retstring;
						wvExpand(mydata,text,strlen(text));
						wvAppendStr(&str,mydata->retstring);
						wvFree(mydata->retstring);
						mydata->retstring = str;
						wvFree(text);
						mydata->currentlen = strlen(mydata->retstring);
						ilvl--;
						}
				ilfo = ((PAP*)(mydata->props))->ilfo;
				ilvl = ((PAP*)(mydata->props))->ilvl;
				lastid = retlfo->lsid;
				olist=1;
				wvTrace(("start number still set to %d\n",(*mydata->liststartnos)[(ilfo-1)*9+ilvl]));
				}
			break;
		case TT_ENTRYB:
			fflush(stdout);
		    wvTrace(("ilvl is %d\n",((PAP*)(mydata->props))->ilvl));
			if ( ilfo )
				{
				wvTrace(("str is %s\n",mydata->sd->elements[TT_ENTRY].str[0]));
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ENTRY].str[0])+1);
				strcpy(text,mydata->sd->elements[TT_ENTRY].str[0]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				/*
				mydata->liststartnos[(ilfo-1)*9+ilvl]++;
				*/
				}
			break;
		case TT_ENTRYE:
		    wvTrace(("ilvl is %d\n",((PAP*)(mydata->props))->ilvl));
			if ( ilfo )
				{
				wvTrace(("str is %s\n",mydata->sd->elements[TT_ENTRY].str[1]));
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ENTRY].str[1])+1);
				strcpy(text,mydata->sd->elements[TT_ENTRY].str[1]);
				str = mydata->retstring;
				wvExpand(mydata,text,strlen(text));
				wvAppendStr(&str,mydata->retstring);
				wvFree(mydata->retstring);
				mydata->retstring = str;
				wvFree(text);
				mydata->currentlen = strlen(mydata->retstring);
				wvTrace(("no in list is %d\n",9*(*(mydata->nolfo))));
				/*
				(*mydata->liststartnos)[(((PAP*)(mydata->props))->ilfo-1)*9+((PAP*)(mydata->props))->ilvl]++;
				*/
				(*mydata->liststartnos)[(ilfo-1)*9+ilvl]++;
				}
			break;
		case TT_BOLDB:
			HANDLE_B_CHAR_ELE(TT_BOLD,fBold,bold,1)
			break;
		case TT_DispFldRMarkB:
			HANDLE_B_CHAR_ELE(TT_DispFldRMark,fDispFldRMark,FldRMark,1)
			break;
		case TT_RMarkDelB:
			HANDLE_B_CHAR_ELE(TT_RMarkDel,fRMarkDel,delete,1)
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
			HANDLE_E_CHAR_ELE(TT_RMarkDel,fRMarkDel,delete,1)
			break;
		case TT_DispFldRMarkE:
			HANDLE_E_CHAR_ELE(TT_DispFldRMark,fDispFldRMark,FldRMark,1)
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
			HANDLE_B_CHAR_ELE(TT_RMark,fRMark,added,1)
			break;
		case TT_RMarkE:
			HANDLE_E_CHAR_ELE(TT_RMark,fRMark,added,1)
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

		case TT_SINGLEUB:
		case TT_WORDUB:
		case TT_DOUBLEUB:
		case TT_DOTTEDUB:
		case TT_HIDDENUB:
		case TT_THICKUB:
		case TT_DASHUB:
		case TT_DOTUB:
		case TT_DOTDASHUB:
		case TT_DOTDOTDASHUB:
		case TT_WAVEUB:
			HANDLE_B_CHAR_ELE(s_Tokens[tokenIndex].m_type-1,kul,kul,(s_Tokens[tokenIndex].m_type-TT_SINGLEUB)/3+1)
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


		case TT_SINGLEUE:
		case TT_WORDUE:
		case TT_DOUBLEUE:
		case TT_DOTTEDUE:
		case TT_HIDDENUE:
		case TT_THICKUE:
		case TT_DASHUE:
		case TT_DOTUE:
		case TT_DOTDASHUE:
		case TT_DOTDOTDASHUE:
		case TT_WAVEUE:
			HANDLE_E_CHAR_ELE(s_Tokens[tokenIndex].m_type-2,kul,kul,(s_Tokens[tokenIndex].m_type-TT_SINGLEUE)/3+1)
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

		case TT_LasVegasB:
		case TT_BackgroundBlinkB:
		case TT_SparkleTextB:
		case TT_MarchingAntsB:
		case TT_MarchingRedAntsB:
		case TT_ShimmerB:
			HANDLE_B_CHAR_ELE(s_Tokens[tokenIndex].m_type-1,sfxtText,animation,(s_Tokens[tokenIndex].m_type-TT_LasVegasB)/3+1)
			break;

		case TT_LasVegasE:
		case TT_BackgroundBlinkE:
		case TT_SparkleTextE:
		case TT_MarchingAntsE:
		case TT_MarchingRedAntsE:
		case TT_ShimmerE:
			HANDLE_E_CHAR_ELE(s_Tokens[tokenIndex].m_type-2,sfxtText,animation,(s_Tokens[tokenIndex].m_type-TT_LasVegasE)/3+1)
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

		case TT_ANIMATIONB:
			wvTrace(("flag is %d\n",((CHP*)(mydata->props))->sfxtText)); 
			wvTrace(("str is %s\n",mydata->sd->elements[TT_ANIMATION].str[0])); 
			if ( (((CHP*)(mydata->props))->sfxtText) && (animation == 0) ) 
				{ 
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ANIMATION].str[0])+1); 
				strcpy(text,mydata->sd->elements[TT_ANIMATION].str[0]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring); 
				animation=1; 
				}
			
			break;
		case TT_ANIMATIONE:
			wvTrace(("flag is %d\n",((CHP*)(mydata->props))->sfxtText)); 
			wvTrace(("str is %s\n",mydata->sd->elements[TT_ANIMATION].str[0])); 
			if (animation) 
				{ 
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ANIMATION].str[1])+1); 
				strcpy(text,mydata->sd->elements[TT_ANIMATION].str[1]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring); 
				animation=0; 
				}
			break;

		case TT_IBSTANNO:
			sprintf(buffer,"%d",((ATRD*)(mydata->props))->ibst);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;

		case TT_xstUsrInitl:
			str = wvWideStrToMB(((ATRD*)(mydata->props))->xstUsrInitl+1);
			sprintf(buffer,"%s",str);
			wvFree(str);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmParaBefore:
			if (fintable)
				{
				if ((mydata->sd->elements[TT_TABLEOVERRIDES].str) && (mydata->sd->elements[TT_TABLEOVERRIDES].str[0]))
					{
					text = (char *)malloc(strlen(mydata->sd->elements[TT_TABLEOVERRIDES].str[0])+1); 
					strcpy(text,mydata->sd->elements[TT_TABLEOVERRIDES].str[0]); 
					str = mydata->retstring; 

					wvExpand(mydata,text,strlen(text)); 
					sprintf(buffer,"%.2fmm",wvTwipsToMM(atoi(mydata->retstring)));
					wvFree(mydata->retstring); 

					mydata->retstring = str; 
					wvFree(text); 
					wvAppendStr(&mydata->retstring,buffer);
					mydata->currentlen = strlen(mydata->retstring); 
					break;
					}
				}
			sprintf(buffer,"%.2fmm",wvTwipsToMM(((PAP*)(mydata->props))->dyaBefore));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmParaAfter:
			if (fintable)
				{
				if ((mydata->sd->elements[TT_TABLEOVERRIDES].str) && (mydata->sd->elements[TT_TABLEOVERRIDES].str[2]))
					{
					text = (char *)malloc(strlen(mydata->sd->elements[TT_TABLEOVERRIDES].str[2])+1); 
					strcpy(text,mydata->sd->elements[TT_TABLEOVERRIDES].str[2]); 
					str = mydata->retstring; 

					wvExpand(mydata,text,strlen(text)); 
					sprintf(buffer,"%.2fmm",wvTwipsToMM(atoi(mydata->retstring)));
					wvFree(mydata->retstring); 

					mydata->retstring = str; 
					wvFree(text); 
					wvAppendStr(&mydata->retstring,buffer);
					mydata->currentlen = strlen(mydata->retstring); 
					break;
					}
				}
			sprintf(buffer,"%.2fmm",wvTwipsToMM(((PAP*)(mydata->props))->dyaAfter));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmParaLeft:
			if (fintable)
				{
				if ((mydata->sd->elements[TT_TABLEOVERRIDES].str) && (mydata->sd->elements[TT_TABLEOVERRIDES].str[3]))
					{
					text = (char *)malloc(strlen(mydata->sd->elements[TT_TABLEOVERRIDES].str[3])+1); 
					strcpy(text,mydata->sd->elements[TT_TABLEOVERRIDES].str[3]); 
					str = mydata->retstring; 

					wvExpand(mydata,text,strlen(text)); 
					sprintf(buffer,"%.2fmm",wvTwipsToMM(atoi(mydata->retstring)));
					wvFree(mydata->retstring); 

					mydata->retstring = str; 
					wvFree(text); 
					wvAppendStr(&mydata->retstring,buffer);
					mydata->currentlen = strlen(mydata->retstring); 
					break;
					}
				}
			sprintf(buffer,"%.2fmm",wvTwipsToMM(((PAP*)(mydata->props))->dxaLeft));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmParaRight:
			if (fintable)
				{
				if ((mydata->sd->elements[TT_TABLEOVERRIDES].str) && (mydata->sd->elements[TT_TABLEOVERRIDES].str[1]))
					{
					text = (char *)malloc(strlen(mydata->sd->elements[TT_TABLEOVERRIDES].str[1])+1); 
					strcpy(text,mydata->sd->elements[TT_TABLEOVERRIDES].str[1]); 
					str = mydata->retstring; 

					wvExpand(mydata,text,strlen(text)); 
					sprintf(buffer,"%.2fmm",wvTwipsToMM(atoi(mydata->retstring)));
					wvFree(mydata->retstring); 

					mydata->retstring = str; 
					wvFree(text); 
					wvAppendStr(&mydata->retstring,buffer);
					mydata->currentlen = strlen(mydata->retstring); 
					break;
					}
				}
			sprintf(buffer,"%.2fmm",wvTwipsToMM(((PAP*)(mydata->props))->dxaRight));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmParaLeft1:
			if (fintable)
				{
				if ((mydata->sd->elements[TT_TABLEOVERRIDES].str) && (mydata->sd->elements[TT_TABLEOVERRIDES].str[4]))
					{
					text = (char *)malloc(strlen(mydata->sd->elements[TT_TABLEOVERRIDES].str[4])+1); 
					strcpy(text,mydata->sd->elements[TT_TABLEOVERRIDES].str[4]); 
					str = mydata->retstring; 

					wvExpand(mydata,text,strlen(text)); 
					sprintf(buffer,"%.2fmm",wvTwipsToMM(atoi(mydata->retstring)));
					wvFree(mydata->retstring); 

					mydata->retstring = str; 
					wvFree(text); 
					wvAppendStr(&mydata->retstring,buffer);
					mydata->currentlen = strlen(mydata->retstring); 
					break;
					}
				}
			sprintf(buffer,"%.2fmm",wvTwipsToMM(((PAP*)(mydata->props))->dxaLeft1));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmPadTop:
			if (isPAPConform(&mydata->lastpap,(PAP*)(mydata->props)))
				sprintf(buffer,"%.2fmm",wvPointsToMM(((PAP*)(mydata->props))->brcBetween.dptSpace));
			else
				sprintf(buffer,"%.2fmm",wvPointsToMM(((PAP*)(mydata->props))->brcTop.dptSpace));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmPadRight:
			sprintf(buffer,"%.2fmm",wvPointsToMM(((PAP*)(mydata->props))->brcRight.dptSpace));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmPadBottom:
			if (isPAPConform(mydata->nextpap,(PAP*)(mydata->props)))
				sprintf(buffer,"%.2fmm",wvPointsToMM(((PAP*)(mydata->props))->brcBetween.dptSpace));
			else
				sprintf(buffer,"%.2fmm",wvPointsToMM(((PAP*)(mydata->props))->brcBottom.dptSpace));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmPadLeft:
			sprintf(buffer,"%.2fmm",wvPointsToMM(((PAP*)(mydata->props))->brcLeft.dptSpace));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_mmLineHeight:
			if (wvTwipsToMM(((PAP*)(mydata->props))->lspd.dyaLine))
				sprintf(buffer,"%fmm",fabs(wvTwipsToMM(((PAP*)(mydata->props))->lspd.dyaLine)));
			else
				sprintf(buffer,"normal");
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;

		case TT_pixPicWidth:
			{
			FSPA *fspa = (FSPA *)(mydata->props);
			sprintf(buffer,"%d",(int)(wvTwipsToHPixels(fspa->xaRight - fspa->xaLeft)+0.5));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			}
			break;
		case TT_pixPicHeight:
			{
			FSPA *fspa = (FSPA *)(mydata->props);
			sprintf(buffer,"%d",(int)(wvTwipsToVPixels(fspa->yaBottom-fspa->yaTop)+0.5));
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			}
			break;
		case TT_htmlNextLineGuess:
			{
			FSPA *fspa = (FSPA *)(mydata->props);
			if (fspa->wr == 1)
				sprintf(buffer,"<br>");
			else
				sprintf(buffer,"\n");
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			}
			break;
		case TT_htmlAlignGuess:
			{
			FSPA *fspa = (FSPA *)(mydata->props);
			switch(fspa->wr)
				{
				default:
				case 1:
				case 3:
					sprintf(buffer,"none");
					break;
				case 0:
				case 5:
					sprintf(buffer,"left");
					break;
				case 2:
				case 4:
					if (fspa->wrk == 1)
						sprintf(buffer,"right");
					else
						sprintf(buffer,"left");
					break;
				}
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			}
			break;
		case TT_ibstRMark:
			sprintf(buffer,"%d",((CHP*)(mydata->props))->ibstRMark);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_ibstRMarkDel:
			sprintf(buffer,"%d",((CHP*)(mydata->props))->ibstRMarkDel);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_ibstDispFldRMark:
			sprintf(buffer,"%d",((CHP*)(mydata->props))->ibstDispFldRMark);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_dttmRMark:
			wvAppendStr(&mydata->retstring,wvDTTMtoUnix(&(((CHP*)(mydata->props))->dttmRMark)));
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_dttmRMarkDel:
			wvAppendStr(&mydata->retstring,wvDTTMtoUnix(&(((CHP*)(mydata->props))->dttmRMarkDel)));
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_dttmDispFldRMark:
			wvAppendStr(&mydata->retstring,wvDTTMtoUnix(&(((CHP*)(mydata->props))->dttmDispFldRMark)));
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_xstDispFldRMark:
			text = wvWideStrToMB(((CHP*)(mydata->props))->xstDispFldRMark);
			wvAppendStr(&mydata->retstring,text);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_PropRMarkB:
			HANDLE_B_CHAR_ELE(TT_PropRMark,fPropRMark,proprmark,1)
			break;
		case TT_PropRMarkE:
			HANDLE_E_CHAR_ELE(TT_PropRMark,fPropRMark,proprmark,1)
			break;
		case TT_ibstPropRMark:
			wvTrace(("flag is %d\n",((CHP*)(mydata->props))->ibstPropRMark)); 
			sprintf(buffer,"%d",((CHP*)(mydata->props))->ibstPropRMark);
			wvAppendStr(&mydata->retstring,buffer);
			mydata->currentlen = strlen(mydata->retstring);
			break;
		case TT_dttmPropRMark:
			wvAppendStr(&mydata->retstring,wvDTTMtoUnix(&(((CHP*)(mydata->props))->dttmPropRMark)));
			mydata->currentlen = strlen(mydata->retstring);
			break;

		case TT_TABLEB:
			if ( (((PAP*)(mydata->props))->fInTable) && (table == 0) ) 
				{ 
				text = (char *)malloc(strlen(mydata->sd->elements[TT_TABLE].str[0])+1); 
				strcpy(text,mydata->sd->elements[TT_TABLE].str[0]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring);
				table=1;
				}
			break;
		case TT_ROWB:
			if ( (((PAP*)(mydata->props))->fInTable) && (((PAP*)(mydata->props))->fTtp == 0) && (fttp == 1) ) 
				{ 
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ROW].str[0])+1); 
				strcpy(text,mydata->sd->elements[TT_ROW].str[0]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring);
				fttp=0;
				}
			break;
		case TT_CELLB:
			if ( (fintable != 1) && (((PAP*)(mydata->props))->fInTable == 1) )
				{
				wvTrace(("the current cell is %d\n",mydata->whichcell));
				wvTrace(("the end boundary is %d\n",  ((PAP*)(mydata->props))->ptap.rgdxaCenter[mydata->whichcell+1] ));
				wvTrace(("the table look for this cell is %d\n",((PAP*)(mydata->props))->ptap.tlp.itl));
				if (((PAP*)(mydata->props))->ptap.tlp.itl)
					{
					wvTrace(("table look is %d\n",((PAP*)(mydata->props))->ptap.tlp.itl));
					}
				HANDLE_B_PARA_ELE(TT_CELL,fInTable,fintable,1)
				}
			break;
		case TT_CELLE:
			if (*(mydata->endcell))
				{
				if (fintable == 1)
					{
					mydata->whichcell++;
					wvTrace(("inc whichcell to %d\n",mydata->whichcell));
					}
				HANDLE_E_PARA_ELE(TT_CELL,fInTable,fintable,1)
				*(mydata->endcell)=0;
				}
			break;
		case TT_ROWE:	
			if ( (((PAP*)(mydata->props))->fTtp == 1) && (fttp == 0) ) 
				{ 
				mydata->whichrow++;
				text = (char *)malloc(strlen(mydata->sd->elements[TT_ROW].str[1])+1); 
				strcpy(text,mydata->sd->elements[TT_ROW].str[1]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring);
				mydata->whichcell=0;
				fttp=1;
				}
			break;

		case TT_TABLEE:
			if ( (((PAP*)(mydata->props))->fInTable == 0) && (table == 1) ) 
				{ 
				text = (char *)malloc(strlen(mydata->sd->elements[TT_TABLE].str[1])+1); 
				strcpy(text,mydata->sd->elements[TT_TABLE].str[1]); 
				str = mydata->retstring; 
				wvExpand(mydata,text,strlen(text)); 
				wvAppendStr(&str,mydata->retstring); 
				wvFree(mydata->retstring); 
				mydata->retstring = str; 
				wvFree(text); 
				mydata->currentlen = strlen(mydata->retstring);
				table=0;
				mydata->whichrow=0;
				*(mydata->intable)=0;
				}
			break;
	
		}

	}

void startElement(void *userData, const char *name, const char **atts)
	{
	unsigned int nAtts;
	const XML_Char **p;
	unsigned int tokenIndex,i;
	state_data *mydata = (state_data *)userData;

	p = atts;
	while (*p)
	++p;
	nAtts = (p - atts) >> 1;

	tokenIndex = s_mapNameToToken(name);
	if (mydata->current != NULL)
		wvTrace(("current not NULL\n"));
	wvTrace(("element %s started\n",name));
	switch (s_Tokens[tokenIndex].m_type)
		{
		case TT_DOCUMENT:
		case TT_PARA:
		case TT_COMMENT:
		case TT_SECTION:
		case TT_BOLD:
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
		case TT_SINGLEU:
		case TT_WORDU:
		case TT_DOUBLEU:
		case TT_DOTTEDU:
		case TT_HIDDENU:
		case TT_THICKU:
		case TT_DASHU:
		case TT_DOTU:
		case TT_DOTDASHU:
		case TT_DOTDOTDASHU:
		case TT_WAVEU:
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
		case TT_ANIMATION:
		case TT_PropRMark:
		case TT_LasVegas:
		case TT_BackgroundBlink:
		case TT_SparkleText:
		case TT_MarchingAnts:
		case TT_MarchingRedAnts:
		case TT_Shimmer:
		case TT_DispFldRMark:
		case TT_OLIST:
		case TT_TEXT:
		case TT_ULIST:
		case TT_ENTRY:
		case TT_TABLE:
		case TT_ROW:
		case TT_CELL:
			mydata->elements[s_Tokens[tokenIndex].m_type].str = (char **)malloc(sizeof(char *)*2);
			mydata->elements[s_Tokens[tokenIndex].m_type].nostr=2;
			for(i=0;i<2;i++)
				mydata->elements[s_Tokens[tokenIndex].m_type].str[i] = NULL;
			mydata->currentele = &(mydata->elements[s_Tokens[tokenIndex].m_type]);
			break;
		case TT_PICTURE:
		case TT_MARGIN:
			mydata->elements[s_Tokens[tokenIndex].m_type].str = (char **)malloc(sizeof(char *)*1);
			mydata->elements[s_Tokens[tokenIndex].m_type].nostr=1;
			mydata->elements[s_Tokens[tokenIndex].m_type].str[0] = NULL;
			mydata->currentele = &(mydata->elements[s_Tokens[tokenIndex].m_type]);
			break;
		case TT_CHAR:
			mydata->elements[s_Tokens[tokenIndex].m_type].str = (char **)malloc(sizeof(char *)*2);
			mydata->elements[s_Tokens[tokenIndex].m_type].nostr=2;
			for(i=0;i<2;i++)
				mydata->elements[s_Tokens[tokenIndex].m_type].str[i] = NULL;
			mydata->currentele = &(mydata->elements[s_Tokens[tokenIndex].m_type]);
			break;
			break;
		case TT_TABLEOVERRIDES:
		case TT_JUSTIFICATION:
		case TT_numbering:
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
		case TT_BORDER:
			mydata->elements[s_Tokens[tokenIndex].m_type].str = (char **)malloc(sizeof(char *)*27);
			mydata->elements[s_Tokens[tokenIndex].m_type].nostr=27;
			for(i=0;i<27;i++)
				mydata->elements[s_Tokens[tokenIndex].m_type].str[i] = NULL;
			mydata->currentele = &(mydata->elements[s_Tokens[tokenIndex].m_type]);
			break;

		case TT_STYLE:
			wvTrace(("style element, no atts is %d\n",nAtts));
			for(i=0;i<nAtts;i++)
				wvTrace(("%s is %s\n",atts[i*2],atts[(i*2)+1]));
			break;

		case TT_BEGIN:
		case TT_LEFT:
		case TT_Arabic:
		case TT_NONED:
		case TT_ParaBefore:
			mydata->current = &(mydata->currentele->str[0]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_END:
		case TT_CENTER:
		case TT_UpperRoman:
		case TT_SINGLED:
		case TT_ParaRight:
			mydata->current = &(mydata->currentele->str[1]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_RIGHT:
		case TT_LowerRoman:
		case TT_THICKD:
		case TT_ParaAfter:
			mydata->current = &(mydata->currentele->str[2]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BLOCK:
		case TT_UpperCaseN:
		case TT_DOUBLED:
		case TT_ParaLeft:
			mydata->current = &(mydata->currentele->str[3]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ASIAN:
		case TT_LowerCaseN:
		case TT_NUMBER4D:
		case TT_ParaLeft1:
			mydata->current = &(mydata->currentele->str[4]);
			wvAppendStr(mydata->current,"<begin>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_HAIRLINED:
		case TT_DOTD:
		case TT_DASHLARGEGAPD:
		case TT_DOTDASHD:
		case TT_DOTDOTDASHD:
		case TT_TRIPLED:
		case TT_thin_thicksmallgapD:
		case TT_thick_thinsmallgapD:
		case TT_thin_thick_thinsmallgapD:
		case TT_thin_thickmediumgapD:
		case TT_thick_thinmediumgapD:
		case TT_thin_thick_thinmediumgapD:
		case TT_thin_thicklargegapD:
		case TT_thick_thinlargegapD:
		case TT_thin_thick_thinlargegapD:
		case TT_WAVED:
		case TT_DOUBLEWAVED:
		case TT_DASHSMALLGAPD:
		case TT_DASHDOTSTROKEDD:
		case TT_EMBOSS3DD:
		case TT_ENGRAVE3DD:
		case TT_DEFAULTD:
			mydata->current = &(mydata->currentele->str[5+(s_Tokens[tokenIndex].m_type-TT_HAIRLINED)]);
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
		case TT_COLSPAN:
			wvAppendStr(mydata->current,"<colspan/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_CELLWIDTH:
			wvAppendStr(mydata->current,"<cellwidth/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_TABLERELWIDTH:
			wvAppendStr(mydata->current,"<tablerelwidth/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_CELLBGCOLOR:
			wvAppendStr(mydata->current,"<cellbgcolor/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_PARABGCOLOR:
			wvAppendStr(mydata->current,"<parabgcolor/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_PARAFGCOLOR:
			wvAppendStr(mydata->current,"<parafgcolor/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ROWSPAN:
			wvAppendStr(mydata->current,"<rowspan/>");
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
		case TT_DispFldRMarkB:
			wvAppendStr(mydata->current,"<DispFldRMark.begin/>");
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
		case TT_DispFldRMarkE:
			wvAppendStr(mydata->current,"<DispFldRMark.end/>");
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
		case TT_PARAMARGIN:
			wvAppendStr(mydata->current,"<paramargin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERTopSTYLE:
			wvAppendStr(mydata->current,"<bordertopstyle/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERTopCOLOR:
			wvAppendStr(mydata->current,"<bordertopcolor/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERLeftSTYLE:
			wvAppendStr(mydata->current,"<borderleftstyle/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERLeftCOLOR:
			wvAppendStr(mydata->current,"<borderleftcolor/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERRightSTYLE:
			wvAppendStr(mydata->current,"<borderrightstyle/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERRightCOLOR:
			wvAppendStr(mydata->current,"<borderrightcolor/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERBottomSTYLE:
			wvAppendStr(mydata->current,"<borderbottomstyle/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BORDERBottomCOLOR:
			wvAppendStr(mydata->current,"<borderbottomcolor/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_nfc:
			wvAppendStr(mydata->current,"<nfc/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_START:
			wvAppendStr(mydata->current,"<start/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_COLORB:
			wvAppendStr(mydata->current,"<color.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_SINGLEUB:
			wvAppendStr(mydata->current,"<singleu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WORDUB:
			wvAppendStr(mydata->current,"<wordu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOUBLEUB:
			wvAppendStr(mydata->current,"<doubleu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTTEDUB:
			wvAppendStr(mydata->current,"<dottedu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_HIDDENUB:
			wvAppendStr(mydata->current,"<hiddenu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_THICKUB:
			wvAppendStr(mydata->current,"<thicku.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DASHUB:
			wvAppendStr(mydata->current,"<dashu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTUB:
			wvAppendStr(mydata->current,"<dotu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDASHUB:
			wvAppendStr(mydata->current,"<dotdashu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDOTDASHUB:
			wvAppendStr(mydata->current,"<dotdotdashu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WAVEUB:
			wvAppendStr(mydata->current,"<waveu.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SINGLEUE:
			wvAppendStr(mydata->current,"<singleu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WORDUE:
			wvAppendStr(mydata->current,"<wordu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOUBLEUE:
			wvAppendStr(mydata->current,"<doubleu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTTEDUE:
			wvAppendStr(mydata->current,"<dottedu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_HIDDENUE:
			wvAppendStr(mydata->current,"<hiddenu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_THICKUE:
			wvAppendStr(mydata->current,"<thicku.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DASHUE:
			wvAppendStr(mydata->current,"<dashu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTUE:
			wvAppendStr(mydata->current,"<dotu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDASHUE:
			wvAppendStr(mydata->current,"<dotdashu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_DOTDOTDASHUE:
			wvAppendStr(mydata->current,"<dotdotdashu.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_WAVEUE:
			wvAppendStr(mydata->current,"<waveu.end/>");
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

		case TT_ANIMATIONB:
			wvAppendStr(mydata->current,"<animation.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ANIMATIONE:
			wvAppendStr(mydata->current,"<animation.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_IBSTANNO:
			wvAppendStr(mydata->current,"<ibstanno/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_xstUsrInitl:
			wvAppendStr(mydata->current,"<xstUsrInitl/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmParaBefore:
			wvAppendStr(mydata->current,"<mmParaBefore/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmParaAfter:
			wvAppendStr(mydata->current,"<mmParaAfter/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmParaLeft:
			wvAppendStr(mydata->current,"<mmParaLeft/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmParaRight:
			wvAppendStr(mydata->current,"<mmParaRight/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmParaLeft1:
			wvAppendStr(mydata->current,"<mmParaLeft1/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmPadTop:
			wvAppendStr(mydata->current,"<mmPadTop/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmPadRight:
			wvAppendStr(mydata->current,"<mmPadRight/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmPadBottom:
			wvAppendStr(mydata->current,"<mmPadBottom/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmPadLeft:
			wvAppendStr(mydata->current,"<mmPadLeft/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_mmLineHeight:
			wvAppendStr(mydata->current,"<mmLineHeight/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_pixPicWidth:
			wvAppendStr(mydata->current,"<pixPicWidth/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_pixPicHeight:
			wvAppendStr(mydata->current,"<pixPicHeight/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_htmlAlignGuess:
			wvAppendStr(mydata->current,"<htmlAlignGuess/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_htmlNextLineGuess:
			wvAppendStr(mydata->current,"<htmlNextLineGuess/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ibstRMark:
			wvAppendStr(mydata->current,"<ibstrmark/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ibstRMarkDel:
			wvAppendStr(mydata->current,"<ibstrmarkdel/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ibstDispFldRMark:
			wvAppendStr(mydata->current,"<ibstdispfldrmark/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_dttmRMark:
			wvAppendStr(mydata->current,"<dttmrmark/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_dttmRMarkDel:
			wvAppendStr(mydata->current,"<dttmrmarkdel/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_dttmDispFldRMark:
			wvAppendStr(mydata->current,"<dttmdispfldrmark/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_xstDispFldRMark:
			wvAppendStr(mydata->current,"<xstDispFldRMark/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_PropRMarkB:
			wvAppendStr(mydata->current,"<proprmark.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_PropRMarkE:
			wvAppendStr(mydata->current,"<proprmark.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ibstPropRMark:
			wvAppendStr(mydata->current,"<ibstPropRMark/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_dttmPropRMark:
			wvAppendStr(mydata->current,"<dttmPropRMark/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_LasVegasB:
			wvAppendStr(mydata->current,"<lasvegas.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BackgroundBlinkB:
			wvAppendStr(mydata->current,"<backgroundblink.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SparkleTextB:
			wvAppendStr(mydata->current,"<sparkletext.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_MarchingAntsB:
			wvAppendStr(mydata->current,"<marchingants.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_MarchingRedAntsB:
			wvAppendStr(mydata->current,"<marchingredants.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ShimmerB:
			wvAppendStr(mydata->current,"<shimmer.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_LasVegasE:
			wvAppendStr(mydata->current,"<lasvegas.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_BackgroundBlinkE:
			wvAppendStr(mydata->current,"<backgroundblink.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_SparkleTextE:
			wvAppendStr(mydata->current,"<sparkletext.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_MarchingAntsE:
			wvAppendStr(mydata->current,"<marchingants.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_MarchingRedAntsE:
			wvAppendStr(mydata->current,"<marchingredants.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ShimmerE:
			wvAppendStr(mydata->current,"<shimmer.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_TEXTB:
			wvAppendStr(mydata->current,"<text.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
			break;
		case TT_TEXTE:
			wvAppendStr(mydata->current,"<text.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;

		case TT_OLISTB:
			wvAppendStr(mydata->current,"<olist.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
			break;
		case TT_OLISTE:
			wvAppendStr(mydata->current,"<olist.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ULISTB:
			wvAppendStr(mydata->current,"<ulist.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
			break;
		case TT_ULISTE:
			wvAppendStr(mydata->current,"<ulist.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ENTRYB:
			wvAppendStr(mydata->current,"<entry.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
			break;
		case TT_ENTRYE:
			wvAppendStr(mydata->current,"<entry.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_TABLEB:
			wvAppendStr(mydata->current,"<table.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
			break;
		case TT_TABLEE:
			wvAppendStr(mydata->current,"<table.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_ROWB:
			wvAppendStr(mydata->current,"<row.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
			break;
		case TT_ROWE:
			wvAppendStr(mydata->current,"<row.end/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
		case TT_CELLB:
			wvAppendStr(mydata->current,"<cell.begin/>");
			mydata->currentlen = strlen(*(mydata->current));
			break;
			break;
		case TT_CELLE:
			wvAppendStr(mydata->current,"<cell.end/>");
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
		case TT_ParaBefore:
		case TT_ParaRight:
		case TT_ParaAfter:
		case TT_ParaLeft:
		case TT_ParaLeft1:
		case TT_LEFT:
		case TT_RIGHT:
		case TT_CENTER:
		case TT_BLOCK:
		case TT_ASIAN:
		case TT_Arabic:
		case TT_UpperRoman:
		case TT_LowerRoman:
		case TT_UpperCaseN:
		case TT_LowerCaseN:
		case TT_NONED:
		case TT_SINGLED:
		case TT_THICKD:
		case TT_DOUBLED:
		case TT_NUMBER4D:
		case TT_HAIRLINED:
		case TT_DOTD:
		case TT_DASHLARGEGAPD:
		case TT_DOTDASHD:
		case TT_DOTDOTDASHD:
		case TT_TRIPLED:
		case TT_thin_thicksmallgapD:
		case TT_thick_thinsmallgapD:
		case TT_thin_thick_thinsmallgapD:
		case TT_thin_thickmediumgapD:
		case TT_thick_thinmediumgapD:
		case TT_thin_thick_thinmediumgapD:
		case TT_thin_thicklargegapD:
		case TT_thick_thinlargegapD:
		case TT_thin_thick_thinlargegapD:
		case TT_WAVED:
		case TT_DOUBLEWAVED:
		case TT_DASHSMALLGAPD:
		case TT_DASHDOTSTROKEDD:
		case TT_EMBOSS3DD:
		case TT_ENGRAVE3DD:
		case TT_DEFAULTD:
			wvAppendStr(mydata->current,"</begin>");
			wvTrace(("When we finish the str is %s\n",*(mydata->current)));
			mydata->currentlen=0;
			mydata->current=NULL;
			break;
		case TT_TITLE:
		case TT_CHARSET:
		case TT_COLSPAN:
		case TT_CELLWIDTH:
		case TT_ROWSPAN:
		case TT_CELLBGCOLOR:
		case TT_PARABGCOLOR:
		case TT_PARAFGCOLOR:
		case TT_TABLERELWIDTH:
		case TT_VERSION:
		case TT_JUST:
		case TT_PARAMARGIN:
		case TT_BORDERTopSTYLE:
		case TT_BORDERTopCOLOR:
		case TT_BORDERLeftSTYLE:
		case TT_BORDERLeftCOLOR:
		case TT_BORDERRightSTYLE:
		case TT_BORDERRightCOLOR:
		case TT_BORDERBottomSTYLE:
		case TT_BORDERBottomCOLOR:
		case TT_nfc:
		case TT_START:
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

		case TT_SINGLEUB:
		case TT_WORDUB:
		case TT_DOUBLEUB:
		case TT_DOTTEDUB:
		case TT_HIDDENUB:
		case TT_THICKUB:
		case TT_DASHUB:
		case TT_DOTUB:
		case TT_DOTDASHUB:
		case TT_DOTDOTDASHUB:
		case TT_WAVEUB:
		case TT_SINGLEUE:
		case TT_WORDUE:
		case TT_DOUBLEUE:
		case TT_DOTTEDUE:
		case TT_HIDDENUE:
		case TT_THICKUE:
		case TT_DASHUE:
		case TT_DOTUE:
		case TT_DOTDASHUE:
		case TT_DOTDOTDASHUE:
		case TT_WAVEUE:

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
		case TT_IBSTANNO:
		case TT_xstUsrInitl:
		case TT_mmParaBefore:
		case TT_mmParaAfter:
		case TT_mmParaLeft:
		case TT_mmParaRight:
		case TT_mmParaLeft1:
		case TT_mmPadTop:
		case TT_mmPadRight:
		case TT_mmPadBottom:
		case TT_mmPadLeft:
		case TT_mmLineHeight:
		case TT_pixPicWidth:
		case TT_pixPicHeight:
		case TT_htmlAlignGuess:
		case TT_htmlNextLineGuess:
		case TT_ibstRMark:
		case TT_ibstRMarkDel:
		case TT_ibstDispFldRMark:
		case TT_dttmRMark:
		case TT_dttmRMarkDel:
		case TT_dttmDispFldRMark:
		case TT_xstDispFldRMark:
		case TT_PropRMarkB:
		case TT_PropRMarkE:
		case TT_ibstPropRMark:
		case TT_dttmPropRMark:
		case TT_LasVegasB:
		case TT_BackgroundBlinkB:
		case TT_SparkleTextB:
		case TT_MarchingAntsB:
		case TT_MarchingRedAntsB:
		case TT_ShimmerB:
		case TT_LasVegasE:
		case TT_BackgroundBlinkE:
		case TT_SparkleTextE:
		case TT_MarchingAntsE:
		case TT_MarchingRedAntsE:
		case TT_ShimmerE:
		case TT_ANIMATIONB:
		case TT_ANIMATIONE:
		case TT_DispFldRMarkB:
		case TT_DispFldRMarkE:
		case TT_TEXTB:
		case TT_TEXTE:
		case TT_OLISTB:
		case TT_OLISTE:
		case TT_ULISTB:
		case TT_ULISTE:
		case TT_ENTRYB:
		case TT_ENTRYE:
		case TT_TABLEB:
		case TT_TABLEE:
		case TT_ROWB:
		case TT_ROWE:
		case TT_CELLB:
		case TT_CELLE:

		case TT_STYLE:
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
		case TT_COLSPAN:
		case TT_ROWSPAN:
		case TT_CELLBGCOLOR:
		case TT_PARABGCOLOR:
		case TT_PARAFGCOLOR:
		case TT_TABLERELWIDTH:
		case TT_CELLWIDTH:
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
