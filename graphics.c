#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
				
void wvDumpPicture(U32 pos,FILE *fd)
	{
	static int i;
    U32 j;
	U32 len;
	FILE *out;
	char buffer[64];

	fseek(fd,pos,SEEK_SET);
	len = read_32ubit(fd);
	sprintf(buffer,"graphicdump%d",i++);
	out = fopen(buffer,"wb");
	fseek(fd,pos,SEEK_SET);
    for (j=0;j<len;j++)
        fputc(getc(fd),out);
    fclose(out);
	}
