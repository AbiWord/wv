#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include <string.h>

/*
This file is a port of the Somar Software freeware DLL functions to get 
summary info from OLE 2.0 document files, named CPPSUM available from
the Somar Software WWW site at http://www.somar.com

The port (as always) is written by Caolan.McNamara@ul.ie and is released
under the LGPL

An example of usage of the summary stream is shown in wvSummary.c
*/


void wvGetPropHeader(PropHeader *header,wvStream *file)
	{
	int i;
	header->byteOrder=read_16ubit(file);
	header->wFormat=read_16ubit(file);
	header->osVersion1=read_16ubit(file);
	header->osVersion2=read_16ubit(file);
	for (i=0;i<16;i++)
	 	header->classId[i] = read_8ubit(file);
	header->cSections=read_32ubit(file);
	}

void wvGetFIDAndOffset(FIDAndOffset *fid,wvStream *file)
	{
	int i;
	for (i=0;i<4;i++)
		fid->dwords[i] = read_32ubit(file);
	fid->dwOffset = read_32ubit(file);
	}

void wvReleaseSummaryInfo(SummaryInfo *si)
	{
	if (si == NULL)
		return;
	if (si->aProps != NULL)
		free(si->aProps);
	if (si->data != NULL)
		free(si->data);
	}

void wvGetSummaryInfo(SummaryInfo *si,wvStream *file,U32 offset)
	{
	U32 i;

	wvStream_offset(file,offset);

	si->cBytes = read_32ubit(file);
	si->cProps = read_32ubit(file);

	si->aProps = NULL;
	si->data = NULL;

	if (si->cProps == 0)
		return;

	si->aProps = (aPro *)malloc(sizeof(aPro)* si->cProps);
	for(i=0;i<si->cProps;i++)
		{
		si->aProps[i].propID = read_32ubit(file);
		si->aProps[i].dwOffset = read_32ubit(file);
		si->aProps[i].dwOffset = si->aProps[i].dwOffset - 8 - si->cProps*8;
		}
	if (si->cBytes - 8*si->cProps > 0)
		{
		si->data = (U8 *)malloc(si->cBytes - 8*si->cProps);
		for (i=0;i<si->cBytes - 8*si->cProps;i++)
			si->data[i] = read_8ubit(file);
		}
	}

/* 
0 is success
1 is failure
-1 is wmf files, cannot be converted
*/
int wvSumInfoForceString(char *lpStr, U16 cbStr, U32 pid, SummaryInfo *si)
	{
	int len;
	PropValue Prop;
	U16 yr, mon, day, hr, min, sec;

	if (1 == wvGetProperty(&Prop, si, pid))
		return(1);

    if (Prop.vtType == VT_I4) 
		sprintf(lpStr,"%d",Prop.vtValue.vtLong);
	else if (Prop.vtType == VT_LPSTR)
		{
		len = (int) Prop.vtValue.vtBSTR.cBytes;
		if (len > cbStr) 
			len = cbStr;
		if (len <= 0) 
			*(lpStr) = '\0';
		else 
			{
			strncpy(lpStr, Prop.vtValue.vtBSTR.ch, len);
			*(lpStr + len - 1) = '\0'; /* len includes terminating null*/
			}
		}
	else if (Prop.vtType == VT_FILETIME)
		{
		wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec, pid, si);
		sprintf(lpStr,"%d/%d/%d %d:%d:%d",day,mon,yr,hr,min,sec);
		}
	else
		{
		strcpy(lpStr,"Cannot be made into a str\n");
		return(-1);
		}
	wvReleaseProperty(&Prop);
    return(0);
	}

int wvSumInfoGetString(char *lpStr, U16 cbStr, U32 pid, SummaryInfo *si)
	{
	int len;
	PropValue Prop;
	if (1 == wvGetProperty(&Prop, si, pid))
		return(1);

    if (Prop.vtType != VT_LPSTR) 
		return(1);
    len = (int) Prop.vtValue.vtBSTR.cBytes;
    if (len > cbStr) 
		len = cbStr;
    if (len <= 0) 
        *(lpStr) = '\0';
    else 
		{
        strncpy(lpStr, Prop.vtValue.vtBSTR.ch, len);
        *(lpStr + len - 1) = '\0'; /* len includes terminating null*/
    	}
	wvReleaseProperty(&Prop);
    return(0);
	}

int wvSumInfoGetPreview(char *lpStr, U16 cbStr, U32 pid, SummaryInfo *si)
	{
	int len,i;
	PropValue Prop;
	FILE *outfile = fopen("preview","w+b");
	if (1 == wvGetProperty(&Prop, si, pid))
		return(1);

    if (Prop.vtType != VT_WMF) 
		return(1);
    len = (int) Prop.vtValue.vtBSTR.cBytes;

#if 0
	i=0;
#else
	/*
	it appears that this is a wmf file once you go 16bytes in,
	it might have been a clp file, but the records seem all wrong
	for the appropiate clp header, so we'll just skip them for
	now
	*/
	i=16;
#endif
	for (;i<len;i++)
    	fputc(Prop.vtValue.vtBSTR.ch[i],outfile);
	fclose(outfile);
	sprintf(lpStr,"preview");
	wvReleaseProperty(&Prop);
    return(0);
	}

int wvSumInfoGetLong(U32 *lpLong,U32 pid, SummaryInfo *si)
	{
	int ret;
	PropValue Prop;

	if (1 == wvGetProperty(&Prop, si, pid))
        return(1);

    if (Prop.vtType == VT_I4) 
		{
    	*lpLong = Prop.vtValue.vtLong;
		ret = 0;
		}
	else
		ret = 1;
	wvReleaseProperty(&Prop);
    return(ret);
	}

