/*
Released under GPL, written by Caolan.McNamara@ul.ie.

Copyright (C) 1998,1999 
	Caolan McNamara

Real Life: Caolan McNamara           *  Doing: MSc in HCI
Work: Caolan.McNamara@ul.ie          *  Phone: +353-86-8790257
URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
How would you have done it?
*/

/*warning: this software requires laola's lls to be installed*/

/*

this code is often all over the shop, being more of an organic entity
that a carefully planed piece of code, so no laughing there at the back!

and send me patches by all means, but think carefully before sending me
a patch that doesnt fix a bug or add a feature but instead just changes
the style of coding, i.e no more thousand line patches that fix my 
indentation.

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "config.h"
#include "wv.h"
#include "roman.h"
#include "utf.h"
#ifdef HAVE_WMF
#include "gdwmfapi.h"

#include <sys/mman.h>

extern listentry *ourlist;
extern int list;
#endif
#if 0
#include "rc4.h"
#include "md5.h"
#endif

extern char *outputfilename;
extern char *truetypedir;
extern char *symbolurl;
extern char *wingdingurl;
extern char *patternurl;
extern FILE *outputfile;
extern FILE *erroroutput;
extern char *imagesurl;
extern char *imagesdir;
extern int nofontfaces;
extern int incenter;
extern int inrightjust;
extern char backgroundcolor[8];
extern long int cp;
extern long int realcp;

extern document_style *doc_style;

#if 0
RETSIGTYPE reaper(int);
void signal_handle (int sig, SigHandler * handler);
#endif
RETSIGTYPE timeingout(int );

#if 0
int wvDecrypt(FILE *mainfd,char *password,U32 lKey)
	{
	long end,pos;
	FILE *out;
	U32 temp;
	U8 myb;
	rc4_key key;
	int in;
	char *input;
	char name[4096];
	U8 mpassword[16];
	MD5_CTX mdContext;
    int i;

	
	/*
	memset(mpassword,0,16);

	strcpy(mpassword,"password1");
	*/

	MD5Init (&mdContext);
	MD5Update (&mdContext, "password1", strlen("password1"));
	MD5Final (&mdContext);

	prepare_key(mdContext.digest,16,&key);

	if (0 != fseek(mainfd,0L,SEEK_END))
		fprintf(stderr,"shag\n");
	end = ftell(mainfd);
	fprintf(stderr,"end is %ld\n",end);

	for (i=20;i<0x45;i++)
		{
		sprintf(name,"/tmp/outputtest%d",i);
		out = fopen(name,"w+b");
		
		fseek(mainfd,i,SEEK_SET);

		for(pos=i;pos<end;pos++)
			fputc(getc(mainfd),out);

		fseek(out,0L,SEEK_SET);

		in = fileno(out);
		input = mmap(0,end-i,PROT_READ|PROT_WRITE,MAP_SHARED,in,0);

		rc4(input,end-i,&key);

		munmap(input, end-i);
		fclose(out);
		}

	}
#endif

int wvOLESummaryStream(char *filename,FILE **summary)
	{
	int ret;
	FILE *input,*mainfd,*tablefd0,*tablefd1,*data;
	input = fopen(filename,"rb");
    ret = wvOLEDecode(input,&mainfd,&tablefd0,&tablefd1,&data,summary);
	return(ret);
	}


/*
im only going to use fontfaces if the language is Ascii based, as there
would only be grief working out fonts for eastern fonts the way the
word system is at the moment
*/
int use_fontfacequery(chp *achp)
    {
    if (nofontfaces)
        return 0;
    error(erroroutput,"fontface query %d %d %d %d\n",achp->ftcFE,achp->idctHint,achp->ftcAscii,achp->ftcOther);
    if ((achp->ftcFE == 0) && (achp->idctHint == 0) && (achp->ftcAscii == achp->ftcOther))
        return 1;
    return 0;
    }


