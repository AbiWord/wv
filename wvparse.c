#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

int wvInitParser(wvParseStruct *ps,FILE *fp)
	{
	int ret=0;
	ret = wvOLEDecode(fp,&ps->mainfd,&ps->tablefd0,&ps->tablefd1,
		&ps->data,&ps->summary);
	if (ret) 
		return ret;
	if (ps->mainfd == NULL) 
		{
		ret = 4;
		wvOLEFree();
		}
	return ret;
	}
