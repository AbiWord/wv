/*
Released under GPL, written by Caolan.McNamara@ul.ie.

Copyright (C) 1998,1999 
	Caolan McNamara

Real Life: Caolan McNamara           *  Doing: MSc in HCI
Work: Caolan.McNamara@ul.ie          *  Phone: +353-86-8790257
URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
How would you have done it?
*/

/*warning: this software requires laola's lls to be installed*/

/*

this code is often all over the shop, being more of an organic entity
that a carefully planed piece of code, so no laughing there at the back!

and send me patches by all means, but think carefully before sending me
a patch that doesnt fix a bug or add a feature but instead just changes
the style of coding, i.e no more thousand line patches that fix my 
indentation.

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "config.h"
#include "wv.h"
#include "roman.h"
#include "utf.h"
#ifdef HAVE_WMF
#include "gdwmfapi.h"

#include <sys/mman.h>

extern listentry *ourlist;
extern int list;
#endif
#if 0
#include "rc4.h"
#include "md5.h"
#endif

#if 0
int wvDecrypt(FILE *mainfd,char *password,U32 lKey)
	{
	long end,pos;
	FILE *out;
	U32 temp;
	U8 myb;
	rc4_key key;
	int in;
	char *input;
	char name[4096];
	U8 mpassword[16];
	MD5_CTX mdContext;
    int i;

	
	/*
	memset(mpassword,0,16);

	strcpy(mpassword,"password1");
	*/

	MD5Init (&mdContext);
	MD5Update (&mdContext, "password1", strlen("password1"));
	MD5Final (&mdContext);

	prepare_key(mdContext.digest,16,&key);

	if (0 != fseek(mainfd,0L,SEEK_END))
		fprintf(stderr,"shag\n");
	end = ftell(mainfd);
	fprintf(stderr,"end is %ld\n",end);

	for (i=20;i<0x45;i++)
		{
		sprintf(name,"/tmp/outputtest%d",i);
		out = fopen(name,"w+b");
		
		fseek(mainfd,i,SEEK_SET);

		for(pos=i;pos<end;pos++)
			fputc(getc(mainfd),out);

		fseek(out,0L,SEEK_SET);

		in = fileno(out);
		input = mmap(0,end-i,PROT_READ|PROT_WRITE,MAP_SHARED,in,0);

		rc4(input,end-i,&key);

		munmap(input, end-i);
		fclose(out);
		}

	}
#endif

int wvOLESummaryStream(char *filename,FILE **summary)
	{
	int ret;
	FILE *input,*mainfd,*tablefd0,*tablefd1,*data;
	input = fopen(filename,"rb");
    ret = wvOLEDecode(input,&mainfd,&tablefd0,&tablefd1,&data,summary);
	return(ret);
	}

U32 read_32ubit(FILE *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = read_16ubit(in);
	temp2 = read_16ubit(in); 
	ret = temp2;
	ret = ret << 16;
	ret += temp1;
	return(ret);
	}

U32 sread_32ubit(U8 *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = sread_16ubit(in);
	temp2 = sread_16ubit(in+2);
	ret = temp2;
	ret = ret << 16;
	ret+=temp1;
	return(ret);
	}

U32 bread_32ubit(U8 *in,U16 *pos)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = sread_16ubit(in);
	temp2 = sread_16ubit(in+2);
	ret = temp2;
	ret = ret << 16;
	ret+=temp1;
	(*pos)+=4;
	return(ret);
	}

U16 read_16ubit(FILE *in)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = getc(in);
	temp2 = getc(in);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	return(ret);
	}


U16 sread_16ubit(U8 *in)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = *in;
	temp2 = *(in+1);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	return(ret);
	}

U16 bread_16ubit(U8 *in,U16 *pos)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = *in;
	temp2 = *(in+1);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	(*pos)+=2;
	return(ret);
	}

U32 dread_32ubit(FILE *in,U8 **list)
	{
	U8 *temp;
	U32 ret;
	if (in != NULL)
		return(read_32ubit(in));
	else
		{
		temp = *list;
		(*list)+=4;
		ret = sread_32ubit(temp);
		return(ret);
		}
	}

