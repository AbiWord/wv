#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

U32 wvGetBlip(Blip *blip,FILE *fd)
	{
	U32 i,count;
	MSOFBH amsofbh;
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

	count += wvGetMSOFBH(&amsofbh,fd);
	wvTrace(("count is %d\n",count));
	wvError(("HERE is %x\n",ftell(fd)));
	switch(amsofbh.fbt-msofbtBlipFirst)
		{
		case msoblipWMF:
		case msoblipEMF:
		case msoblipPICT:
			count += wvGetMetafile(&blip->blip.metafile,&amsofbh,fd);	
			break;
		case msoblipJPEG:
		case msoblipPNG:
		case msoblipDIB:
			count += wvGetBitmap(&blip->blip.bitmap,&amsofbh,fd);
			break;
		}
	wvTrace(("count is %d\n",count));
	blip->type = amsofbh.fbt-msofbtBlipFirst;
	return(count);
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


U32 wvGetBitmap(BitmapBlip *abm,MSOFBH  *amsofbh,FILE *fd)
    {
	U32 i,count;
	char extra=0;
	FILE *tmp;
	wvError(("starting bitmap at %x\n",ftell(fd)));
    for (i=0;i<16;i++)
        abm->m_rgbUid[i] = getc(fd);
	count=16;

    abm->m_rgbUidPrimary[0] = 0;

    switch (amsofbh->fbt-msofbtBlipFirst)
        {
        case msoblipPNG:
			wvError(("msoblipPNG\n"));
          /*  sprintf(buffer,"%s-wv-%d.png",aimage,no++);*/
            if (amsofbh->inst ^ msobiPNG)
                extra=1;
            break;
        case msoblipJPEG:
			wvError(("msoblipJPEG\n"));
          /*  sprintf(buffer,"%s-wv-%d.jpg",aimage,no++);*/
            if (amsofbh->inst ^  msobiJFIF)
                extra=1;
            break;
        case msoblipDIB:
			wvError(("msoblipDIB\n"));
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
	tmp = fopen("/tmp/testimage","w+b");
	/*
	tmp = tmpfile();
	*/
    for (i=count;i<amsofbh->cbLength;i++)
        fputc(getc(fd),tmp);
    rewind(tmp);
	abm->m_pvBits=(void *)tmp;
	count+=i;
    return(count);
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
			wvError(("msoblipEMF\n"));
			/*
            sprintf(buffer,"%s-wv-%d.emf",aimage,no++);
			*/
            if (amsofbh->inst ^ msobiEMF)
                extra=1;
            break;
        case msoblipWMF:
			wvError(("msoblipWMF\n"));
			/*
            sprintf(buffer,"%s-wv-%d.wmf",aimage,no++);
			*/
            if (amsofbh->inst ^  msobiWMF)
                extra=1;
            break;
        case msoblipPICT:
			wvError(("msoblipPICT\n"));
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
    count +=50;


#if 0
    if (amf->m_fCompression == msocompressionDeflate)
        decompressf = setdecom();
#endif

    tmp = tmpfile();
    for (i=count;i<amsofbh->cbLength;i++)
        fputc(getc(fd),tmp);
	count+=i;

    rewind(tmp);

#if 0
    if (decompressf)
        {
        decompress(tmp,buffer,amf->m_cbSave,amf->m_cb);
    	rewind(tmp);
        }

    if (((decompressf) && (amf->m_fCompression == msocompressionDeflate)) || (amf->m_fCompression == msocompressionNone))
        if (amsofbh->fbt-msofbtBlipFirst == msoblipWMF) /*wmf only for now*/
            {
            wvTrace(("converting wmf, final length should be %d, name %s\n",amf-
            convertwmf(buffer);
            wvTrace(("converted wmf\n"));
            }

#endif
	amf->m_pvBits=(void *)tmp;
	return(count);
    }

