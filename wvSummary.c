#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

/*
This is a simple example that take an ole file and prints the
title and the last modified time of the document according to
the summaryinformation stream

Caolan.McNamara@ul.ie
*/

int main(int argc,char **argv)
	{
	char szTemp[256];
	SummaryInfo si;
	int ret;
	U16 yr, mon, day, hr, min, sec;
	U32 along;
	FILE *s;

	wvInitError();

	if (argc < 2)
		{
		fprintf(stderr,"Usage: wvSummary oledocument\n");
		return(1);
		}
	
	ret = wvOLESummaryStream(argv[1],&s);
	if (ret)
		{
		fprintf(stderr,"sorry problem with getting ole streams from %s\n",argv[1]);
		return(ret);
		}

	if (s == NULL)
		{
		fprintf(stderr,"no summary stream\n");
		return(0);
		}

	ret = wvSumInfoOpenStream(&si,s);

	if (ret)
		{
		fprintf(stderr,"open stream failed\n");
		return(ret);
		}


	ret = wvSumInfoGetString(szTemp, 256, PID_TITLE, &si);

	if (!ret)
		fprintf(stderr,"The title is %s\n",szTemp);
	else
		fprintf(stderr,"no title found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_SUBJECT, &si);

	if (!ret)
		fprintf(stderr,"The subject is %s\n",szTemp);
	else
		fprintf(stderr,"no subject found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_AUTHOR, &si);

	if (!ret)
		fprintf(stderr,"The author is %s\n",szTemp);
	else
		fprintf(stderr,"no author found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_KEYWORDS, &si);

	if (!ret)
		fprintf(stderr,"The keywords are %s\n",szTemp);
	else
		fprintf(stderr,"no keywords found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_COMMENTS, &si);

	if (!ret)
		fprintf(stderr,"The comments are %s\n",szTemp);
	else
		fprintf(stderr,"no comments found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_TEMPLATE, &si);

	if (!ret)
		fprintf(stderr,"The template was %s\n",szTemp);
	else
		fprintf(stderr,"no template found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_LASTAUTHOR, &si);

	if (!ret)
		fprintf(stderr,"The last author was %s\n",szTemp);
	else
		fprintf(stderr,"no last author found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_REVNUMBER, &si);

	if (!ret)
		fprintf(stderr,"The rev no was %s\n",szTemp);
	else
		fprintf(stderr,"no rev no found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_APPNAME, &si);

	if (!ret)
		fprintf(stderr,"The app name was %s\n",szTemp);
	else
		fprintf(stderr,"no app name found\n");

	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_TOTAL_EDITTIME,&si);

	if (!ret)
	    fprintf(stderr,"Total edit time was %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		fprintf(stderr,"no total edit time found\n");


	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_LASTPRINTED,&si);

	if (!ret)
	    fprintf(stderr,"Last printed on %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		fprintf(stderr,"no last printed time found\n");

	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_CREATED,&si);

	if (!ret)
	    fprintf(stderr,"Created on %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		fprintf(stderr,"no creation time found\n");

	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_LASTSAVED,&si);

	if (!ret)
	    fprintf(stderr,"Last Saved on %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		fprintf(stderr,"no lastsaved date found\n");

	ret = wvSumInfoGetLong(&along,PID_PAGECOUNT, &si);

	if (!ret)
	    fprintf(stderr,"PageCount is %d\n",along);
	else
		fprintf(stderr,"no pagecount\n");

	ret = wvSumInfoGetLong(&along,PID_WORDCOUNT, &si);

	if (!ret)
	    fprintf(stderr,"WordCount is %d\n",along);
	else
		fprintf(stderr,"no wordcount\n");


	ret = wvSumInfoGetLong(&along,PID_CHARCOUNT, &si);

	if (!ret)
	    fprintf(stderr,"CharCount is %d\n",along);
	else
		fprintf(stderr,"no charcount\n");

	ret = wvSumInfoGetLong(&along,PID_SECURITY, &si);

	if (!ret)
	    fprintf(stderr,"Security is %d\n",along);
	else
		fprintf(stderr,"no security\n");

	ret = wvSumInfoGetPreview(szTemp, 256, PID_THUMBNAIL, &si);

	if (!ret)
	    fprintf(stderr,"preview is %s\n",szTemp);
	else
		fprintf(stderr,"no preview found\n");

	wvReleaseSummaryInfo(&si);

	fclose(s);
	return(ret);
	}
