#include <stdio.h>
#include <stdlib.h>
#include "wv.h"
#include <string.h>

/*modify this to handle cbSTSHI < the current size*/
void wvGetSTSHI(STSHI *item,U16 cbSTSHI,FILE *fd)
	{
	U16 temp16;
	int i;
	U16 count=0;

#ifdef PURIFY
	wvInitSTSHI(item);
#endif

    item->cstd = read_16ubit(fd);
	count+=2;
	wvTrace("there are %d std\n",item->cstd);
    item->cbSTDBaseInFile = read_16ubit(fd);
	count+=2;
	temp16 = read_16ubit(fd);
	count+=2;
    item->fStdStylenamesWritten = temp16 & 0x01;
    item->reserved = (temp16 & 0xfe) >> 1;
    item->stiMaxWhenSaved = read_16ubit(fd);
	count+=2;
    item->istdMaxFixedWhenSaved = read_16ubit(fd);
	count+=2;
    item->nVerBuiltInNamesWhenSaved = read_16ubit(fd);
	count+=2;
	for (i=0;i<3;i++)
		{
    	item->rgftcStandardChpStsh[i] = read_16ubit(fd);
		count+=2;
		}

	while(count<cbSTSHI)
		{
		getc(fd);
		count++;
		}
	}

void wvInitSTSHI(STSHI *item)
	{
	int i;

    item->cstd = 0;
    item->cbSTDBaseInFile = 0;
    item->fStdStylenamesWritten = 0;
    item->reserved = 0;
    item->stiMaxWhenSaved = 0;
    item->istdMaxFixedWhenSaved = 0;
    item->nVerBuiltInNamesWhenSaved = 0;
	for (i=0;i<3;i++)
    	item->rgftcStandardChpStsh[i] = 0;
	}

void wvReleaseSTD(STD *item)
	{
	int i;
	if (!item)
		return;
	wvFree(item->xstzName);

	for (i=0;i<item->cupx;i++)
		{
		if ((item->cupx == 1) || ((item->cupx == 2) && (i==1)))
			wvFree(item->grupxf[i].upx.chpx.grpprl);
		else if ((item->cupx == 2) && (i==0))
			wvFree(item->grupxf[i].upx.papx.grpprl);

		}

	if (item->sgc == sgcChp)
		if (item->grupe)
			wvReleaseCHPX(&(item->grupe[0].chpx));
	wvFree(item->grupxf);
	wvFree(item->grupe);
	}

void wvInitSTD(STD *item)
	{
	item->sti=0;
	item->fScratch=0;
	item->fInvalHeight=0;
	item->fHasUpe=0;
	item->fMassCopy=0;
	item->sgc=0;
	item->istdBase=0;
	item->cupx=0;
	item->istdNext=0;
	item->bchUpe=0;
	item->fAutoRedef=0;
	item->fHidden=0;
	item->reserved=0;
	item->xstzName=NULL;
	item->grupxf=NULL;
	item->grupe=NULL;
	}

