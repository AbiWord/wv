#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wv.h"

/*
STTBF (STring TaBle stored in File)

Word has many tables of strings that are stored as Pascal type strings.
STTBFs consist of an optional short containing 0xFFFF, indicating that the
strings are extended character strings, a short indicating how many strings
are included in the string table, another short indicating the size in bytes
of the extra data stored with each string and each string followed by the
extra data. Non-extended charater Pascal strings begin with a single byte
length count which describes how many characters follow the length byte in
the string. If pst is a pointer to an array of characters storing a Pascal
style string then the length of the string is *pst+1. In an STTBF Pascal
style strings are concatenated one after another until the length of the
STTBF recorded in the FIB is exhausted. Extra data associated with a string
may also be stored in an sttbf. When extra data is stored for an STTBF, it
is written at the end of each string. For example: The extra data for an
STTBF consists of a short. If the string "Cat" were stored, the actual entry
in the string table would consist of a length byte containing 3 (3 for
"Cat") followed by the bytes 'C' 'a' 't', followed by the 2 bytes containing
the short. Extended character strings are stored just the same, except they
have a double byte length count and each extended character occupies two
bytes.
*/


void wvGetSTTBF(STTBF *anS,U32 offset,U32 len,FILE *fd)
	{
	int i,j;
	U16 slen;

	anS->s8strings=NULL;
	anS->u16strings=NULL;
	anS->extradata=NULL;

	wvTrace(("sttbf offset is %x,len %d\n",offset,len));
	if (len == 0)
		{
		anS->nostrings=0;
		return;
		}
	fseek(fd,offset,SEEK_SET);
	anS->extendedflag = read_16ubit(fd);
	if (anS->extendedflag != 0xFFFF)
		{
		/*old U8 strings*/
		anS->nostrings = anS->extendedflag;
		}
	else
		{
		/*U16 chars*/
		anS->nostrings = read_16ubit(fd);
		}
	anS->extradatalen = read_16ubit(fd);

	if (anS->extendedflag == 0xFFFF)
		anS->u16strings = (U16 **)malloc(sizeof(U16 *)*anS->nostrings);
	else
		anS->s8strings = (S8 **)malloc(sizeof(S8 *)*anS->nostrings);

	if (anS->extradatalen)
		{
		anS->extradata = (U8 **)malloc(sizeof(U8 *)*anS->nostrings);
		for (i=0;i<anS->nostrings;i++)
			anS->extradata[i] = (U8 *)malloc(anS->extradatalen);
		}

	if (anS->extendedflag == 0xFFFF)
		{
		for (i=0;i<anS->nostrings;i++)
			{
			slen = read_16ubit(fd);
			if (slen == 0)
				anS->u16strings[i] = NULL;
			else
				{
				anS->u16strings[i] = (U16 *)malloc(sizeof(U16)*(slen+1));
				for (j=0;j<slen;j++)
					anS->u16strings[i][j] = read_16ubit(fd);
				anS->u16strings[i][j]=0;
				}
			if (anS->extradatalen)
				for (j=0;j<anS->extradatalen;j++)
					anS->extradata[i][j] = getc(fd);
			}
		}
	else
		{
		for (i=0;i<anS->nostrings;i++)
			{
			slen = getc(fd);
			if (slen == 0)
				anS->s8strings[i] = NULL;
			else
				{
				anS->s8strings[i] = (S8 *)malloc(slen+1);
				for (j=0;j<slen;j++)
					anS->s8strings[i][j] = getc(fd);
				anS->s8strings[i][j]=0;
				}
			if (anS->extradatalen)
				for (j=0;j<anS->extradatalen;j++)
					anS->extradata[i][j] = getc(fd);
			}
		}
	}

void wvReleaseSTTBF(STTBF *item)
    {
    int i;

    if (item->s8strings!= NULL)
        {
        for(i=0;i<item->nostrings;i++)
            free(item->s8strings[i]);
        free(item->s8strings);
        }
    if (item->u16strings!= NULL)
        {
        for(i=0;i<item->nostrings;i++)
            free(item->u16strings[i]);
        free(item->u16strings);
        }
    if (item->extradata!= NULL)
        {
        for(i=0;i<item->nostrings;i++)
            free(item->extradata[i]);
        free(item->extradata);
        }
    }


void wvListSTTBF(STTBF *item)
    {
    int i,j;
	U16 *letter;

    if (item->s8strings!= NULL)
        {
        for(i=0;i<item->nostrings;i++)
            fprintf(stderr,"string is %s\n",item->s8strings[i]);
        }
    else if (item->u16strings!= NULL)
        {
        for(i=0;i<item->nostrings;i++)
			{
            fprintf(stderr,"string is ");
			letter = item->u16strings[i];
			while((letter != NULL) && (*letter != 0))
            	fprintf(stderr,"%c",*letter++);
            fprintf(stderr,"\n");
			}
        }

    if (item->extradata!= NULL)
        {
        for(i=0;i<item->nostrings;i++)
            for(j=0;j<item->extradatalen;j++)
				fprintf(stderr," %x ",item->extradata[i][j]);
        fprintf(stderr,"\n");
        }
    }


char *wvGetTitle(STTBF *item)
	{
	char *title = NULL;

	if ((item) && (item->nostrings >= 3))
        {
        if (item->extendedflag == 0xFFFF)
			title = wvWideStrToMB(item->u16strings[ibstAssocTitle]);
        else
            {
            if (item->s8strings[ibstAssocTitle] != NULL)
				{
				title = (char *)malloc(strlen(item->s8strings[ibstAssocTitle])+1);
                strcpy(title,(char *)item->s8strings[ibstAssocTitle]);
				}
            }
        }
	if (title == NULL)
		{
		title = (char *)malloc(strlen("Untitled")+1);
		strcpy(title,"Untitled");
		}
	return(title);
	}

void wvGetSTTBF6(STTBF *anS,U32 offset,U32 len,FILE *fd)
	{
	int i,j;
	U16 slen;

	anS->s8strings=NULL;
	anS->u16strings=NULL;
	anS->extradata=NULL;

	wvTrace(("word 6 sttbf offset is %x,len %d\n",offset,len));
	if (len == 0)
		{
		anS->nostrings=0;
		return;
		}
	fseek(fd,offset,SEEK_SET);
	anS->nostrings = ibstAssocMaxWord6;
	anS->extendedflag = ibstAssocMaxWord6;	/*just for the sake of it*/
	anS->extradatalen = 0;
	anS->s8strings = (S8 **)malloc(sizeof(S8 *)*anS->nostrings);
	if (len != (U32)getc(fd))
		wvTrace(("word 6 sttbf len does not match up correctly, strange\n"));
	for (i=0;i<anS->nostrings;i++)
		{
		slen = getc(fd);
		if (slen == 0)
			anS->s8strings[i] = NULL;
		else
			{
			anS->s8strings[i] = (S8 *)malloc(slen+1);
			for (j=0;j<slen;j++)
				anS->s8strings[i][j] = getc(fd);
			anS->s8strings[i][j]=0;
			}
		}
	}


U16 *UssrStrBegin(STTBF *sttbf,int no)
	{
	if (no >= sttbf->nostrings)
		return(NULL);

	return(sttbf->u16strings[no]+11);
	}




