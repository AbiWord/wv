#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "wv.h"

/* some C compilers won't let us initialize globals variables to stderr */
static FILE init_file;
FILE *wverror=&init_file;
FILE *wvwarn=&init_file;
FILE *wvtrace=&init_file;

void wvRealError(char *file, int line,char *fmt, ...)
    {
    va_list argp;
    if (wverror == NULL) return;
    if (wverror == &init_file) wverror = stderr; /* if not initialized, do it now */
    fprintf(wverror, "wvError: (%s:%d) ",file,line);
    va_start(argp, fmt);
    vfprintf(wverror, fmt, argp);
    va_end(argp);
    fflush(wverror);
    }

void wvWarning(char *fmt, ...)
    {
    va_list argp;
    if (wvwarn == NULL) return;
    if (wvwarn == &init_file) wvwarn = stderr; /* if not initialized, do it now */
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
    if (wvtrace == NULL) return;
    if (wvtrace == &init_file) wvtrace = stderr; /* if not initialized, do it now */
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