void decode_list_nfc(int value,int no_type)
    {
    char roman[81];
    switch(no_type)
        {
        case 0:
            fprintf(outputfile,"%d",value++);
            break;
        case 1:
            fprintf(outputfile,"%s",decimalToRoman(value,roman));
            break;
        case 2:
            fprintf(outputfile,"%s",ms_strlower(decimalToRoman(value,roman)));
            break;
        case 3:
            fprintf(outputfile,"%c",64+value); /*uppercase letter*/
            break;
        case 4:
            fprintf(outputfile,"%c",96+value); /*lowercase letter*/
            break;
		/*a nine appears to exist as well to means to use symbols*/
		case 9:
			while(value > 0)
				{
				switch(value%4)
					{
					case 1:
						fprintf(outputfile,"*");
						break;
					case 2:
						fprintf(outputfile,"%s","&#8224");	/*these are windows specific, must fix*/
						break;
					case 3:
						fprintf(outputfile,"%s","&#8225");	/*these are windows specific, must fix*/
						break;
					case 0:
						fprintf(outputfile,"%c",167);
						break;
					}
				value-=4;
				}
			break;
        default:
            break;
        }
    }

int decode_symbol(U16 fontspec)
	{
	error(erroroutput,"given symbol %ld, converting to %ld\n",fontspec,fontspec-61472);
	fontspec = fontspec-61472;
	if (fontspec < 95)
		fontspec+=34;
	fprintf(outputfile,"<img src=\"%s/%d.gif\">",symbolfontdir(),fontspec);
	return(1);
	}
	
char *symbolfontdir(void)
	{
	if (symbolurl != NULL)
		return(symbolurl);
	return(SYMBOLFONTDIR);
	}

int decode_wingding(U16 fontspec)
	{
	error(erroroutput,"given wingding %ld, converting to %ld\n",fontspec,fontspec-61472);
	fontspec = fontspec-61472;
	fontspec+=34;
	if (fontspec > 86) 
		fontspec+=4;
	if (fontspec == 114)
		fontspec = 153;
	else if (fontspec == 115)
		fontspec = 160;
	else if (fontspec == 116)
		fontspec = 163;
	else if (fontspec == 117)
		fontspec = 165;
	else if (fontspec == 118)
		fontspec = 164;
	else if (fontspec == 119)
		fontspec = 168;
	else if (fontspec == 120)
		fontspec = 167;
	else if (fontspec == 121)
		fontspec = 171;
	else if (fontspec == 122)
		fontspec = 172;
	else if (fontspec == 123)
		fontspec = 170;
	else if (fontspec == 124)
		fontspec = 184;
	else if (fontspec == 125)
		fontspec = 169;
	else if (fontspec == 129)
		fontspec = 116;
	else if (fontspec == 130)
		fontspec = 117;
	else if (fontspec == 131)
		fontspec = 114;
	else if (fontspec == 132)
		fontspec = 115;
	else if ( (fontspec > 132) && (fontspec < 156) )
		fontspec-=5;
	else if ( (fontspec >= 156) && (fontspec < 164 ) )
		fontspec-=38;
	else if (fontspec == 164)
		fontspec=151;
	else if (fontspec == 165)
		fontspec=152;
	else if (fontspec == 166)
		fontspec=161;
	else if  ((fontspec > 166) && (fontspec < 173 ) )
		fontspec-=13;
	else if (fontspec == 173)
		fontspec=162;
	else if (fontspec == 174)
		fontspec=164;
	else if ((fontspec > 174) && (fontspec < 186))
		fontspec-=2;
	else if ( (fontspec >= 186) && (fontspec < 257) )
		fontspec--;
	else if (fontspec > 256)
		fontspec-=170;
	
	fprintf(outputfile,"<img src=\"%s/%d.gif\">",wingdingfontdir(),fontspec);
	return(1);
	}

