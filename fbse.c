#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvCopyBlip(Blip *dest,Blip *src)
	{
	int i;
	wvCopyFBSE(&dest->fbse,&src->fbse);
	dest->type = src->type;

	if (src->name)
		{
		dest->name = (U16 *)malloc(src->fbse.cbName*sizeof(U16));
		for(i=0;i<src->fbse.cbName;i++)
			dest->name[i] = src->name[i];
		}
	else
		dest->name=NULL;
	switch(dest->type)
        {
        case msoblipWMF:
        case msoblipEMF:
        case msoblipPICT:
            wvCopyMetafile(&dest->blip.metafile,&(src->blip.metafile));
            break;
        case msoblipJPEG:
        case msoblipPNG:
        case msoblipDIB:
			wvCopyBitmap(&dest->blip.bitmap,&(src->blip.bitmap));
            break;
        }
	}

void wvReleaseBlip(Blip *blip)
	{
	wvFree(blip->name);
	}

U32 wvGetBlip(Blip *blip,FILE *fd,FILE *delay)
	{
	U32 i,count,count2;
	MSOFBH amsofbh;
	long pos=0;
	count = wvGetFBSE(&blip->fbse,fd);
	wvTrace(("count is %d\n",count));
	if (blip->fbse.cbName == 0)
		blip->name=NULL;
	else
		blip->name=(U16 *)malloc(sizeof(U16) * blip->fbse.cbName);
	for(i=0;i<blip->fbse.cbName;i++)
		blip->name[i] = read_16ubit(fd);
	count+=blip->fbse.cbName*2;
	wvTrace(("count is %d\n",count));
	wvTrace(("offset %x\n",blip->fbse.foDelay));

	if (delay)
		{
		pos = ftell(delay);
		fseek(delay,blip->fbse.foDelay,SEEK_SET);
		wvTrace(("offset %x\n",blip->fbse.foDelay));
		fd = delay;
		}
	
	count2 = wvGetMSOFBH(&amsofbh,fd);
	wvTrace(("count is %d\n",count2));
	wvTrace(("HERE is %x %x (%d)\n",ftell(fd),amsofbh.fbt,amsofbh.fbt-msofbtBlipFirst));
	wvTrace(("type is %x\n",amsofbh.fbt));
	switch(amsofbh.fbt-msofbtBlipFirst)
		{
		case msoblipWMF:
		case msoblipEMF:
		case msoblipPICT:
			count2 += wvGetMetafile(&blip->blip.metafile,&amsofbh,fd);	
			break;
		case msoblipJPEG:
		case msoblipPNG:
		case msoblipDIB:
			count2 += wvGetBitmap(&blip->blip.bitmap,&amsofbh,fd);
			break;
		}
	wvTrace(("count is %d\n",count2));
	blip->type = amsofbh.fbt-msofbtBlipFirst;

	if (delay)
		{
		fseek(delay,pos,SEEK_SET);
		return(count);
		}
	
	return(count+count2);
	}

U32 wvGetFBSE(FBSE *afbse,FILE *fd)
    {
    int i;
    afbse->btWin32 = getc(fd);
    afbse->btMacOS = getc(fd);
    for (i=0;i<16;i++)
        afbse->rgbUid[i] = getc(fd);
    afbse->tag = read_16ubit(fd);
    afbse->size = read_32ubit(fd);
    afbse->cRef = read_32ubit(fd);
    afbse->foDelay = read_32ubit(fd);
	wvTrace(("location is %x, size is %d\n",afbse->foDelay,afbse->size));
    afbse->usage = getc(fd);
    afbse->cbName = getc(fd);
	wvTrace(("name len is %d\n",afbse->cbName));
    afbse->unused2 = getc(fd);
    afbse->unused3 = getc(fd);
	return(36);
    }

void wvCopyFBSE(FBSE *dest,FBSE *src)
    {
	memcpy(dest,src,sizeof(FBSE));
    }


U32 wvGetBitmap(BitmapBlip *abm,MSOFBH  *amsofbh,FILE *fd)
    {
	U32 i,count;
	char extra=0;
	FILE *tmp;
	wvTrace(("starting bitmap at %x\n",ftell(fd)));
    for (i=0;i<16;i++)
        abm->m_rgbUid[i] = getc(fd);
	count=16;

    abm->m_rgbUidPrimary[0] = 0;

    switch (amsofbh->fbt-msofbtBlipFirst)
        {
        case msoblipPNG:
			wvTrace(("msoblipPNG\n"));
          /*  sprintf(buffer,"%s-wv-%d.png",aimage,no++);*/
            if (amsofbh->inst ^ msobiPNG)
                extra=1;
            break;
        case msoblipJPEG:
			wvTrace(("msoblipJPEG\n"));
          /*  sprintf(buffer,"%s-wv-%d.jpg",aimage,no++);*/
            if (amsofbh->inst ^  msobiJFIF)
                extra=1;
            break;
        case msoblipDIB:
			wvTrace(("msoblipDIB\n"));
          /*  sprintf(buffer,"%s-wv-%d.dib",aimage,no++); */
            if (amsofbh->inst ^ msobiDIB)
                extra=1;
            break;
        }

    if (extra)
        {
        for (i=0;i<16;i++)
            abm->m_rgbUidPrimary[i] = getc(fd);
        count+=16;
        }

    abm->m_bTag = getc(fd);
    count++;
    abm->m_pvBits=NULL;
	tmp = tmpfile();
    for (i=count;i<amsofbh->cbLength;i++)
        fputc(getc(fd),tmp);
    rewind(tmp);
	abm->m_pvBits=(void *)tmp;
	count+=i;
    return(count);
    }

