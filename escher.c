#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "wv.h"

int HandleBitmap(char *name,BitmapBlip *bitmap);
int HandleMetafile(char *name,MetaFileBlip *bitmap);

char *wvHtmlGraphic(wvParseStruct *ps,Blip *blip)
	{
	static int i;
	char buffer[10];
	char *name;
	FILE *fd;
	char test[3];
	sprintf(buffer,"%d",i++);
	name = strdup(ps->filename);
	wvTrace(("name is %s\n",name));
	remove_suffix (name, ".doc");
	if (ps->dir != NULL)
		{
		char *tempa,*tempb;
		tempb = strdup(ps->dir);
		tempa = base_name(name);
		wvAppendStr(&tempb,"/");
		wvAppendStr(&tempb,tempa);
		wvFree(name);
		name = tempb;
		}

	wvAppendStr(&name,buffer);
	/* 
	temp hack to test older included bmps in word 6 and 7,
	should be wrapped in a modern escher strucure before getting
	to here, and then handled as normal
	*/
	wvTrace(("type is %d\n",blip->type));
	switch(blip->type)
		{
		case msoblipJPEG:
        case msoblipDIB:
        case msoblipPNG:
			fd = (FILE *)(blip->blip.bitmap.m_pvBits);
			test[2] = '\0';
			test[0] = getc(fd);
			test[1] = getc(fd);
			rewind(fd);
			if (!(strcmp(test,"BM")))
				{
				wvAppendStr(&name,".bmp");
				if (0 != HandleBitmap(name,&blip->blip.bitmap))
					return(NULL);
				remove_suffix (name, ".bmp");
				bmptopng(name);
				wvAppendStr(&name,".png");
				return(name);
				}
		default:
			break;
		}

	switch(blip->type)
        {
        case msoblipWMF:
			wvAppendStr(&name,".wmf");
			if (0 != HandleMetafile(name,&blip->blip.metafile))
				return(NULL);
			break;
        case msoblipEMF:
			wvAppendStr(&name,".emf");
			if (0 != HandleMetafile(name,&blip->blip.metafile))
				return(NULL);
			break;
        case msoblipPICT:
			wvAppendStr(&name,".pict");
			if (0 != HandleMetafile(name,&blip->blip.metafile))
				return(NULL);
            break;
        case msoblipJPEG:
			wvAppendStr(&name,".jpg");
			if (0 != HandleBitmap(name,&blip->blip.bitmap))
				return(NULL);
			break;
        case msoblipDIB:
			wvAppendStr(&name,".dib");
			if (0 != HandleBitmap(name,&blip->blip.bitmap))
				return(NULL);
			break;
        case msoblipPNG:
			wvAppendStr(&name,".png");
			if (0 != HandleBitmap(name,&blip->blip.bitmap))
				return(NULL);
            break;
        }
	return(name);
	}


int HandleBitmap(char *name,BitmapBlip *bitmap)
	{
	int c;
	FILE *fd;
	fd = fopen(name,"wb");
	if (fd == NULL)
		{
		wvError(("Cannot open %s for writing:%s\n",name,strerror(errno)));
		return(-1);
		}
	while (EOF != (c = getc((FILE *)(bitmap->m_pvBits))))
		fputc(c,fd);
	fclose(fd);
	wvTrace(("Name is %s\n",name));
	return(0);
	}

int HandleMetafile(char *name,MetaFileBlip *bitmap)
	{
	int c;
	FILE *fd;
	fd = fopen(name,"wb");
	if (fd == NULL)
		{
		wvError(("Cannot open %s for writing:%s\n",name,strerror(errno)));
		return(-1);
		}
	while (EOF != (c = getc((FILE *)(bitmap->m_pvBits))))
		fputc(c,fd);
	fclose(fd);
	wvTrace(("Name is %s\n",name));
	return(0);
	}

void wvReleaseEscher(escherstruct *item)
	{
	wvReleaseDggContainer(&item->dggcontainer);
	wvReleaseDgContainer(&item->dgcontainer);
	}

