#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "wv.h"

extern FILE *outputfile;
extern FILE *erroroutput;
extern char *outputfilename;
extern char *imagesdir;

int external_knowledge_0x01 = 0;/*when we magically know that we will not
							      be using a delay stream, i.e. big gobs of
								  emperical evidence, left external so that
								  if im wrong it'll be looking me in the face
								*/

char failsafe[1];

extern int errno;

U16 idlist[NOOFIDS] = {0,0x216,0x3D4,0x542,0x6E0,0x46A,0x7A8,0x800};


long get_picture_header(U32 fcPic,FILE *data, U32 *len, U16 *datatype)
	{
	U8 i,j;
	error(erroroutput,"seeking to %x\n",fcPic);
	fseek(data,fcPic,SEEK_SET);
	*len = read_32ubit(data);
	error(erroroutput,"the no of bytes in this PIC is %d\n",*len);
	i = getc(data);
	error(erroroutput,"the no of bytes of the PIC is %x\n",i);
	*datatype = read_16ubit(data);
	error(erroroutput,"an mm is %x\n",*datatype);
	error(erroroutput,"an mm is %x\n",read_16ubit(data));
	error(erroroutput,"an mm is %x\n",read_16ubit(data));
	error(erroroutput,"an mm is %x\n",read_16ubit(data));
	for (j=0;j<(i-13);j++)
		getc(data);
	*len = *len - i;
	return(ftell(data));
	}


