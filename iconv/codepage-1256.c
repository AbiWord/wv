#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "iconv_internal.h"

size_t wvConvert1256ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
    size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert1256ToUnicode((unsigned char)(**inbuf));
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


U16 Convert1256ToUnicode(U16 char8)
	{
	switch (char8)
		{
		case 0x80:
			return(0x20AC);	/*	#EURO SIGN */
		case 0x81:
			return(0x067E);	/*	#ARABIC LETTER PEH */
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
			return(0x0679);	/*	#ARABIC LETTER TTEH */
		case 0x8B:
			return(0x2039);	/*	#SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
		case 0x8C:
			return(0x0152);	/*	#LATIN CAPITAL LIGATURE OE */
		case 0x8D:
			return(0x0686);	/*	#ARABIC LETTER TCHEH */
		case 0x8E:
			return(0x0698);	/*	#ARABIC LETTER JEH */
		case 0x8F:
			return(0x0688);	/*	#ARABIC LETTER DDAL */
		case 0x90:
			return(0x06AF);	/*	#ARABIC LETTER GAF */
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
			return(0x06A9);	/*	#ARABIC LETTER KEHEH */
		case 0x99:
			return(0x2122);	/*	#TRADE MARK SIGN */
		case 0x9A:
			return(0x0691);	/*	#ARABIC LETTER RREH */
		case 0x9B:
			return(0x203A);	/*	#SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
		case 0x9C:
			return(0x0153);	/*	#LATIN SMALL LIGATURE OE */
		case 0x9D:
			return(0x200C);	/*	#ZERO WIDTH NON-JOINER */
		case 0x9E:
			return(0x200D);	/*	#ZERO WIDTH JOINER */
		case 0x9F:
			return(0x06BA);	/*	#ARABIC LETTER NOON GHUNNA */
		case 0xA0:
			return(0x00A0);	/*	#NO-BREAK SPACE */
		case 0xA1:
			return(0x060C);	/*	#ARABIC COMMA */
		case 0xA2:
			return(0x00A2);	/*	#CENT SIGN */
		case 0xA3:
			return(0x00A3);	/*	#POUND SIGN */
		case 0xA4:
			return(0x00A4);	/*	#CURRENCY SIGN */
		case 0xA5:
			return(0x00A5);	/*	#YEN SIGN */
		case 0xA6:
			return(0x00A6);	/*	#BROKEN BAR */
		case 0xA7:
			return(0x00A7);	/*	#SECTION SIGN */
		case 0xA8:
			return(0x00A8);	/*	#DIAERESIS */
		case 0xA9:
			return(0x00A9);	/*	#COPYRIGHT SIGN */
		case 0xAA:
			return(0x06BE);	/*	#ARABIC LETTER HEH DOACHASHMEE */
		case 0xAB:
			return(0x00AB);	/*	#LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
		case 0xAC:
			return(0x00AC);	/*	#NOT SIGN */
		case 0xAD:
			return(0x00AD);	/*	#SOFT HYPHEN */
		case 0xAE:
			return(0x00AE);	/*	#REGISTERED SIGN */
		case 0xAF:
			return(0x00AF);	/*	#MACRON */
		case 0xB0:
			return(0x00B0);	/*	#DEGREE SIGN */
		case 0xB1:
			return(0x00B1);	/*	#PLUS-MINUS SIGN */
		case 0xB2:
			return(0x00B2);	/*	#SUPERSCRIPT TWO */
		case 0xB3:
			return(0x00B3);	/*	#SUPERSCRIPT THREE */
		case 0xB4:
			return(0x00B4);	/*	#ACUTE ACCENT */
		case 0xB5:
			return(0x00B5);	/*	#MICRO SIGN */
		case 0xB6:
			return(0x00B6);	/*	#PILCROW SIGN */
		case 0xB7:
			return(0x00B7);	/*	#MIDDLE DOT */
		case 0xB8:
			return(0x00B8);	/*	#CEDILLA */
		case 0xB9:
			return(0x00B9);	/*	#SUPERSCRIPT ONE */
		case 0xBA:
			return(0x061B);	/*	#ARABIC SEMICOLON */
		case 0xBB:
			return(0x00BB);	/*	#RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
		case 0xBC:
			return(0x00BC);	/*	#VULGAR FRACTION ONE QUARTER */
		case 0xBD:
			return(0x00BD);	/*	#VULGAR FRACTION ONE HALF */
		case 0xBE:
			return(0x00BE);	/*	#VULGAR FRACTION THREE QUARTERS */
		case 0xBF:
			return(0x061F);	/*	#ARABIC QUESTION MARK */
		case 0xC0:
			return(0x06C1);	/*	#ARABIC LETTER HEH GOAL */
		case 0xC1:
			return(0x0621);	/*	#ARABIC LETTER HAMZA */
		case 0xC2:
			return(0x0622);	/*	#ARABIC LETTER ALEF WITH MADDA ABOVE */
		case 0xC3:
			return(0x0623);	/*	#ARABIC LETTER ALEF WITH HAMZA ABOVE */
		case 0xC4:
			return(0x0624);	/*	#ARABIC LETTER WAW WITH HAMZA ABOVE */
		case 0xC5:
			return(0x0625);	/*	#ARABIC LETTER ALEF WITH HAMZA BELOW */
		case 0xC6:
			return(0x0626);	/*	#ARABIC LETTER YEH WITH HAMZA ABOVE */
		case 0xC7:
			return(0x0627);	/*	#ARABIC LETTER ALEF */
		case 0xC8:
			return(0x0628);	/*	#ARABIC LETTER BEH */
		case 0xC9:
			return(0x0629);	/*	#ARABIC LETTER TEH MARBUTA */
		case 0xCA:
			return(0x062A);	/*	#ARABIC LETTER TEH */
		case 0xCB:
			return(0x062B);	/*	#ARABIC LETTER THEH */
		case 0xCC:
			return(0x062C);	/*	#ARABIC LETTER JEEM */
		case 0xCD:
			return(0x062D);	/*	#ARABIC LETTER HAH */
		case 0xCE:
			return(0x062E);	/*	#ARABIC LETTER KHAH */
		case 0xCF:
			return(0x062F);	/*	#ARABIC LETTER DAL */
		case 0xD0:
			return(0x0630);	/*	#ARABIC LETTER THAL */
		case 0xD1:
			return(0x0631);	/*	#ARABIC LETTER REH */
		case 0xD2:
			return(0x0632);	/*	#ARABIC LETTER ZAIN */
		case 0xD3:
			return(0x0633);	/*	#ARABIC LETTER SEEN */
		case 0xD4:
			return(0x0634);	/*	#ARABIC LETTER SHEEN */
		case 0xD5:
			return(0x0635);	/*	#ARABIC LETTER SAD */
		case 0xD6:
			return(0x0636);	/*	#ARABIC LETTER DAD */
		case 0xD7:
			return(0x00D7);	/*	#MULTIPLICATION SIGN */
		case 0xD8:
			return(0x0637);	/*	#ARABIC LETTER TAH */
		case 0xD9:
			return(0x0638);	/*	#ARABIC LETTER ZAH */
		case 0xDA:
			return(0x0639);	/*	#ARABIC LETTER AIN */
		case 0xDB:
			return(0x063A);	/*	#ARABIC LETTER GHAIN */
		case 0xDC:
			return(0x0640);	/*	#ARABIC TATWEEL */
		case 0xDD:
			return(0x0641);	/*	#ARABIC LETTER FEH */
		case 0xDE:
			return(0x0642);	/*	#ARABIC LETTER QAF */
		case 0xDF:
			return(0x0643);	/*	#ARABIC LETTER KAF */
		case 0xE0:
			return(0x00E0);	/*	#LATIN SMALL LETTER A WITH GRAVE */
		case 0xE1:
			return(0x0644);	/*	#ARABIC LETTER LAM */
		case 0xE2:
			return(0x00E2);	/*	#LATIN SMALL LETTER A WITH CIRCUMFLEX */
		case 0xE3:
			return(0x0645);	/*	#ARABIC LETTER MEEM */
		case 0xE4:
			return(0x0646);	/*	#ARABIC LETTER NOON */
		case 0xE5:
			return(0x0647);	/*	#ARABIC LETTER HEH */
		case 0xE6:
			return(0x0648);	/*	#ARABIC LETTER WAW */
		case 0xE7:
			return(0x00E7);	/*	#LATIN SMALL LETTER C WITH CEDILLA */
		case 0xE8:
			return(0x00E8);	/*	#LATIN SMALL LETTER E WITH GRAVE */
		case 0xE9:
			return(0x00E9);	/*	#LATIN SMALL LETTER E WITH ACUTE */
		case 0xEA:
			return(0x00EA);	/*	#LATIN SMALL LETTER E WITH CIRCUMFLEX */
		case 0xEB:
			return(0x00EB);	/*	#LATIN SMALL LETTER E WITH DIAERESIS */
		case 0xEC:
			return(0x0649);	/*	#ARABIC LETTER ALEF MAKSURA */
		case 0xED:
			return(0x064A);	/*	#ARABIC LETTER YEH */
		case 0xEE:
			return(0x00EE);	/*	#LATIN SMALL LETTER I WITH CIRCUMFLEX */
		case 0xEF:
			return(0x00EF);	/*	#LATIN SMALL LETTER I WITH DIAERESIS */
		case 0xF0:
			return(0x064B);	/*	#ARABIC FATHATAN */
		case 0xF1:
			return(0x064C);	/*	#ARABIC DAMMATAN */
		case 0xF2:
			return(0x064D);	/*	#ARABIC KASRATAN */
		case 0xF3:
			return(0x064E);	/*	#ARABIC FATHA */
		case 0xF4:
			return(0x00F4);	/*	#LATIN SMALL LETTER O WITH CIRCUMFLEX */
		case 0xF5:
			return(0x064F);	/*	#ARABIC DAMMA */
		case 0xF6:
			return(0x0650);	/*	#ARABIC KASRA */
		case 0xF7:
			return(0x00F7);	/*	#DIVISION SIGN */
		case 0xF8:
			return(0x0651);	/*	#ARABIC SHADDA */
		case 0xF9:
			return(0x00F9);	/*	#LATIN SMALL LETTER U WITH GRAVE */
		case 0xFA:
			return(0x0652);	/*	#ARABIC SUKUN */
		case 0xFB:
			return(0x00FB);	/*	#LATIN SMALL LETTER U WITH CIRCUMFLEX */
		case 0xFC:
			return(0x00FC);	/*	#LATIN SMALL LETTER U WITH DIAERESIS */
		case 0xFD:
			return(0x200E);	/*	#LEFT-TO-RIGHT MARK */
		case 0xFE:
			return(0x200F);	/*	#RIGHT-TO-LEFT MARK */
		case 0xFF:
			return(0x06D2);	/*	#ARABIC LETTER YEH BARREE */
		default:
			return(char8);
		}
	}
