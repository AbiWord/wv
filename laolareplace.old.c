
/*
Released under GPL, written by 
	Andrew Scriven <andy.scriven@research.natpower.co.uk>

Copyright (C) 1998
	Andrew Scriven
*/

/*
-----------------------------------------------------------------------
Andrew Scriven
Research and Engineering
Electron Building, Windmill Hill, Whitehill Way, Swindon, SN5 6PB, UK
Phone (44) 1793 896206, Fax (44) 1793 896251
-----------------------------------------------------------------------


The interface to OLEdecode now has
  int OLEdecode(char *filename, FILE **mainfd, FILE **tablefd0, FILE 
**tablefd1,FILE **data,FILE **summary)	
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <assert.h>

#include "wv.h"

#define THEMIN(a,b) ((a)<(b) ? (a) : (b))

#define MAXBLOCKS 256

extern FILE *erroroutput;

struct pps_block
  {
  char name[64];
  char type;
  struct pps_block *previous;
  struct pps_block *next;
  struct pps_block *directory;
  S32 start;
  S32 size;
  int level;
  };

typedef struct pps_block pps_entry;

/* Routine prototypes */
unsigned short int ShortInt(unsigned char* array);
U32 LongInt(unsigned char* array);

unsigned short int ShortInt(unsigned char* array)
{
return sread_16ubit(array);
}

U32 LongInt(unsigned char* array)
{
return sread_32ubit(array);
}

pps_entry **pps_list=NULL;
unsigned char *SDepot=NULL;

/* recurse to follow forward/backward list of root pps's */
void unravel(pps_entry *pps_node)
{
  if (pps_node != NULL)
  	{
	if (pps_node->previous != NULL) 
		unravel(pps_node->previous);
	pps_node->level = 0;
	if(pps_node->next != NULL) 
		unravel(pps_node->next);
	}
}