void wvGetSTD(STD *item,FILE *fd)
	{
	U16 temp16;
	U16 len,i,j;
	int pos;

#ifdef PURIFY
	wvInitSTD(item);
#endif

	temp16 = read_16ubit(fd);
	item->sti = temp16 & 0x0fff;
    item->fScratch = (temp16 & 0x1000) >> 12;
    item->fInvalHeight = (temp16 & 0x2000) >> 13;
    item->fHasUpe = (temp16 & 0x4000) >> 14;
    item->fMassCopy = (temp16 & 0x8000) >> 15;
	temp16 = read_16ubit(fd);
    item->sgc = temp16 & 0x000f;
    item->istdBase = (temp16 & 0xfff0) >> 4;
	temp16 = read_16ubit(fd);
    item->cupx = temp16 & 0x000f;
    item->istdNext = (temp16 & 0xfff0) >> 4;
    item->bchUpe = read_16ubit(fd);
	temp16 = read_16ubit(fd);
    item->fAutoRedef = temp16 & 0x0001;
    item->fHidden = (temp16 & 0x0002) >> 1;
    item->reserved = (temp16 & 0xfffc) >> 2;

	pos=10;

	len = read_16ubit(fd);
	wvTrace("doing a std, str len is %d\n",len+1);
	item->xstzName = malloc((len+1) * sizeof(XCHAR));
	pos+=2;

	for(i=0;i<len+1;i++)
		{
		item->xstzName[i] = read_16ubit(fd);
		wvTrace("sample letter is %c\n",item->xstzName[i]);
		pos+=2;
		}
	wvTrace("string ended\n");


	wvTrace("cupx is %d\n",item->cupx);
	if (item->cupx == 0)
		{
		item->grupxf = NULL;
		item->grupe = NULL;
		return;
		}

	item->grupxf = (UPXF *)malloc(sizeof(UPXF) * item->cupx);
	if (item->grupxf == NULL)
		{
		wvError("Couuldn't alloc %d bytes for UPXF\n",sizeof(UPXF) * item->cupx);
		return;
		}

	item->grupe = (UPE *)malloc(sizeof(UPE) * item->cupx);
	if (item->grupe == NULL)
		{
		wvError("Couuldn't alloc %d bytes for UPE\n",sizeof(UPE) * item->cupx);
		return;
		}

	for (i=0;i<item->cupx;i++)
		{
		if ((pos+1)/2 != pos/2)
			{
			/*eat odd bytes*/
			fseek(fd,1,SEEK_CUR);
			pos++;
			}
		
		item->grupxf[i].cbUPX = read_16ubit(fd);
		wvTrace("cbUPX is %d\n",item->grupxf[i].cbUPX);
		pos+=2;

		if ((item->cupx == 1) || ((item->cupx == 2) && (i==1)))
			{
			item->grupxf[i].upx.chpx.grpprl = (U8 *)malloc(item->grupxf[i].cbUPX);
			for(j=0;j<item->grupxf[i].cbUPX;j++)
				{
				item->grupxf[i].upx.chpx.grpprl[j] = getc(fd);
				pos++;
				}
			}
		else if ((item->cupx == 2) && (i==0))
			{
			item->grupxf[i].upx.papx.istd = read_16ubit(fd);
			pos+=2;
			item->grupxf[i].upx.papx.grpprl = (U8 *)malloc(item->grupxf[i].cbUPX-2);
			for(j=0;j<item->grupxf[i].cbUPX-2;j++)
				{
				item->grupxf[i].upx.papx.grpprl[j] = getc(fd);
				pos++;
				}
			}
		else 
			{
			wvTrace("Strange cupx option\n");
			fseek(fd,item->grupxf[i].cbUPX,SEEK_CUR);
			pos+=item->grupxf[i].cbUPX;
			}
		}



	/*eat odd bytes*/
	if ((pos+1)/2 != pos/2)
		fseek(fd,1,SEEK_CUR);
	}

void wvReleaseSTSH(STSH *item)
	{
	int i;
	for (i=0;i<item->Stshi.cstd;i++)
		{
		wvTrace("Releaseing %d std\n",i);
		wvReleaseSTD(&(item->std[i]));
		}
	wvFree(item->std);
	}