void wvInitEscher(escherstruct *item)
	{
	wvInitDggContainer(&item->dggcontainer);
	wvInitDgContainer(&item->dgcontainer);
	}

void wvGetEscher(escherstruct *item,U32 offset,U32 len,FILE *fd,FILE *delay)
	{
	U32 count=0;
	MSOFBH amsofbh;
	long pos;
	fseek(fd,offset,SEEK_SET);
	wvTrace(("offset %x, len %d\n",offset,len));
	wvInitEscher(item);
	while (count < len)
		{
		count += wvGetMSOFBH(&amsofbh,fd);
		wvTrace(("count is %x,len is %x, next len is %x\n",count,len,amsofbh.cbLength));
		wvTrace(("type is %x\n	",amsofbh.fbt));
		switch(amsofbh.fbt)
			{
			case msofbtDggContainer:
				count += wvGetDggContainer(&item->dggcontainer,&amsofbh,fd,delay);
				break;
			case msofbtDgContainer:
				count += wvGetDgContainer(&item->dgcontainer,&amsofbh,fd);
				break;
			default:
				wvError(("Not a container, panic (%x)\n",amsofbh.fbt));
				return;
				break;
			}
		}
	wvTrace(("offset %x, len %d (pos %x)\n",offset,len,ftell(fd)));
	}

void wvReleaseDggContainer(DggContainer *item)
	{
	wvReleaseSplitMenuColors(&item->splitmenucolors);
	wvReleaseDgg(&item->dgg);
	wvReleaseBstoreContainer(&item->bstorecontainer);
	}

void wvInitDggContainer(DggContainer *item)
	{
	wvInitSplitMenuColors(&item->splitmenucolors);
	wvInitDgg(&item->dgg);
	wvInitBstoreContainer(&item->bstorecontainer);
	}

U32 wvGetDggContainer(DggContainer *item,MSOFBH *msofbh,FILE *fd,FILE *delay)
	{
	MSOFBH amsofbh;
	U32 count=0;

	while (count < msofbh->cbLength)
		{
		count += wvGetMSOFBH(&amsofbh,fd);
		wvTrace(("len is %x, type is %x, count %x,fullen %x\n",amsofbh.cbLength,amsofbh.fbt,count,msofbh->cbLength));
		wvTrace(("type is %x\n	",amsofbh.fbt));
		switch(amsofbh.fbt)
			{
			case msofbtDgg:
				count +=wvGetDgg(&item->dgg,&amsofbh,fd);
				break;
			case msofbtSplitMenuColors:
				count +=wvGetSplitMenuColors(&item->splitmenucolors,&amsofbh,fd);
				break;
			case msofbtBstoreContainer:
				count +=wvGetBstoreContainer(&item->bstorecontainer,&amsofbh,fd,delay);
				wvTrace(("type is %d (number is %d\n",item->bstorecontainer.blip[item->bstorecontainer.no_fbse-1].type,item->bstorecontainer.no_fbse));
				break;
			default:
				count +=wvEatmsofbt(&amsofbh,fd);
				wvError(("Eating type 0x%x\n",amsofbh.fbt));
				break;
			}
		}
	/*
	For some reason I appear to have an extra byte associated either with
	this or its wrapper, I will investigate further.
	*/
	getc(fd);
	count++;
	
	return(count);
	}

void wvReleaseDgContainer (DgContainer * item)
	{
	U32 i;
	for (i = 0; i < item->no_spgrcontainer; i++)
		wvReleaseSpgrContainer (&(item->spgrcontainer[i]));
	wvFree (item->spgrcontainer);
	}

void wvInitDgContainer (DgContainer * item)
	{
	item->no_spgrcontainer = 0;
	item->spgrcontainer=NULL;
	}

void wvReleaseBstoreContainer(BstoreContainer *item)
	{
	U32 i;
	for (i=0;i<item->no_fbse;i++)
		wvReleaseBlip(&item->blip[i]);
	wvFree(item->blip);
	}

void wvInitBstoreContainer(BstoreContainer *item)
	{
	item->no_fbse=0;
	item->blip=NULL;
	}