char *patterndir(void)
	{
	if (patternurl != NULL)
		return(patternurl);
	return(PATTERNDIR);
	}
	
char *wingdingfontdir(void)
	{
	if (wingdingurl != NULL)
		return(wingdingurl);
	return(WINGDINGFONTDIR);
	}


char *ms_basename(char *filename)
	{
	char *temppointer=NULL;
	if ((filename != NULL) && (filename[0] != '\0'))
		{
		temppointer = filename+strlen(filename);
#ifndef WINDOWS
		while ((temppointer != filename) && (*(temppointer-1) != '/'))
			temppointer--;
#else
		while ((temppointer != filename) && (*(temppointer-1) != '\\'))
			temppointer--;
#endif
		}
	return temppointer;
	}

void outputimgsrc(char *filename, int width, int height)
	{
	char *temppointer;
	int widthadjust=50;
	int heightadjust=50;


	if (filename!= NULL)
		{
		temppointer = ms_basename(filename);

		if (width == 0)
			{
			if ((imagesurl == NULL) && (imagesdir == NULL))
				fprintf(outputfile,"<img src=\"%s\"><br>",temppointer);
			else if ( (imagesurl == NULL) && (imagesdir != NULL) )
				fprintf(outputfile,"<img src=\"%s/%s\"><br>",imagesdir,temppointer);
			else
				fprintf(outputfile,"<img src=\"%s/%s\"><br>",imagesurl,temppointer);
			}
		else
			{
			if ((doc_style != NULL) && (doc_style->vtwips != 0))
				heightadjust = doc_style->vtwips;

			if ((doc_style != NULL) && (doc_style->htwips != 0))
				widthadjust = doc_style->htwips;
			
			if ((imagesurl == NULL) && (imagesdir == NULL))
				fprintf(outputfile,"<img width=\"%d\" height=\"%d\" src=\"%s\"><br>",width/widthadjust,height/heightadjust,temppointer);
			else if ( (imagesurl == NULL) && (imagesdir != NULL) )
				fprintf(outputfile,"<img width=\"%d\" height=\"%d\" src=\"%s/%s\"><br>",width/widthadjust,height/heightadjust,imagesdir,temppointer);
			else
				fprintf(outputfile,"<img width=\"%d\" height=\"%d\" src=\"%s/%s\"><br>",width/widthadjust,height/heightadjust,imagesurl,temppointer);
			}


		error(erroroutput,"img success!\n");
		}
	else
		error(erroroutput,"filename was null!\n");
	}

char *get_image_prefix(void)
	{
	char *imageprefix;
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
	return(imageprefix);
	}

char *ms_strlower(char *in)
	{
	char *useme = in;
	while(*useme != '\0')
		{
		*useme = tolower(*useme);
		useme++;
		}
	return(in);
	}


U32 read_32ubit(FILE *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = read_16ubit(in);
	temp2 = read_16ubit(in); 
	ret = temp2;
	ret = ret << 16;
	ret += temp1;
	return(ret);
	}

U32 sread_32ubit(U8 *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = sread_16ubit(in);
	temp2 = sread_16ubit(in+2);
	ret = temp2;
	ret = ret << 16;
	ret+=temp1;
	return(ret);
	}

U32 bread_32ubit(U8 *in,U16 *pos)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = sread_16ubit(in);
	temp2 = sread_16ubit(in+2);
	ret = temp2;
	ret = ret << 16;
	ret+=temp1;
	(*pos)+=4;
	return(ret);
	}

U16 read_16ubit(FILE *in)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = getc(in);
	temp2 = getc(in);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	return(ret);
	}


U16 sread_16ubit(U8 *in)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = *in;
	temp2 = *(in+1);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	return(ret);
	}

U16 bread_16ubit(U8 *in,U16 *pos)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = *in;
	temp2 = *(in+1);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	(*pos)+=2;
	return(ret);
	}

