#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include "iconv_internal.h"


size_t wvConvert874ToUnicode(const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
    {
    U16 c;
    size_t ret=0;
    while(  (*inbytesleft) && (*outbytesleft)   )
        {
        c = Convert874ToUnicode((unsigned char)(**inbuf));
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


U16 Convert874ToUnicode(U16 char8)
	{
	switch (char8)
		{
		case 0x80:
			return(0x20AC);	/*	#EURO SIGN */
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
			return(0xffff);
		case 0x85:
			return(0x2026);	/*	#HORIZONTAL ELLIPSIS */
		case 0x86:
		case 0x87:
		case 0x88:
		case 0x89:
		case 0x8A:
		case 0x8B:
		case 0x8C:
		case 0x8D:
		case 0x8E:
		case 0x8F:
		case 0x90:
			return(0xffff);
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
		case 0x99:	
		case 0x9A:	
		case 0x9B:	
		case 0x9C:	
		case 0x9D:	
		case 0x9E:	
		case 0x9F:
			return(0xffff);
		case 0xA0:
			return(0x00A0);	/*	#NO-BREAK SPACE */
		case 0xA1:
			return(0x0E01);	/*	#THAI CHARACTER KO KAI */
		case 0xA2:
			return(0x0E02);	/*	#THAI CHARACTER KHO KHAI */
		case 0xA3:
			return(0x0E03);	/*	#THAI CHARACTER KHO KHUAT */
		case 0xA4:
			return(0x0E04);	/*	#THAI CHARACTER KHO KHWAI */
		case 0xA5:
			return(0x0E05);	/*	#THAI CHARACTER KHO KHON */
		case 0xA6:
			return(0x0E06);	/*	#THAI CHARACTER KHO RAKHANG */
		case 0xA7:
			return(0x0E07);	/*	#THAI CHARACTER NGO NGU */
		case 0xA8:
			return(0x0E08);	/*	#THAI CHARACTER CHO CHAN */
		case 0xA9:
			return(0x0E09);	/*	#THAI CHARACTER CHO CHING */
		case 0xAA:
			return(0x0E0A);	/*	#THAI CHARACTER CHO CHANG */
		case 0xAB:
			return(0x0E0B);	/*	#THAI CHARACTER SO SO */
		case 0xAC:
			return(0x0E0C);	/*	#THAI CHARACTER CHO CHOE */
		case 0xAD:
			return(0x0E0D);	/*	#THAI CHARACTER YO YING */
		case 0xAE:
			return(0x0E0E);	/*	#THAI CHARACTER DO CHADA */
		case 0xAF:
			return(0x0E0F);	/*	#THAI CHARACTER TO PATAK */
		case 0xB0:
			return(0x0E10);	/*	#THAI CHARACTER THO THAN */
		case 0xB1:
			return(0x0E11);	/*	#THAI CHARACTER THO NANGMONTHO */
		case 0xB2:
			return(0x0E12);	/*	#THAI CHARACTER THO PHUTHAO */
		case 0xB3:
			return(0x0E13);	/*	#THAI CHARACTER NO NEN */
		case 0xB4:
			return(0x0E14);	/*	#THAI CHARACTER DO DEK */
		case 0xB5:
			return(0x0E15);	/*	#THAI CHARACTER TO TAO */
		case 0xB6:
			return(0x0E16);	/*	#THAI CHARACTER THO THUNG */
		case 0xB7:
			return(0x0E17);	/*	#THAI CHARACTER THO THAHAN */
		case 0xB8:
			return(0x0E18);	/*	#THAI CHARACTER THO THONG */
		case 0xB9:
			return(0x0E19);	/*	#THAI CHARACTER NO NU */
		case 0xBA:
			return(0x0E1A);	/*	#THAI CHARACTER BO BAIMAI */
		case 0xBB:
			return(0x0E1B);	/*	#THAI CHARACTER PO PLA */
		case 0xBC:
			return(0x0E1C);	/*	#THAI CHARACTER PHO PHUNG */
		case 0xBD:
			return(0x0E1D);	/*	#THAI CHARACTER FO FA */
		case 0xBE:
			return(0x0E1E);	/*	#THAI CHARACTER PHO PHAN */
		case 0xBF:
			return(0x0E1F);	/*	#THAI CHARACTER FO FAN */
		case 0xC0:
			return(0x0E20);	/*	#THAI CHARACTER PHO SAMPHAO */
		case 0xC1:
			return(0x0E21);	/*	#THAI CHARACTER MO MA */
		case 0xC2:
			return(0x0E22);	/*	#THAI CHARACTER YO YAK */
		case 0xC3:
			return(0x0E23);	/*	#THAI CHARACTER RO RUA */
		case 0xC4:
			return(0x0E24);	/*	#THAI CHARACTER RU */
		case 0xC5:
			return(0x0E25);	/*	#THAI CHARACTER LO LING */
		case 0xC6:
			return(0x0E26);	/*	#THAI CHARACTER LU */
		case 0xC7:
			return(0x0E27);	/*	#THAI CHARACTER WO WAEN */
		case 0xC8:
			return(0x0E28);	/*	#THAI CHARACTER SO SALA */
		case 0xC9:
			return(0x0E29);	/*	#THAI CHARACTER SO RUSI */
		case 0xCA:
			return(0x0E2A);	/*	#THAI CHARACTER SO SUA */
		case 0xCB:
			return(0x0E2B);	/*	#THAI CHARACTER HO HIP */
		case 0xCC:
			return(0x0E2C);	/*	#THAI CHARACTER LO CHULA */
		case 0xCD:
			return(0x0E2D);	/*	#THAI CHARACTER O ANG */
		case 0xCE:
			return(0x0E2E);	/*	#THAI CHARACTER HO NOKHUK */
		case 0xCF:
			return(0x0E2F);	/*	#THAI CHARACTER PAIYANNOI */
		case 0xD0:
			return(0x0E30);	/*	#THAI CHARACTER SARA A */
		case 0xD1:
			return(0x0E31);	/*	#THAI CHARACTER MAI HAN-AKAT */
		case 0xD2:
			return(0x0E32);	/*	#THAI CHARACTER SARA AA */
		case 0xD3:
			return(0x0E33);	/*	#THAI CHARACTER SARA AM */
		case 0xD4:
			return(0x0E34);	/*	#THAI CHARACTER SARA I */
		case 0xD5:
			return(0x0E35);	/*	#THAI CHARACTER SARA II */
		case 0xD6:
			return(0x0E36);	/*	#THAI CHARACTER SARA UE */
		case 0xD7:
			return(0x0E37);	/*	#THAI CHARACTER SARA UEE */
		case 0xD8:
			return(0x0E38);	/*	#THAI CHARACTER SARA U */
		case 0xD9:
			return(0x0E39);	/*	#THAI CHARACTER SARA UU */
		case 0xDA:
			return(0x0E3A);	/*	#THAI CHARACTER PHINTHU */
		case 0xDB:
		case 0xDC:	   
		case 0xDD:	   
		case 0xDE:
			return(0xffff);
		case 0xDF:
			return(0x0E3F);	/*	#THAI CURRENCY SYMBOL BAHT */
		case 0xE0:
			return(0x0E40);	/*	#THAI CHARACTER SARA E */
		case 0xE1:
			return(0x0E41);	/*	#THAI CHARACTER SARA AE */
		case 0xE2:
			return(0x0E42);	/*	#THAI CHARACTER SARA O */
		case 0xE3:
			return(0x0E43);	/*	#THAI CHARACTER SARA AI MAIMUAN */
		case 0xE4:
			return(0x0E44);	/*	#THAI CHARACTER SARA AI MAIMALAI */
		case 0xE5:
			return(0x0E45);	/*	#THAI CHARACTER LAKKHANGYAO */
		case 0xE6:
			return(0x0E46);	/*	#THAI CHARACTER MAIYAMOK */
		case 0xE7:
			return(0x0E47);	/*	#THAI CHARACTER MAITAIKHU */
		case 0xE8:
			return(0x0E48);	/*	#THAI CHARACTER MAI EK */
		case 0xE9:
			return(0x0E49);	/*	#THAI CHARACTER MAI THO */
		case 0xEA:
			return(0x0E4A);	/*	#THAI CHARACTER MAI TRI */
		case 0xEB:
			return(0x0E4B);	/*	#THAI CHARACTER MAI CHATTAWA */
		case 0xEC:
			return(0x0E4C);	/*	#THAI CHARACTER THANTHAKHAT */
		case 0xED:
			return(0x0E4D);	/*	#THAI CHARACTER NIKHAHIT */
		case 0xEE:
			return(0x0E4E);	/*	#THAI CHARACTER YAMAKKAN */
		case 0xEF:
			return(0x0E4F);	/*	#THAI CHARACTER FONGMAN */
		case 0xF0:
			return(0x0E50);	/*	#THAI DIGIT ZERO */
		case 0xF1:
			return(0x0E51);	/*	#THAI DIGIT ONE */
		case 0xF2:
			return(0x0E52);	/*	#THAI DIGIT TWO */
		case 0xF3:
			return(0x0E53);	/*	#THAI DIGIT THREE */
		case 0xF4:
			return(0x0E54);	/*	#THAI DIGIT FOUR */
		case 0xF5:
			return(0x0E55);	/*	#THAI DIGIT FIVE */
		case 0xF6:
			return(0x0E56);	/*	#THAI DIGIT SIX */
		case 0xF7:
			return(0x0E57);	/*	#THAI DIGIT SEVEN */
		case 0xF8:
			return(0x0E58);	/*	#THAI DIGIT EIGHT */
		case 0xF9:
			return(0x0E59);	/*	#THAI DIGIT NINE */
		case 0xFA:
			return(0x0E5A);	/*	#THAI CHARACTER ANGKHANKHU */
		case 0xFB:
			return(0x0E5B);	/*	#THAI CHARACTER KHOMUT */
		case 0xFC:
		case 0xFD:
		case 0xFE:
		case 0xFF:	
			return(0xffff);
		default:
			return(char8);
		}
	}