U32 wvGetBstoreContainer(BstoreContainer *item,MSOFBH *msofbh,FILE *fd,FILE *delay)
	{
	MSOFBH amsofbh;
	U32 count=0;
	while (count < msofbh->cbLength)
		{
		count += wvGetMSOFBH(&amsofbh,fd);
		wvTrace(("type is %x\n	",amsofbh.fbt));
		switch(amsofbh.fbt)
			{
			case msofbtBSE:
				wvTrace(("Blip at %x\n",ftell(fd)));
				item->no_fbse++;
				item->blip = (Blip *)realloc(item->blip,sizeof(Blip)*item->no_fbse);
				count += wvGetBlip((&item->blip[item->no_fbse-1]),fd,delay);
				wvTrace(("type is %d (number is %d\n",item->blip[item->no_fbse-1].type,item->no_fbse));
				break;
			default:
				count +=wvEatmsofbt(&amsofbh,fd);
				wvError(("Eating type 0x%x\n",amsofbh.fbt));
				break;
			}
		}
	return(count);
	}

U32 wvGetDgContainer(DgContainer *item,MSOFBH *msofbh,FILE *fd)
	{
	MSOFBH amsofbh;
	U32 count=0;

	while (count < msofbh->cbLength)
		{
		count += wvGetMSOFBH(&amsofbh,fd);
		wvTrace(("len is %x, type is %x, count %x,fullen %x\n",amsofbh.cbLength,amsofbh.fbt,count,msofbh->cbLength));
		wvTrace(("type is %x\n	",amsofbh.fbt));
		switch(amsofbh.fbt)
			{
			case msofbtDg:
				count += wvGetFDG(&item->fdg,fd);
				break;
			case msofbtSpgrContainer:
				item->no_spgrcontainer++;
				item->spgrcontainer = (SpgrContainer *)realloc(item->spgrcontainer,sizeof(SpgrContainer)*item->no_spgrcontainer);
				count += wvGetSpgrContainer(&(item->spgrcontainer[item->no_spgrcontainer-1]),&amsofbh,fd);
				break;
			default:
				count +=wvEatmsofbt(&amsofbh,fd);
				wvError(("Eating type 0x%x\n",amsofbh.fbt));
				break;
			}
		}
	return(count);
	}

FSPContainer *wvFindSPID(SpgrContainer *item,S32 spid)
	{
	U32 i;
	FSPContainer *t;
	for(i=0;i<item->no_spcontainer;i++)
		{
		if (item->spcontainer[i].fsp.spid == spid)
			{
			wvTrace(("FOUND IT\n"));
			return(&(item->spcontainer[i]));
			}
		}
	for (i=0;i<item->no_spgrcontainer;i++)
		{
		t = wvFindSPID(&(item->spgrcontainer[i]),spid);
		if (t)
			return(t);
		}
	return(NULL);
	}


void wvReleaseSpgrContainer(SpgrContainer *item)
	{
	U32 i;
	for(i=0;i<item->no_spcontainer;i++)
		wvReleaseFSPContainer(&(item->spcontainer[i]));
	wvFree(item->spcontainer);
	for(i=0;i<item->no_spgrcontainer;i++)
		wvReleaseSpgrContainer(&(item->spgrcontainer[i]));
	wvFree(item->spgrcontainer);
	}

	