U32 dread_32ubit(FILE *in,U8 **list)
	{
	U8 *temp;
	U32 ret;
	if (in != NULL)
		return(read_32ubit(in));
	else
		{
		temp = *list;
		(*list)+=4;
		ret = sread_32ubit(temp);
		return(ret);
		}
	}

U16 dread_16ubit(FILE *in,U8 **list)
	{
	U8 *temp;
	U16 ret;
	if (in != NULL)
		return(read_16ubit(in));
	else
		{
		temp = *list;
		(*list)+=2;
		ret = sread_16ubit(temp);
		return(ret);
		}
	}

U8 dgetc(FILE *in,U8 **list)
	{
	U8 *temp;
	if (in != NULL)
		return(getc(in));
	else
		{
		temp = *list;
		(*list)++;
		return(sgetc(temp));
		}
	}

U8 sgetc(U8 *in)
	{
	return(*in);
	}

U8 bgetc(U8 *in,U16 *pos)
	{
	(*pos)++;
	return(*in);
	}


int isodd(int i)
	{
	if ( (i/2) == ((i+1)/2) )
		return 0;
	return 1;
	}

void oprintf(int silentflag,char *fmt, ...)
	{
    va_list argp;
	if (!silentflag)
		{
		va_start(argp, fmt);
		vfprintf(outputfile, fmt, argp);
		va_end(argp);
		}
    }

void error(FILE *stream,char *fmt, ...)
	{
	#ifdef DEBUG
    va_list argp;
    fprintf(stream, "error: ");
    va_start(argp, fmt);
    vfprintf(stream, fmt, argp);
    va_end(argp);
    fprintf(stream, "\n");
	fflush(stream);
	#endif
    }

#if 0
RETSIGTYPE reaper (int ignored)
    {
#ifdef MUST_REINSTALL_SIGHANDLERS
    signal_handle (SIGCHLD, reaper);
#endif
    while (WAITPID (-1, 0, WNOHANG) > 0)
        ;

    }
#endif

RETSIGTYPE timeingout(int ignored)
    {
	fprintf(erroroutput,"\nconversion took too long, assuming something wrong and aborting\n");
	fprintf(erroroutput,"\nset timeout value higher (or dont set it) to try for longer\n");
	exit(-1);
    }

int setdecom(void)
	{
#ifdef SYSTEM_ZLIB
	return(1);
#endif
	fprintf(erroroutput,"Warning: mswordview was not compiled against zlib, so wmf files cannot be\ndecompressed\n");
	return(0);
	}

void sectionbreak(sep *asep)
	{
	/*
	i may need to put code here to add 1 to a number based
	on the even odd section breaks, or maybe that only affects
	what side of a book the page appears ?
	*/
	if (incenter)
    	fprintf(outputfile,"</center>");
    if (inrightjust)
    	fprintf(outputfile,"</div>");
	if (asep != NULL)
		{
		switch (asep->bkc)
			{
			case 0:
				fprintf(outputfile,"\n<br><img src=\"%s/sectionendcontinous.gif\"><br>\n",patterndir());
				break;
			case 1:
				fprintf(outputfile,"\n<br><img src=\"%s/sectionendcolumn.gif\"><br>\n",patterndir());
				break;
			case 2:
				fprintf(outputfile,"\n<br><img src=\"%s/sectionendnewpage.gif\"><br>\n",patterndir());
				break;
			case 3:
				fprintf(outputfile,"\n<br><img src=\"%s/sectionendeven.gif\"><br>\n",patterndir());
				break;
			case 4:
				fprintf(outputfile,"\n<br><img src=\"%s/sectionendodd.gif\"><br>\n",patterndir());
				break;
			}
		}
	else
		error(erroroutput,"agh, null sep\n");
    if (incenter)
    	fprintf(outputfile,"<center>");
    if (inrightjust)
    	fprintf(outputfile,"<div>");
	}

