#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include 	"utf.h"
#include "iconv_internal.h"

size_t wvConvertUnicodeToUTF_8(const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
	{
	U16 c2;
	size_t ret=0;
	int len;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
		c2 = (U8)**inbuf;
		(*inbuf)++;
		c2 = (c2<< 8)&0xFF00;
		c2 += (U8)**inbuf;
		(*inbuf)++;

        (*inbytesleft)-=2;

    	len = our_wctomb(*outbuf,c2);

        (*outbuf)+=len;
        (*outbytesleft)-=len;
		ret+=len;
        }
	return(ret);
	}