U32 wvGetSpgrContainer(SpgrContainer *item,MSOFBH *msofbh,FILE *fd)
	{
	MSOFBH amsofbh;
	U32 count=0;

	item->spgrcontainer=NULL;
	item->no_spgrcontainer=0;
	item->spcontainer=NULL;
	item->no_spcontainer=0;

	while (count < msofbh->cbLength)
		{
		count += wvGetMSOFBH(&amsofbh,fd);
		wvTrace(("len is %x, type is %x, count %x,fullen %x\n",amsofbh.cbLength,amsofbh.fbt,count,msofbh->cbLength));
		wvTrace(("type is %x\n	",amsofbh.fbt));
		switch(amsofbh.fbt)
			{
			case msofbtSpContainer:
				item->no_spcontainer++;
				item->spcontainer = realloc(item->spcontainer,sizeof(FSPContainer)*item->no_spcontainer);
				count += wvGetFSPContainer(&(item->spcontainer[item->no_spcontainer-1]),&amsofbh,fd);
				break;
			case msofbtSpgrContainer:
				item->no_spgrcontainer++;
				item->spgrcontainer = realloc(item->spgrcontainer,sizeof(SpgrContainer)*item->no_spgrcontainer);
				count += wvGetSpgrContainer(&(item->spgrcontainer[item->no_spgrcontainer-1]),&amsofbh,fd);
				break;
			default:
				count +=wvEatmsofbt(&amsofbh,fd);
				wvError(("Eating type 0x%x\n",amsofbh.fbt));
				break;
			}
		}
	return(count);
	}


U32 wvGetFDG(FDG *afdg,FILE *fd)
    {
    afdg->csp = read_32ubit(fd);
    afdg->spidCur = read_32ubit(fd);
    wvTrace(("there are %d shapes here, the last is %x\n",afdg->csp,afdg->spidCur));
	return(8);
    }


void wvInitSplitMenuColors(SplitMenuColors *splitmenucolors)
	{
	splitmenucolors->noofcolors=0;
	splitmenucolors->colors=NULL;
	}

void wvReleaseSplitMenuColors(SplitMenuColors *splitmenucolors)
	{
	wvFree(splitmenucolors->colors);
	}
	
U32 wvGetSplitMenuColors(SplitMenuColors *splitmenucolors,MSOFBH *amsofbh,FILE *fd)
	{
	U32 i=0;
	splitmenucolors->noofcolors = amsofbh->cbLength/4;
	if (splitmenucolors->noofcolors)
		{
		splitmenucolors->colors = (U32 *)malloc(sizeof(U32)*splitmenucolors->noofcolors);
		for(i=0;i<splitmenucolors->noofcolors;i++)
			splitmenucolors->colors[i] = read_32ubit(fd);
		}
	return(i*4);
	}

void wvReleaseDgg(Dgg *dgg)
	{
	wvFree(dgg->fidcl);
	}

void wvInitDgg(Dgg *dgg)
	{
	dgg->fidcl=NULL;
	}

U32 wvGetDgg(Dgg *dgg,MSOFBH *amsofbh,FILE *fd)
	{
	U32 count=0;
	U32 no;
	U32 i;
	count+=wvGetFDGG(&dgg->fdgg,fd);
	if (dgg->fdgg.cidcl != 0)
		{
		wvTrace(("There are %d bytes left\n",amsofbh->cbLength-count));
		no = (amsofbh->cbLength-count)/8;
		if (no != dgg->fdgg.cidcl)
			{
			wvWarning("Must be %d, not %d as specs, test algor gives %d\n",no,dgg->fdgg.cidcl,dgg->fdgg.cspSaved-dgg->fdgg.cidcl);
			}
		if (no)
			{
			dgg->fidcl = (FIDCL *)malloc(sizeof(FIDCL) * no);
			for(i=0;i<no;i++)
				count+=wvGetFIDCL(&(dgg->fidcl[i]),fd);
			}
		}
	return(count);
	}

U32 wvGetFIDCL(FIDCL *afidcl,FILE *fd)
    {
    afidcl->dgid = read_32ubit(fd);
    afidcl->cspidCur = read_32ubit(fd);
    wvTrace(("dgid %d cspidCur %d\n",afidcl->dgid,afidcl->cspidCur));
	return(8);
    }


U32 wvGetFDGG(FDGG *afdgg,FILE *fd)
    {
    afdgg->spidMax = read_32ubit(fd);
    afdgg->cidcl = read_32ubit(fd);
    afdgg->cspSaved = read_32ubit(fd);
    afdgg->cdgSaved = read_32ubit(fd);
    wvTrace(("spidMax %d cidcl %d cspSaved %d cdgSaved %d\n",afdgg->spidMax,afdgg->cidcl,afdgg->cspSaved, afdgg->cdgSaved ));
	return(16);
    }
	

