#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvInitParser(wvParseStruct *ps,FILE *fp)
	{
	int ret=0,reason=0;
	ret = wvOLEDecode(fp,&ps->mainfd,&ps->tablefd0,&ps->tablefd1,
		&ps->data,&ps->summary);
	if (ret) 
		return ret;
	if (ps->mainfd == NULL) 
		{
		ret = 4;
		wvOLEFree();
		}

	wvGetFIB(&ps->fib,ps->mainfd);

	ret = wvQuerySupported(&ps->fib,&reason);
    if (ret)
		{
		wvError("%s",wvReason(reason));
		return(ret);
		}

	ps->tablefd = wvWhichTableStream(&ps->fib,ps->tablefd0,ps->tablefd1);

	return ret;
	}
