#include <stdio.h>
#include <stdlib.h>
#include "wv.h"

void wvGetPICF(PICF *apicf,FILE *infd,U32 offset)
	{
	U8 temp;

	fseek(infd,offset,SEEK_SET);

	apicf->lcb = read_32ubit(infd);
	apicf->cbHeader = read_16ubit(infd);
	apicf->mfp_mm = (S16)read_16ubit(infd);
	apicf->mfp_xExt = (S16)read_16ubit(infd);
	apicf->mfp_yExt = (S16)read_16ubit(infd);
	apicf->mfp_hMF = (S16)read_16ubit(infd);
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
	}


extern int external_knowledge_0x01;

void oldwvGetPICF(PICF *apicf,FILE *infd,U32 offset)
	{
	U32 count=0;
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
		wvError(("rats\n"));
	else
		{
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