int wv0x08(Blip *blip,S32 spid,wvParseStruct *ps)
	{
	int ret=0;
	U32 i;
	escherstruct item;
	FSPContainer *answer;
	wvTrace(("spid is %x\n",spid));
	wvGetEscher(&item,ps->fib.fcDggInfo,ps->fib.lcbDggInfo,ps->tablefd,ps->mainfd);

	for(i=0;i<item.dgcontainer.no_spgrcontainer;i++)
		{
		answer = wvFindSPID(&(item.dgcontainer.spgrcontainer[i]),spid);
		if (answer)
			break;
		}

	i=0;
	if (!answer)
		wvError(("Damn found nothing\n"));
	else if (answer->fopte)
		{
		while(answer->fopte[i].pid != 0)
			{
			if (answer->fopte[i].pid == 260)
				{
				wvTrace(("has a blip reference of %d\n",answer->fopte[i].op));
				wvTrace(("no blips is %d\n",item.dggcontainer.bstorecontainer.no_fbse));
				wvTrace(("type is %d (number is %d\n",item.dggcontainer.bstorecontainer.blip[item.dggcontainer.bstorecontainer.no_fbse-1].type,item.dggcontainer.bstorecontainer.no_fbse));
				if (answer->fopte[i].op <= item.dggcontainer.bstorecontainer.no_fbse)
					{
					wvTrace(("Copied Blip\n"));
					wvCopyBlip(blip,&(item.dggcontainer.bstorecontainer.blip[answer->fopte[i].op-1]));
					wvTrace(("type is %d\n",blip->type));
					ret = 1;
					break;
					}
				}
			i++;
			}
		}
	wvTrace(("spid is %x\n",spid));
	wvReleaseEscher(&item);
	return(ret);
	}

int wv0x01(Blip *blip,FILE *fd,U32 len)
	{
	MSOFBH amsofbh;
	FSPContainer item;
	U32 count=0;
	char test[3];
	int ret=0;


	if (fd == NULL)
		return(0);

	/* 
	temp hack to test older included bmps in word 6 and 7,
	should be wrapped in a modern escher strucure before getting
	to here, and then handled as normal
	*/
	test[2] = '\0';
	test[0] = getc(fd);
	test[1] = getc(fd);
	rewind(fd);
	if (!(strcmp(test,"BM")))
		{
		blip->blip.bitmap.m_pvBits = fd;
		blip->type=msoblipDIB;
		return(1);
		}

	while (count < len)
		{
		wvTrace(("count is %x,len is %x\n",count,len));
		count += wvGetMSOFBH(&amsofbh,fd);
		wvTrace(("type is %x\n	",amsofbh.fbt));
		switch(amsofbh.fbt)
			{
			case msofbtSpContainer:
				wvTrace(("Container at %x\n",ftell(fd)));
				count += wvGetFSPContainer(&item,&amsofbh,fd);
				wvReleaseFSPContainer(&item);
				break;
			case msofbtBSE:
				wvTrace(("Blip at %x\n",ftell(fd)));
				count += wvGetBlip(blip,fd,NULL);
				ret=1;
				break;
			default:
				wvError(("Not a shape container\n"));
				return(0);
				break;
			}
		}
	return(ret);
	}

U32 wvGetFSP(FSP *fsp,FILE *fd)
	{
	fsp->spid = read_32ubit(fd);
	wvTrace(("SPID is %x\n",fsp->spid));
	fsp->grfPersistent = read_32ubit(fd);
	return(8);
	}


U32 wvGetFSPGR(FSPGR *item,FILE *fd)
	{
	/* It is supposed to be a RECT, but its only 4 long so... */
	item->rcgBounds.left = read_32ubit(fd);
	item->rcgBounds.right = read_32ubit(fd);
	item->rcgBounds.top = read_32ubit(fd);
	item->rcgBounds.bottom = read_32ubit(fd);
	return(16);
	}