void wvGetSTSH(STSH *item,U32 offset,U32 len,FILE *fd)
	{
	U16 cbStshi,cbStd,i;
	if (len == 0)
		{
		item->Stshi.cstd=0;
		item->std=NULL;
		return;
		}
	fseek(fd,offset,SEEK_SET);
	wvTrace("stsh offset len is %x %d\n",offset,len);
	cbStshi = read_16ubit(fd);
	wvGetSTSHI(&(item->Stshi),cbStshi,fd);
	
	if (item->Stshi.cstd == 0)
		{
		item->std=NULL;
		return;
		}
	item->std = (STD *)malloc(sizeof(STD) * item->Stshi.cstd);
	if (item->std == NULL)
		{
		wvError("No mem for STD list, of size %d\n",sizeof(STD) * item->Stshi.cstd);
		return;
		}
	for(i=0;i<item->Stshi.cstd;i++)
		{
		cbStd = read_16ubit(fd);
		wvTrace("index is %d,cbStd is %d, should end on %x\n",i,cbStd,ftell(fd)+cbStd);
		if (cbStd == 0)
			wvInitSTD(&(item->std[i]));
		else
			{
			wvGetSTD(&(item->std[i]),fd);

			/* 
			at this stage we should create the UPE from the available
			data
			*/
			if ((item->std[i].istdBase >= i) && (item->std[i].istdBase != istdNil))
				{
				wvError("ISTD's out of sequence, current no is %d, but base is %d\n",i,item->std[i].istdBase);
				continue;
				}
			
			switch (item->std[i].sgc)
				{
				case sgcPara:
					wvTrace("doing paragraph, len is %d\n",item->std[i].grupxf[0].cbUPX);
					wvInitPAPFromIstd(&(item->std[i].grupe[0].apap),item->std[i].istdBase,item);
					wvAddPAPXFromBucket(&(item->std[i].grupe[0].apap),&(item->std[i].grupxf[0]),item);

					wvInitCHPFromIstd(&(item->std[i].grupe[1].achp),item->std[i].istdBase,item);
					
					wvAddCHPXFromBucket(&(item->std[i].grupe[1].achp),&(item->std[i].grupxf[1]),item);

					if (item->std[i].grupe[1].achp.istd != istdNormalChar)
						{
						wvWarning("chp should have had istd set to istdNormalChar, doing it manually\n");
						item->std[i].grupe[1].achp.istd = istdNormalChar;
						}

					break;
				case sgcChp:
					/*wvTrace("sgcChp style, len is %d %d\n",item->std[i].grupxf[0].cbUPX,item->std[i].grupe[0].chpx.cbGrpprl);*/
					wvInitCHPXFromIstd(&(item->std[i].grupe[0].chpx),item->std[i].istdBase,item);
					wvMergeCHPXFromBucket(&(item->std[i].grupe[0].chpx),&(item->std[i].grupxf[0]));
					/* UPE.chpx.istd is set to the style's istd */
					item->std[i].grupe[0].chpx.istd = i;	 /*?*/
					break;
				default:
					wvWarning("New document type\n");
					break;
				}
			}
		wvTrace("actually ended on %x\n",ftell(fd));
		}
	}
















int nostyles;

extern FILE *erroroutput;
extern int noconfig_styles;


