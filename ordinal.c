#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include <string.h>

/*
http://216.46.233.38/code/functions/convert_numbers_to_words/index.htm

wvOrdinal numbers to words with this easy to use set of functions.  
Now, 5675 becomes Five Thousand Six Hundred Seventy Five.

'-- Sean Kelly - seank@microsoft.com
'-- Windows CE Tools

'-- Sean works for Microsoft in the CE tools division.
'-- The views and opinions expressed in the code are Sean's and
'-- do not necessarily represent those of his employer.
*/

/* 
I did a quick hack job to convert this to C, from the original VB.
Caolan.McNamara@ul.ie
*/

char *cvtText[31];
void cvtInit(void);
char *convert100(U32 x);
char *wvOrdinal(U32 x);

#if 0
int main(void)
	{
	char *temp;
	temp = wvOrdinal(500);
	if (temp)
		{
		printf("%s\n",temp);
		wvFree(temp);
		return(0);
		}
	fprintf(stderr,"Memory Allocation Problems\n");
	return(1);
	}
#endif

void cvtInit(void)
	{
	cvtText[0] = "--";
	cvtText[1] = "One";
    cvtText[2] = "Two";
    cvtText[3] = "Three";
    cvtText[4] = "Four";
    cvtText[5] = "Five";
    cvtText[6] = "Six";
    cvtText[7] = "Seven";
    cvtText[8] = "Eight";
    cvtText[9] = "Nine";
    cvtText[10] = "Ten";
    cvtText[11] = "Eleven";
    cvtText[12] = "Twelve";
    cvtText[13] = "Thirteen";
    cvtText[14] = "Fourteen";
    cvtText[15] = "Fifteen";
    cvtText[16] = "Sixteen";
    cvtText[17] = "Seventeen";
    cvtText[18] = "Eighteen";
    cvtText[19] = "Ninteen";
    cvtText[20] = "Twenty";
    cvtText[21] = "Thirty";
    cvtText[22] = "Forty";
    cvtText[23] = "Fifty";
    cvtText[24] = "Sixty";
    cvtText[25] = "Seventy";
    cvtText[26] = "Eigthty";
    cvtText[27] = "Ninty";
    cvtText[28] = "Hundred";
    cvtText[29] = "Thousand";
    cvtText[30] = "Million";
	}

char *convert100(U32 x)
	{
	U16 t;
	char *cvt100;
	
	cvt100=(char *)malloc(4096);
	if (cvt100 == NULL)
		return(NULL);
	
	if (x > 999)
		{
		strcpy(cvt100,"Error 100");
		return(cvt100);
		}
	
	if (x > 99)
		{
		t = x / 100;
		sprintf(cvt100,"%s %s ",cvtText[t],cvtText[28]);
		x = x - (t * 100);
		}
	
	if (x > 20)
		{
		t = x / 10;
		wvStrcat(cvt100,cvtText[t + 18]);
		wvStrcat(cvt100," ");
		x = x - (t * 10);
		}
	
	if (x > 0)
		{
		wvStrcat(cvt100,cvtText[x]);
		wvStrcat(cvt100," ");
		}
	return(cvt100);
	}

char *wvOrdinal(U32 x)
	{
	U32 t;
	char *Cvt;
	char *temp;
	
	Cvt=(char *)malloc(4096);
	if (Cvt == NULL)
		return(NULL);
	cvtInit();
	
	if (x > 999999999) 
		{
		strcpy(Cvt,"Number too large");
		return(Cvt);
		}
	
	if (x > 999999)
		{
		t = x / 1000000;
		temp = convert100(t);
		if (temp)
			{
			sprintf(Cvt,"%s%s ",temp,cvtText[30]);
			wvFree(temp);
			}
		else
			{
			wvFree(Cvt);
			return(NULL);
			}
		x = x - (t * 1000000);
		}
	
	if (x > 999)
		{
		t = x / 1000;
		temp = convert100(t);
		if (temp)
			{
			wvStrcat(Cvt,temp);
			wvFree(temp);
			}
		else
			{
			wvFree(Cvt);
			return(NULL);
			}
		wvStrcat(Cvt,cvtText[29]);
		wvStrcat(Cvt," ");
		x = x - (t * 1000);
		}
	
	if (x > 0)
		{
		temp = convert100(x);
		if (temp)
			{
			wvStrcat(Cvt,temp);
			wvFree(temp);
			}
		else
			{
			wvFree(Cvt);
			return(NULL);
			}
		}
	return(Cvt);
	}
