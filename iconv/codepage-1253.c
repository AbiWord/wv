#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "iconv_internal.h"

size_t wvConvert1253ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
    size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert1253ToUnicode((unsigned char)(**inbuf));
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

U16 Convert1253ToUnicode(U16 char8)
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
			return(0x0192);	/*	#LATIN SMALL LETTER F WITH HOOK */
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
		case 0x8D:
		case 0x8E:
		case 0x8F:
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
		case 0x9D:
		case 0x9E:
		case 0x9F:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xA1:
			return(0x0385);	/*	#GREEK DIALYTIKA TONOS */
		case 0xA2:
			return(0x0386);	/*	#GREEK CAPITAL LETTER ALPHA WITH TONOS */
		case 0xAA:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xAF:
			return(0x2015);	/*	#HORIZONTAL BAR */
		case 0xB0:
			return(0x00B0);	/*	#DEGREE SIGN */
		case 0xB1:
			return(0x00B1);	/*	#PLUS-MINUS SIGN */
		case 0xB2:
			return(0x00B2);	/*	#SUPERSCRIPT TWO */
		case 0xB3:
			return(0x00B3);	/*	#SUPERSCRIPT THREE */
		case 0xB4:
			return(0x0384);	/*	#GREEK TONOS */
		case 0xB8:
			return(0x0388);	/*	#GREEK CAPITAL LETTER EPSILON WITH TONOS */
		case 0xB9:
			return(0x0389);	/*	#GREEK CAPITAL LETTER ETA WITH TONOS */
		case 0xBA:
			return(0x038A);	/*	#GREEK CAPITAL LETTER IOTA WITH TONOS */
		case 0xBB:
			return(0x00BB);	/*	#RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
		case 0xBC:
			return(0x038C);	/*	#GREEK CAPITAL LETTER OMICRON WITH TONOS */
		case 0xBD:
			return(0x00BD);	/*	#VULGAR FRACTION ONE HALF */
		case 0xBE:
			return(0x038E);	/*	#GREEK CAPITAL LETTER UPSILON WITH TONOS */
		case 0xBF:
			return(0x038F);	/*	#GREEK CAPITAL LETTER OMEGA WITH TONOS */
		case 0xC0:
			return(0x0390);	/*	#GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS */
		case 0xC1:
			return(0x0391);	/*	#GREEK CAPITAL LETTER ALPHA */
		case 0xC2:
			return(0x0392);	/*	#GREEK CAPITAL LETTER BETA */
		case 0xC3:
			return(0x0393);	/*	#GREEK CAPITAL LETTER GAMMA */
		case 0xC4:
			return(0x0394);	/*	#GREEK CAPITAL LETTER DELTA */
		case 0xC5:
			return(0x0395);	/*	#GREEK CAPITAL LETTER EPSILON */
		case 0xC6:
			return(0x0396);	/*	#GREEK CAPITAL LETTER ZETA */
		case 0xC7:
			return(0x0397);	/*	#GREEK CAPITAL LETTER ETA */
		case 0xC8:
			return(0x0398);	/*	#GREEK CAPITAL LETTER THETA */
		case 0xC9:
			return(0x0399);	/*	#GREEK CAPITAL LETTER IOTA */
		case 0xCA:
			return(0x039A);	/*	#GREEK CAPITAL LETTER KAPPA */
		case 0xCB:
			return(0x039B);	/*	#GREEK CAPITAL LETTER LAMDA */
		case 0xCC:
			return(0x039C);	/*	#GREEK CAPITAL LETTER MU */
		case 0xCD:
			return(0x039D);	/*	#GREEK CAPITAL LETTER NU */
		case 0xCE:
			return(0x039E);	/*	#GREEK CAPITAL LETTER XI */
		case 0xCF:
			return(0x039F);	/*	#GREEK CAPITAL LETTER OMICRON */
		case 0xD0:
			return(0x03A0);	/*	#GREEK CAPITAL LETTER PI */
		case 0xD1:
			return(0x03A1);	/*	#GREEK CAPITAL LETTER RHO */
		case 0xD2:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xD3:
			return(0x03A3);	/*	#GREEK CAPITAL LETTER SIGMA */
		case 0xD4:
			return(0x03A4);	/*	#GREEK CAPITAL LETTER TAU */
		case 0xD5:
			return(0x03A5);	/*	#GREEK CAPITAL LETTER UPSILON */
		case 0xD6:
			return(0x03A6);	/*	#GREEK CAPITAL LETTER PHI */
		case 0xD7:
			return(0x03A7);	/*	#GREEK CAPITAL LETTER CHI */
		case 0xD8:
			return(0x03A8);	/*	#GREEK CAPITAL LETTER PSI */
		case 0xD9:
			return(0x03A9);	/*	#GREEK CAPITAL LETTER OMEGA */
		case 0xDA:
			return(0x03AA);	/*	#GREEK CAPITAL LETTER IOTA WITH DIALYTIKA */
		case 0xDB:
			return(0x03AB);	/*	#GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA */
		case 0xDC:
			return(0x03AC);	/*	#GREEK SMALL LETTER ALPHA WITH TONOS */
		case 0xDD:
			return(0x03AD);	/*	#GREEK SMALL LETTER EPSILON WITH TONOS */
		case 0xDE:
			return(0x03AE);	/*	#GREEK SMALL LETTER ETA WITH TONOS */
		case 0xDF:
			return(0x03AF);	/*	#GREEK SMALL LETTER IOTA WITH TONOS */
		case 0xE0:
			return(0x03B0);	/*	#GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS */
		case 0xE1:
			return(0x03B1);	/*	#GREEK SMALL LETTER ALPHA */
		case 0xE2:
			return(0x03B2);	/*	#GREEK SMALL LETTER BETA */
		case 0xE3:
			return(0x03B3);	/*	#GREEK SMALL LETTER GAMMA */
		case 0xE4:
			return(0x03B4);	/*	#GREEK SMALL LETTER DELTA */
		case 0xE5:
			return(0x03B5);	/*	#GREEK SMALL LETTER EPSILON */
		case 0xE6:
			return(0x03B6);	/*	#GREEK SMALL LETTER ZETA */
		case 0xE7:
			return(0x03B7);	/*	#GREEK SMALL LETTER ETA */
		case 0xE8:
			return(0x03B8);	/*	#GREEK SMALL LETTER THETA */
		case 0xE9:
			return(0x03B9);	/*	#GREEK SMALL LETTER IOTA */
		case 0xEA:
			return(0x03BA);	/*	#GREEK SMALL LETTER KAPPA */
		case 0xEB:
			return(0x03BB);	/*	#GREEK SMALL LETTER LAMDA */
		case 0xEC:
			return(0x03BC);	/*	#GREEK SMALL LETTER MU */
		case 0xED:
			return(0x03BD);	/*	#GREEK SMALL LETTER NU */
		case 0xEE:
			return(0x03BE);	/*	#GREEK SMALL LETTER XI */
		case 0xEF:
			return(0x03BF);	/*	#GREEK SMALL LETTER OMICRON */
		case 0xF0:
			return(0x03C0);	/*	#GREEK SMALL LETTER PI */
		case 0xF1:
			return(0x03C1);	/*	#GREEK SMALL LETTER RHO */
		case 0xF2:
			return(0x03C2);	/*	#GREEK SMALL LETTER FINAL SIGMA */
		case 0xF3:
			return(0x03C3);	/*	#GREEK SMALL LETTER SIGMA */
		case 0xF4:
			return(0x03C4);	/*	#GREEK SMALL LETTER TAU */
		case 0xF5:
			return(0x03C5);	/*	#GREEK SMALL LETTER UPSILON */
		case 0xF6:
			return(0x03C6);	/*	#GREEK SMALL LETTER PHI */
		case 0xF7:
			return(0x03C7);	/*	#GREEK SMALL LETTER CHI */
		case 0xF8:
			return(0x03C8);	/*	#GREEK SMALL LETTER PSI */
		case 0xF9:
			return(0x03C9);	/*	#GREEK SMALL LETTER OMEGA */
		case 0xFA:
			return(0x03CA);	/*	#GREEK SMALL LETTER IOTA WITH DIALYTIKA */
		case 0xFB:
			return(0x03CB);	/*	#GREEK SMALL LETTER UPSILON WITH DIALYTIKA */
		case 0xFC:
			return(0x03CC);	/*	#GREEK SMALL LETTER OMICRON WITH TONOS */
		case 0xFD:
			return(0x03CD);	/*	#GREEK SMALL LETTER UPSILON WITH TONOS */
		case 0xFE:
			return(0x03CE);	/*	#GREEK SMALL LETTER OMEGA WITH TONOS */
		case 0xFF:
			return(0xffff);	/*      	#UNDEFINED */
		default:
			return(char8);
		}
	}