obj_by_spid * get_blips(U32 fcDggInfo,U32 lcbDggInfo,FILE *tablefd,FILE *mainfd,int *noofblips,int streamtype,obj_by_spid **realhead)
	{
	U16 element;
	U32 len;
	U32 fulllen=0;
	U32 full02len=0;
	U32 full00len=0;
	long pos02;
	long pos00;
	U32 templen1;
	U32 templen2;
	U32 noofpics;
	U32 offset;
	int i;
	FILE *graphic;
	char *buffer=NULL;
	obj_by_spid *head=NULL;
	obj_by_spid *p=NULL;
	char *imageprefix=NULL;

	int currentspid=0;
	
	U16 spids[1024];
	char **names;
	int *no;
	char *names01[1024];
	static char *names08[1024];
	static int no01=0;
	static int no08=0;
	int decompressf=0;
	int decodewmf=0;
	U32 inlen=0;
	U32 outlen=0;

	U16 id;
	U32 op;
	U16 msofbh;
	U16 extra=0;
	U32 flags;
	fsp_list *afsp_list;
	fbse_list *afbse_list;


	afsp_list = wvParseEscher(&afbse_list,fcDggInfo,lcbDggInfo,tablefd,mainfd);


	wvGetSPID(0x402,afsp_list,afbse_list);


	while(afsp_list != NULL)
		{
		error(erroroutput,"test spid is %x\n",afsp_list->afsp.spid);
		afsp_list = afsp_list->next;
		}

	*noofblips=0;

	if (imagesdir == NULL)
		{
		imageprefix = (char *)malloc(strlen(outputfilename)+1);
		if (imageprefix == NULL)
			{
			fprintf(erroroutput,"arrgh, no mem\n");
			exit(-1);
			}
		strcpy(imageprefix,outputfilename);
		}
	else
		{
		imageprefix = (char *)malloc(strlen(imagesdir)+2+strlen(ms_basename(outputfilename)));
		if (imageprefix == NULL)
			{
			fprintf(erroroutput,"arrgh, no mem\n");
			exit(-1);
			}
		sprintf(imageprefix,"%s/%s",imagesdir,ms_basename(outputfilename));
		}


	if (streamtype == 0x01)
		{
		mainfd=tablefd;
		names=names01;
		no = &no01;
		memset(names, 0, 1024);
		}
	else		
		{
		names=names08;
		no = &no08;
		}

	if ( (*no) == 0)	
		memset(names, 0, 1024);

	if ((realhead != NULL) && (*realhead != NULL))
		{
		error(erroroutput,"adding onto an existing list\n");
		p=*realhead;
		head=*realhead;
		error(erroroutput,"REAL TEST, filename is %s\n",(*realhead)->filename);
		error(erroroutput,"head isnt null here\n");

		error(erroroutput,"TEST, no is %d, filename is %s\n",*no,p->filename);

		while (p->next != NULL)
			p = p->next;
		}

	fseek(tablefd,fcDggInfo,SEEK_SET);

	while(fulllen < lcbDggInfo)
		{
		error(erroroutput,"at this moment fullen is %d, lcbDggInfo is %d, pos is %x, streamtype is %d\n",fulllen,lcbDggInfo,ftell(tablefd),streamtype);
		msofbh = read_16ubit(tablefd);
		element = read_16ubit(tablefd);
		len = read_32ubit(tablefd);

		fulllen+=8;
		error(erroroutput,"len is %x, element is %x\n",len,element);
		error(erroroutput,"dec, fullen is %d, lcbDggInfo is %d\n",fulllen,lcbDggInfo);
		switch (element)
			{
			case 0xf000:
				error(erroroutput,"f000 here\n");
				pos00 = ftell(tablefd);
				full00len=0;
				templen2 = len;
				while (full00len < templen2)
					{
					error(erroroutput,"at this moment full00en is %d, templen is %d\n",full00len,templen2);
					msofbh = read_16ubit(tablefd);
					element = read_16ubit(tablefd);
					len = read_32ubit(tablefd);
					full00len+=8;
					error(erroroutput,"len is %x, element is %x\n",len,element);
					error(erroroutput,"dec, fullen is %d, lcbDggInfo is %d\n",full00len,lcbDggInfo);
					switch (element)
						{
						case 0xf006:
							error(erroroutput,"1-->%d\n",read_32ubit(tablefd));
							error(erroroutput,"2-->%d\n",read_32ubit(tablefd));
							error(erroroutput,"3-->%d\n",read_32ubit(tablefd));
							noofpics = read_32ubit(tablefd);
							full00len+=16;
							error(erroroutput,"there are %d pics visible\n",noofpics);
							/*
							for (i=0;i<noofpics;i++)
								{
								read_32ubit(tablefd);
								read_32ubit(tablefd);
								full00len+=8;
								}
							*/
							for (i=16;i<len;i++)
								{
								getc(tablefd);
								full00len++;
								}
							break;
						case 0xf00b:
							id = read_16ubit(tablefd);
							op = read_32ubit(tablefd);
							error(erroroutput,"id is %x\n",id);
							error(erroroutput,"op is %x\n",op);
							full00len+=6;

							if ((id & 0x4000) && (!(id & 0x8000)))
								{
								error(erroroutput,"talking about blit id of %x,mine %x\n",op,id&0x3fff);
								p->filename=names[op-1];
								if (p->filename==NULL)
									{
									/*
									this means that there was no blip in the blip store to match with this,
									from looking at it it looks like this means that there will be data in
									the client data to fill this blank
									so im going to temporarily allow a temp in the filename when we come
									to blips to be assigned
									*/
									}
								}

							if (p==NULL)
								error(erroroutput,"SO p is NULL\n");
							else if (p->filename == NULL)
								error(erroroutput,"SO p->filenamae is NULL\n");
							else
								error(erroroutput,"SO spid %x has a filename of %s\n",p->spid,p->filename);

							for(i=0;i<len-6;i++)
								{
								getc(tablefd);
								full00len++;
								}
							break;
						case 0xf001: /*container for pics*/
							break;
						case 0xf007: /*a pic, yeah!!*/
							error(erroroutput,"window pic type is %d\n",getc(tablefd));	
							error(erroroutput,"mac pic type is %d\n",getc(tablefd));	
							full00len+=2;
							for(i=0;i<18;i++)
								getc(tablefd);
							full00len+=18;
							error(erroroutput,"pic data len is (%x)\n",read_32ubit(tablefd));
							error(erroroutput,"blid no is %d\n",read_32ubit(tablefd));
							offset = read_32ubit(tablefd);
							full00len+=12;
							error(erroroutput,"file offset into main stream is %x\n",offset);

							read_32ubit(tablefd);
							full00len+=4;

							buffer = (char *)malloc(strlen("-graphic100-mswv.tiff")+80+strlen(imageprefix));
							if (buffer==NULL)
								{
								fprintf(erroroutput,"no mem\n");
								exit(-1);
								}

							error(erroroutput,"were at %x\n",ftell(tablefd));
							if ((offset != 0xffffffffUL) || (streamtype == 0x01))
								{
								if (streamtype == 0x08)
									fseek(mainfd,offset,SEEK_SET);
								error(erroroutput,"were at %x\n",ftell(mainfd));
								msofbh = read_16ubit(mainfd);
								error(erroroutput,"msofbh is %x, inst is %x\n",msofbh ,msofbh>> 4);
								msofbh = msofbh >> 4;
								extra=0;
								for (i=0;i<NOOFIDS;i++)
									{
									if ((msofbh ^ idlist[i]) == 1)
										{
										error(erroroutput,"extra 16\n");
										extra=16;
										}
									}
								element = read_16ubit(mainfd);
								error(erroroutput,"element is %x\n",element);
								len = read_32ubit(mainfd);
								if (streamtype == 0x01)
									full00len+=8;
								error(erroroutput,"graphic len is %x, pos is %x\n",len,ftell(mainfd));

								if ((element >= 0xf018) && (element <= 0xf117))
									{
									switch (element - 0xf018)
										{
										case 6:
											sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"png");
											for(i=0;i<(17+extra);i++)
												getc(mainfd);
											len-=(17+extra);
											if (streamtype == 0x01)
												full00len+=(17+extra);
											break;
										case 5:
											sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"jpg");
											error(erroroutput,"extra is %d\n",extra);
											for(i=0;i<(17+extra);i++)
												getc(mainfd);
											len-=(17+extra);
											if (streamtype == 0x01)
												full00len+=(17+extra);
											error(erroroutput,"jpg the current pos of the beast is %x\n",ftell(mainfd));
											break;
										case 4:
											sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"pict");
											for(i=0;i<(17+extra);i++)
												getc(mainfd);
											len-=(17+extra);
											if (streamtype == 0x01)
												full00len+=(17+extra);
											break;
										case 3:
											decompressf = setdecom();
											if (decompressf)
												sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"wmf");
											else
												sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"wmf.lz");
											decodewmf=1;
											for(i=0;i<(16+extra);i++)
												getc(mainfd);
											outlen = read_32ubit(mainfd);
											error(erroroutput,"uncompressed size would be %d\n",outlen);
											for(i=0;i<24;i++)
												getc(mainfd);
											inlen = read_32ubit(mainfd);
											error(erroroutput,"compressed size is %d\n",inlen);
											error(erroroutput,"compression is %d\n",getc(mainfd));
											error(erroroutput,"filter is %d\n",getc(mainfd));
											len-=(50+extra);
											if (streamtype == 0x01)
												full00len+=(50+extra);
											break;
										case 2:
											decompressf = setdecom();
											if (decompressf)
												sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"emf");
											else
												sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"emf.lz");
											for(i=0;i<(16+extra);i++)
												getc(mainfd);
											outlen = read_32ubit(mainfd);
											error(erroroutput,"uncompressed size would be %d\n",outlen);
											for(i=0;i<24;i++)
												getc(mainfd);
											inlen = read_32ubit(mainfd);
											error(erroroutput,"compressed size is %d\n",inlen);
											error(erroroutput,"compression is %d\n",getc(mainfd));
											error(erroroutput,"filter is %d\n",getc(mainfd));
											len-=(50+extra);
											if (streamtype == 0x01)
												full00len+=(50+extra);
											break;
										case 7:
											sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"dib");
											for(i=0;i<(17+extra);i++)
												getc(mainfd);
											len-=(17+extra);
											if (streamtype == 0x01)
												full00len+=(17+extra);
											break;
										default:
											sprintf(buffer,"%s-graphic-dontknow%d-mswv-%d.jpg",imageprefix,(*no)+1,streamtype);
											len=0;
											break;
										}


									error(erroroutput,"this the current pos of the beast is %x\n",ftell(mainfd));

									if (decompressf)
										/*
										graphic = tmpfile();
										*/
										graphic = fopen("/tmp/test.wmf.lzzed","w+b");
									else
										graphic = fopen(buffer,"wb");

									if (graphic == NULL)
										{
										error(erroroutput,"warning couldnt create file %s, ignoring \n(failure was :%s)\n",buffer,strerror(errno));
										for (i=0;i<len;i++)
											{
											getc(mainfd);
											if (streamtype == 0x01)
												full00len++;
											}
										}
									else
										{
										error(erroroutput,"len changed to %d\n",len);
										for (i=0;i<len;i++)
											{
											putc(getc(mainfd),graphic);
											if (streamtype == 0x01)
												full00len++;
											}
										if (decompressf)
											{
											fflush(graphic);
											rewind(graphic);
											decompress(graphic,buffer,inlen,outlen);
											decompressf=0;
											}
											
										fclose(graphic);
										if (decodewmf)
											convertwmf(buffer);
										}
									}

								if (buffer!=NULL)
									names[(*no)++] = buffer;
								else
									(*no)++;

								error(erroroutput,"name is %s, index %d\n",names[(*no)-1],(*no)-1);

								error(erroroutput,"blit no %d is filename %s\n",*no,names[(*no)-1]);


								if (p != NULL)
									if (p->filename == NULL)
										p->filename = buffer;
								
								error(erroroutput,"THE FILENAME is %s\n",buffer);
								}
							else
								{
								error(erroroutput,"would have been %s-graphic-badoffset%d-mswv.jpg",imageprefix,(*no)+1);
								error(erroroutput,"THE FILENAME is %s\n",buffer);
								free(buffer);
								}
							error(erroroutput,"location is %x\n",ftell(tablefd));
							break;
						default:
							error(erroroutput,"unrecognized element %x len is %d\n",element,len);
							error(erroroutput,"were here at %x\n",ftell(tablefd));
							for(i=0;i<len;i++)
								{
								getc(tablefd);
								full00len++;
								}
							break;
						}
					}
				templen2++;
				fulllen+=templen2;
				pos00+=templen2;
				error(erroroutput,"the position to seek to is fulllen is %d,%x\n",fulllen,pos00);
				fseek(tablefd,pos00,SEEK_SET);
				break;
			case 0xf002: /*container for page or something*/
				error(erroroutput,"f002 here\n");
				pos02 = ftell(tablefd);
				full02len=0;
				templen1 = len;
				while (full02len < templen1)
					{
					error(erroroutput,"at this moment full02en is %d, templen is %d\n",full02len,templen1);
					msofbh = read_16ubit(tablefd);
					element = read_16ubit(tablefd);
					len = read_32ubit(tablefd);
					full02len+=8;
					error(erroroutput,"len is %x, element is %x\n",len,element);
					error(erroroutput,"dec, fullen is %d, lcbDggInfo is %d\n",full02len,lcbDggInfo);
					switch (element)
						{
						case 0xf008: /*msofbtDg*/
							read_32ubit(tablefd);
							read_32ubit(tablefd);
							full02len+=8;
							break;
						case 0xf003: /*container for many shapes*/
							break;
						case 0xf004: /*container for a shape*/
							break;
						case 0xf00a:
							spids[currentspid++] = read_32ubit(tablefd);
							error(erroroutput,"The ID of this ENTITY is %x\n",spids[currentspid-1]);
							/*
							this is the identity of the object, the real spid as far 
							as im concerned, following this will be the shape property
							table that will have a list of blits associated with
							this id, (referred to by an index into the previous blit 
							table)
							*/
							flags = read_32ubit(tablefd);
							error(erroroutput,"flags is %x\n",flags);

							if (head == NULL)
								{
								head = (obj_by_spid*) malloc(sizeof(obj_by_spid));
								error(erroroutput,"head is null\n");
								if (head == NULL)
									{
									error(erroroutput,"no mem\n");
									exit(-1);
									}
								p = head;
								}
							else
								{
								p->next = (obj_by_spid*) malloc(sizeof(obj_by_spid));
								error(erroroutput,"head is not null\n");
								if (p->next == NULL)
									{
									error(erroroutput,"no mem\n");
									exit(-1);
									}
								p = p->next;
								}
							
							p->spid = spids[currentspid-1];
							/*
							p->filename = failsafe;
							*/
							p->filename = NULL;
							p->next = NULL;
							(*noofblips)++;
							
							full02len+=8;
							for(i=0;i<len-8;i++)
								{
								getc(tablefd);
								full02len++;
								}
							break;
						case 0xf00b:
							id = read_16ubit(tablefd);
							op = read_32ubit(tablefd);
							error(erroroutput,"id is %x\n",id);
							error(erroroutput,"op is %x\n",op);

							if ((id & 0x4000) && (!(id & 0x8000)))
								{
								error(erroroutput,"talking about blit id of %x,mine %x\n",op,id&0x3fff);
								p->filename=names[op-1];
								if (p->filename==NULL)
									{
									/*
									this means that there was no blip in the blip store to match with this,
									from looking at it it looks like this means that there will be data in
									the client data to fill this blank
									so im going to temporarily allow a temp in the filename when we come
									to blips to be assigned
									*/
									}
								}

							if (p==NULL)
								error(erroroutput,"SO p is NULL\n");
							else if (p->filename == NULL)
								error(erroroutput,"SO p->filenamae is NULL\n");
							else
								error(erroroutput,"SO spid %x has a filename of %s\n",p->spid,p->filename);

							full02len+=6;
							for(i=0;i<len-6;i++)
								{
								getc(tablefd);
								full02len++;
								}
							break;
						default:
							error(erroroutput,"unrecognized element %x len is %d\n",element,len);
							error(erroroutput,"were here at %x\n",ftell(tablefd));
							for(i=0;i<len;i++)
								{
								getc(tablefd);
								full02len++;
								}
							break;
						}
					}
				templen1++;
				fulllen+=templen1;
				pos02+=templen1;
				error(erroroutput,"the position to seek to is fulllen %d,%x\n",fulllen,pos02);
				fseek(tablefd,pos02,SEEK_SET);
				break;
			case 0xf005: /*container for shape rules*/
				break;
			case 0xf004: /*container for a shape*/
				break;
			case 0xf007: /*a pic, yeah!!*/
				error(erroroutput,"window pic type is %d\n",getc(tablefd));	
				error(erroroutput,"mac pic type is %d\n",getc(tablefd));	
				fulllen+=2;
				for(i=0;i<18;i++)
					getc(tablefd);
				fulllen+=18;
				error(erroroutput,"pic data len is (%x)\n",read_32ubit(tablefd));
				error(erroroutput,"blid no is %d\n",read_32ubit(tablefd));
				offset = read_32ubit(tablefd);
				fulllen+=12;
				error(erroroutput,"file offset into main stream is %x\n",offset);

				read_32ubit(tablefd);
				fulllen+=4;

				buffer = (char *)malloc(strlen("-graphic100-mswv.tiff")+80+strlen(imageprefix));
				if (buffer==NULL)
					{
					fprintf(erroroutput,"no mem\n");
					exit(-1);
					}

				error(erroroutput,"were at %x\n",ftell(tablefd));
				if ((offset != 0xffffffffUL) || (streamtype == 0x01))
					{
					if (streamtype == 0x08)
						fseek(mainfd,offset,SEEK_SET);
					error(erroroutput,"were at %x\n",ftell(mainfd));
					msofbh = read_16ubit(mainfd);
					error(erroroutput,"element is %x, inst is %x\n",msofbh ,msofbh>> 4);
					msofbh = msofbh >> 4;
					extra=0;
					for (i=0;i<NOOFIDS;i++)
						{
						if ((msofbh ^ idlist[i]) == 1)
							{
							error(erroroutput,"extra 16\n");
							extra=16;
							}
						}
					element = read_16ubit(mainfd);
					len = read_32ubit(mainfd);
					if (streamtype == 0x01)
						fulllen+=8;
					error(erroroutput,"graphic len is %x, element is %x\n",len,element);

					if ((element >= 0xf018) && (element <= 0xf117))
						{
						switch (element - 0xf018)
							{
							case 6:
								sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"png");
								for(i=0;i<(17+extra);i++)
									getc(mainfd);
								len-=(17+extra);
								if (streamtype == 0x01)
									fulllen+=(17+extra);
								break;
							case 5:
								sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"jpg");
								for(i=0;i<(17+extra);i++)
									getc(mainfd);
								len-=(17+extra);
								if (streamtype == 0x01)
									fulllen+=(17+extra);
								break;
							case 4:
								sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"pict");
								for(i=0;i<(17+extra);i++)
									getc(mainfd);
								len-=(17+extra);
								if (streamtype == 0x01)
									fulllen+=(17+extra);
								break;
							case 3:
								decompressf = setdecom();
								if (decompressf)
									sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"wmf");
								else
									sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"wmf.lz");
								decodewmf=1;
								for(i=0;i<(16+extra);i++)
									getc(mainfd);
								outlen = read_32ubit(mainfd);
								error(erroroutput,"uncompressed size would be %d\n",outlen);
								for(i=0;i<24;i++)
									getc(mainfd);
								inlen = read_32ubit(mainfd);
								error(erroroutput,"compressed size is %d\n",inlen);
								error(erroroutput,"compression is %d\n",getc(mainfd));
								error(erroroutput,"filter is %d\n",getc(mainfd));
								len-=(50+extra);
								if (streamtype == 0x01)
									fulllen+=(50+extra);
								break;
							case 2:
								decompressf = setdecom();
								if (decompressf)
									sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"emf");
								else
									sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"emf.lz");
								for(i=0;i<(16+extra);i++)
									getc(mainfd);
								outlen = read_32ubit(mainfd);
								error(erroroutput,"uncompressed size would be %d\n",outlen);
								for(i=0;i<24;i++)
									getc(mainfd);
								inlen = read_32ubit(mainfd);
								error(erroroutput,"compressed size is %d\n",inlen);
								error(erroroutput,"compression is %d\n",getc(mainfd));
								error(erroroutput,"filter is %d\n",getc(mainfd));
								len-=(50+extra);
								if (streamtype == 0x01)
									fulllen+=(50+extra);
								break;
							case 7:
								sprintf(buffer,"%s-graphic%d-mswv-%d.%s",imageprefix,(*no)+1,streamtype,"dib");
								for(i=0;i<(17+extra);i++)
									getc(mainfd);
								len-=(17+extra);
								if (streamtype == 0x01)
									fulllen+=(17+extra);
								break;
							default:
								sprintf(buffer,"%s-graphic-dontknow%d-mswv-%d.jpg",imageprefix,(*no)+1,streamtype);
								len=0;
								break;
							}

						error(erroroutput,"the current pos of the beast is %x\n",ftell(mainfd));
						if (decompressf)
						/*
							graphic = tmpfile();
						*/
							graphic = fopen("/tmp/test.wmf2.lzzed","w+b");
						else
							graphic = fopen(buffer,"wb");
						if (graphic == NULL)
							{
							fprintf(erroroutput,"warning couldnt create file %s, ignoring \n(failure was :%s)\n",buffer,strerror(errno));
							for (i=0;i<len;i++)
								{
								getc(mainfd);
								if (streamtype == 0x01)
									fulllen++;
								}
							}
						else
							{
							error(erroroutput,"len changed to %d\n",len);
							for (i=0;i<len;i++)
								{
								putc(getc(mainfd),graphic);
								if (streamtype == 0x01)
									fulllen++;
								}
							if (decompressf)
								{
								fflush(graphic);
								rewind(graphic);
								decompress(graphic,buffer,inlen,outlen);
								decompressf=0;
								}
								
							fclose(graphic);
							if (decodewmf)
								convertwmf(buffer);
							}
						}

					if (buffer!=NULL)
						names[(*no)++] = buffer;
					else
						(*no)++;

					error(erroroutput,"name is %s, index %d\n",names[(*no)-1],(*no)-1);

					error(erroroutput,"blit no %d is filename %s\n",*no,names[(*no)-1]);


					if (p != NULL)
						if (p->filename == NULL)
							p->filename = buffer;
					
					error(erroroutput,"THE FILENAME is %s\n",buffer);
					}
				else
					{
					error(erroroutput,"would have been %s-graphic-badoffset%d-mswv.jpg",imageprefix,(*no)+1);
					error(erroroutput,"THE FILENAME is %s\n",buffer);
					free(buffer);
					}
				error(erroroutput,"location is %x\n",ftell(tablefd));
				break;
			case 0xf00a:
