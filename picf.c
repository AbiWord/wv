#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"

int wvGetPICF(wvVersion ver,PICF *apicf,wvStream *fd)
	{
	int ret=0;
	FILE *f;
	U8 temp;
	U32 i;
	long pos=wvStream_tell(fd);

	apicf->lcb = read_32ubit(fd);
	apicf->cbHeader = read_16ubit(fd);
	wvTrace(("size of pic is %x (%d)\n",apicf->cbHeader,apicf->cbHeader));
	apicf->mfp_mm = (S16)read_16ubit(fd);
	wvTrace(("mm type is %d\n",apicf->mfp_mm));
	apicf->mfp_xExt = (S16)read_16ubit(fd);
	apicf->mfp_yExt = (S16)read_16ubit(fd);
	apicf->mfp_hMF = (S16)read_16ubit(fd);
	if (apicf->mfp_mm == 99)
		wvGetBITMAP(&(apicf->obj.bitmap),fd);
	else 
		wvGetrc(&(apicf->obj.arc),fd);
	apicf->dxaGoal = (S16)read_16ubit(fd);
	apicf->dyaGoal = (S16)read_16ubit(fd);
	apicf->mx = (S16)read_16ubit(fd);
	apicf->my = (S16)read_16ubit(fd);
	apicf->dxaCropLeft = (S16)read_16ubit(fd);
	apicf->dyaCropTop = (S16)read_16ubit(fd);
	apicf->dxaCropRight = (S16)read_16ubit(fd);
	apicf->dyaCropBottom = (S16)read_16ubit(fd);
	temp = read_8ubit(fd);

	apicf->brcl = temp & 0x0F;
	apicf->fFrameEmpty = (temp & 0x10)>>4;
	apicf->fBitmap = (temp&0x20)>>5;
	wvTrace(("bitmap is %d\n",apicf->fBitmap));
	apicf->fDrawHatch = (temp&0x40)>>6;
	apicf->fError = (temp&0x80)>>7;

	apicf->bpp = read_8ubit(fd);
	wvGetBRC(ver,&(apicf->brcTop),fd);
	wvGetBRC(ver,&(apicf->brcLeft),fd);
	wvGetBRC(ver,&(apicf->brcBottom),fd);
	wvGetBRC(ver,&(apicf->brcRight),fd);
	apicf->dxaOrigin = (S16)read_16ubit(fd);
	apicf->dyaOrigin = (S16)read_16ubit(fd);
	if (ver == WORD8)
		apicf->cProps = (S16)read_16ubit(fd);
	else
		apicf->cProps = 0;
	pos = wvStream_tell(fd)-pos;
	for(i=pos;i<apicf->cbHeader;i++)
		read_8ubit(fd);
	wvTrace(("pos is finally %x\n",wvStream_tell(fd)));
	wvTrace(("len of data is %d\n",apicf->lcb-apicf->cbHeader));
	wvTrace(("ends at %x\n",wvStream_tell(fd)+apicf->lcb-apicf->cbHeader));
	f = tmpfile();
	if (f == NULL)
		{
		wvError(("Couldnt create tmpfile: %s\n",strerror(errno)));
		apicf->rgb = NULL;
		return(ret);
		}
	/*
	sprintf(buffer,"/tmp/newtest-%d",s++);
	f = fopen(buffer,"w+b");
	*/
	i=0;

	if (apicf->mfp_mm < 90)
		{
		U32 len;
		wvTrace(("test\n"));
		ret=1;
		len = apicf->lcb-apicf->cbHeader;

		i = wvEatOldGraphicHeader(fd,len);
		wvTrace(("len is %d, header len guess is %d\n",len,i));
		if (i+1 >= len)
			{
			wvTrace(("all read ok methinks\n"));
			apicf->rgb = NULL;
			return(ret);
			}
		len -= i;
#if 0
		/*
		a msofbtSpContainer amsofbh
		a msofbtSp amsofbh 
		a fopt
		a msofbtBSE amsofbh
		a BSE amsofbh
		a FBSE for a dib, and then
		all this dib information
		*/
#else
		len += 14;
		wvTrace(("len is now %d\n",len));
		fputc(0x42,f);
		fputc(0x4D,f);

		fputc(len&0x000000FF,f);
		fputc((len&0x0000FF00)>>8,f);
		fputc((len&0x00FF0000)>>16,f);
		fputc((len&0xFF000000)>>24,f);

		fputc(0x00,f);
		fputc(0x00,f);
		fputc(0x00,f);
		fputc(0x00,f);

		fputc(0x36,f);
		fputc(0x00,f);
		fputc(0x00,f);
		fputc(0x00,f);
#endif
		}

	for(;i<apicf->lcb-apicf->cbHeader;i++)
		fputc(read_8ubit(fd),f);

	rewind(f);
	wvStream_FILE_create(&apicf->rgb, f);
	return(ret);
	}

