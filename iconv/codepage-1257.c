#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include "iconv_internal.h"

size_t wvConvert1257ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
    size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert1257ToUnicode((unsigned char)(**inbuf));
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


U16 Convert1257ToUnicode(U16 char8)
	{
	switch (char8)
		{
		case 0x80:
			return(0x20AC);	/*	#EURO SIGN */
		case 0x81:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x82:
			return(0x201A);	/*	#SINGLE LOW-9 QUOTATION MARK */
		case 0x83:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x84:
			return(0x201E);	/*	#DOUBLE LOW-9 QUOTATION MARK */
		case 0x85:
			return(0x2026);	/*	#HORIZONTAL ELLIPSIS */
		case 0x86:
			return(0x2020);	/*	#DAGGER */
		case 0x87:
			return(0x2021);	/*	#DOUBLE DAGGER */
		case 0x88:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x89:
			return(0x2030);	/*	#PER MILLE SIGN */
		case 0x8A:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x8B:
			return(0x2039);	/*	#SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
		case 0x8C:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x8D:
			return(0x00A8);	/*	#DIAERESIS */
		case 0x8E:
			return(0x02C7);	/*	#CARON */
		case 0x8F:
			return(0x00B8);	/*	#CEDILLA */
		case 0x90:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x91:
			return(0x2018);	/*	#LEFT SINGLE QUOTATION MARK */
		case 0x92:
			return(0x2019);	/*	#RIGHT SINGLE QUOTATION MARK */
		case 0x93:
			return(0x201C);	/*	#LEFT DOUBLE QUOTATION MARK */
		case 0x94:
			return(0x201D);	/*	#RIGHT DOUBLE QUOTATION MARK */
		case 0x95:
			return(0x2022);	/*	#BULLET */
		case 0x96:
			return(0x2013);	/*	#EN DASH */
		case 0x97:
			return(0x2014);	/*	#EM DASH */
		case 0x98:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x99:
			return(0x2122);	/*	#TRADE MARK SIGN */
		case 0x9A:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x9B:
			return(0x203A);	/*	#SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
		case 0x9C:
			return(0xffff);	/*      	#UNDEFINED */
		case 0x9D:
			return(0x00AF);	/*	#MACRON */
		case 0x9E:
			return(0x02DB);	/*	#OGONEK */
		case 0x9F:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xA0:
			return(0x00A0);	/*	#NO-BREAK SPACE */
		case 0xA1:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xA2:
			return(0x00A2);	/*	#CENT SIGN */
		case 0xA3:
			return(0x00A3);	/*	#POUND SIGN */
		case 0xA4:
			return(0x00A4);	/*	#CURRENCY SIGN */
		case 0xA5:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xA8:
			return(0x00D8);	/*	#LATIN CAPITAL LETTER O WITH STROKE */
		case 0xAA:
			return(0x0156);	/*	#LATIN CAPITAL LETTER R WITH CEDILLA */
		case 0xAF:
			return(0x00C6);	/*	#LATIN CAPITAL LETTER AE */
		case 0xB8:
			return(0x00F8);	/*	#LATIN SMALL LETTER O WITH STROKE */
		case 0xBA:
			return(0x0157);	/*	#LATIN SMALL LETTER R WITH CEDILLA */
		case 0xBF:
			return(0x00E6);	/*	#LATIN SMALL LETTER AE */
		case 0xC0:
			return(0x0104);	/*	#LATIN CAPITAL LETTER A WITH OGONEK */
		case 0xC1:
			return(0x012E);	/*	#LATIN CAPITAL LETTER I WITH OGONEK */
		case 0xC2:
			return(0x0100);	/*	#LATIN CAPITAL LETTER A WITH MACRON */
		case 0xC3:
			return(0x0106);	/*	#LATIN CAPITAL LETTER C WITH ACUTE */
		case 0xC6:
			return(0x0118);	/*	#LATIN CAPITAL LETTER E WITH OGONEK */
		case 0xC7:
			return(0x0112);	/*	#LATIN CAPITAL LETTER E WITH MACRON */
		case 0xC8:
			return(0x010C);	/*	#LATIN CAPITAL LETTER C WITH CARON */
		case 0xCA:
			return(0x0179);	/*	#LATIN CAPITAL LETTER Z WITH ACUTE */
		case 0xCB:
			return(0x0116);	/*	#LATIN CAPITAL LETTER E WITH DOT ABOVE */
		case 0xCC:
			return(0x0122);	/*	#LATIN CAPITAL LETTER G WITH CEDILLA */
		case 0xCD:
			return(0x0136);	/*	#LATIN CAPITAL LETTER K WITH CEDILLA */
		case 0xCE:
			return(0x012A);	/*	#LATIN CAPITAL LETTER I WITH MACRON */
		case 0xCF:
			return(0x013B);	/*	#LATIN CAPITAL LETTER L WITH CEDILLA */
		case 0xD0:
			return(0x0160);	/*	#LATIN CAPITAL LETTER S WITH CARON */
		case 0xD1:
			return(0x0143);	/*	#LATIN CAPITAL LETTER N WITH ACUTE */
		case 0xD2:
			return(0x0145);	/*	#LATIN CAPITAL LETTER N WITH CEDILLA */
		case 0xD3:
			return(0x00D3);	/*	#LATIN CAPITAL LETTER O WITH ACUTE */
		case 0xD4:
			return(0x014C);	/*	#LATIN CAPITAL LETTER O WITH MACRON */
		case 0xD8:
			return(0x0172);	/*	#LATIN CAPITAL LETTER U WITH OGONEK */
		case 0xD9:
			return(0x0141);	/*	#LATIN CAPITAL LETTER L WITH STROKE */
		case 0xDA:
			return(0x015A);	/*	#LATIN CAPITAL LETTER S WITH ACUTE */
		case 0xDB:
			return(0x016A);	/*	#LATIN CAPITAL LETTER U WITH MACRON */
		case 0xDD:
			return(0x017B);	/*	#LATIN CAPITAL LETTER Z WITH DOT ABOVE */
		case 0xDE:
			return(0x017D);	/*	#LATIN CAPITAL LETTER Z WITH CARON */
		case 0xE0:
			return(0x0105);	/*	#LATIN SMALL LETTER A WITH OGONEK */
		case 0xE1:
			return(0x012F);	/*	#LATIN SMALL LETTER I WITH OGONEK */
		case 0xE2:
			return(0x0101);	/*	#LATIN SMALL LETTER A WITH MACRON */
		case 0xE3:
			return(0x0107);	/*	#LATIN SMALL LETTER C WITH ACUTE */
		case 0xE6:
			return(0x0119);	/*	#LATIN SMALL LETTER E WITH OGONEK */
		case 0xE7:
			return(0x0113);	/*	#LATIN SMALL LETTER E WITH MACRON */
		case 0xE8:
			return(0x010D);	/*	#LATIN SMALL LETTER C WITH CARON */
		case 0xEA:
			return(0x017A);	/*	#LATIN SMALL LETTER Z WITH ACUTE */
		case 0xEB:
			return(0x0117);	/*	#LATIN SMALL LETTER E WITH DOT ABOVE */
		case 0xEC:
			return(0x0123);	/*	#LATIN SMALL LETTER G WITH CEDILLA */
		case 0xED:
			return(0x0137);	/*	#LATIN SMALL LETTER K WITH CEDILLA */
		case 0xEE:
			return(0x012B);	/*	#LATIN SMALL LETTER I WITH MACRON */
		case 0xEF:
			return(0x013C);	/*	#LATIN SMALL LETTER L WITH CEDILLA */
		case 0xF0:
			return(0x0161);	/*	#LATIN SMALL LETTER S WITH CARON */
		case 0xF1:
			return(0x0144);	/*	#LATIN SMALL LETTER N WITH ACUTE */
		case 0xF2:
			return(0x0146);	/*	#LATIN SMALL LETTER N WITH CEDILLA */
		case 0xF4:
			return(0x014D);	/*	#LATIN SMALL LETTER O WITH MACRON */
		case 0xF8:
			return(0x0173);	/*	#LATIN SMALL LETTER U WITH OGONEK */
		case 0xF9:
			return(0x0142);	/*	#LATIN SMALL LETTER L WITH STROKE */
		case 0xFA:
			return(0x015B);	/*	#LATIN SMALL LETTER S WITH ACUTE */
		case 0xFB:
			return(0x016B);	/*	#LATIN SMALL LETTER U WITH MACRON */
		case 0xFD:
			return(0x017C);	/*	#LATIN SMALL LETTER Z WITH DOT ABOVE */
		case 0xFE:
			return(0x017E);	/*	#LATIN SMALL LETTER Z WITH CARON */
		case 0xFF:
			return(0x02D9);	/*	#DOT ABOVE */
		default:
			return(char8);
		}
	}