#if 0
				error(erroroutput,"The ID of this ENTITY is %x\n",read_32ubit(tablefd));
				flags = read_32ubit(tablefd);
				error(erroroutput,"flags is %x\n",flags);
				fulllen+=8;
				for(i=0;i<len-8;i++)
					{
					getc(tablefd);
					fulllen++;
					}
#endif							
				spids[currentspid++] = read_32ubit(tablefd);
				error(erroroutput,"The ID of this ENTITY is %x\n",spids[currentspid-1]);
				/*
				this is the identity of the object, the real spid as far 
				as im concerned, following this will be the shape property
				table that will have a list of blits associated with
				this id, (referred to by an index into the previous blit 
				table)
				*/
				flags = read_32ubit(tablefd);
				error(erroroutput,"flags is %x\n",flags);

				if (head == NULL)
					{
					head = (obj_by_spid*) malloc(sizeof(obj_by_spid));
					error(erroroutput,"head is null\n");
					if (head == NULL)
						{
						error(erroroutput,"no mem\n");
						exit(-1);
						}
					p = head;
					}
				else
					{
					p->next = (obj_by_spid*) malloc(sizeof(obj_by_spid));
					error(erroroutput,"head is not null\n");
					if (p->next == NULL)
						{
						error(erroroutput,"no mem\n");
						exit(-1);
						}
					p = p->next;
					}
				
				p->spid = spids[currentspid-1];
				/*
				p->filename = failsafe;
				*/
				p->filename = NULL;
				p->next = NULL;
				(*noofblips)++;
				
				fulllen+=8;
				for(i=0;i<len-8;i++)
					{
					getc(tablefd);
					fulllen++;
					}
				break;
			case 0xf00b:
				id = read_16ubit(tablefd);
				op = read_32ubit(tablefd);
				error(erroroutput,"id is %x\n",id);
				error(erroroutput,"op is %x\n",op);

				if ((id & 0x4000) && (!(id & 0x8000)))
					{
					error(erroroutput,"talking about blit id of %x,mine %x\n",op,id&0x3fff);
					p->filename=names[op-1];
					if (p->filename==NULL)
						{
						/*
						this means that there was no blip in the blip store to match with this,
						from looking at it it looks like this means that there will be data in
						the client data to fill this blank
						so im going to temporarily allow a temp in the filename when we come
						to blips to be assigned
						*/
						}
					}

				if (p==NULL)
					error(erroroutput,"SO p is NULL\n");
				else if (p->filename == NULL)
					error(erroroutput,"SO p->filenamae is NULL\n");
				else
					error(erroroutput,"SO spid %x has a filename of %s\n",p->spid,p->filename);

				fulllen+=6;
				for(i=0;i<len-6;i++)
					{
					getc(tablefd);
					fulllen++;
					}
				break;
