#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
This is a simple example that take a word file and returns if
0 if the file is simple &
1 if the file is complex &
2 if the file wasn't parsable by the ole code.

Caolan.McNamara@ul.ie
*/

int main(int argc,char **argv)
	{
	int ret;
	FIB fib;
	wvStream *mainfd,*tablefd0,*tablefd1,*data,*summary;

	if (argc < 2)
		{
		fprintf(stderr,"Usage: wvSimpleCLX worddocument\n");
		return(1);
		}

	ret = wvOLEDecode(argv[1],&mainfd,&tablefd0,&tablefd1,&data,&summary);
	if (ret)
		{
		wvError(("sorry problem with getting ole streams from %s\n",argv[1]));
		return(2);
		}

	if (mainfd == NULL)
		{
		wvError(("No WordDocument stream found, this is not a word document\n"));
		wvError(("use wvSummary to try and determine the type of file\n"));
		return(2);
		}

	wvGetFIB(&fib,mainfd);

	if (fib.fComplex)
		printf("%s is a complex word doc\n",argv[1]);
	else
		printf("%s is a simple word doc\n",argv[1]);
		
	return(fib.fComplex);
	}
