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

	switch(ret)
		{
		case 0:
			break;
		case 2:
			ret = wvOpenPreOLE(&fp,&ps->mainfd,&ps->tablefd0,&ps->tablefd1,
					&ps->data,&ps->summary);
			return(ret);
			break;
		case 3:
			wvError(("Bad Ole\n"));
			return(3);
			break;
		default:
			return(-1);
			break;
		}

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

	if ( (ret&0x7fff) != WORD8 )
		ps->data = ps->mainfd;

	if ( (ret != WORD8) && (ret != WORD7) && (ret!= WORD6) )
		{
		/* return the errors and the encrypted files*/
		if (!(ret & 0x8000))
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

int wvOpenPreOLE(FILE **input, FILE **mainfd, FILE **tablefd0, FILE **tablefd1,FILE **data, FILE **summary)
	{
	int ret=-1;

	*mainfd=*input;
	*tablefd0=*input;
	*tablefd1=*input;
	*data=*input;
	*summary=NULL;

	if (*input)
		rewind(*input);
	else
		return(ret);
	getc(*input);
	if (0xa5 == getc(*input))
		{
		wvError(("Theres a good change that this is a pre word 6 doc of nFib %d\n",read_16ubit(*input)));
		rewind(*input);
		return(-1);
		}
	return(ret);
	}
