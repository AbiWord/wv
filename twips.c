#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/* 
basically the definition of a twip is that there
are 1440 twips per inch, now for html we need this
figure in pixels, so we have to take some screen
resolution as a standard to work from.

if we were to take hozitontal twips and a 1280 pixel
wide screen then there are
1440 twips per 74 pixels
*/

#define TWIPS_PER_INCH 1440
#define PIXELS_PER_INCH 74

static S16 pperi = PIXELS_PER_INCH;

void wvSetPixelsPerInch(S16 pixels)
	{
	pperi = pixels;
	}

	
float wvTwipsToPixels(S16 twips)
	{
	float ret = ((float)(pperi*twips))/TWIPS_PER_INCH;
	return(ret);
	}

float wvTwipsToMM(S16 twips)
	{
	float ret;
	ret = ((float)twips)/TWIPS_PER_INCH;
	ret = ret*25.0;
	return(ret);
	}

/* [A twip ] is one-twentieth of a point size*/
float wvPointsToMM(S16 points)
	{
	return(wvTwipsToMM(points*20));
	}
