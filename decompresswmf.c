#include "config.h"

#if defined(SYSTEM_ZLIB) && defined(HAVE_MMAP)
#include <zlib.h>
#include <sys/mman.h>
#endif

#include <stdio.h>
#include <errno.h>
#include "wv.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
written by thisguy@somewhere.com who doesnt want his name in the source
*/

/*
theres some notes in the notes dir on compression
*/

int setdecom(void)
    {
#ifdef SYSTEM_ZLIB
	return(1);
#endif
	wvError(("libwv was not compiled against zlib, so wmf files cannot be decompress\n"));
	return(0);
	}

int decompress(FILE *inputfile,FILE *outputfile,U32 inlen,U32 outlen)
	{
#if defined(SYSTEM_ZLIB) && defined(HAVE_MMAP)
	char *compr;
	char *uncompr;
	int err;
	uLong uncomprLen, comprLen;
	
	
	char *input,*output;
	int out;
	int in;
	
	

	if (inputfile == NULL)
		{
		wvError(("danger, file to decompress is NULL\n"));
		return(-1);
		}

	in = fileno(inputfile);

	input = mmap(0,inlen,PROT_READ|PROT_WRITE,MAP_SHARED,in,0);

	if (input == (char *)-1)
		{
		wvError(("unable to mmap inputfile\n"));
		return(-1);
		}

	out = fileno(outputfile);
	lseek(out,outlen,SEEK_SET);
	if (out == -1)
		{
		wvError(("unable to create outputfile\n"));
		munmap(input,inlen);
		exit(-1);
		}
	if (-1 == write(out,"0",1))
		{
		wvError(("unable to write to outputfile\n"));
		munmap(input,inlen);
		close(out);
		exit(-1);
		}
	lseek(out,0,SEEK_SET);

	output = mmap(0,outlen,PROT_READ|PROT_WRITE,MAP_SHARED,out,0);

	if (output == (char *)-1)
		{
		wvError(("map out failed\n"));
		wvError(("%s\n",strerror(errno)));
		munmap(input,inlen);
		close(out);
		exit(-1);
		}

	/* set the size of the file*/
	comprLen = inlen;

	/* Read in the file contents */
	compr = input;
	uncompr = output;
	if (compr == NULL) 
		{
		wvError(("no mem to decompress wmf files\n"));
		return(-1);
		}
	if (uncompr == NULL) 
		{
		wvError(("no mem to decompress wmf files\n"));
		return(-1);
		}
	
	uncomprLen = outlen;	/* This was the trick :( */

	wvTrace(("len is %d %d\n",uncomprLen,comprLen));

	err = uncompress(uncompr, &uncomprLen, compr, comprLen);

	munmap(input,inlen);
	munmap(output,outlen);

	if (err != Z_OK) 
		{
		wvError(("decompress error: %d\n", err)); 
		return(-1); 
		} 
#else
	wvError(("System does not have mmap, if you are so inclined rewrite decompresswmf to not require this\n"));
#endif
	return 0;
	}