void pagebreak(void)
	{
	if (incenter)
    	fprintf(outputfile,"</center>");
    if (inrightjust)
    	fprintf(outputfile,"</div>");
    fprintf(outputfile,"\n<br><img src=\"%s/pagebreak.gif\"><br>\n",patterndir());
    if (incenter)
    	fprintf(outputfile,"<center>");
    if (inrightjust)
    	fprintf(outputfile,"<div>");
	}

void columnbreak(void)
	{
	if (incenter)
    	fprintf(outputfile,"</center>");
    if (inrightjust)
    	fprintf(outputfile,"</div>");
    fprintf(outputfile,"\n<br><img src=\"%s/columnbreak.gif\"><br>\n",patterndir());
    if (incenter)
    	fprintf(outputfile,"<center>");
    if (inrightjust)
    	fprintf(outputfile,"<div>");
	}

void check_auto_color(chp *achp)
    {
    /*
    if the foreground color is auto (black basically) then see if the bg is a conflicting
    color
    */
	if ( (!strcmp(backgroundcolor,"#000000")) && ( (achp->color[0] == '\0') || (!(strcmp(achp->color,"#000000"))))	)
		{
		error(erroroutput,"black on black\n");
		strcpy(achp->color,"#ffffff");
		}
	else if ( (!strcmp(backgroundcolor,"#000078")) && ( (achp->color[0] == '\0') || (!(strcmp(achp->color,"#000000"))))     )
		{
		error(erroroutput,"black on blue\n");
		strcpy(achp->color,"#ffffff");
		}
    }

/*this finds the beginning of a bookmark given a particular cp, adds
the beginning tag and returns the next bookmark cp*/
U32 decode_b_bookmark(bookmark_limits *l_bookmarks, STTBF *bookmarks)
	{
	U32 i=0;
	U16 *letter16;
	S8 *letter8;
	while (i<l_bookmarks->bookmark_b_no)
		{
		if (l_bookmarks->bookmark_b_cps[i] == realcp)
			{
			l_bookmarks->bookmark_b_cps[i] = 0xffff;		/*mark it off the list*/
			fprintf(outputfile,"<a name=\"");
			if (bookmarks->extendedflag == 0xFFFF)
				{
				letter16 = bookmarks->u16strings[i];
				while (*letter16 != '\0')
					fprintf(outputfile,"%c",*letter16++);
				}
			else
				{
				letter8 = bookmarks->s8strings[i];
				while (*letter8 != '\0')
					fprintf(outputfile,"%c",*letter8++);
				}
			fprintf(outputfile,"\">");
			if (i == l_bookmarks->bookmark_b_no-1)
				return(0xffffffff);
			else
				return(l_bookmarks->bookmark_b_cps[i+1]);
			}
		i++;
		}

	i=0;
	while (i<l_bookmarks->bookmark_b_no)
		{
		if ( (l_bookmarks->bookmark_b_cps[i] != 0xffff) && (l_bookmarks->bookmark_b_cps[i] > realcp))
			return(l_bookmarks->bookmark_b_cps[i]);
		i++;
		}

	if ((l_bookmarks->bookmark_b_no) > 0)
		return(l_bookmarks->bookmark_b_cps[0]);
		
	return(0xffffffff);
	}

U32 decode_e_bookmark(bookmark_limits *l_bookmarks)
	{
	int i=0;
	
	while (i<l_bookmarks->bookmark_e_no)
		{
		if (l_bookmarks->bookmark_e_cps[i] == realcp)
			{
			l_bookmarks->bookmark_e_cps[i] = 0xffff;		/*mark it off the list*/
			fprintf(outputfile,"</A>");
			if (i == l_bookmarks->bookmark_e_no-1)
				return(0xffffffff);
			else
				return(l_bookmarks->bookmark_e_cps[i+1]);
			}
		i++;
		}

	i=0;
	while (i<l_bookmarks->bookmark_e_no)
		{
		if ((l_bookmarks->bookmark_e_cps[i] != 0xffff) && (l_bookmarks->bookmark_e_cps[i] > realcp))
			return(l_bookmarks->bookmark_e_cps[i]);
		i++;
		}
		
	if ((l_bookmarks->bookmark_e_no) > 0)
		return(l_bookmarks->bookmark_e_cps[0]);
		
	return(0xffffffff);
	}

