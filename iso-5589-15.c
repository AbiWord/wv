#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

U16 wvConvertUnicodeToiso8859_15(U16 char16)
	{
	switch (char16)
		{
		case 0x20AC:
			return(0xA4);
			break;
		case 0x0160:
			return(0xA6);
			break;
		case 0x0161:
			return(0xA8);
			break;
		case 0x017D:
			return(0xB4);
			break;
		case 0x0152:
			return(0xBC);
			break;
		case 0x0153:
			return(0xBD);
			break;
		case 0x0178:
			return(0xBE);
			break;
		default:
			if (char16 < 0xFF)
				return(char16);
			else
				return(0xffff);
			break;
		}
	}


/*
returns 0xffff is the 1252 char is a char that does not
exist in iso-8859-15, in which case for html output you 
can go down the gif or special tag inclusion route
*/
U16 wvConvert1252Toiso8859_15(U16 char8)
	{
	switch (char8)
		{
		case 0x80:
			return(0xA4); /*EURO SIGN*/
			break;  
		case 0x81:          /*UNDEFINED*/
			return(0xffff);
			break;
		case 0x82:
			return(0xffff);  /*SINGLE LOW-9 QUOTATION MARK*/
			break;
		case 0x83:    
			return(0xffff);  /*LATIN SMALL LETTER F WITH HOOK*/
			break;
		case 0x84:    
			return(0xffff);  /*DOUBLE LOW-9 QUOTATION MARK*/
			break;
		case 0x85:    
			return(0xffff);  /*HORIZONTAL ELLIPSIS*/
			break;
		case 0x86:    
			return(0xffff);  /*DAGGER*/
			break;
		case 0x87:    
			return(0xffff);  /*DOUBLE DAGGER*/
			break;
		case 0x88:    
			return(0xffff);  /*MODIFIER LETTER CIRCUMFLEX ACCENT*/
			break;
		case 0x89:    
			return(0xffff);  /*PER MILLE SIGN*/
			break;
		case 0x8A:    
			return(0xA6);  /*LATIN CAPITAL LETTER S WITH CARON*/
			break;
		case 0x8B:    
			return(0xffff);  /*SINGLE LEFT-POINTING ANGLE QUOTATION MARK*/
			break;
		case 0x8C:    
			return(0xBC);  /*LATIN CAPITAL LIGATURE OE*/
			break;
		case 0x8D:            /*UNDEFINED*/
			return(0xffff);
			break;
		case 0x8E:    
			return(0xB4);  /*LATIN CAPITAL LETTER Z WITH CARON*/
			break;
		case 0x8F:            /*UNDEFINED*/
			return(0xffff);
			break;
		case 0x90:            /*UNDEFINED*/
			return(0xffff);
			break;
		case 0x91:    
			return(0xffff);  /*LEFT SINGLE QUOTATION MARK*/
			break;
		case 0x92:    
			return(0xffff);  /*RIGHT SINGLE QUOTATION MARK*/
			break;
		case 0x93:    
			return(0xffff);  /*LEFT DOUBLE QUOTATION MARK*/
			break;
		case 0x94:    
			return(0xffff);  /*RIGHT DOUBLE QUOTATION MARK*/
			break;
		case 0x95:    
			return(0xffff);  /*BULLET*/
			break;
		case 0x96:    
			return(0xffff);  /*EN DASH*/
			break;
		case 0x97:    
			return(0xffff);  /*EM DASH*/
			break;
		case 0x98:    
			return(0xffff);  /*SMALL TILDE*/
			break;
		case 0x99:    
			return(0xffff);  /*TRADE MARK SIGN*/
			break;
		case 0x9A:    
			return(0xA8);  /*LATIN SMALL LETTER S WITH CARON*/
			break;
		case 0x9B:    
			return(0xffff);  /*SINGLE RIGHT-POINTING ANGLE QUOTATION MARKS*/
			break;
		case 0x9C:    
			return(0xBD);  /*LATIN SMALL LIGATURE OE*/
			break;
		case 0x9D:            /*UNDEFINED*/
			return(0xffff);
			break;
		case 0x9E:    
			return(0x017E);  /*LATIN SMALL LETTER Z WITH CARON*/
			break;
		case 0x9F:    
			return(0xBE);  /*LATIN CAPITAL LETTER Y WITH DIAERESIS*/
			break;
		default:
			return(char8);
		}
	}
