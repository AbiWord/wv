#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
returns 1 if the char has a html name

converts the chars into their html equivalent if
possible
*/
int wvConvert1252ToHtml(U16 char8)
	{
	switch (char8)
		{
		case 11:
            printf("<br>");
            return(1);
        case 30:
        case 31:
        case 45:
            printf("-");
            return(1);
        case 12:
        case 13:
        case 14:
        case 7:
            return(1);
        case 34:
            printf("&quot;");
            return(1);
        case 38:
            printf("&amp;");
            return(1);
        case 60:
            printf("&lt;");
            return(1);
        case 62:
            printf("&gt;");
            return(1);
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
			printf("...");  /*HORIZONTAL ELLIPSIS*/
#if 0
			printf("&hellip;");  /*HORIZONTAL ELLIPSIS*/
#endif
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
		case 0x8E:    
			printf("&#381;");  /*LATIN CAPITAL LETTER Z WITH CARON*/
			return(1); 
		case 0x91:    
#if 0
			printf("&lsquo;");  /*LEFT SINGLE QUOTATION MARK*/
#endif
			printf("`");  /*LEFT SINGLE QUOTATION MARK*/
			return(1); 
		case 0x92:    
#if 0
			printf("&rsquo;");  /*RIGHT SINGLE QUOTATION MARK*/
#endif
			printf("'");  /*RIGHT SINGLE QUOTATION MARK*/
			return(1); 
		case 0x93:    
#if 0
			printf("&ldquo;");  /*LEFT DOUBLE QUOTATION MARK*/
#endif
			printf("\"");  /*LEFT DOUBLE QUOTATION MARK*/
			return(1); 
		case 0x94:    
#if 0
			printf("&rdquo;");  /*RIGHT DOUBLE QUOTATION MARK*/
#endif
			printf("\"");  /*RIGHT DOUBLE QUOTATION MARK*/
			return(1); 
		case 0x95:    
			printf("&bull;");  /*BULLET*/
			return(1); 
		case 0x96:    
			printf("-");  /*EN DASH*/
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
		case 0x9E:    
			printf("&382;");  /*LATIN SMALL LETTER Z WITH CARON*/
			return(1); 
		case 0x9F:    
			printf("&Yuml;");  /*LATIN CAPITAL LETTER Y WITH DIAERESIS*/
			return(1); 
		case 0xA0:
			printf("&nbsp;");
			return(1);
		default:
			return(0);
		}
	}
