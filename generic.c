#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wv.h"

int wvGetEmpty_PLCF(U32 **cps,U32 *nocps,U32 offset,U32 len,FILE *fd)
	{
	U32 i;
	if (len == 0)
		{
		*cps = NULL;
		*nocps = 0;
		}
	else
        {
        *nocps=len/4;
        *cps = (U32 *) malloc(*nocps * sizeof(U32));
        if (*cps == NULL)
            {
            wvError(("NO MEM 3, failed to alloc %d bytes\n",*nocps * sizeof(U32)));
            return(1);
            }
        fseek(fd,offset,SEEK_SET);
        for(i=0;i<*nocps;i++)
            (*cps)[i] = read_32ubit(fd);
        }
	return(0);
	}

void wvFree(void *ptr)
	{
	if (ptr != NULL)
		free(ptr);
	ptr = NULL;
	}

/*
If the
second most significant bit is clear, then this indicates the actual file
offset of the unicode character (two bytes). If the second most significant
bit is set, then the actual address of the codepage-1252 compressed version
of the unicode character (one byte), is actually at the offset indicated by
clearing this bit and dividing by two.
*/
/*
flag = 1 means that this is a one byte char, 0 means that this is a type
byte char
*/
U32 wvNormFC(U32 fc,int *flag)
	{
	if (fc & 0x40000000UL)
        {
        fc = fc & 0xbfffffffUL;
        fc = fc/2;
		if (flag) *flag = 1;
        }
	else
		if (flag) *flag = 0;
	return(fc);
	}

U16 wvGetChar(FILE *fd,int chartype)
    {
    if (chartype == 1)
        return(getc(fd));
    else
        return(read_16ubit(fd));
    return(0);
    }

int wvIncFC(int chartype)
	{
    if (chartype == 1)
		return(1);
	return(2);
	}


int wvStrlen(const char *str)
	{
	if (str == NULL)
		return(0);
	return(strlen(str));
	}

char *wvStrcat(char *dest, const char *src)
	{
	if (src != NULL)
		return(strcat(dest,src));
	else
		return(dest);
	}

void wvAppendStr(char **orig,const char *add)
	{
	int pos;
	wvTrace(("got this far\n"));
	pos = wvStrlen(*orig);
	wvTrace(("len is %d %d\n",pos,wvStrlen(add)));
	(*orig) = (char *)realloc(*orig,pos+wvStrlen(add)+1);
	(*orig)[pos] = '\0';
	wvTrace(("3 test str of %s\n",*orig));
	wvStrcat(*orig,add);
	wvTrace(("3 test str of %s\n",*orig));
	}

