#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvText(state_data *myhandle,FILE *mainfd,FILE *tablefd0,FILE *tablefd1,FILE *data)
	{
	FILE *tablefd;
	FIB fib;
	int ret,reason;

	wvGetFIB(&fib,mainfd);

	ret = wvQuerySupported(&fib,&reason);
    if (ret)
		{
		wvError("%s",wvReason(reason));
		return(ret);
		}

	tablefd = wvWhichTableStream(&fib,tablefd0,tablefd1);
	if (fib.fComplex)
		wvDecodeComplex(&fib,mainfd,tablefd,data);
	else
		wvDecodeSimple(&fib,myhandle,mainfd,tablefd,data);
	return(0);
	}
