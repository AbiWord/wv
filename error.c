#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"

FILE *wverror=NULL;
FILE *wvwarn=NULL;
FILE *wvtrace=NULL;

void wvInitError(void)
	{
	wverror=stderr;
	wvwarn=stderr;
	wvtrace=stderr;
	}

char * wvFmtMsg(char *fmt, ...)
	{
	static char mybuf[1024];
	
    va_list argp;
    va_start(argp, fmt);
    vsprintf(mybuf, fmt, argp);
    va_end(argp);

	return mybuf;
}

void wvRealError(char *file, int line, char * msg)
    {
	if (wverror == NULL)
		return;
    fprintf(wverror, "wvError: (%s:%d) %s ",file,line, msg);
    fflush(wverror);
    }

void wvWarning(char *fmt, ...)
    {
    va_list argp;
	if (wvwarn == NULL)
		return;
    fprintf(wvwarn, "wvWarning: ");
    va_start(argp, fmt);
    vfprintf(wvwarn, fmt, argp);
    va_end(argp);
    fflush(wvwarn);
    }

void wvRealTrace(char *file, int line, char * msg)
    {
      if (wvtrace == NULL)
	return;
    fprintf(wvtrace , "wvTrace: (%s:%d) %s ",file,line, msg);
    fflush(wvtrace);
    }


void wvSetErrorStream(FILE *in)
    {
	wverror = in;
    }

void wvSetWarnStream(FILE *in)
	{
	wvwarn = in;
	}

void wvSetTraceStream(FILE *in)
	{
	wvtrace = in;
	}