#if 0				
			case 0xf11e:
				/*i dont quite understand this for now*/
				len=25;
				for(i=0;i<len;i++)
					{
					getc(tablefd);
					fulllen++;
					}
				break;
#endif
			default:
				error(erroroutput,"unrecognized element %x len is %d\n",element,len);
				error(erroroutput,"were here at %x\n",ftell(tablefd));
				for(i=0;i<len;i++)
					{
					getc(tablefd);
					fulllen++;
					}
				break;
			}
		}

	if (imageprefix != NULL)
		free(imageprefix);
	return(head);
	}

void output_draw(U32 cp,textportions *portions)
	{
	int i;
	U16 ourspid=0;
	obj_by_spid *p;
	
	for (i=0;i<portions->noofficedraw;i++)
		{
		if (cp == portions->officedrawcps[i])
			{
			ourspid = portions->fspas[i].spid;
			break;
			}
		}
	
	error(erroroutput,"outspid is %x in output_draw, only able to handle blips, not real draw objects as of yet\n",ourspid);

	error(erroroutput,"noofficedraw is %d portions->noofblipdata is %d\n",portions->noofficedraw,portions->noofblipdata);

	p = portions->ablipdata;

	if ((ourspid != 0) && (p != 0)) /*Craig J Copi <cjc5@po.cwru.edu>*/
		{
		/*search to see if theres a filename associated with this spid*/
		for(i=0;i<portions->noofblipdata;i++)
			{
			error(erroroutput,"p->spid is %x\n",p->spid);
			if (ourspid == p->spid)
				{
				outputimgsrc(p->filename,0,0);
				return;
				}
			p = p->next;
			}
		}
	error(erroroutput,"given spid %x, no luck\n",ourspid);
	}