U32 wvEatOldGraphicHeader(wvStream *fd,U32 len)
	{
	U32 X,entry,count=0,test;
	U16 pad;
	test = read_32ubit(fd); 	/*0x00090001*/
	if (test != 0x00090001L)
		wvError(("Old Graphic\n"));
	count+=4;
	test = read_16ubit(fd);	/*0x0300*/
	if (test != 0x0300)
		wvError(("Old Graphic\n"));
	count+=2;

	read_32ubit(fd); 	/*changes*/
	count+=4;
	test = read_16ubit(fd);	/*0x0000*/
	if (test != 0x00000000L)
		wvError(("Old Graphic\n"));
	count+=2;
	X = read_32ubit(fd); 	/*changes, lets call this X*/
	wvError(("X is %x\n",X));
	count+=4;
	read_16ubit(fd);	/*0x0000*/
	if (test != 0x00000000L)
		wvError(("Old Graphic\n"));
	count+=2;

	/*
	while ( entry != X)
	*/
	do
		{
		entry = read_32ubit(fd);
		count+=4;
		wvTrace(("Entry is %x, %x, count is %d\n",entry,wvStream_tell(fd),count));
		switch(entry)
			{
			case 3:
				read_16ubit(fd);
				count+=2;
				wvTrace(("suspect that we are finished, count %d, len %d\n",count,len));
				break;
			default:
				{
				U32 len = entry-2;
				U32 i;
				wvTrace(("len is %d, predict end of %x\n",len,wvStream_tell(fd)+(entry-2)*2));
				for(i=0;i<len;i++)
					{
					test = read_16ubit(fd);
					if ((i==0) && ((test ==  0x0f43) || (test == 0x0b41)))
						{
						wvTrace(("Found a Bitmap, Will strip header and return with bitmap data\n"));
						count+=2;
						pad = test;
						test = read_32ubit(fd);	/*0x00cc0020*/
						if (test != 0x00cc0020)
							wvError(("Old Graphic\n"));
						count+=4;

						if (pad == 0x0f43)
							{
							test = read_16ubit(fd); /*0x0000*/
							if (test != 0x0000)
								wvError(("Old Graphic\n"));
							count+=2;
							}
						
						read_16ubit(fd);	/*width*/
						count+=2;
						read_16ubit(fd);	/*height*/
						count+=2;
						test = read_32ubit(fd);	/*0x00000000L*/
						if (test != 0x00000000L)
							wvError(("Old Graphic\n"));
						count+=4;
						read_16ubit(fd);	/*width*/
						count+=2;
						read_16ubit(fd);	/*height*/
						count+=2;
						test = read_32ubit(fd);	/*0x00000000L*/
						if (test != 0x00000000L)
							wvError(("Old Graphic\n"));
						count+=4;
						return(count);
						}
					count+=2;
					}
				}
				break;
			}
		}
	while ( count+1 < len);
	wvTrace(("Entry is %x %x, %d\n",entry,wvStream_tell(fd),count));
	return(count);
	}

extern int external_knowledge_0x01;

#if 0
void oldwvGetPICF(PICF *apicf,wvStream *fd,U32 offset)
	{
	U32 count=0;
	U8 temp;
	wvStream *out;
	fbse_list *pic_list;
	fbse_list *tpic_list;
	fsp_list *afsp_list;
	fsp_list *tfsp_list;
	fopte_list *tfopte_list;

	fseek(fd,offset,SEEK_SET);

	apicf->lcb = read_32ubit(fd);
	apicf->cbHeader = read_16ubit(fd);
	apicf->mfp_mm = (S16)read_16ubit(fd);
	apicf->mfp_xExt = (S16)read_16ubit(fd);
	apicf->mfp_yExt = (S16)read_16ubit(fd);
	apicf->mfp_hMF = (S16)read_16ubit(fd);
	if (apicf->mfp_mm == 99)
		wvGetBITMAP(&(apicf->obj.bitmap),fd);
	else 
		wvGetrc(&(apicf->obj.arc),fd);
	apicf->dxaGoal = (S16)read_16ubit(fd);
	apicf->dyaGoal = (S16)read_16ubit(fd);
	apicf->mx = (S16)read_16ubit(fd);
	apicf->my = (S16)read_16ubit(fd);
	apicf->dxaCropLeft = (S16)read_16ubit(fd);
	apicf->dyaCropTop = (S16)read_16ubit(fd);
	apicf->dxaCropRight = (S16)read_16ubit(fd);
	apicf->dyaCropBottom = (S16)read_16ubit(fd);
	temp = getc(fd);

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

	apicf->bpp = getc(fd);
	wvGetBRC(0,&(apicf->brcTop),fd);
	wvGetBRC(0,&(apicf->brcLeft),fd);
	wvGetBRC(0,&(apicf->brcBottom),fd);
	wvGetBRC(0,&(apicf->brcRight),fd);
	apicf->dxaOrigin = (S16)read_16ubit(fd);
	apicf->dyaOrigin = (S16)read_16ubit(fd);
	apicf->cProps = (S16)read_16ubit(fd);
	/*
	apicf->rgb = "/tmp/wvscratch2";
	*/
	count+=68;
	/*
	out = fopen(apicf->rgb,"w+b");
	*/
	out = tmpfile();
	for (;count<apicf->lcb;count++)
		fputc(getc(fd),out);
	rewind(out);

	external_knowledge_0x01 = 1;	/*no delay streams in use*/
	afsp_list = wvParseEscher(&pic_list,0,(apicf->lcb)-68,out,out);
	external_knowledge_0x01 = 0;	/*reenable delay streams (not necessary)*/

	fclose(out);

	apicf->rgb = NULL;

	if (pic_list == NULL)
		wvError(("rats\n"));
	else
		{
		apicf->rgb = (S8 *)wvMalloc(strlen(pic_list->filename)+1);
		strcpy(apicf->rgb,pic_list->filename);
		}

	while (afsp_list != NULL)
		{
		while (afsp_list->afopte_list != NULL)
			{
			tfopte_list = afsp_list->afopte_list;
			afsp_list->afopte_list = afsp_list->afopte_list->next;
			wvFree(tfopte_list);
			}
		tfsp_list = afsp_list;
		afsp_list = afsp_list->next;
		wvFree(tfsp_list);
		}

	while (pic_list != NULL)
		{
		tpic_list = pic_list;
		pic_list = pic_list->next;
		wvFree(tpic_list);
		}

	}
#endif
