#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include "iconv_internal.h"

size_t wvConvert1251ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
    size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert1251ToUnicode((unsigned char)(**inbuf));
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
returns 0xffff is the 1251 char is a undefined char,
in which case for html output you can go down the gif
inclusion route
*/

U16 Convert1251ToUnicode(U16 char8)
	{
	switch (char8)
		{
		case 0x80:
			return(0x0402);	/*	#CYRILLIC CAPITAL LETTER DJE */
		case 0x81:
			return(0x0403);	/*	#CYRILLIC CAPITAL LETTER GJE */
		case 0x82:
			return(0x201A);	/*	#SINGLE LOW-9 QUOTATION MARK */
		case 0x83:
			return(0x0453);	/*	#CYRILLIC SMALL LETTER GJE */
		case 0x84:
			return(0x201E);	/*	#DOUBLE LOW-9 QUOTATION MARK */
		case 0x85:
			return(0x2026);	/*	#HORIZONTAL ELLIPSIS */
		case 0x86:
			return(0x2020);	/*	#DAGGER */
		case 0x87:
			return(0x2021);	/*	#DOUBLE DAGGER */
		case 0x88:
			return(0x20AC);	/*	#EURO SIGN */
		case 0x89:
			return(0x2030);	/*	#PER MILLE SIGN */
		case 0x8A:
			return(0x0409);	/*	#CYRILLIC CAPITAL LETTER LJE */
		case 0x8B:
			return(0x2039);	/*	#SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
		case 0x8C:
			return(0x040A);	/*	#CYRILLIC CAPITAL LETTER NJE */
		case 0x8D:
			return(0x040C);	/*	#CYRILLIC CAPITAL LETTER KJE */
		case 0x8E:
			return(0x040B);	/*	#CYRILLIC CAPITAL LETTER TSHE */
		case 0x8F:
			return(0x040F);	/*	#CYRILLIC CAPITAL LETTER DZHE */
		case 0x90:
			return(0x0452);	/*	#CYRILLIC SMALL LETTER DJE */
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
			return(0xffff);	/*  #UNDEFINED */
		case 0x99:
			return(0x2122);	/*	#TRADE MARK SIGN */
		case 0x9A:
			return(0x0459);	/*	#CYRILLIC SMALL LETTER LJE */
		case 0x9B:
			return(0x203A);	/*	#SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
		case 0x9C:
			return(0x045A);	/*	#CYRILLIC SMALL LETTER NJE */
		case 0x9D:
			return(0x045C);	/*	#CYRILLIC SMALL LETTER KJE */
		case 0x9E:
			return(0x045B);	/*	#CYRILLIC SMALL LETTER TSHE */
		case 0x9F:
			return(0x045F);	/*	#CYRILLIC SMALL LETTER DZHE */
		case 0xA1:
			return(0x040E);	/*	#CYRILLIC CAPITAL LETTER SHORT U */
		case 0xA2:
			return(0x045E);	/*	#CYRILLIC SMALL LETTER SHORT U */
		case 0xA3:
			return(0x0408);	/*	#CYRILLIC CAPITAL LETTER JE */
		case 0xA5:
			return(0x0490);	/*	#CYRILLIC CAPITAL LETTER GHE WITH UPTURN */
		case 0xA8:
			return(0x0401);	/*	#CYRILLIC CAPITAL LETTER IO */
		case 0xAA:
			return(0x0404);	/*	#CYRILLIC CAPITAL LETTER UKRAINIAN IE */
		case 0xAF:
			return(0x0407);	/*	#CYRILLIC CAPITAL LETTER YI */
		case 0xB2:
			return(0x0406);	/*	#CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I */
		case 0xB3:
			return(0x0456);	/*	#CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I */
		case 0xB4:
			return(0x0491);	/*	#CYRILLIC SMALL LETTER GHE WITH UPTURN */
		case 0xB8:
			return(0x0451);	/*	#CYRILLIC SMALL LETTER IO */
		case 0xB9:
			return(0x2116);	/*	#NUMERO SIGN */
		case 0xBA:
			return(0x0454);	/*	#CYRILLIC SMALL LETTER UKRAINIAN IE */
		case 0xBC:
			return(0x0458);	/*	#CYRILLIC SMALL LETTER JE */
		case 0xBD:
			return(0x0405);	/*	#CYRILLIC CAPITAL LETTER DZE */
		case 0xBE:
			return(0x0455);	/*	#CYRILLIC SMALL LETTER DZE */
		case 0xBF:
			return(0x0457);	/*	#CYRILLIC SMALL LETTER YI */
		case 0xC0:
			return(0x0410);	/*	#CYRILLIC CAPITAL LETTER A */
		case 0xC1:
			return(0x0411);	/*	#CYRILLIC CAPITAL LETTER BE */
		case 0xC2:
			return(0x0412);	/*	#CYRILLIC CAPITAL LETTER VE */
		case 0xC3:
			return(0x0413);	/*	#CYRILLIC CAPITAL LETTER GHE */
		case 0xC4:
			return(0x0414);	/*	#CYRILLIC CAPITAL LETTER DE */
		case 0xC5:
			return(0x0415);	/*	#CYRILLIC CAPITAL LETTER IE */
		case 0xC6:
			return(0x0416);	/*	#CYRILLIC CAPITAL LETTER ZHE */
		case 0xC7:
			return(0x0417);	/*	#CYRILLIC CAPITAL LETTER ZE */
		case 0xC8:
			return(0x0418);	/*	#CYRILLIC CAPITAL LETTER I */
		case 0xC9:
			return(0x0419);	/*	#CYRILLIC CAPITAL LETTER SHORT I */
		case 0xCA:
			return(0x041A);	/*	#CYRILLIC CAPITAL LETTER KA */
		case 0xCB:
			return(0x041B);	/*	#CYRILLIC CAPITAL LETTER EL */
		case 0xCC:
			return(0x041C);	/*	#CYRILLIC CAPITAL LETTER EM */
		case 0xCD:
			return(0x041D);	/*	#CYRILLIC CAPITAL LETTER EN */
		case 0xCE:
			return(0x041E);	/*	#CYRILLIC CAPITAL LETTER O */
		case 0xCF:
			return(0x041F);	/*	#CYRILLIC CAPITAL LETTER PE */
		case 0xD0:
			return(0x0420);	/*	#CYRILLIC CAPITAL LETTER ER */
		case 0xD1:
			return(0x0421);	/*	#CYRILLIC CAPITAL LETTER ES */
		case 0xD2:
			return(0x0422);	/*	#CYRILLIC CAPITAL LETTER TE */
		case 0xD3:
			return(0x0423);	/*	#CYRILLIC CAPITAL LETTER U */
		case 0xD4:
			return(0x0424);	/*	#CYRILLIC CAPITAL LETTER EF */
		case 0xD5:
			return(0x0425);	/*	#CYRILLIC CAPITAL LETTER HA */
		case 0xD6:
			return(0x0426);	/*	#CYRILLIC CAPITAL LETTER TSE */
		case 0xD7:
			return(0x0427);	/*	#CYRILLIC CAPITAL LETTER CHE */
		case 0xD8:
			return(0x0428);	/*	#CYRILLIC CAPITAL LETTER SHA */
		case 0xD9:
			return(0x0429);	/*	#CYRILLIC CAPITAL LETTER SHCHA */
		case 0xDA:
			return(0x042A);	/*	#CYRILLIC CAPITAL LETTER HARD SIGN */
		case 0xDB:
			return(0x042B);	/*	#CYRILLIC CAPITAL LETTER YERU */
		case 0xDC:
			return(0x042C);	/*	#CYRILLIC CAPITAL LETTER SOFT SIGN */
		case 0xDD:
			return(0x042D);	/*	#CYRILLIC CAPITAL LETTER E */
		case 0xDE:
			return(0x042E);	/*	#CYRILLIC CAPITAL LETTER YU */
		case 0xDF:
			return(0x042F);	/*	#CYRILLIC CAPITAL LETTER YA */
		case 0xE0:
			return(0x0430);	/*	#CYRILLIC SMALL LETTER A */
		case 0xE1:
			return(0x0431);	/*	#CYRILLIC SMALL LETTER BE */
		case 0xE2:
			return(0x0432);	/*	#CYRILLIC SMALL LETTER VE */
		case 0xE3:
			return(0x0433);	/*	#CYRILLIC SMALL LETTER GHE */
		case 0xE4:
			return(0x0434);	/*	#CYRILLIC SMALL LETTER DE */
		case 0xE5:
			return(0x0435);	/*	#CYRILLIC SMALL LETTER IE */
		case 0xE6:
			return(0x0436);	/*	#CYRILLIC SMALL LETTER ZHE */
		case 0xE7:
			return(0x0437);	/*	#CYRILLIC SMALL LETTER ZE */
		case 0xE8:
			return(0x0438);	/*	#CYRILLIC SMALL LETTER I */
		case 0xE9:
			return(0x0439);	/*	#CYRILLIC SMALL LETTER SHORT I */
		case 0xEA:
			return(0x043A);	/*	#CYRILLIC SMALL LETTER KA */
		case 0xEB:
			return(0x043B);	/*	#CYRILLIC SMALL LETTER EL */
		case 0xEC:
			return(0x043C);	/*	#CYRILLIC SMALL LETTER EM */
		case 0xED:
			return(0x043D);	/*	#CYRILLIC SMALL LETTER EN */
		case 0xEE:
			return(0x043E);	/*	#CYRILLIC SMALL LETTER O */
		case 0xEF:
			return(0x043F);	/*	#CYRILLIC SMALL LETTER PE */
		case 0xF0:
			return(0x0440);	/*	#CYRILLIC SMALL LETTER ER */
		case 0xF1:
			return(0x0441);	/*	#CYRILLIC SMALL LETTER ES */
		case 0xF2:
			return(0x0442);	/*	#CYRILLIC SMALL LETTER TE */
		case 0xF3:
			return(0x0443);	/*	#CYRILLIC SMALL LETTER U */
		case 0xF4:
			return(0x0444);	/*	#CYRILLIC SMALL LETTER EF */
		case 0xF5:
			return(0x0445);	/*	#CYRILLIC SMALL LETTER HA */
		case 0xF6:
			return(0x0446);	/*	#CYRILLIC SMALL LETTER TSE */
		case 0xF7:
			return(0x0447);	/*	#CYRILLIC SMALL LETTER CHE */
		case 0xF8:
			return(0x0448);	/*	#CYRILLIC SMALL LETTER SHA */
		case 0xF9:
			return(0x0449);	/*	#CYRILLIC SMALL LETTER SHCHA */
		case 0xFA:
			return(0x044A);	/*	#CYRILLIC SMALL LETTER HARD SIGN */
		case 0xFB:
			return(0x044B);	/*	#CYRILLIC SMALL LETTER YERU */
		case 0xFC:
			return(0x044C);	/*	#CYRILLIC SMALL LETTER SOFT SIGN */
		case 0xFD:
			return(0x044D);	/*	#CYRILLIC SMALL LETTER E */
		case 0xFE:
			return(0x044E);	/*	#CYRILLIC SMALL LETTER YU */
		case 0xFF:
			return(0x044F);	/*	#CYRILLIC SMALL LETTER YA */
		default:
			return(char8);
		}
	}