U16 dread_16ubit(FILE *in,U8 **list)
	{
	U8 *temp;
	U16 ret;
	if (in != NULL)
		return(read_16ubit(in));
	else
		{
		temp = *list;
		(*list)+=2;
		ret = sread_16ubit(temp);
		return(ret);
		}
	}

U8 dgetc(FILE *in,U8 **list)
	{
	U8 *temp;
	if (in != NULL)
		return(getc(in));
	else
		{
		temp = *list;
		(*list)++;
		return(sgetc(temp));
		}
	}

U8 sgetc(U8 *in)
	{
	return(*in);
	}

U8 bgetc(U8 *in,U16 *pos)
	{
	(*pos)++;
	return(*in);
	}

void windows_to_unicode(U8 in)
	{
	/*
	according to
	http://www.pemberley.com/janeinfo/latin1.html#unicode
	and backed up by
	ftp://ftp.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
	these are the characters which windows's cp1252 which word uses in 
	8 bit mode that have to changed to a different unicode number to
	work correctly
	*/

	/*
	so the plan would be to pass in 8 bit letters and if we ourselves
	are in unicode mode, then translate them into unicode and off we go,
	if we are *not* in unicode ourselves then make do with stupid 
	graphics
	*/
	U16 letter;

	switch (in)
		{
		case 130:
			letter = 8218;
			break;
		case 131:
			letter = 402;
			break;
		case 132:
			letter = 8222;
			break;
		case 133:
			letter = 8230;
			break;
		case 134:
			letter = 8224;
			break;
		case 135:
			letter = 8225;
			break;
		case 136:
			letter = 710;
			break;
		case 137:
			letter = 8240;
			break;
		case 138:
			letter = 352;
			break;
		case 139:
			letter = 8249;
			break;
		case 140:
			letter = 338;
			break;
		case 145:
			letter = 8216;
			break;
		case 146:
			letter = 8217;
			break;
		case 147:
			letter = 8220;
			break;
		case 148:
			letter = 8221;
			break;
		case 149:
			letter = 8226;
			break;
		case 150:
			letter = 8211;
			break;
		case 151:
			letter = 8212;
			break;
		case 152:
			letter = 732;
			break;
		case 153:
			letter = 8482;
			break;
		case 154:
			letter = 353;
			break;
		case 155:
			letter = 8250;
			break;
		case 156:
			letter = 339;
			break;
		case 159:
			letter = 376;
			break;
		}
	}

int add_t(int **vals,S16 *p,int plen)
    {
    static int maxno=5,cno=0;
    int i;

    if (cno == 0)
        *vals = (int *)malloc(maxno * sizeof(int *));

    if (cno+plen > maxno)
        {
        maxno+=plen;
        *vals = (int*)realloc(*vals,maxno * sizeof(int));
        }

    for(i=0;i<plen;i++)
        {
        (*vals)[cno] = p[i+1] - p[i];
        fprintf(stderr,"the val is %d\n",(*vals)[cno]);
        cno++;
        }
    return(cno);
    }


/* This function calculates the GCF using a loop with the modulus operator*/
int gcf(int high, int low)
{
  /* Initialize the variables */
  int temp, remainder = 0;
  fprintf(stderr,"high %d, low %d\n",high,low);

  /* Ensures that the higher number is in the variable high (A simple sort) */
  if(low < high)
    {
      temp = high;
      high = low;
      low = temp;
    }

  /* Loop that determines the GCF and continues until a remainder of 1 or 0 is calculated */
  do
    {
      /* The modulus operator determines the remainder if high was divided by low */
      remainder = high % low;
      if(remainder != 0)
        {
          /* A remainder of 1 indicates that the numbers are prime */
          if(remainder == 1)
            {
              printf("\nThese two numbers are prime numbers so:");
              return(1);
            }
          /* To continue, the low number becomes the high, and the remainder becmes the low */
          else
            {
              high = low;
              low = remainder;
            }

        }
    }
  while(remainder != 0);

  /* The low number is now the GCF if the numbers are not both prime */
  return(low);

}

int gcf_list(int *vals,int cno)
    {
    int i;
    i=1;
    if (cno > 1)
        {
        do
            {
            vals[i] = gcf(vals[i-1],vals[i]);
            i++;
            }
        while(i != cno);
        }
    return(vals[i-1]);
    }


