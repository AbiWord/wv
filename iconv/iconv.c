#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "iconv.h"
#include "iconv_internal.h"

typedef struct _convertt
	{
	char *name;
	wvConvertToUnicode proc;
	}convertt;
	
#define NOINPUTS 9
/*
#define NOINPUTS 13
*/

convertt inputs[NOINPUTS] = 
{
	{ "CP874", wvConvert874ToUnicode },
/*
	{ "CP932", wvConvert932ToUnicode },
	{ "CP936", wvConvert936ToUnicode },
	{ "CP949", wvConvert949ToUnicode },
	{ "CP950", wvConvert950ToUnicode },
*/
	{ "CP1250", wvConvert1250ToUnicode },
	{ "CP1251", wvConvert1251ToUnicode },
	{ "CP1252", wvConvert1252ToUnicode },
	{ "CP1253", wvConvert1253ToUnicode },
	{ "CP1254", wvConvert1254ToUnicode },
	{ "CP1255", wvConvert1255ToUnicode },
	{ "CP1256", wvConvert1256ToUnicode },
	{ "CP1257", wvConvert1257ToUnicode }
};

#define NOOUTPUTS 4

convertt outputs[NOOUTPUTS] = 
{
	{ "utf-8", 			wvConvertUnicodeToUTF_8},
	{ "iso-8859-15", 	wvConvertUnicodeToiso8859_15},
	{ "koi8-r", 		wvConvertUnicodeToKOI8_R},
	{ "tis-620", 		wvConvertUnicodeToTIS620}
};

iconv_t iconv_open (const char *tocode, const char *fromcode)
	{
	int i;
	/* if converting from unicode */
	if (!(strcasecmp(fromcode,"UCS-2")))
		{
		for (i=0;i<NOOUTPUTS;i++)
			{
			if (!(strcasecmp(tocode,outputs[i].name)))
				return(outputs[i].proc);
			}
		}
	/* if converting to unicode */
	else if (!(strcasecmp(tocode,"UCS-2")))
		{
		for (i=0;i<NOINPUTS;i++)
			{
			if (!(strcasecmp(fromcode,inputs[i].name)))
				return(inputs[i].proc);
			}
		}
	return((iconv_t)-1);	
	}

int iconv_close (iconv_t cd)
	{
	return(0);
	}

size_t iconv (iconv_t cd, char **inbuf, size_t *inbytesleft, 
	char **outbuf, size_t *outbytesleft)
	{
	int i=0,j=0;
	U16 out;
	size_t len;
	wvConvertToUnicode proc= (wvConvertToUnicode)cd;
	len = proc(inbuf,inbytesleft,outbuf,outbytesleft);
	return(len);
	}
