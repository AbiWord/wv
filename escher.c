#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "wv.h"

int HandleBitmap(char *name,BitmapBlip *bitmap);

char *wvHtmlGraphic(wvParseStruct *ps,Blip *blip)
	{
	static int i;
	char buffer[10];
	char *name;
	FILE *fd;
	char test[3];
	sprintf(buffer,"%d",i++);
	name = strdup(ps->filename);
	remove_suffix (name, ".doc");
	wvAppendStr(&name,buffer);

	/* 
	temp hack to test older included bmps in word 6 and 7,
	should be wrapped in a modern escher strucure before getting
	to here, and then handled as normal
	*/
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
		bmptojpg(name);
		wvAppendStr(&name,".jpg");
		return(name);
		}

	switch(blip->type)
        {
        case msoblipWMF:
        case msoblipEMF:
        case msoblipPICT:
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
		wvError(("%s\n",strerror(errno)));
		return(-1);
		}
	while (EOF != (c = getc((FILE *)(bitmap->m_pvBits))))
		fputc(c,fd);
	fclose(fd);
	wvError(("Name is %s\n",name));
	return(0);
	}

int wv0x01(Blip *blip,FILE *fd,U32 len)
	{
	MSOFBH amsofbh;
	FSPContainer item;
	U32 count=0;
	int ret=0;

	/* 
	temp hack to test older included bmps in word 6 and 7,
	should be wrapped in a modern escher strucure before getting
	to here, and then handled as normal
	*/
	char test[3];
	test[2] = '\0';
	test[0] = getc(fd);
	test[1] = getc(fd);
	rewind(fd);
	if (!(strcmp(test,"BM")))
		{
		blip->blip.bitmap.m_pvBits = fd;
		return(1);
		}

	while (count < len)
		{
		wvTrace(("count is %x,len is %x\n",count,len));
		count += wvGetMSOFBH(&amsofbh,fd);
		switch(amsofbh.fbt)
			{
			case msofbtSpContainer:
				wvError(("Container at %x\n",ftell(fd)));
				count += wvGetFSPContainer(&item,&amsofbh,fd);
				break;
			case msofbtBSE:
				wvError(("Blip at %x\n",ftell(fd)));
				count += wvGetBlip(blip,fd);
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

U32 wvReleaseFSPContainer(FSPContainer *item,MSOFBH *msofbh,FILE *fd)
	{
	}

U32 wvGetFSPContainer(FSPContainer *item,MSOFBH *msofbh,FILE *fd)
	{
	MSOFBH amsofbh;
	U32 count=0;

	while (count < msofbh->cbLength)
		{
		count += wvGetMSOFBH(&amsofbh,fd);
		wvError(("len is %x, type is %x, count %x,fullen %x\n",amsofbh.cbLength,amsofbh.fbt,count,msofbh->cbLength));
		switch(amsofbh.fbt)
			{
			case msofbtSpgr:
				wvError(("unimp\n"));
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
				wvError(("unimp\n"));
				break;

			case msofbtTextbox:
				wvError(("unimp\n"));
				break;
			case msofbtClientTextbox:
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