char *notoday(int no)
	{
	switch(no)
		{
		case 0:
			return("Sun");
			break;
		case 1:
			return("Mon");
			break;
		case 2:
			return("Tue");
			break;
		case 3:
			return("Wed");
			break;
		case 4:
			return("Thur");
			break;
		case 5:
			return("Fri");
			break;
		case 6:
			return("Sat");
			break;
		}
	return("Sat");
	}

void convertwmf(char *filename)
	{
#ifdef HAVE_WMF
	HMETAFILE file;
    int check;
    FILE *out;
    CSTRUCT rstruct;
    CSTRUCT *cstruct = &rstruct;
    GDStruct gdstruct;
	static int text;

	error(erroroutput,"1 converting wmf to gif, via libwmf\n");
#ifdef HAVE_TTF 
	if (text == 0)
		{
		ourlist=NULL;
		list=0;

		if (truetypedir != NULL)
			{
			ourlist = get_tt_list(truetypedir,&list);
			if ((ourlist == NULL) || (list == 0))
				{
				fprintf(erroroutput,"found no tt fonts in \"%s\" trying default dir\n",truetypedir);
				ourlist = NULL;
				}
			}
		if (ourlist == NULL)
			{
			ourlist = get_tt_list(TTFDIR,&list);
			if ((ourlist == NULL) || (list == 0))
				{
				fprintf(erroroutput,"found no tt fonts in \"%s\"\n",TTFDIR);
				ourlist = NULL;
				}
			}
		text++;
		}
#endif

	error(erroroutput,"2 converting wmf to gif, via libwmf\n");
	wmfinit(cstruct);

    wmffunctions = &gd_wmffunctions;

    cstruct->userdata = (void *)&gdstruct;

    check = FileIsPlaceable(filename);

    if (1 == check)
        {
        file = GetPlaceableMetaFile(filename);
        if (file != NULL)
            wmffunctions->set_pmf_size(cstruct,file);
        }
    else
        file = GetMetaFile(filename);

    if (file == NULL)
        {
        fprintf(stderr,"A problem, couldn't open <%s> as a wmf\n",filename);
		/*
        if (ourlist != NULL)
            free(ourlist);
		*/
        return;
        }

	error(erroroutput,"3 converting wmf to gif, via libwmf\n");

    cstruct->preparse = 1;
    PlayMetaFile((void *)cstruct,file);

    gdstruct.im_out = gdImageCreate(cstruct->realwidth, cstruct->realheight);
    gdImageColorResolve(gdstruct.im_out, 0xff, 0xff, 0xff);

    cstruct->preparse = 0;
    PlayMetaFile((void *)cstruct,file);

	strcat(filename,".gif");

    out = fopen(filename, "wb");
    if (out == NULL)
        {
        fprintf(stderr,"A problem, couldn't open <%s> for output\n",filename);
		/*
        if (ourlist != NULL)
            free(ourlist);
		*/
        return;
        }

	error(erroroutput,"4 converting wmf to gif, via libwmf\n");

    /* write gif */
    gdImageGif(gdstruct.im_out, out);
    fclose(out);

    gdImageDestroy(gdstruct.im_out);

	error(erroroutput,"5 converting wmf to gif, via libwmf\n");

    free(file->pmh);
    free(file->wmfheader);
    free(file);
	/*
    if (ourlist != NULL)
		{
        free(ourlist);
		ourlist = NULL;
		}
	*/
	error(erroroutput,"6 converting wmf to gif, via libwmf\n");
	
#else
	fprintf(erroroutput,"unable to convert wmf to gif as mswordview \
was compiled without libwmf support\n\
libwmf can be found at http://www.csn.ul.ie/~caolan/docs/libwmf.html\n");
#endif
	}

