#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "iconv_internal.h"


size_t wvConvert1255ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
    size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert1255ToUnicode((unsigned char)(**inbuf));
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


U16 Convert1255ToUnicode(U16 char8)
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
			return(0x02C6);	/*	#MODIFIER LETTER CIRCUMFLEX ACCENT */
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
			return(0x02DC);	/*	#SMALL TILDE */
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
		case 0xA4:
			return(0x20AA);	/*	#NEW SHEQEL SIGN */
		case 0xAA:
			return(0x00D7);	/*	#MULTIPLICATION SIGN */
		case 0xBA:
			return(0x00F7);	/*	#DIVISION SIGN */
		case 0xC0:
			return(0x05B0);	/*	#HEBREW POINT SHEVA */
		case 0xC1:
			return(0x05B1);	/*	#HEBREW POINT HATAF SEGOL */
		case 0xC2:
			return(0x05B2);	/*	#HEBREW POINT HATAF PATAH */
		case 0xC3:
			return(0x05B3);	/*	#HEBREW POINT HATAF QAMATS */
		case 0xC4:
			return(0x05B4);	/*	#HEBREW POINT HIRIQ */
		case 0xC5:
			return(0x05B5);	/*	#HEBREW POINT TSERE */
		case 0xC6:
			return(0x05B6);	/*	#HEBREW POINT SEGOL */
		case 0xC7:
			return(0x05B7);	/*	#HEBREW POINT PATAH */
		case 0xC8:
			return(0x05B8);	/*	#HEBREW POINT QAMATS */
		case 0xC9:
			return(0x05B9);	/*	#HEBREW POINT HOLAM */
		case 0xCA:
			return(0xffff);	/*	#UNDEFINED */
		case 0xCB:
			return(0x05BB);	/*	#HEBREW POINT QUBUTS */
		case 0xCC:
			return(0x05BC);	/*	#HEBREW POINT DAGESH OR MAPIQ */
		case 0xCD:
			return(0x05BD);	/*	#HEBREW POINT METEG */
		case 0xCE:
			return(0x05BE);	/*	#HEBREW PUNCTUATION MAQAF */
		case 0xCF:
			return(0x05BF);	/*	#HEBREW POINT RAFE */
		case 0xD0:
			return(0x05C0);	/*	#HEBREW PUNCTUATION PASEQ */
		case 0xD1:
			return(0x05C1);	/*	#HEBREW POINT SHIN DOT */
		case 0xD2:
			return(0x05C2);	/*	#HEBREW POINT SIN DOT */
		case 0xD3:
			return(0x05C3);	/*	#HEBREW PUNCTUATION SOF PASUQ */
		case 0xD4:
			return(0x05F0);	/*	#HEBREW LIGATURE YIDDISH DOUBLE VAV */
		case 0xD5:
			return(0x05F1);	/*	#HEBREW LIGATURE YIDDISH VAV YOD */
		case 0xD6:
			return(0x05F2);	/*	#HEBREW LIGATURE YIDDISH DOUBLE YOD */
		case 0xD7:
			return(0x05F3);	/*	#HEBREW PUNCTUATION GERESH */
		case 0xD8:
			return(0x05F4);	/*	#HEBREW PUNCTUATION GERSHAYIM */
		case 0xD9:
		case 0xDA:
		case 0xDB:
		case 0xDC:
		case 0xDD:
		case 0xDE:
		case 0xDF:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xE0:
			return(0x05D0);	/*	#HEBREW LETTER ALEF */
		case 0xE1:
			return(0x05D1);	/*	#HEBREW LETTER BET */
		case 0xE2:
			return(0x05D2);	/*	#HEBREW LETTER GIMEL */
		case 0xE3:
			return(0x05D3);	/*	#HEBREW LETTER DALET */
		case 0xE4:
			return(0x05D4);	/*	#HEBREW LETTER HE */
		case 0xE5:
			return(0x05D5);	/*	#HEBREW LETTER VAV */
		case 0xE6:
			return(0x05D6);	/*	#HEBREW LETTER ZAYIN */
		case 0xE7:
			return(0x05D7);	/*	#HEBREW LETTER HET */
		case 0xE8:
			return(0x05D8);	/*	#HEBREW LETTER TET */
		case 0xE9:
			return(0x05D9);	/*	#HEBREW LETTER YOD */
		case 0xEA:
			return(0x05DA);	/*	#HEBREW LETTER FINAL KAF */
		case 0xEB:
			return(0x05DB);	/*	#HEBREW LETTER KAF */
		case 0xEC:
			return(0x05DC);	/*	#HEBREW LETTER LAMED */
		case 0xED:
			return(0x05DD);	/*	#HEBREW LETTER FINAL MEM */
		case 0xEE:
			return(0x05DE);	/*	#HEBREW LETTER MEM */
		case 0xEF:
			return(0x05DF);	/*	#HEBREW LETTER FINAL NUN */
		case 0xF0:
			return(0x05E0);	/*	#HEBREW LETTER NUN */
		case 0xF1:
			return(0x05E1);	/*	#HEBREW LETTER SAMEKH */
		case 0xF2:
			return(0x05E2);	/*	#HEBREW LETTER AYIN */
		case 0xF3:
			return(0x05E3);	/*	#HEBREW LETTER FINAL PE */
		case 0xF4:
			return(0x05E4);	/*	#HEBREW LETTER PE */
		case 0xF5:
			return(0x05E5);	/*	#HEBREW LETTER FINAL TSADI */
		case 0xF6:
			return(0x05E6);	/*	#HEBREW LETTER TSADI */
		case 0xF7:
			return(0x05E7);	/*	#HEBREW LETTER QOF */
		case 0xF8:
			return(0x05E8);	/*	#HEBREW LETTER RESH */
		case 0xF9:
			return(0x05E9);	/*	#HEBREW LETTER SHIN */
		case 0xFA:
			return(0x05EA);	/*	#HEBREW LETTER TAV */
		case 0xFB:
		case 0xFC:
			return(0xffff);	/*      	#UNDEFINED */
		case 0xFD:
			return(0x200E);	/*	#LEFT-TO-RIGHT MARK */
		case 0xFE:
			return(0x200F);	/*	#RIGHT-TO-LEFT MARK */
		case 0xFF:
			return(0xffff);	/*      	#UNDEFINED */
		default:
			return(char8);
		}
	}
