#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvInitParser(wvParseStruct *ps,FILE *fp)
	{
	int ret=0,reason=0;
	ps->userData=NULL;

	ret = wvOLEDecode(fp,&ps->mainfd,&ps->tablefd0,&ps->tablefd1,
		&ps->data,&ps->summary);
	if (ret) 
		return ret;
	if (ps->mainfd == NULL) 
		{
		ret = 4;
		wvOLEFree();
		wvError("Not a word document\n");
		return(-1);
		}

	wvGetFIB(&ps->fib,ps->mainfd);

	ret = wvQuerySupported(&ps->fib,&reason);
    if ( (ret > 1) && (ret != 2) && (ret != 3) )
		{
		wvError("%s\n",wvReason(reason));
		return(ret);
		}
	ret = 0;

	ps->tablefd = wvWhichTableStream(&ps->fib,ps);

	return ret;
	}