void wvGetMSOFBH(MSOFBH *amsofbh,FILE *infd)
	{
	U16 dtemp=0;
	dtemp=read_16ubit(infd);

#ifdef PURIFY
	amsofbh->ver = 0;
	amsofbh->inst = 0;
#endif

	amsofbh->ver = dtemp & 0x000F;
	amsofbh->inst = dtemp >> 4;
	amsofbh->fbt = read_16ubit(infd);
	amsofbh->cbLength = read_32ubit(infd);
	}

void wvGetFDGG(FDGG *afdgg,FILE *infd)
	{
	afdgg->spidMax = read_32ubit(infd);
	afdgg->cidcl = read_32ubit(infd);
	afdgg->cspSaved = read_32ubit(infd);
	afdgg->cdgSaved = read_32ubit(infd);
	error(erroroutput,"spidMax %d cidcl %d cspSaved %d cdgSaved %d\n",afdgg->spidMax,afdgg->cidcl,afdgg->cspSaved, afdgg->cdgSaved );
	}

void wvGetFIDCL(FIDCL *afidcl,FILE *infd)
	{
	afidcl->dgid = read_32ubit(infd);
	afidcl->cspidCur = read_32ubit(infd);
	error(erroroutput,"dgid %d cspidCur %d\n",afidcl->dgid,afidcl->cspidCur);
	}

void wvGetFBSE(FBSE *afbse,FILE *infd)
	{
	static int id;
	int i;
	afbse->btWin32 = getc(infd);
	afbse->btMacOS = getc(infd);
	for (i=0;i<16;i++)
		afbse->rgbUid[i] = getc(infd);
	afbse->tag = read_16ubit(infd);
	afbse->size = read_32ubit(infd);
	afbse->cRef = read_32ubit(infd);
	if (afbse->cRef == 0)
		error(erroroutput,"no file at all\n");
	afbse->foDelay = read_32ubit(infd);
	afbse->usage = getc(infd);
	afbse->cbName = getc(infd);
	afbse->unused2 = getc(infd);
	afbse->unused3 = getc(infd);
	error(erroroutput,"this BSE id is %d\n",id++);
	}

int wvQueryDelayStream(FBSE *afbse)
	{
	if ((afbse->btWin32 ==  msoblipERROR) && (afbse->btMacOS == msoblipERROR))
		return(0);
	if (external_knowledge_0x01)
		return(0);
	if (afbse->foDelay == 0xffffffff)
		return(1);
	return(1);
	}