void wvCopyBitmap(BitmapBlip *dest,BitmapBlip *src)
    {
	U8 i;
    for (i=0;i<16;i++)
		{
        dest->m_rgbUid[i] = src->m_rgbUid[i];
        dest->m_rgbUidPrimary[i] = src->m_rgbUidPrimary[i];
		}

    dest->m_bTag = src->m_bTag;
	dest->m_pvBits = src->m_pvBits;
    }


U32 wvGetMetafile(MetaFileBlip *amf,MSOFBH *amsofbh,FILE *fd)
    {
    char extra=0;
    U32 i,count;
	FILE *tmp;
    U8 decompressf=0;

    for (i=0;i<16;i++)
        amf->m_rgbUid[i] = getc(fd);
    count=16;

    amf->m_rgbUidPrimary[0] = 0;

    switch (amsofbh->fbt-msofbtBlipFirst)
        {
        case msoblipEMF:
			wvTrace(("msoblipEMF\n"));
			/*
            sprintf(buffer,"%s-wv-%d.emf",aimage,no++);
			*/
            if (amsofbh->inst ^ msobiEMF)
                extra=1;
            break;
        case msoblipWMF:
			wvTrace(("msoblipWMF\n"));
			/*
            sprintf(buffer,"%s-wv-%d.wmf",aimage,no++);
			*/
            if (amsofbh->inst ^  msobiWMF)
                extra=1;
            break;
        case msoblipPICT:
			wvTrace(("msoblipPICT\n"));
			/*
            sprintf(buffer,"%s-wv-%d.pict",aimage,no++);
			*/
            if (amsofbh->inst ^ msobiPICT)
                extra=1;
            break;
        }

    if (extra)
        {
        for (i=0;i<16;i++)
            amf->m_rgbUidPrimary[i] = getc(fd);
        count+=16;
        }


    amf->m_cb = read_32ubit(fd);
    amf->m_rcBounds.bottom = read_32ubit(fd);
    amf->m_rcBounds.top = read_32ubit(fd);
    amf->m_rcBounds.right = read_32ubit(fd);
    amf->m_rcBounds.left = read_32ubit(fd);
    amf->m_ptSize.y = read_32ubit(fd);
    amf->m_ptSize.x = read_32ubit(fd);
    amf->m_cbSave = read_32ubit(fd);
    amf->m_fCompression = getc(fd);
    amf->m_fFilter = getc(fd);
    amf->m_pvBits=NULL;
    count +=34;


    if (amf->m_fCompression == msocompressionDeflate)
        decompressf = setdecom();
	
    tmp = tmpfile();
	
    for (i=count;i<amsofbh->cbLength;i++)
        fputc(getc(fd),tmp);
	count+=i;

    if (decompressf)
        {
		/*
		FILE *final = fopen("/tmp/test.wmf","w+b");
		*/
		FILE *final = tmpfile();
    	rewind(tmp);
        decompress(tmp,final,amf->m_cbSave,amf->m_cb);
		fclose(tmp);
		tmp = final;
    	rewind(tmp);
        }
#if 0

    if (((decompressf) && (amf->m_fCompression == msocompressionDeflate)) || (amf->m_fCompression == msocompressionNone))
        if (amsofbh->fbt-msofbtBlipFirst == msoblipWMF) /*wmf only for now*/
            {
            wvTrace(("converting wmf, final length should be %d, name %s\n",amf-
            convertwmf(buffer);
            wvTrace(("converted wmf\n"));
            }

#endif
    rewind(tmp);
	amf->m_pvBits=(void *)tmp;
	return(count);
    }


void wvCopyMetafile(MetaFileBlip *dest,MetaFileBlip *src)
    {
	U8 i;
    for (i=0;i<16;i++)
		{
        dest->m_rgbUid[i] = src->m_rgbUid[i];
		dest->m_rgbUidPrimary[i] = src->m_rgbUidPrimary[i];
		}
    dest->m_cb = src->m_cb;
    dest->m_rcBounds.bottom = src->m_rcBounds.bottom;
    dest->m_rcBounds.top = src->m_rcBounds.top;
    dest->m_rcBounds.right = src->m_rcBounds.right;
    dest->m_rcBounds.left = src->m_rcBounds.left;
    dest->m_ptSize.y = src->m_ptSize.y;
    dest->m_ptSize.x = src->m_ptSize.x;
    dest->m_cbSave = src->m_cbSave;
    dest->m_fCompression = src->m_fCompression;
    dest->m_fFilter = src->m_fFilter;
	dest->m_pvBits=src->m_pvBits;
    }
