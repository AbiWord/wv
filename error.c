#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "wv.h"

FILE *wverror=stderr;
FILE *wvwarn=stderr;
FILE *wvtrace=stderr;

void wvRealError(char *file, int line,char *fmt, ...)
    {
    va_list argp;
	if (wverror == NULL)
		return;
    fprintf(wverror, "wvError: (%s:%d) ",file,line);
    va_start(argp, fmt);
    vfprintf(wverror, fmt, argp);
    va_end(argp);
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

void wvRealTrace(char *file, int line,char *fmt, ...)
    {
#ifdef DEBUG
    va_list argp;
    if (wvtrace == NULL)
        return;
    fprintf(wvtrace , "wvTrace: (%s:%d) ",file,line);
    va_start(argp, fmt);
    vfprintf(wvtrace, fmt, argp);
    va_end(argp);
    fflush(wvtrace);
#endif
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
