#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "iconv_internal.h"

size_t wvConvertUnicodeToiso8859_15(const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
	{
	size_t ret=0;
	U16 c1,c2;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
		c2 = (U8)**inbuf;
        (*inbuf)++;
        c2 = (c2<< 8)&0xFF00;
        c2 += (U8)**inbuf;
        (*inbuf)++;
        (*inbytesleft)-=2;
		
        c1 = ConvertUnicodeToiso8859_15(c2);
		if (c1 == 0xffff)
        	**outbuf = '?';
		else
			**outbuf = (char)c1;
        (*outbuf)++;
        (*outbytesleft)--;
		ret++;
        }
	return(ret);
	}

U16 ConvertUnicodeToiso8859_15(U16 char16)
	{
	switch (char16)
		{
		case 0x20AC:
			return(0xA4);
			break;
		case 0x0160:
			return(0xA6);
			break;
		case 0x0161:
			return(0xA8);
			break;
		case 0x017D:
			return(0xB4);
			break;
		case 0x0152:
			return(0xBC);
			break;
		case 0x0153:
			return(0xBD);
			break;
		case 0x0178:
			return(0xBE);
			break;
		default:
			if (char16 < 0xFF)
				return(char16);
			else
				return(0xffff);
			break;
		}
	}
