#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvHtml(wvParseStruct *ps)
	{
	if (ps->fib.fComplex)
		wvDecodeComplex(ps);
	else
		wvDecodeSimple(ps,Dmain);
	return(0);
	}