int wvOLEDecode(FILE *input, FILE **mainfd, FILE **tablefd0, FILE
**tablefd1,FILE **data, FILE **summary)
{
  FILE *OLEfile=NULL;
  FILE *sbfile=NULL;
  FILE *infile=NULL;
  int BlockSize=0,Offset=0;
  int c,i,j,len,bytes;
  char *p;
  unsigned char *s,*t;
  unsigned char *Block,*BDepot=NULL,*Depot=NULL,*Root=NULL;
  U32 depot_len;
  
  char Main[]="WordDocument";
  char Table0[]="0Table";
  char Table1[]="1Table";
  char Data[]="Data";
  char Summary[]="\005SummaryInformation";
  S32 FilePos=0x0;
  S32 num_bbd_blocks;
  S32 root_list[MAXBLOCKS], sbd_list[MAXBLOCKS];
  S32 pps_size,pps_start=-1;
  S32 linkto;
  int root_entry=-1;
  S32 fullen;
  S32 temppos;
  
  *mainfd = NULL;
  *tablefd0 = NULL;
  *tablefd1 = NULL;
  *data = NULL;
  *summary = NULL;

  if(input == NULL) return 1;
  /* peek into file to guess file type */
  c=getc(input);
  if (c==EOF)
  	{
    wvError(("File is empty.\n"));
	return(2);
	}
  	
  ungetc(c,input);

#if 0
  if(isprint(c)) 
  		{
		wvError(("File looks like a plain text file.\n"));
		return 2;
	/* check for MS OLE wrapper */
		} 
	else 
#endif	
	if(c==0xd0) {
     Block = (unsigned char *)malloc(512);
	 if (Block == NULL)
	 	{
       	wvError(("1 ===========> probable corrupt ole file, unable to allocate %d bytes\n",512));
		return(3);
		}
     /* read header block */
     if(fread(Block,512,1,input)!=1) {
       wvError(("1 ===========> Input file has faulty OLE format\n"));
    return 3;
     }
     num_bbd_blocks=(S32)LongInt(Block+0x2c);
	 if ((num_bbd_blocks == 0) || (num_bbd_blocks < 0))
		{
       	wvError(("2 ===========> Input file has ridiculous bbd, mem for the depot was %d\n",512*num_bbd_blocks));
		return(3);
		}
     BDepot = (unsigned char *)malloc(512*num_bbd_blocks);
	 if (BDepot == NULL)
	 	{
       	wvError(("2 ===========> couldnt alloc ole mem for the depot of %d\n",512*num_bbd_blocks));
		return(3);
		}
     s = BDepot;
     root_list[0]=LongInt(Block+0x30);
     sbd_list[0]=(S16)LongInt(Block+0x3c);
    wvTrace(("num_bbd_blocks %d, root start %d, sbd start %d\n",num_bbd_blocks,root_list[0],sbd_list[0]));
	temppos = ftell(input);
	fseek(input,0,SEEK_END);
	fullen = ftell(input);
	fseek(input,temppos,SEEK_SET);

     /* read big block Depot */
     for(i=0;i<(int)num_bbd_blocks;i++) 
	 	{
		if (0x4c+(i*4) > 512)
			{
			wvError(("2.1 ===========> Input file has faulty bbd\n"));
			return 3;
			}
       	FilePos = 512*(LongInt(Block+0x4c+(i*4))+1);
		if (FilePos > fullen)
			{
			wvError(("2.2 ===========> Input file has faulty bbd\n"));
			return 3;
			}
       	if (-1 == fseek(input,FilePos,SEEK_SET))
			{
			wvError(("2.3 ===========> Input file has faulty bbd\n"));
			return(3);
			}
       	if(fread(s,512,1,input)!=1) 
			{
			wvError(("2.4 ===========> Input file has faulty bbd\n"));
			return 3;
			}
       	s += 0x200;
     	}

     /* Extract the sbd block list */
     for(len=1;len<MAXBLOCKS;len++)
	 	{
		if (((sbd_list[len-1]*4) < (512*num_bbd_blocks))  && ((sbd_list[len-1]*4) > 0))
			sbd_list[len] = LongInt(BDepot+(sbd_list[len-1]*4));
		else
			{
         	wvError(("3 ===========> Input file has faulty OLE format\n"));
			return(3);
			}
		if(sbd_list[len]==-2) break;
     	}
     SDepot = (unsigned char *)malloc(512*len);
	 if (SDepot== NULL)
	 	{
       	wvError(("1 ===========> probable corrupt ole file, unable to allocate %d bytes\n",512*len));
		return(3);
		}
     s = SDepot;
     /* Read in Small Block Depot */
     for(i=0;i<len;i++) {
       FilePos = 512 *(sbd_list[i]+1);
       fseek(input,FilePos,SEEK_SET);
       if(fread(s,512,1,input)!=1) {
         wvError(("3 ===========> Input file has faulty OLE format\n"));
         return 3;
       }
       s += 0x200;
     }
     /* Extract the root block list */
     for(len=1;len<MAXBLOCKS;len++)
		{
		if ( ((root_list[len-1]*4) >= (512*num_bbd_blocks)) || ( (root_list[len-1]*4) < 0) )
			{
         	wvError(("3.1 ===========> Input file has faulty OLE format\n"));
			return(3);
			}
		root_list[len] = LongInt(BDepot+(root_list[len-1]*4));
		if(root_list[len]==-2) break;
		}
     Root = (unsigned char *)malloc(512*len);
	 if (Root == NULL)
	 	{
       	wvError(("1 ===========> probable corrupt ole file, unable to allocate %d bytes\n",512*len));
		return(3);
		}
     s = Root;
     /* Read in Root stream data */
     for(i=0;i<len;i++) {
       FilePos = 512 *(root_list[i]+1);
       fseek(input,FilePos,SEEK_SET);
       if(fread(s,512,1,input)!=1) {
         wvError(("4 ===========> Input file has faulty OLE format\n"));
         return 3;
       }
       s += 0x200;
     }

     /* assign space for pps list */
     pps_list = (pps_entry **)malloc(len*4*sizeof(pps_entry *));
	 if (pps_list == NULL)
	 	{
       	wvError(("1 ===========> probable corrupt ole file, unable to allocate %d bytes\n",len*4*sizeof(pps_entry *)));
		return(3);
		}
     for(j=0;j<len*4;j++) 
	 	{
	 	pps_list[j] = (pps_entry *)malloc(sizeof(pps_entry));
		if (pps_list[j] == NULL)
			{
			wvError(("1 ===========> probable corrupt ole file, unable to allocate %d bytes\n",sizeof(pps_entry)));
			return(3);
			}
		}
     /* Store pss entry details and look out for Root Entry */
     for(j=0;j<len*4;j++) {
       pps_list[j]->level = -1;
       s = Root+(j*0x80);
       i=ShortInt(s+0x40);
	   if (((j*0x80) + i) >= (512 * len))
	   	{
		wvError(("1.1 ===========> probable corrupt ole file\n"));
		return(3);
		}
       for(p=pps_list[j]->name,t=s;t<s+i;t++) 
	   	*p++ = *t++;
       s+=0x42;
       pps_list[j]->type = *s;
       if(pps_list[j]->type == 5) {
         root_entry = j; /* this is root */
         pps_list[j]->level = 0;
       }
       s+=0x02;
       linkto = LongInt(s);
       if ((linkto != -1) && (linkto < (len*4)) && (linkto > -1))
	   	pps_list[j]->previous = pps_list[linkto];
       else pps_list[j]->previous = NULL;
       s+=0x04;
       linkto = LongInt(s);
       if ((linkto != -1) && (linkto < (len*4)) && (linkto > -1))
	   	pps_list[j]->next = pps_list[linkto];
       else pps_list[j]->next = NULL;
       s+=0x04;
       linkto = LongInt(s);
       if ((linkto != -1) && (linkto < (len*4)) && (linkto > -1))
	   	pps_list[j]->directory = pps_list[linkto];
       else pps_list[j]->directory = NULL;
       s+=0x28;
       pps_list[j]->start = LongInt(s);
       s+=0x04;
       pps_list[j]->size = LongInt(s);
     }

     /* go through the pps entries, tagging them with level number
        use recursive routine to follow list starting at root entry */
     unravel(pps_list[root_entry]->directory);

     /* go through the level 0 list looking for named entries */
     for(j=0;j<len*4;j++) {
       if(pps_list[j]->level != 0) continue; /* skip nested stuff */
       pps_start = pps_list[j]->start;
       pps_size  = pps_list[j]->size;
       OLEfile = NULL;
       if(pps_list[j]->type==5) {  /* Root entry */
         OLEfile = tmpfile();
         sbfile = OLEfile;
         wvTrace(("Reading sbFile %d\n",pps_start));
       }
       else if(!strcmp(pps_list[j]->name,Main)) {
         OLEfile = tmpfile();
         *mainfd = OLEfile;
         wvTrace(("Reading Main %d\n",pps_start));
       }
       else if(!strcmp(pps_list[j]->name,Table0)) {
         OLEfile = tmpfile();
         *tablefd0 = OLEfile;
         wvTrace(("Reading Table0 %d\n",pps_start));
       }
       else if(!strcmp(pps_list[j]->name,Table1)) {
         OLEfile = tmpfile();
         *tablefd1 = OLEfile;
         wvTrace(("Reading Table1 %d\n",pps_start));
       }
       else if(!strcmp(pps_list[j]->name,Data)) {
         OLEfile = tmpfile();
         *data = OLEfile;
         wvTrace(("Reading Data %d\n",pps_start));
       }
       else if(!strcmp(pps_list[j]->name,Summary)) {
         OLEfile = tmpfile();
         *summary= OLEfile;
         wvTrace(("Reading Summary%d\n",pps_start));
       }
	   	
       if(pps_size<=0) OLEfile = NULL;
       if(OLEfile == NULL) continue;
	   /* 
       if (pps_size>=4096 | OLEfile==sbfile) 
	   */
       if ((pps_size>=4096) || (OLEfile==sbfile))
	   {
         Offset = 1;
         BlockSize = 512;
         infile = input;
         Depot = BDepot;
  		 depot_len=512*num_bbd_blocks;
       } else {
         Offset = 0;
         BlockSize = 64;
         infile = sbfile;
         Depot = SDepot;
  		 depot_len= 512*len;
       }
       while(pps_start != -2) {
         wvTrace(("Reading block %d, Offset %x\n",pps_start,Offset));
         FilePos = (pps_start+Offset)* BlockSize;
         bytes = THEMIN(BlockSize,pps_size);
         if (fseek(infile,FilePos,SEEK_SET) != 0) {
		 wvError(("6 ===========> Input file has faulty OLE format\n"));
		  return(3);
		 }
         if(fread(Block,bytes,1,infile)!=1) {
           wvError(("5 ===========> Input file has faulty OLE format\n"));
			wvFree(Root);
			wvFree(BDepot);
			wvFree(Block);
           return(3);
         }
         fwrite(Block,bytes,1,OLEfile);
		 if (pps_start*4 > depot_len)
		 	{
			wvWarning("5 ===========> Input file has dodgy OLE format\n");
			wvFree(Root);
			wvFree(BDepot);
			wvFree(Block);
			return(3);
			pps_size = 0;
			}
		else
			{
			pps_start = LongInt(Depot+(pps_start*4));
			pps_size -= BlockSize;
		 	}
         if(pps_size <= 0) pps_start=-2;
       }
       rewind(OLEfile);
     }

	for(j=0;j<len*4;j++) {
	 	free(pps_list[j]);
	 }
    free(pps_list);
    wvFree(Root);
    wvFree(BDepot);
    wvFree(Block);
    fclose(input);
    return 0;
  } else {
    /* not a OLE file! */
  if(isprint(c)) 
  		{
		wvError(("File looks like a plain text file.\n"));
		return 2;
		} 
    wvError(("7 ===========> Input file is not an OLE file\n"));
    return 2;
  }
}

void wvOLEFree(void)
    {
	if (SDepot != NULL)
		free(SDepot);
	}