void wvReleaseFSPContainer(FSPContainer *item)
	{
	wvReleaseClientTextbox(&item->clienttextbox);
	wvReleaseClientData(&item->clientdata);
	wvReleaseFOPTEArray(&item->fopte);
	}

void wvInitFSPContainer(FSPContainer *item)
	{
	wvInitFOPTEArray(&item->fopte);
	wvInitClientData(&item->clientdata);
	wvInitClientTextbox(&item->clienttextbox);
	}

U32 wvGetFSPContainer(FSPContainer *item,MSOFBH *msofbh,FILE *fd)
	{
	MSOFBH amsofbh;
	U32 count=0;
	wvInitFSPContainer(item);
	while (count < msofbh->cbLength)
		{
		count += wvGetMSOFBH(&amsofbh,fd);
		wvTrace(("len is %x, type is %x, count %x,fullen %x\n",amsofbh.cbLength,amsofbh.fbt,count,msofbh->cbLength));
		wvTrace(("type is %x\n	",amsofbh.fbt));
		switch(amsofbh.fbt)
			{
			case msofbtSpgr:
				count += wvGetFSPGR(&item->fspgr,fd);
				break;

			case msofbtSp:
				wvTrace(("Getting an fsp\n"));
				count += wvGetFSP(&item->fsp,fd);
				break;

			case msofbtOPT:
				count += wvGetFOPTEArray(&item->fopte,&amsofbh,fd);
				break;

			case msofbtAnchor:
			case msofbtChildAnchor:
			case msofbtClientAnchor:
				count += wvGetFAnchor(&item->fanchor,fd);
				break;

			case msofbtClientData:
				count +=wvGetClientData(&item->clientdata,&amsofbh,fd);
				break;
			case msofbtClientTextbox:
				count +=wvGetClientTextbox(&item->clienttextbox,&amsofbh,fd);
				break;

			case msofbtTextbox:
				wvError(("unimp\n"));
				break;
			case msofbtOleObject:
				wvError(("unimp\n"));
				break;

			case msofbtDeletedPspl:
				wvError(("unimp\n"));
				break;

			default:
				count +=wvEatmsofbt(&amsofbh,fd);
				wvError(("Eating type 0x%x\n",amsofbh.fbt));
				break;
			}
		}
	return(count);
	}

void wvInitClientData(ClientData *item)
	{
	item->data=NULL;
	}

void wvReleaseClientData(ClientData *item)
	{
	wvFree(item->data);
	}

U32 wvGetClientData(ClientData *item,MSOFBH *msofbh,FILE *fd)
	{
	U32 i;
	if (msofbh->cbLength)
		{
		item->data=(U8 *)malloc(msofbh->cbLength);
		for(i=0;i<msofbh->cbLength;i++)
			item->data[i]=getc(fd);
		}
	else
		item->data=NULL;
	return(msofbh->cbLength);
	}
	
U32 wvGetMSOFBH(MSOFBH *amsofbh,FILE *fd)
    {
    U16 dtemp=0;
    dtemp=read_16ubit(fd);

#ifdef PURIFY
    amsofbh->ver = 0;
    amsofbh->inst = 0;
#endif

    amsofbh->ver = dtemp & 0x000F;
    amsofbh->inst = dtemp >> 4;
    amsofbh->fbt = read_16ubit(fd);
    amsofbh->cbLength = read_32ubit(fd);
	return(8);
    }


U32 wvEatmsofbt(MSOFBH *amsofbh,FILE *fd)
	{
	U32 i;
	for(i=0;i<amsofbh->cbLength;i++)
		getc(fd);
	return(amsofbh->cbLength);
	}

void wvInitClientTextbox(ClientTextbox *item)
   {
   item->textid=NULL;
   }

void wvReleaseClientTextbox(ClientTextbox *item)
   {
   wvFree(item->textid);
   }

U32 wvGetClientTextbox(ClientTextbox *item,MSOFBH *amsofbh,FILE *fd)
   {
   item->textid = (U32 *)malloc(amsofbh->cbLength);
   *item->textid = read_32ubit(fd);
   return(amsofbh->cbLength);
   }


