#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "iconv_internal.h"

size_t wvConvert1250ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
    size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert1250ToUnicode((unsigned char)**inbuf);
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
returns 1 if the char has a html name

converts the chars into their html equivalent if
possible
*/
U16 Convert1250ToUnicode(U16 char8)
	{
	switch (char8)
		{
		case 0x80:
			return(0x20AC);	/*	#EURO SIGN	*/
		case 0x81:
			return(0xffff);	/*      	#UNDEFINED	*/
		case 0x82:
			return(0x201A);	/*	#SINGLE LOW-9 QUOTATION MARK	*/
		case 0x83:
			return(0xffff);	/*      	#UNDEFINED	*/
		case 0x84:
			return(0x201E);	/*	#DOUBLE LOW-9 QUOTATION MARK	*/
		case 0x85:
			return(0x2026);	/*	#HORIZONTAL ELLIPSIS	*/
		case 0x86:
			return(0x2020);	/*	#DAGGER	*/
		case 0x87:
			return(0x2021);	/*	#DOUBLE DAGGER	*/
		case 0x88:
			return(0xffff);	/*      	#UNDEFINED	*/
		case 0x89:
			return(0x2030);	/*	#PER MILLE SIGN	*/
		case 0x8A:
			return(0x0160);	/*	#LATIN CAPITAL LETTER S WITH CARON	*/
		case 0x8B:
			return(0x2039);	/*	#SINGLE LEFT-POINTING ANGLE QUOTATION MARK	*/
		case 0x8C:
			return(0x015A);	/*	#LATIN CAPITAL LETTER S WITH ACUTE	*/
		case 0x8D:
			return(0x0164);	/*	#LATIN CAPITAL LETTER T WITH CARON	*/
		case 0x8E:
			return(0x017D);	/*	#LATIN CAPITAL LETTER Z WITH CARON	*/
		case 0x8F:
			return(0x0179);	/*	#LATIN CAPITAL LETTER Z WITH ACUTE	*/
		case 0x90:
			return(0xffff);	/*      	#UNDEFINED	*/
		case 0x91:
			return(0x2018);	/*	#LEFT SINGLE QUOTATION MARK	*/
		case 0x92:
			return(0x2019);	/*	#RIGHT SINGLE QUOTATION MARK	*/
		case 0x93:
			return(0x201C);	/*	#LEFT DOUBLE QUOTATION MARK	*/
		case 0x94:
			return(0x201D);	/*	#RIGHT DOUBLE QUOTATION MARK	*/
		case 0x95:
			return(0x2022);	/*	#BULLET	*/
		case 0x96:
			return(0x2013);	/*	#EN DASH	*/
		case 0x97:
			return(0x2014);	/*	#EM DASH	*/
		case 0x98:
			return(0xffff);	/*      	#UNDEFINED	*/
		case 0x99:
			return(0x2122);	/*	#TRADE MARK SIGN	*/
		case 0x9A:
			return(0x0161);	/*	#LATIN SMALL LETTER S WITH CARON	*/
		case 0x9B:
			return(0x203A);	/*	#SINGLE RIGHT-POINTING ANGLE QUOTATION MARK	*/
		case 0x9C:
			return(0x015B);	/*	#LATIN SMALL LETTER S WITH ACUTE	*/
		case 0x9D:
			return(0x0165);	/*	#LATIN SMALL LETTER T WITH CARON	*/
		case 0x9E:
			return(0x017E);	/*	#LATIN SMALL LETTER Z WITH CARON	*/
		case 0x9F:
			return(0x017A);	/*	#LATIN SMALL LETTER Z WITH ACUTE	*/
		case 0xA1:
			return(0x02C7);	/*	#CARON	*/
		case 0xA2:
			return(0x02D8);	/*	#BREVE	*/
		case 0xA3:
			return(0x0141);	/*	#LATIN CAPITAL LETTER L WITH STROKE	*/
		case 0xA4:
			return(0x00A4);	/*	#CURRENCY SIGN	*/
		case 0xA5:
			return(0x0104);	/*	#LATIN CAPITAL LETTER A WITH OGONEK	*/
		case 0xAA:
			return(0x015E);	/*	#LATIN CAPITAL LETTER S WITH CEDILLA	*/
		case 0xAF:
			return(0x017B);	/*	#LATIN CAPITAL LETTER Z WITH DOT ABOVE	*/
		case 0xB2:
			return(0x02DB);	/*	#OGONEK	*/
		case 0xB3:
			return(0x0142);	/*	#LATIN SMALL LETTER L WITH STROKE	*/
		case 0xB9:
			return(0x0105);	/*	#LATIN SMALL LETTER A WITH OGONEK	*/
		case 0xBA:
			return(0x015F);	/*	#LATIN SMALL LETTER S WITH CEDILLA	*/
		case 0xBC:
			return(0x013D);	/*	#LATIN CAPITAL LETTER L WITH CARON	*/
		case 0xBD:
			return(0x02DD);	/*	#DOUBLE ACUTE ACCENT	*/
		case 0xBE:
			return(0x013E);	/*	#LATIN SMALL LETTER L WITH CARON	*/
		case 0xBF:
			return(0x017C);	/*	#LATIN SMALL LETTER Z WITH DOT ABOVE	*/
		case 0xC0:
			return(0x0154);	/*	#LATIN CAPITAL LETTER R WITH ACUTE	*/
		case 0xC3:
			return(0x0102);	/*	#LATIN CAPITAL LETTER A WITH BREVE	*/
		case 0xC5:
			return(0x0139);	/*	#LATIN CAPITAL LETTER L WITH ACUTE	*/
		case 0xC6:
			return(0x0106);	/*	#LATIN CAPITAL LETTER C WITH ACUTE	*/
		case 0xC8:
			return(0x010C);	/*	#LATIN CAPITAL LETTER C WITH CARON	*/
		case 0xCA:
			return(0x0118);	/*	#LATIN CAPITAL LETTER E WITH OGONEK	*/
		case 0xCC:
			return(0x011A);	/*	#LATIN CAPITAL LETTER E WITH CARON	*/
		case 0xCF:
			return(0x010E);	/*	#LATIN CAPITAL LETTER D WITH CARON	*/
		case 0xD0:
			return(0x0110);	/*	#LATIN CAPITAL LETTER D WITH STROKE	*/
		case 0xD1:
			return(0x0143);	/*	#LATIN CAPITAL LETTER N WITH ACUTE	*/
		case 0xD2:
			return(0x0147);	/*	#LATIN CAPITAL LETTER N WITH CARON	*/
		case 0xD5:
			return(0x0150);	/*	#LATIN CAPITAL LETTER O WITH DOUBLE ACUTE	*/
		case 0xD8:
			return(0x0158);	/*	#LATIN CAPITAL LETTER R WITH CARON	*/
		case 0xD9:
			return(0x016E);	/*	#LATIN CAPITAL LETTER U WITH RING ABOVE	*/
		case 0xDB:
			return(0x0170);	/*	#LATIN CAPITAL LETTER U WITH DOUBLE ACUTE	*/
		case 0xDE:
			return(0x0162);	/*	#LATIN CAPITAL LETTER T WITH CEDILLA	*/
		case 0xE0:
			return(0x0155);	/*	#LATIN SMALL LETTER R WITH ACUTE	*/
		case 0xE3:
			return(0x0103);	/*	#LATIN SMALL LETTER A WITH BREVE	*/
		case 0xE5:
			return(0x013A);	/*	#LATIN SMALL LETTER L WITH ACUTE	*/
		case 0xE6:
			return(0x0107);	/*	#LATIN SMALL LETTER C WITH ACUTE	*/
		case 0xE8:
			return(0x010D);	/*	#LATIN SMALL LETTER C WITH CARON	*/
		case 0xEA:
			return(0x0119);	/*	#LATIN SMALL LETTER E WITH OGONEK	*/
		case 0xEC:
			return(0x011B);	/*	#LATIN SMALL LETTER E WITH CARON	*/
		case 0xEF:
			return(0x010F);	/*	#LATIN SMALL LETTER D WITH CARON	*/
		case 0xF0:
			return(0x0111);	/*	#LATIN SMALL LETTER D WITH STROKE	*/
		case 0xF1:
			return(0x0144);	/*	#LATIN SMALL LETTER N WITH ACUTE	*/
		case 0xF2:
			return(0x0148);	/*	#LATIN SMALL LETTER N WITH CARON	*/
		case 0xF5:
			return(0x0151);	/*	#LATIN SMALL LETTER O WITH DOUBLE ACUTE	*/
		case 0xF6:
			return(0x00F6);	/*	#LATIN SMALL LETTER O WITH DIAERESIS	*/
		case 0xF8:
			return(0x0159);	/*	#LATIN SMALL LETTER R WITH CARON	*/
		case 0xF9:
			return(0x016F);	/*	#LATIN SMALL LETTER U WITH RING ABOVE	*/
		case 0xFB:
			return(0x0171);	/*	#LATIN SMALL LETTER U WITH DOUBLE ACUTE	*/
		case 0xFE:
			return(0x0163);	/*	#LATIN SMALL LETTER T WITH CEDILLA	*/
		case 0xFF:
			return(0x02D9);	/*	#DOT ABOVE	*/
		default:
			return(0);
		}
	}
