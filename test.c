#include <stdlib.h>
#include <stdio.h>

#include "wv.h"

int main(void)
	{
	FILE *mainfd,*tablefd0,*tablefd1,*data,*summary;
	char *filename="temp.doc";
	int ret;

	fib afib;

	/*get ole streams from file*/

	ret = wvOLEdecode(filename,&mainfd,&tablefd0,&tablefd1,&data,&summary);

	if (ret)
		{
		fprintf(stderr,"Was not an ole file\n");
		return(0);
		}

	/*get fib*/
	ret = wvGetFib(&afib,mainfd);
	if (ret)
		{
		fprintf(stderr,"Was not a word document\n");
		return(0);
		}
	}
