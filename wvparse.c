#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "utf.h"

int wvInitParser(wvParseStruct *ps,FILE *fp)
	{
	int ret=0,reason=0;
	ps->userData=NULL;
	ps->lst = NULL;
	ps->intable=0;
	ps->endcell=0;
	ps->vmerges=NULL;

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
		if ((ret != 4) && (ret != 7))
			wvError(("%s\n",wvReason(reason)));
		return(ret);
		}
	ret = 0;
	return ret;
	}

void wvSetPassword(char *password,wvParseStruct *ps)
	{
	int i=0,len;
	/* at this stage we are passing in an utf-8 password and
	later converting it to unicode, we should use the generic
	available mb to wide char stuff, but that isnt very prevalent
	yet, and this is the only time i think i go from utf to 
	unicode */

	while (*password)
		{
		len = our_mbtowc(&(ps->password[i]), password, 5);
		i++;
		password+=len;
		if (i == 16)
			break;
		}
	ps->password[i]=0;
	}