char *wvGetBitmap(BitmapBlip *abm,MSOFBH  *amsofbh,FBSE *afbse,FILE *infd)
	{
	char *aimage;
	char *buffer=NULL;
	int i,count=0,extra=0;
	static int no;
	FILE *out;
	for (i=0;i<16;i++)
		abm->m_rgbUid[i] = getc(infd);

	buffer = (char *)malloc(4096);
	count+=i;
	abm->m_rgbUidPrimary[0] = 0;

	aimage = get_image_prefix();
	switch (amsofbh->fbt-msofbtBlipFirst)
		{
		case msoblipPNG:
			sprintf(buffer,"%s-wv-%d.png",aimage,no++);
			if (amsofbh->inst ^ msobiPNG)
				extra=1;
			break;
		case msoblipJPEG:
			sprintf(buffer,"%s-wv-%d.jpg",aimage,no++);
			if (amsofbh->inst ^  msobiJFIF)
				extra=1;
			break;
		case msoblipDIB:
			sprintf(buffer,"%s-wv-%d.dib",aimage,no++);
			if (amsofbh->inst ^ msobiDIB)
				extra=1;
			break;
		}

	if (extra)
		{
		for (i=0;i<16;i++)
			abm->m_rgbUidPrimary[i] = getc(infd);
		count+=i;
		}

	abm->m_bTag = getc(infd);
	count++;
	abm->m_pvBits=NULL;
	out = fopen(buffer,"wb");
	for (i=count;i<amsofbh->cbLength;i++)
		fputc(getc(infd),out);
	fclose(out);
	free(aimage);
	return(buffer);
	}

char *wvGetMetafile(MetaFileBlip *amf,MSOFBH *amsofbh,FILE *infd)	
	{
	char *aimage;
	int i,extra=0;
	static int no;
	char *buffer;
	char *tbuffer;
	FILE *out;
	U8 decompressf=0;
	int count=0;
	for (i=0;i<16;i++)
		amf->m_rgbUid[i] = getc(infd);
	count+=16;

	amf->m_rgbUidPrimary[0] = 0;

	aimage = get_image_prefix();
	buffer = (char *)malloc(4096);
	switch (amsofbh->fbt-msofbtBlipFirst)
		{
		case msoblipEMF:
			sprintf(buffer,"%s-wv-%d.emf",aimage,no++);
			if (amsofbh->inst ^ msobiEMF)
				extra=1;
			break;
		case msoblipWMF:
			sprintf(buffer,"%s-wv-%d.wmf",aimage,no++);
			if (amsofbh->inst ^  msobiWMF)
				extra=1;
			break;
		case msoblipPICT:
			sprintf(buffer,"%s-wv-%d.pict",aimage,no++);
			if (amsofbh->inst ^ msobiPICT)
				extra=1;
			break;
		}

	if (extra)
		{
		for (i=0;i<16;i++)
			amf->m_rgbUidPrimary[i] = getc(infd);
		count+=i;
		}
			

	amf->m_cb = read_32ubit(infd);
	amf->m_rcBounds.bottom = read_32ubit(infd);
	amf->m_rcBounds.top = read_32ubit(infd);
	amf->m_rcBounds.right = read_32ubit(infd);
	amf->m_rcBounds.left = read_32ubit(infd);
	amf->m_ptSize.y = read_32ubit(infd);
	amf->m_ptSize.x = read_32ubit(infd);
	amf->m_cbSave = read_32ubit(infd);
	amf->m_fCompression = getc(infd);
	amf->m_fFilter = getc(infd);
	amf->m_pvBits=NULL;
	count +=50;


	if (amf->m_fCompression == msocompressionDeflate)
		{
		decompressf = setdecom();
		if (!(decompressf))
			strcat(buffer,".lzed");
		}
	else
		strcat(buffer,".lzed");

	if (decompressf)
		{
		/*tbuffer = "/tmp/wvscratch";*/
		tbuffer = tmpnam(NULL);
		}
	else
		tbuffer = buffer;

	out = fopen(tbuffer,"w+b");
	for (i=count;i<amsofbh->cbLength;i++)
		fputc(getc(infd),out);

	if (decompressf)
		{
		fflush(out);
		rewind(out);
		decompress(out,buffer,amf->m_cbSave,amf->m_cb);
		}

	fclose(out);

	if (((decompressf) && (amf->m_fCompression == msocompressionDeflate)) || (amf->m_fCompression == msocompressionNone))
		if (amsofbh->fbt-msofbtBlipFirst == msoblipWMF)	/*wmf only for now*/
			{
			error(erroroutput,"converting wmf, final length should be %d, name %s\n",amf->m_cb,buffer);
			convertwmf(buffer);
			error(erroroutput,"converted wmf\n");
			}

	if (decompressf)
		unlink(tbuffer);
	free(aimage);
	return(buffer);
	}

U32 wvGetFOPTE(FOPTE *afopte,FILE *infd)
	{
	U32 ret=0;
	U16 dtemp;
	dtemp = read_16ubit(infd);
#ifdef PURIFY
	afopte->pid = 0;
	afopte->fBid = 0;
	afopte->fComplex = 0;
#endif
	afopte->pid = (dtemp & 0x3fff);
	afopte->fBid = ((dtemp & 0x4000)>>14);
	afopte->fComplex = ((dtemp & 0x8000)>>15);
	afopte->op = read_32ubit(infd);

	if ( afopte->fComplex )
		{
		error(erroroutput,"1 complex len is %d (%x)\n",afopte->op,afopte->op);
		ret = afopte->op;	
		}
	else if (afopte->fBid)
		error(erroroutput,"great including graphic number %d %d\n",afopte->op,afopte->op);
	error(erroroutput,"orig %x,pid is %x, val is %x\n",dtemp,afopte->pid,afopte->op);
	return(ret);
	}


void wvGetFSP(FSP *afsp,FILE *infd)
	{
	afsp->spid = read_32ubit(infd);
	afsp->grfPersistent = read_32ubit(infd);
	}

void wvGetFDG(FDG *afdg,FILE *infd)
	{
	afdg->csp = read_32ubit(infd);
	afdg->spidCur = read_32ubit(infd);
	error(erroroutput,"there are %d shapes here, the last is %x\n",afdg->csp,afdg->spidCur);
	}