int wvSumInfoGetTime(U16 *yr, U16 *mon, U16 *day, U16 *hr, U16 *min, U16 *sec, U32 pid, SummaryInfo *si)
	{
	PropValue Prop;
	U16 DosDate;
	U16 DosTime;
	
    int fBefore1980;

#define JAN1980_HIGH 0x01A8E79F
#define JAN1980_LOW  0xE1D58000

	if (1 == wvGetProperty(&Prop, si, pid))
		return(1);

    if (Prop.vtType != VT_FILETIME) 
		return(1);;

        /* 
		 If time < Jan 1, 1980, then add FILETIME of 1980/01/01 00:00:00.
         This is necessary for CoFileTimeToDosDateTime to work.
		*/
    if (Prop.vtValue.vtTime.dwHighDateTime < JAN1980_HIGH) 
		{
        Prop.vtValue.vtTime.dwLowDateTime += JAN1980_LOW;
        Prop.vtValue.vtTime.dwHighDateTime += JAN1980_HIGH;
        if (Prop.vtValue.vtTime.dwLowDateTime < JAN1980_LOW)
            Prop.vtValue.vtTime.dwHighDateTime++; /* overflow */
        fBefore1980 = 1;
    	}
    else 
		fBefore1980 = 0;

    if (!wvFileTimeToDosDateTime(&Prop.vtValue.vtTime, &DosDate, &DosTime))
        return(1);

    *day = DosDate & 0x001F;
    *mon = (DosDate & 0x01E0) >> 5;
	*yr = ((DosDate & 0xFE00) >> 9)+1980;

    *sec = (DosTime & 0x001F)  * 2;
    *min = (DosTime & 0x7E0)>>5;
    *hr  = (DosTime & 0xF800)>>11;

    if (fBefore1980) 
		{
		/* 
			Suppose edittime is actually 1 day, 3 hours.
			Then Y/M/D will be 1980/1/2 at this point.
			Which should be tranlated to 0/0/1.
			The code below also handles cases where edittime > 1 month or even 1 year.
		*/
        *yr = *yr - 1980;
        *mon = *mon - 1;
        *day = *day - 1;
        if (*mon == 0 && *yr > 0) 
			{
            (*yr)--;
            *mon = 12;
        	}
        if (*day == 0 && *mon > 0) 
			{
            (*mon)--;
            switch (*mon) 
				{
                case  1: *day = 31; break;
                case  2: *day = 28; break;
                case  3: *day = 31; break;
                case  4: *day = 30; break;
                case  5: *day = 31; break;
                case  6: *day = 30; break;
                case  7: *day = 31; break;
                case  8: *day = 31; break;
                case  9: *day = 30; break;
                case 10: *day = 31; break;
                case 11: *day = 30; break;
                case 12: *day = 31; break;
            	}
        	}
    	}
    return(0);
	}


void wvReleaseProperty(PropValue *Prop)
	{
	if (Prop != NULL)
		return;
	if (Prop->vtType == VT_LPSTR)
		if (Prop->vtValue.vtBSTR.ch)
			free(Prop->vtValue.vtBSTR.ch);
	}

int wvGetProperty(PropValue *Prop, SummaryInfo *si, U32 pid)
{
    U32 i,j;
	U8 *t;

	Prop->vtValue.vtBSTR.cBytes=0;

    if (si == NULL) 
		return(1);

    for (i = 0; i < si->cProps; i++) 
		{
        if (si->aProps[i].propID == pid) 
			{
			t = si->data+si->aProps[i].dwOffset;
			Prop->vtType = sread_32ubit(t);
			t+=4;
			switch(Prop->vtType)
				{
				case VT_LPSTR:
				case VT_WMF:
					Prop->vtValue.vtBSTR.cBytes = sread_32ubit(t);
					t+=4;
					if (Prop->vtValue.vtBSTR.cBytes == 0)
						{
						Prop->vtValue.vtBSTR.ch=NULL;
						break;
						}
					Prop->vtValue.vtBSTR.ch = (char *)malloc(Prop->vtValue.vtBSTR.cBytes);
					for (j=0;j<Prop->vtValue.vtBSTR.cBytes;j++)
						Prop->vtValue.vtBSTR.ch[j] = *t++;
					break;
				case VT_FILETIME:
					Prop->vtValue.vtTime.dwLowDateTime = sread_32ubit(t);
					t+=4;
					Prop->vtValue.vtTime.dwHighDateTime = sread_32ubit(t);
					break;
				case VT_I4:
				default:
					Prop->vtValue.vtLong = sread_32ubit(t);
					break;
				}
            return(0);
        	}
    }
    return(1);
}

int wvSumInfoOpenStream(SummaryInfo *si,wvStream *stream)
    {
    PropHeader header;
    FIDAndOffset fid;
    U32 i;
   
    wvGetPropHeader(&header,stream);
    if (header.byteOrder != 0xFFFE)
        return(1);

    if (header.wFormat != 0)
        return(1);

    for (i = 0; i < header.cSections; i++)
        {
        wvGetFIDAndOffset(&fid,stream);
        if (fid.dwords[0] == 0XF29F85E0 &&
            fid.dwords[1] == 0X10684FF9 &&
            fid.dwords[2] == 0X000891AB &&
            fid.dwords[3] == 0XD9B3272B) break;
        }
#ifdef DEBUG
    if (i >= header.cSections)
		wvTrace(("possible problem\n"));
#endif
    wvGetSummaryInfo(si,stream,fid.dwOffset);
    return(0);
    }

int wvOLESummaryStream(char *filename,wvStream **summary)
	{
	int ret;
	wvStream *mainfd,*tablefd0,*tablefd1,*data;
    ret = wvOLEDecode(filename,&mainfd,&tablefd0,&tablefd1,&data,summary);
    return(ret);
	}

