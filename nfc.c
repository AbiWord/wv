#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "roman.h"

char *wvGenerateNFC(int value,int no_type)
    {
    char *roman;
	roman = (char *)malloc(81);
	if (roman == NULL)
		return(NULL);
    switch(no_type)
        {
        case 0:
            sprintf(roman,"%d",value);
            break;
        case 1:
            decimalToRoman(value,roman);
            break;
        case 2:
            decimalToRoman(value,roman);
            ms_strlower(roman);
            break;
        case 3:
            sprintf(roman,"%c",64+value); /*uppercase letter*/
            break;
        case 4:
            sprintf(roman,"%c",96+value); /*lowercase letter*/
            break;
		case 5:		/*what does the spec actually mean when it says Ordinal ?*/
			wvFree(roman);
			roman = wvOrdinal(value);
			break;
        /*a nine appears to exist as well to means to use symbols*/
        case 9:
            while(value > 0)
                {
                switch(value%4)
                    {
                    case 1:
                        sprintf(roman,"*");
                        break;
                    case 2:
                        sprintf(roman,"%s","&#8224");  /*these are windows specific, must fix*/
                        break;
                    case 3:
                        sprintf(roman,"%s","&#8225");  /*these are windows specific, must fix*/
                        break;
                    case 0:
                        sprintf(roman,"%c",167);
                        break;
                    }
                value-=4;
                }
            break;
        default:
			wvFree(roman);
			return(NULL);
            break;
        }
	return(roman);
    }
