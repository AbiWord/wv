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
		printf("Usage: wvSummary oledocument\n");
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
		printf("no summary stream\n");
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
		printf("The title is %s\n",szTemp);
	else
		printf("no title found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_SUBJECT, &si);

	if (!ret)
		printf("The subject is %s\n",szTemp);
	else
		printf("no subject found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_AUTHOR, &si);

	if (!ret)
		printf("The author is %s\n",szTemp);
	else
		printf("no author found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_KEYWORDS, &si);

	if (!ret)
		printf("The keywords are %s\n",szTemp);
	else
		printf("no keywords found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_COMMENTS, &si);

	if (!ret)
		printf("The comments are %s\n",szTemp);
	else
		printf("no comments found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_TEMPLATE, &si);

	if (!ret)
		printf("The template was %s\n",szTemp);
	else
		printf("no template found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_LASTAUTHOR, &si);

	if (!ret)
		printf("The last author was %s\n",szTemp);
	else
		printf("no last author found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_REVNUMBER, &si);

	if (!ret)
		printf("The rev no was %s\n",szTemp);
	else
		printf("no rev no found\n");

	ret = wvSumInfoGetString(szTemp, 256, PID_APPNAME, &si);

	if (!ret)
		printf("The app name was %s\n",szTemp);
	else
		printf("no app name found\n");

	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_TOTAL_EDITTIME,&si);

	if (!ret)
	    printf("Total edit time was %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		printf("no total edit time found\n");


	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_LASTPRINTED,&si);

	if (!ret)
	    printf("Last printed on %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		printf("no last printed time found\n");

	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_CREATED,&si);

	if (!ret)
	    printf("Created on %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		printf("no creation time found\n");

	ret = wvSumInfoGetTime(&yr, &mon, &day, &hr, &min, &sec,PID_LASTSAVED,&si);

	if (!ret)
	    printf("Last Saved on %d/%d/%d %d:%d:%d\n",day,mon,yr,hr,min,sec);
	else
		printf("no lastsaved date found\n");

	ret = wvSumInfoGetLong(&along,PID_PAGECOUNT, &si);

	if (!ret)
	    printf("PageCount is %d\n",along);
	else
		printf("no pagecount\n");

	ret = wvSumInfoGetLong(&along,PID_WORDCOUNT, &si);

	if (!ret)
	    printf("WordCount is %d\n",along);
	else
		printf("no wordcount\n");


	ret = wvSumInfoGetLong(&along,PID_CHARCOUNT, &si);

	if (!ret)
	    printf("CharCount is %d\n",along);
	else
		printf("no charcount\n");

	ret = wvSumInfoGetLong(&along,PID_SECURITY, &si);

	if (!ret)
	    printf("Security is %d\n",along);
	else
		printf("no security\n");

	ret = wvSumInfoGetPreview(szTemp, 256, PID_THUMBNAIL, &si);

	if (!ret)
	    printf("preview is %s\n",szTemp);
	else
		printf("no preview found\n");

	wvReleaseSummaryInfo(&si);

	fclose(s);
	return(ret);
	}
