#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvInitParser(wvParseStruct *ps,FILE *fp)
	{
	int ret=0,reason=0;
	ps->userData=NULL;
	ps->lst = NULL;

	wvInitError();
	ps->password[0] = 0;

	ret = wvOLEDecode(fp,&ps->mainfd,&ps->tablefd0,&ps->tablefd1,
		&ps->data,&ps->summary);
	if (ret) 
		return ret;
	if (ps->mainfd == NULL) 
		{
		ret = 4;
		wvOLEFree();
		wvError(("Not a word document\n"));
		return(-1);
		}

	wvGetFIB(&ps->fib,ps->mainfd);

	ps->tablefd = wvWhichTableStream(&ps->fib,ps);

	ret = wvQuerySupported(&ps->fib,&reason);
    if ( (ret > 1) && (ret != 2) && (ret != 3) )
		{
		if (ret != 4) wvError(("%s\n",wvReason(reason)));
		return(ret);
		}
	ret = 0;
	return ret;
	}

void wvSetPassword(char *password,wvParseStruct *ps)
	{
	/* at this stage we are passing in an ascii password and
	later converting it to unicode, this is a flaw, and at a later
	stage we should be sure that the password reaches the
	password engine in unicode, the same as it would under windows*/
	strncpy(ps->password,password,16);
	}
