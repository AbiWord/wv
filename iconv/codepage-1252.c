#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include "iconv_internal.h"

size_t wvConvert1252ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
	size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert1252ToUnicode((unsigned char)(**inbuf));
        (*inbytesleft)--;
        (*inbuf)++;
        **outbuf = ((c>>8)&0x00FF);
        (*outbuf)++;
        **outbuf = (c&0x00FF);
        (*outbuf)++;
        (*outbytesleft)-=2;
		ret+=2;
        }
	return(ret);
    }

/*
returns 0xffff is the 1252 char is a undefined char,
in which case for html output you can go down the gif
inclusion route
*/
U16 Convert1252ToUnicode(U16 char8)
	{
	switch (char8)
		{
		case 0x80:
			return(0x20AC); /*EURO SIGN*/
		case 0x81:          /*UNDEFINED*/
			return(0xffff);
		case 0x82:
			return(0x201A);  /*SINGLE LOW-9 QUOTATION MARK*/
		case 0x83:    
			return(0x0192);  /*LATIN SMALL LETTER F WITH HOOK*/
		case 0x84:    
			return(0x201E);  /*DOUBLE LOW-9 QUOTATION MARK*/
		case 0x85:    
			return(0x2026);  /*HORIZONTAL ELLIPSIS*/
		case 0x86:    
			return(0x2020);  /*DAGGER*/
		case 0x87:    
			return(0x2021);  /*DOUBLE DAGGER*/
		case 0x88:    
			return(0x02C6);  /*MODIFIER LETTER CIRCUMFLEX ACCENT*/
		case 0x89:    
			return(0x2030);  /*PER MILLE SIGN*/
		case 0x8A:    
			return(0x0160);  /*LATIN CAPITAL LETTER S WITH CARON*/
		case 0x8B:    
			return(0x2039);  /*SINGLE LEFT-POINTING ANGLE QUOTATION MARK*/
		case 0x8C:    
			return(0x0152);  /*LATIN CAPITAL LIGATURE OE*/
		case 0x8D:            /*UNDEFINED*/
			return(0xffff);
		case 0x8E:    
			return(0x017D);  /*LATIN CAPITAL LETTER Z WITH CARON*/
		case 0x8F:            /*UNDEFINED*/
			return(0xffff);
		case 0x90:            /*UNDEFINED*/
			return(0xffff);
		case 0x91:    
			return(0x2018);  /*LEFT SINGLE QUOTATION MARK*/
		case 0x92:    
			return(0x2019);  /*RIGHT SINGLE QUOTATION MARK*/
		case 0x93:    
			return(0x201C);  /*LEFT DOUBLE QUOTATION MARK*/
		case 0x94:    
			return(0x201D);  /*RIGHT DOUBLE QUOTATION MARK*/
		case 0x95:    
			return(0x2022);  /*BULLET*/
		case 0x96:    
			return(0x2013);  /*EN DASH*/
		case 0x97:    
			return(0x2014);  /*EM DASH*/
		case 0x98:    
			return(0x02DC);  /*SMALL TILDE*/
		case 0x99:    
			return(0x2122);  /*TRADE MARK SIGN*/
		case 0x9A:    
			return(0x0161);  /*LATIN SMALL LETTER S WITH CARON*/
		case 0x9B:    
			return(0x203A);  /*SINGLE RIGHT-POINTING ANGLE QUOTATION MARKS*/
		case 0x9C:    
			return(0x0153);  /*LATIN SMALL LIGATURE OE*/
		case 0x9D:            /*UNDEFINED*/
			return(0xffff);
		case 0x9E:    
			return(0x017E);  /*LATIN SMALL LETTER Z WITH CARON*/
		case 0x9F:    
			return(0x0178);  /*LATIN CAPITAL LETTER Y WITH DIAERESIS*/
		default:
			return(char8);
		}
	}
