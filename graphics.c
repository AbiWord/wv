#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
				
void wvDumpPicture(U32 pos,wvStream *fd)
	{
	static int i;
    U32 j;
	U32 len;
	FILE *out;
	char buffer[64];

	wvStream_goto(fd,pos);
	len = read_32ubit(fd);
	sprintf(buffer,"graphicdump%d",i++);
	out = fopen(buffer,"wb");
	wvStream_goto(fd,pos);
    for (j=0;j<len;j++)
        fputc(read_8ubit(fd),out);
    fclose(out);
	}
