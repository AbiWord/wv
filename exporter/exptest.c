#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wvexporter.h"

int main(int argc, char *argv[])
{
  char *fname = argv[1];
  wvExporter *exp = NULL;

  /* just to make my life easier */
  wvInitError();

  exp = wvExporter_create(fname);

  if(exp == NULL)
    {
      printf("Exporter is null!\n");
      exit(1);
    }

  wvExporter_summaryPutString(exp, PID_TITLE,"foo bar");
  wvExporter_summaryPutString(exp, PID_AUTHOR, "dom lachowicz");
  wvExporter_summaryPutLong(exp, PID_PAGECOUNT, 1);
  wvExporter_summaryPutLong(exp, PID_WORDCOUNT, 10000);

  /* these are here as reminders that we will have to do this in the future */

  wvExporter_pushSEP(exp, NULL);
  wvExporter_pushPAP(exp, NULL);
  wvExporter_pushCHP(exp, NULL);

  wvExporter_writeChars(exp, "This is a message");
  wvExporter_writeChars(exp, " from Dom Lachowicz\n");

  wvExporter_close(exp);
  return 0;
}