style *decode_stylesheet(FILE *tablefd,U32 stsh,U32 stshlen,config_style *Xin_style)
	{
	U16 sprm=0;
	U16 j;
	int i,upxbytes,len,m;
	U16 thingy;
	config_style *in_style = Xin_style;
	
	U16 cb;
	style *stylelist;
	U16 pos;
	int baseistd;
	int tempistd;

	long temp;

	error(erroroutput,"the new stsh is (%x), len (%x)\n",stsh,stshlen);

	fseek(tablefd,stsh,SEEK_SET);
	error(erroroutput,"not really stsh size is %ld\n",read_16ubit(tablefd));
	
	nostyles = read_16ubit(tablefd);
	error(erroroutput,"count of styles is %d\n",nostyles);
	error(erroroutput,"cbSTDBaseInFile is %ld\n",read_16ubit(tablefd));
	for (i=0;i<7;i++)
		read_16ubit(tablefd);

	stylelist = (style *) malloc(sizeof(style) * nostyles);
	if (stylelist == NULL)
		{
		wvError("arse no mem for styles !\n");
		exit(-1);
		}

	temp = ftell(tablefd);
	wvGetSTSH(&stylelist[0].stsh,stsh,stshlen,tablefd);
	fseek(tablefd,temp,SEEK_SET);

	for (m=0;m<nostyles;m++)
		{
		cb = read_16ubit(tablefd);
		error(erroroutput,"size of std %ld\n",cb);

		init_pap(&stylelist[m].thepap);

		init_chp(&stylelist[m].thechp);

		stylelist[m].name=NULL;
		stylelist[m].end=NULL;
		stylelist[m].begin=NULL;
		stylelist[m].prespace=NULL;
		stylelist[m].postspace=NULL;
		stylelist[m].bold=NULL;
		stylelist[m].italic=NULL;
		stylelist[m].font=NULL;
		stylelist[m].Default=NULL;
		
		error(erroroutput,"m val is %d\n",m);

		if (cb != 0)
			{
			pos=0;

			read_16ubit(tablefd);
			pos+=2;
			j = read_16ubit(tablefd);
			pos+=2;
			baseistd = j>>4;
			error(erroroutput,"based upon istd is %d %d",j,baseistd);
			if (baseistd < 0x0ffe)	
				{
				if (baseistd < m)
					fill_pap(stylelist,m,baseistd);
				else
					error(erroroutput,"how can i be based on a style that i havent seen yet?\n");
				}
			else
				error(erroroutput,"WARNING strange and unsupported istd base %x\n",baseistd);
			j = read_16ubit(tablefd);
			pos+=2;
			error(erroroutput,"# of upx is %d %d\n",j,j>>12); 
			upxbytes = j;
			for (i=0;i<2;i++)
				{
				read_16ubit(tablefd);
				pos+=2;
				}
			len = read_16ubit(tablefd);
			pos+=2;
			error(erroroutput,"name len is %d, m is %d\n",len,m);
			stylelist[m].name = (char *)malloc(len+1);
			error(erroroutput,"name len was %d,\n",len);

			for(i=0;i<len+1;i++)
				{
				thingy = read_16ubit(tablefd);
				pos+=2;
				error(erroroutput,"thingy is %ld (%X) (-%x-) (-%c-)\n",thingy,thingy,thingy,thingy);
				stylelist[m].name[i] = thingy;
				}
			/*
			stylelist[m].name[i] = '\0';
			*/
			error(stderr,"the style name is %s\n",stylelist[m].name);

			error(erroroutput,"pos is %x and %d",ftell(tablefd),pos);

			if ((pos+1)/2 != pos/2)
				{
				/*eat odd bytes*/
				error(erroroutput,"odd offset\n");
				fseek(tablefd,1,SEEK_CUR);
				pos++;
				}

			if (pos == cb)
				{
				error(erroroutput,"continuing\n");
				continue;
				}
			else if (pos > cb)
				{
				error(erroroutput,"rewinding & continuing\n");
				fseek(tablefd,cb-pos,SEEK_CUR);
				continue;
				}
			if ((pos+1)/2 != pos/2)
				{
				/*eat odd bytes*/
				error(erroroutput,"odd offset\n");
				fseek(tablefd,1,SEEK_CUR);
				pos++;
				}

			if (pos == cb)
				{
				error(erroroutput,"continuing\n");
				continue;
				}
			else if (pos > cb)
				{
				error(erroroutput,"rewinding & continuing\n");
				fseek(tablefd,cb-pos,SEEK_CUR);
				continue;
				}

			/*read some kind of len variable, hairy territory again*/
			upxbytes = read_16ubit(tablefd);
			pos+=2;
			error(erroroutput,"1 eat %d bytes",upxbytes);
			j=0;
			if (upxbytes > 1)
				{
				tempistd = read_16ubit(tablefd);
				pos+=2;
				j+=2;
				if (tempistd != m)
					{
					error(erroroutput,"istds mismatch is %x %x\n",m,tempistd);
					/*treat it as a sprm instead*/
					sprm = tempistd;
					}
				else
					{
					if (j < upxbytes)
						{
						sprm = read_16ubit(tablefd);
						j+=2;
						pos+=2;
						}
					}
				}

			while (j<upxbytes)
				{
				error(erroroutput," j was %d\n",j);
				pos-=j;
				decode_sprm(tablefd,sprm,&(stylelist[m].thepap),&(stylelist[m].thechp),NULL,&j,NULL,stylelist,stylelist[m].thepap.istd);
				error(erroroutput," j is %d\n",j);
				pos+=j;
				if (j>=upxbytes)
					break;
				sprm = read_16ubit(tablefd);
				j+=2;
				pos+=2;
				}

			if ((pos+1)/2 != pos/2)
				{
				/*eat odd bytes*/
				error(erroroutput,"odd offset\n");
				fseek(tablefd,1,SEEK_CUR);
				error(erroroutput,"were at %x\n",ftell(tablefd));
				pos++;
				}

			if (pos == cb)
				{
				error(erroroutput,"continuing\n");
				continue;
				}
			else if (pos > cb)
				{
				error(erroroutput,"rewinding & continuing\n");
				fseek(tablefd,cb-pos,SEEK_CUR);
				continue;
				}

			/*then this is the len of bytes of the next stuff*/
			upxbytes = read_16ubit(tablefd);
			pos+=2;
			error(erroroutput,"2 eat %d bytes",upxbytes);

			/*k this is the good stuff*/
			j=0;
			while (j < upxbytes)
				{
				sprm = read_16ubit(tablefd);
				j+=2;
				pos+=2;
				error(erroroutput," j was %d\n",j);
				pos-=j;
				decode_sprm(tablefd,sprm,&(stylelist[m].thepap),&(stylelist[m].thechp),NULL,&j,NULL,stylelist,stylelist[m].thepap.istd);
				error(erroroutput," j is %d\n",j);
				pos+=j;
				}
			
			if ((pos+1)/2 != pos/2)
				{
				/*eat odd bytes*/
				error(erroroutput,"odd offset\n");
				fseek(tablefd,1,SEEK_CUR);
				pos++;
				}

			error(erroroutput,"m val %d, ilfo is %d\n",m,stylelist[m].thepap.ilfo);
			/*
			search for the same name in the config file list, and modify the style
			in relation to that.
			*/
			in_style = Xin_style;
			for (i=0;i<noconfig_styles;i++)
				{
				if (!(strcmp( ms_strlower(stylelist[m].name),ms_strlower(in_style->name) )))
					{
					error(stderr,"found match for %s\n",stylelist[m].name);
					stylelist[m].begin = in_style->begin;
					stylelist[m].end = in_style->end;
					stylelist[m].prespace = in_style->prespace;
					stylelist[m].postspace = in_style->postspace;
					stylelist[m].Default = in_style->Default;
					stylelist[m].bold = in_style->bold;
					stylelist[m].font = in_style->font;
					stylelist[m].italic = in_style->italic;
					break;
					}
				in_style = in_style->next;
				}
			}
		}
	return(stylelist);
	}


