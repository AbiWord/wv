#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
returns 0xffff is the 1252 char is a undefined char,
in which case for html output you can go down the gif
inclusion route
*/
U16 wvConvert1252ToUnicode(U16 char8)
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


/*
returns 1 if the char was in the range of 
1252 that differs from iso-5589-1,

converts the chars into their html equivalent if
possible
*/
int wvConvert1252ToHtml(U16 char8)
	{
	switch (char8)
		{
		/* 
		german characters, im assured that this is the right way to handle them
		by Markus Schulte <markus@dom.de>
		*/
		case 0xc4:
			printf("&Auml;");
			return(1); 
		case 0xe4:
			printf("&auml;");
			return(1); 
		case 0xdc:
			printf("&Uuml;");
			return(1); 
		case 0xfc:
			printf("&uuml;");
			return(1); 
		case 0xd6:
			printf("&Ouml;");
			return(1); 
		case 0xf6:
			printf("&ouml;");
			return(1); 
		case 0xdf:
			printf("&szlig;");
			return(1); 
		/* end german characters */
		case 0x80:
			printf("&euro;"); /*EURO SIGN*/
			return(1); 
		case 0x81:          /*UNDEFINED*/
			return(1); 
		case 0x82:
			printf("&bsquo;");  /*SINGLE LOW-9 QUOTATION MARK*/
			return(1); 
		case 0x83:    
			printf("&fnof;");  /*LATIN SMALL LETTER F WITH HOOK*/
			return(1); 
		case 0x84:    
			printf("&bdquo;");  /*DOUBLE LOW-9 QUOTATION MARK*/
			return(1); 
		case 0x85:    
			printf("&hellip;");  /*HORIZONTAL ELLIPSIS*/
			return(1); 
		case 0x86:    
			printf("&dagger;");  /*DAGGER*/
			return(1); 
		case 0x87:    
			printf("&Dagger;");  /*DOUBLE DAGGER*/
			return(1); 
		case 0x88:    
			printf("&circ;");  /*MODIFIER LETTER CIRCUMFLEX ACCENT*/
			return(1); 
		case 0x89:    
			printf("&permil;");  /*PER MILLE SIGN*/
			return(1); 
		case 0x8A:    
			printf("&Scaron;");  /*LATIN CAPITAL LETTER S WITH CARON*/
			return(1); 
		case 0x8B:    
			printf("&lsaquo;");  /*SINGLE LEFT-POINTING ANGLE QUOTATION MARK*/
			return(1); 
		case 0x8C:    
			printf("&OElig;");  /*LATIN CAPITAL LIGATURE OE*/
			return(1); 
		case 0x8D:            /*UNDEFINED*/
			return(1); 
		case 0x8E:    
			printf("&#381;");  /*LATIN CAPITAL LETTER Z WITH CARON*/
			return(1); 
		case 0x8F:            /*UNDEFINED*/
			return(1); 
		case 0x90:            /*UNDEFINED*/
			return(1); 
		case 0x91:    
			printf("&lsquo;");  /*LEFT SINGLE QUOTATION MARK*/
			return(1); 
		case 0x92:    
			printf("&rsquo;");  /*RIGHT SINGLE QUOTATION MARK*/
			return(1); 
		case 0x93:    
			printf("&ldquo;");  /*LEFT DOUBLE QUOTATION MARK*/
			return(1); 
		case 0x94:    
			printf("&rdquo;");  /*RIGHT DOUBLE QUOTATION MARK*/
			return(1); 
		case 0x95:    
			printf("&bull;");  /*BULLET*/
			return(1); 
		case 0x96:    
			printf("&ndash;");  /*EN DASH*/
			return(1); 
		case 0x97:    
			printf("&mdash;");  /*EM DASH*/
			return(1); 
		case 0x98:    
			printf("&tilde;");  /*SMALL TILDE*/
			return(1); 
		case 0x99:    
			printf("&trade;");  /*TRADE MARK SIGN*/
			return(1); 
		case 0x9A:    
			printf("&scaron;");  /*LATIN SMALL LETTER S WITH CARON*/
			return(1); 
		case 0x9B:    
			printf("&rsaquo;");  /*SINGLE RIGHT-POINTING ANGLE QUOTATION MARKS*/
			return(1); 
		case 0x9C:    
			printf("&oelig;");  /*LATIN SMALL LIGATURE OE*/
			return(1); 
		case 0x9D:            /*UNDEFINED*/
			return(1); 
		case 0x9E:    
			printf("&382;");  /*LATIN SMALL LETTER Z WITH CARON*/
			return(1); 
		case 0x9F:    
			printf("&Yuml;");  /*LATIN CAPITAL LETTER Y WITH DIAERESIS*/
			return(1); 
		default:
			return(0);
		}
	}
