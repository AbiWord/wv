#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "wv.h"

extern FILE *outputfile;
extern FILE *erroroutput;
extern char *outputfilename;

int external_knowledge_0x01 = 0;/*when we magically know that we will not
							      be using a delay stream, i.e. big gobs of
								  emperical evidence, left external so that
								  if im wrong it'll be looking me in the face
								*/

char failsafe[1];

U16 idlist[NOOFIDS] = {0,0x216,0x3D4,0x542,0x6E0,0x46A,0x7A8,0x800};

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
	wvTrace(("spidMax %d cidcl %d cspSaved %d cdgSaved %d\n",afdgg->spidMax,afdgg->cidcl,afdgg->cspSaved, afdgg->cdgSaved ));
	}

void wvGetFIDCL(FIDCL *afidcl,FILE *infd)
	{
	afidcl->dgid = read_32ubit(infd);
	afidcl->cspidCur = read_32ubit(infd);
	wvTrace(("dgid %d cspidCur %d\n",afidcl->dgid,afidcl->cspidCur));
	}

void wvGetFBSE(FBSE *afbse,FILE *infd)
	{
	int i;
	afbse->btWin32 = getc(infd);
	afbse->btMacOS = getc(infd);
	for (i=0;i<16;i++)
		afbse->rgbUid[i] = getc(infd);
	afbse->tag = read_16ubit(infd);
	afbse->size = read_32ubit(infd);
	afbse->cRef = read_32ubit(infd);
	afbse->foDelay = read_32ubit(infd);
	afbse->usage = getc(infd);
	afbse->cbName = getc(infd);
	afbse->unused2 = getc(infd);
	afbse->unused3 = getc(infd);
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
	return("/tmp/rubbish");
#if 0
	char *aimage;
	char *buffer=NULL;
	int count=0,extra=0;
	U32 i;
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
#endif
	}

char *wvGetMetafile(MetaFileBlip *amf,MSOFBH *amsofbh,FILE *infd)	
	{
	return("/tmp/rubbish");
#if 0
	char *aimage;
	int extra=0;
	U32 i;
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
			wvStrcat(buffer,".lzed");
		}
	else
		wvStrcat(buffer,".lzed");

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
			wvTrace(("converting wmf, final length should be %d, name %s\n",amf->m_cb,buffer));
			convertwmf(buffer);
			wvTrace(("converted wmf\n"));
			}

	if (decompressf)
		unlink(tbuffer);
	free(aimage);
	return(buffer);
#endif
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
		wvTrace(("1 complex len is %d (%x)\n",afopte->op,afopte->op));
		ret = afopte->op;	
		}
#if 0
	else if (afopte->fBid)
		wvTrace(("great including graphic number %d %d\n",afopte->op,afopte->op));
#endif
	wvTrace(("orig %x,pid is %x, val is %x\n",dtemp,afopte->pid,afopte->op));
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
	wvTrace(("there are %d shapes here, the last is %x\n",afdg->csp,afdg->spidCur));
	}

fsp_list *wvParseEscher(fbse_list **pic_list,U32 fcDggInfo,U32 lcbDggInfo,FILE *tablefd,FILE *mainfd)
	{
	int remainder;
	U32 i,j;
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
	long finish,k=0;
	int pid;
	char *name;
	long lastpos = ftell(mainfd);
	char dodgyhack=1;
	U32 dtest;

	fseek(tablefd,fcDggInfo,SEEK_SET);

	for (i=0;i<lcbDggInfo;i++)
		fputc(fgetc(tablefd),out);

	fseek(out,0,SEEK_SET);

	/* must begin with msofbtDggContainer*/
	wvGetMSOFBH(&rmsofbh,out);	
	wvTrace(("the id here is %x, loc %x, ends at %x\n",rmsofbh.fbt,ftell(out),ftell(out)+rmsofbh.cbLength));
	if (rmsofbh.fbt != msofbtDggContainer)
		{
		wvTrace(("possible problem %x\n",rmsofbh.fbt));
		dodgyhack=0;
		}

	while( (U32)(ftell(out)) <  lcbDggInfo )
		{
		if ((dodgyhack) && ((U32)(ftell(out)) == rmsofbh.cbLength+8))
			{
			wvTrace(("come to the end of the root wrapper\n"));
			wvTrace(("magin no is (%x)\n",getc(out)));
			}
	
		
		wvGetMSOFBH(&amsofbh,out);	
		wvTrace(("the id here is %x, loc %x, ends at %x\n",amsofbh.fbt,ftell(out),ftell(out)+amsofbh.cbLength));
		switch(amsofbh.fbt)
			{
			case msofbtDgg:
				wvGetFDGG(&afdgg,out);
				afidcl = (FIDCL *)malloc(sizeof(FIDCL) * afdgg.cdgSaved);
				wvTrace(("no of FIDCL is %d\n",afdgg.cdgSaved));
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
						wvTrace(("blip name char\n"));
				wvTrace(("the offset is %x\n",pfbse_list->afbse.foDelay));

				if (wvQueryDelayStream(&(pfbse_list->afbse)))
					{
					if (pfbse_list->afbse.foDelay == 0xffffffff)
						{
						wvTrace(("invalid blip aborting\n"));
						break;
						}
					temp = out;
					out = mainfd;
					if (pfbse_list->afbse.cRef)
						{
						wvTrace(("looking in mainfd\n"));
						fseek(out,0,SEEK_END);
						dtest = (U32)ftell(out);
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
					wvTrace(("bad offset for blip\n"));
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
					wvTrace(("file is out of sync, pos is %x ended at %x\n",ftell(out),finish));
					for(j=ftell(out);k<finish;k++)
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
				wvTrace(("yes, spid id no %x was found\n",pfsp_list->afsp.spid));
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
							wvTrace(("must be a top level property list\n"));
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
				wvTrace(("the ver was %x\n",amsofbh.ver));
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
		wvTrace(("graphic name is %s, bse no is %d\n",pfbse_list->filename,++i));
		pfbse_list = pfbse_list->next;
		}
	*pic_list = afbse_list;
	fseek(mainfd,lastpos,SEEK_SET);
	return(afsp_list);
	}

fbse_list *wvGetSPID(U32 spid,fsp_list *afsp_list,fbse_list *afbse_list)
	{
	fopte_list *temp;
	U32 i;

	while (afsp_list != NULL)
		{
		if (afsp_list->afsp.spid == spid)
			{
			wvTrace(("found the correct spid\n"));
			temp = afsp_list->afopte_list;
			while (temp != NULL)
				{
				if ((temp->afopte.fBid) && (!(temp->afopte.fComplex)))
					{
					wvTrace(("found a graphic to go with the spid, no %d\n",temp->afopte.op));
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
	U32 i;
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