void windows_to_unicode(U8 in)
	{
	/*
	according to
	http://www.pemberley.com/janeinfo/latin1.html#unicode
	and backed up by
	ftp://ftp.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
	these are the characters which windows's cp1252 which word uses in 
	8 bit mode that have to changed to a different unicode number to
	work correctly
	*/

	/*
	so the plan would be to pass in 8 bit letters and if we ourselves
	are in unicode mode, then translate them into unicode and off we go,
	if we are *not* in unicode ourselves then make do with stupid 
	graphics
	*/
	U16 letter;

	switch (in)
		{
		case 130:
			letter = 8218;
			break;
		case 131:
			letter = 402;
			break;
		case 132:
			letter = 8222;
			break;
		case 133:
			letter = 8230;
			break;
		case 134:
			letter = 8224;
			break;
		case 135:
			letter = 8225;
			break;
		case 136:
			letter = 710;
			break;
		case 137:
			letter = 8240;
			break;
		case 138:
			letter = 352;
			break;
		case 139:
			letter = 8249;
			break;
		case 140:
			letter = 338;
			break;
		case 145:
			letter = 8216;
			break;
		case 146:
			letter = 8217;
			break;
		case 147:
			letter = 8220;
			break;
		case 148:
			letter = 8221;
			break;
		case 149:
			letter = 8226;
			break;
		case 150:
			letter = 8211;
			break;
		case 151:
			letter = 8212;
			break;
		case 152:
			letter = 732;
			break;
		case 153:
			letter = 8482;
			break;
		case 154:
			letter = 353;
			break;
		case 155:
			letter = 8250;
			break;
		case 156:
			letter = 339;
			break;
		case 159:
			letter = 376;
			break;
		}
	}

int add_t(int **vals,S16 *p,int plen)
    {
    static int maxno=5,cno=0;
    int i;

    if (cno == 0)
        *vals = (int *)malloc(maxno * sizeof(int *));

    if (cno+plen > maxno)
        {
        maxno+=plen;
        *vals = (int*)realloc(*vals,maxno * sizeof(int));
        }

    for(i=0;i<plen;i++)
        {
        (*vals)[cno] = p[i+1] - p[i];
        fprintf(stderr,"the val is %d\n",(*vals)[cno]);
        cno++;
        }
    return(cno);
    }


/* This function calculates the GCF using a loop with the modulus operator*/
int gcf(int high, int low)
{
  /* Initialize the variables */
  int temp, remainder = 0;
  fprintf(stderr,"high %d, low %d\n",high,low);

  /* Ensures that the higher number is in the variable high (A simple sort) */
  if(low < high)
    {
      temp = high;
      high = low;
      low = temp;
    }

  /* Loop that determines the GCF and continues until a remainder of 1 or 0 is calculated */
  do
    {
      /* The modulus operator determines the remainder if high was divided by low */
      remainder = high % low;
      if(remainder != 0)
        {
          /* A remainder of 1 indicates that the numbers are prime */
          if(remainder == 1)
            {
              printf("\nThese two numbers are prime numbers so:");
              return(1);
            }
          /* To continue, the low number becomes the high, and the remainder becmes the low */
          else
            {
              high = low;
              low = remainder;
            }

        }
    }
  while(remainder != 0);

  /* The low number is now the GCF if the numbers are not both prime */
  return(low);

}

int gcf_list(int *vals,int cno)
    {
    int i;
    i=1;
    if (cno > 1)
        {
        do
            {
            vals[i] = gcf(vals[i-1],vals[i]);
            i++;
            }
        while(i != cno);
        }
    return(vals[i-1]);
    }