fsp_list *wvParseEscher(fbse_list **pic_list,U32 fcDggInfo,U32 lcbDggInfo,FILE *tablefd,FILE *mainfd)
	{
	int remainder;
	int i,j;
	MSOFBH amsofbh;
	MSOFBH rmsofbh;
	FDGG afdgg;
	FIDCL *afidcl;
	FBSE afbse;
	MetaFileBlip amf;
	BitmapBlip abm;
	fsp_list *afsp_list=NULL;
	fsp_list *pfsp_list=NULL;
	fopte_list *pfopte_list=NULL;
	fbse_list *afbse_list=NULL;
	fbse_list *pfbse_list=NULL;
	FDG afdg;
	FILE *temp=NULL;
	/*
	FILE *out = fopen("drawingtest","w+b");
	*/
	FILE *out = tmpfile();
	long finish;
	int pid;
	char *name;
	long lastpos = ftell(mainfd);
	char dodgyhack=1;
	long dtest;

	fseek(tablefd,fcDggInfo,SEEK_SET);

	for (i=0;i<lcbDggInfo;i++)
		fputc(fgetc(tablefd),out);

	fseek(out,0,SEEK_SET);

	/* must begin with msofbtDggContainer*/
	wvGetMSOFBH(&rmsofbh,out);	
	error(erroroutput,"the id here is %x, loc %x, ends at %x\n",rmsofbh.fbt,ftell(out),ftell(out)+rmsofbh.cbLength);
	if (rmsofbh.fbt != msofbtDggContainer)
		{
		error(erroroutput,"possible problem %x\n",rmsofbh.fbt);
		dodgyhack=0;
		}

	while( ftell(out) <  lcbDggInfo )
		{
		if ((dodgyhack) && (ftell(out) == rmsofbh.cbLength+8))
			{
			error(erroroutput,"come to the end of the root wrapper\n");
			error(erroroutput,"magin no is (%x)\n",getc(out));
			}
	
		
		wvGetMSOFBH(&amsofbh,out);	
		error(erroroutput,"the id here is %x, loc %x, ends at %x\n",amsofbh.fbt,ftell(out),ftell(out)+amsofbh.cbLength);
		switch(amsofbh.fbt)
			{
			case msofbtDgg:
				wvGetFDGG(&afdgg,out);
				afidcl = (FIDCL *)malloc(sizeof(FIDCL) * afdgg.cdgSaved);
				error(erroroutput,"no of FIDCL is %d\n",afdgg.cdgSaved);
				j=16;
				for(i=0;i<afdgg.cdgSaved;i++)
					{
					wvGetFIDCL(&(afidcl[i]),out);
					j+=8;
					}
				for(;j<amsofbh.cbLength;j++)
					getc(out);
				free(afidcl);
				break;
			case msofbtBSE:
				finish = ftell(out)+amsofbh.cbLength;
				if (afbse_list == NULL)
					{
					afbse_list = (fbse_list *)malloc(sizeof(fbse_list));
					pfbse_list = afbse_list;
					}
				else
					{
					pfbse_list->next = (fbse_list *)malloc(sizeof(fbse_list));
					pfbse_list = pfbse_list->next;
					}
				pfbse_list->next = NULL;
				pfbse_list->filename[0] = '\0';
			
				wvGetFBSE(&(pfbse_list->afbse),out);


				if (pfbse_list->afbse.cbName != 0)
					while(read_16ubit(out) != 0)
						error(erroroutput,"blip name char\n");
				error(erroroutput,"the offset is %x\n",pfbse_list->afbse.foDelay);

				if (wvQueryDelayStream(&(pfbse_list->afbse)))
					{
					if (pfbse_list->afbse.foDelay == 0xffffffff)
						{
						error(erroroutput,"invalid blip aborting\n");
						break;
						}
					temp = out;
					out = mainfd;
					if (pfbse_list->afbse.cRef)
						{
						error(erroroutput,"looking in mainfd\n");
						fseek(out,0,SEEK_END);
						dtest = ftell(out);
						if (pfbse_list->afbse.foDelay <  dtest) 
							{
							fseek(out,pfbse_list->afbse.foDelay,SEEK_SET);
							dtest = 0;
							}
						else
							dtest = 1;
						}
					else 
						dtest = 1;
					}
				else if (!(pfbse_list->afbse.cRef))
					dtest = 1;
				else
					dtest = 0;

				if (dtest == 1)
					error(erroroutput,"bad offset for blip\n");
				else
					{
					wvGetMSOFBH(&amsofbh,out);
					name = NULL;
					switch(amsofbh.fbt-msofbtBlipFirst)
						{
						case msoblipWMF:
						case msoblipEMF:
						case msoblipPICT:
							name = wvGetMetafile(&amf,&amsofbh,out);
							break;
						case msoblipJPEG:
						case msoblipPNG:
						case msoblipDIB:
							name = wvGetBitmap(&abm,&amsofbh,&afbse,out);
							break;
						default:
							for(i=0;i<amsofbh.cbLength;i++)
								getc(out);
							break;
						}

					if (name != NULL)
						{
						strcpy(pfbse_list->filename,name);
						free(name);
						}
					}

				if (wvQueryDelayStream(&(pfbse_list->afbse)))
					out = temp;

				/*sync the file*/
				if (ftell(out) != finish)
					{
					error(erroroutput,"file is out of sync, pos is %x ended at %x\n",ftell(out),finish);
					for(i=ftell(out);i<finish;i++)
						getc(out);
					}
				break;
			case msofbtSp:
				if (afsp_list == NULL)
					{
					afsp_list = (fsp_list *)malloc(sizeof(fsp_list));
					pfsp_list = afsp_list;
					pfsp_list->next = NULL;
					}
				else
					{
					pfsp_list->next = (fsp_list *)malloc(sizeof(fsp_list));
					pfsp_list = pfsp_list->next;
					pfsp_list->next = NULL;
					}
				wvGetFSP(&(pfsp_list->afsp),out);
				pfsp_list->afopte_list=NULL;
				error(erroroutput,"yes, spid id no %x was found\n",pfsp_list->afsp.spid);
				break;
			case msofbtDg:
				wvGetFDG(&afdg,out);
				break;
			case msofbtOPT:
				remainder = amsofbh.cbLength;
				pid = 0;
				i=0;
				/*
				while (pid != 0x1ff)
				*/
				while (remainder >= 6)
					{
					if (i==0)
						{
						if (pfsp_list == NULL)
							{
							error(erroroutput,"must be a top level property list\n");
							pfsp_list = (fsp_list *)malloc(sizeof(fsp_list));
							}
						pfsp_list->afopte_list = (fopte_list *)malloc(sizeof(fopte_list));
						pfopte_list = pfsp_list->afopte_list;
						}
					else
						{
						pfopte_list->next =(fopte_list *) malloc(sizeof(fopte_list));
						pfopte_list = pfopte_list->next;
						}
					pfopte_list->next = NULL;
					remainder -= wvGetFOPTE(&(pfopte_list->afopte),out);
					remainder -=6;
					i+=6;
					pid = pfopte_list->afopte.pid;
					if (i+6>amsofbh.cbLength)	/*eat the leftover*/
						break;
					}
				for(;i<amsofbh.cbLength;i++)
					getc(out);
				break;
			default:
				error(erroroutput,"the ver was %x\n",amsofbh.ver);
				if (amsofbh.ver != 0xf)
					for(i=0;i<amsofbh.cbLength;i++)
						getc(out);
				break;
			}
		}


	fclose(out);

	pfbse_list = afbse_list;
	i=0;
	while (pfbse_list != NULL)
		{
		error(erroroutput,"graphic name is %s, bse no is %d\n",pfbse_list->filename,++i);
		pfbse_list = pfbse_list->next;
		}
	*pic_list = afbse_list;
	fseek(mainfd,lastpos,SEEK_SET);
	return(afsp_list);
	}

