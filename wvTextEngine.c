#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvText(state_data *myhandle,wvParseStruct *ps)
	{
	FILE *tablefd;
	int ret,reason;

	if (ps->fib.fComplex)
		wvDecodeComplex(&ps->fib,ps->mainfd,ps->tablefd,ps->data);
	else
		wvDecodeSimple(myhandle,ps);
	return(0);
	}
