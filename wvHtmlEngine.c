#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvHtml(state_data *sdata,wvParseStruct *ps)
	{
	FILE *tablefd;
	int ret,reason;

	if (ps->fib.fComplex)
		wvDecodeComplex(&ps->fib,ps->mainfd,ps->tablefd,ps->data);
	else
		wvDecodeSimple(sdata,ps);
	return(0);
	}