fbse_list *wvGetSPID(U32 spid,fsp_list *afsp_list,fbse_list *afbse_list)
	{
	fopte_list *temp;
	int i;

	while (afsp_list != NULL)
		{
		if (afsp_list->afsp.spid == spid)
			{
			error(erroroutput,"found the correct spid\n");
			temp = afsp_list->afopte_list;
			while (temp != NULL)
				{
				if ((temp->afopte.fBid) && (!(temp->afopte.fComplex)))
					{
					error(erroroutput,"found a graphic to go with the spid, no %d\n",temp->afopte.op);
					for (i=1;i<temp->afopte.op;i++)
						afbse_list = afbse_list->next;
					return(afbse_list);
					break;
					}
				temp = temp->next;
				}
			break;
			}
		afsp_list = afsp_list->next;
		}

	return(NULL);
	}


U32 wvGetSPIDfromCP(U32 cp,textportions *portions)
	{
	int i;
	for (i=0;i<portions->noofficedraw;i++)
        if (cp == portions->officedrawcps[i])
            return(portions->fspas[i].spid);
	return(0xffffffffL);
	}

void wvGetBITMAP(BITMAP *bmp,FILE *infd)
	{
	int i;
	for (i=0;i<14;i++)
		bmp->bm[i] = getc(infd);
	}

void wvGetrc(rc *arc,FILE *infd)
	{
	int i;
	for (i=0;i<14;i++)
		arc->bm[i] = getc(infd);
	}

void wvGetPICF(PICF *apicf,FILE *infd,U32 offset)
	{
	int count=0;
	U8 temp;
	FILE *out;
	fbse_list *pic_list;
	fbse_list *tpic_list;
	fsp_list *afsp_list;
	fsp_list *tfsp_list;
	fopte_list *tfopte_list;

	fseek(infd,offset,SEEK_SET);

	apicf->lcb = read_32ubit(infd);
	apicf->cbHeader = read_16ubit(infd);
	apicf->mfp_mm = (S16)read_16ubit(infd);
	apicf->mfp_xExt = (S16)read_16ubit(infd);
	apicf->mfp_yExt = (S16)read_16ubit(infd);
	apicf->mfp_hMF = (S16)read_16ubit(infd);
	error(erroroutput,"0x01 type is %d\n",apicf->mfp_mm);
	if (apicf->mfp_mm == 99)
		wvGetBITMAP(&(apicf->obj.bitmap),infd);
	else 
		wvGetrc(&(apicf->obj.arc),infd);
	apicf->dxaGoal = (S16)read_16ubit(infd);
	apicf->dyaGoal = (S16)read_16ubit(infd);
	apicf->mx = (S16)read_16ubit(infd);
	apicf->my = (S16)read_16ubit(infd);
	apicf->dxaCropLeft = (S16)read_16ubit(infd);
	apicf->dyaCropTop = (S16)read_16ubit(infd);
	apicf->dxaCropRight = (S16)read_16ubit(infd);
	apicf->dyaCropBottom = (S16)read_16ubit(infd);
	temp = getc(infd);

#ifdef PURIFY
	apicf->brcl = 0;
	apicf->fFrameEmpty = 0;
	apicf->fBitmap = 0;
	apicf->fDrawHatch = 0;
	apicf->fError = 0;
#endif

	apicf->brcl = temp & 0x0F;
	apicf->fFrameEmpty = (temp & 0x10)>>4;
	apicf->fBitmap = (temp&0x20)>>5;
	apicf->fDrawHatch = (temp&0x40)>>6;
	apicf->fError = (temp&0x80)>>7;

	apicf->bpp = getc(infd);
	wvGetBRC(0,&(apicf->brcTop),infd);
	wvGetBRC(0,&(apicf->brcLeft),infd);
	wvGetBRC(0,&(apicf->brcBottom),infd);
	wvGetBRC(0,&(apicf->brcRight),infd);
	apicf->dxaOrigin = (S16)read_16ubit(infd);
	apicf->dyaOrigin = (S16)read_16ubit(infd);
	apicf->cProps = (S16)read_16ubit(infd);
	/*
	apicf->rgb = "/tmp/wvscratch2";
	*/
	count+=68;
	/*
	out = fopen(apicf->rgb,"w+b");
	*/
	out = tmpfile();
	for (;count<apicf->lcb;count++)
		fputc(getc(infd),out);
	rewind(out);

	external_knowledge_0x01 = 1;	/*no delay streams in use*/
	afsp_list = wvParseEscher(&pic_list,0,(apicf->lcb)-68,out,out);
	external_knowledge_0x01 = 0;	/*reenable delay streams (not necessary)*/

	fclose(out);

	apicf->rgb = NULL;

	if (pic_list == NULL)
		error(erroroutput,"rats\n");
	else
		{
		if (pic_list->filename[0] != '\0')
			error(erroroutput,"filename is now %s\n",pic_list->filename);
		apicf->rgb = (S8 *)malloc(strlen(pic_list->filename)+1);
		strcpy(apicf->rgb,pic_list->filename);
		}

	while (afsp_list != NULL)
		{
		while (afsp_list->afopte_list != NULL)
			{
			tfopte_list = afsp_list->afopte_list;
			afsp_list->afopte_list = afsp_list->afopte_list->next;
			free(tfopte_list);
			}
		tfsp_list = afsp_list;
		afsp_list = afsp_list->next;
		free(tfsp_list);
		}

	while (pic_list != NULL)
		{
		tpic_list = pic_list;
		pic_list = pic_list->next;
		free(tpic_list);
		}

	}