void fill_pap(style *stylelist,int m,int b)
	{
	stylelist[m].thepap.fInTable = stylelist[b].thepap.fInTable;
	stylelist[m].thepap.fTtp= stylelist[b].thepap.fTtp;
	stylelist[m].thepap.ilvl = stylelist[b].thepap.ilvl;
	stylelist[m].thepap.ilfo = stylelist[b].thepap.ilfo;
	stylelist[m].thepap.list_data=stylelist[b].thepap.list_data;
	stylelist[m].thepap.leftmargin=stylelist[b].thepap.leftmargin;
	stylelist[m].thepap.firstline=stylelist[b].thepap.firstline;

	stylelist[m].thechp.fBold=stylelist[b].thechp.fBold;
	stylelist[m].thechp.fItalic=stylelist[b].thechp.fItalic;
	stylelist[m].thechp.fCaps =stylelist[b].thechp.fCaps;
	stylelist[m].thechp.sfxtText=stylelist[b].thechp.sfxtText;
	stylelist[m].thechp.ftcAscii=stylelist[b].thechp.ftcAscii;
	stylelist[m].thechp.ftcFE=stylelist[b].thechp.ftcFE;
	stylelist[m].thechp.ftcOther=stylelist[b].thechp.ftcOther;
	stylelist[m].thechp.fontsize=stylelist[b].thechp.fontsize;
	stylelist[m].thechp.supersubscript= stylelist[b].thechp.supersubscript;
	stylelist[m].thechp.fontcode=stylelist[b].thechp.fontcode;
	stylelist[m].thechp.fontspec=stylelist[b].thechp.fontspec;
	strcpy(stylelist[m].thechp.color,stylelist[b].thechp.color);
	stylelist[m].thechp.underline=stylelist[b].thechp.underline;
	stylelist[m].thechp.fSpec=stylelist[b].thechp.fSpec;
	stylelist[m].thechp.fObj=stylelist[b].thechp.fObj;
	stylelist[m].thechp.idctHint=stylelist[b].thechp.idctHint;
	stylelist[m].thechp.fcPic=stylelist[b].thechp.fcPic;
	stylelist[m].thechp.fData=stylelist[b].thechp.fData;
	stylelist[m].thechp.fStrike=stylelist[b].thechp.fStrike;
	stylelist[m].thechp.fDStrike=stylelist[b].thechp.fDStrike;
	}


int allowedfont(style *sheet,U16 istd)
	{
	if (sheet[istd].font != NULL)
        {
        if (!(strcmp(sheet[istd].font,"off")))
            return(0);
        else
            return(1);
        }
    else if (sheet[istd].Default != NULL)
        {
        if (!(strcmp(sheet[istd].Default,"off")))
            {
            return(0);
            error(stderr,"font face turned off\n");
            }
        else
            return(1);
        }
	return(1);
	}
