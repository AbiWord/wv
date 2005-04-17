/* wvWare
 * Copyright (C) Caolan McNamara, Dom Lachowicz, and others
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

#include "glib.h"
#include "ms-ole.h"
#include "ms-ole-summary.h"

/*
 * This is a simple example that take an ole file and prints some
 * information from the summaryinformation stream
 */

int
main (int argc, char *argv[])
{
    char *str = NULL;
    gboolean ret = FALSE;
    short s = 0;
    long l = 0;

    MsOle *ole = NULL;
    MsOleSummary *summary = NULL;

    if (argc < 2)
      {
	  fprintf (stderr, "Usage: wvSummary oledocument\n");
	  return (1);
      }

    ms_ole_open (&ole, argv[1]);
    if (!ole)
      {
	  fprintf (stderr, "sorry problem with getting ole streams from %s\n",
		   argv[1]);
	  return 1;
      }

    summary = ms_ole_summary_open (ole);
    if (!summary)
      {
	  fprintf (stderr, "Could not open summary stream\n");
	  return 1;
      }

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_TITLE, &ret);

    if (ret && str)
	printf ("The title is %s\n", str);
    else
	printf ("no title found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_SUBJECT, &ret);

    if (ret && str)
	printf ("The subject is %s\n", str);
    else
	printf ("no subject found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_AUTHOR, &ret);

    if (ret && str)
	printf ("The author is %s\n", str);
    else
	printf ("no author found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_KEYWORDS, &ret);

    if (ret && str)
	printf ("The keywords are %s\n", str);
    else
	printf ("no keywords found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_COMMENTS, &ret);

    if (ret && str)
	printf ("The comments are %s\n", str);
    else
	printf ("no comments found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_TEMPLATE, &ret);

    if (ret && str)
	printf ("The template was %s\n", str);
    else
	printf ("no template found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_LASTAUTHOR, &ret);

    if (ret && str)
	printf ("The last author was %s\n", str);
    else
	printf ("no last author found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_REVNUMBER, &ret);

    if (ret && str)
	printf ("The rev # was %s\n", str);
    else
	printf ("no rev no found\n");

    str = ms_ole_summary_get_string (summary, MS_OLE_SUMMARY_APPNAME, &ret);

    if (ret && str)
	printf ("The app name was %s\n", str);
    else
	printf ("no app name found\n");

    l = ms_ole_summary_get_long (summary, MS_OLE_SUMMARY_PAGECOUNT, &ret);

    if (ret)
	printf ("PageCount is %d\n", l);
    else
	printf ("no pagecount\n");

    l = ms_ole_summary_get_long (summary, MS_OLE_SUMMARY_WORDCOUNT, &ret);

    if (ret)
	printf ("WordCount is %d\n", l);
    else
	printf ("no wordcount\n");

    l = ms_ole_summary_get_long (summary, MS_OLE_SUMMARY_CHARCOUNT, &ret);

    if (ret)
	printf ("CharCount is %d\n", l);
    else
	printf ("no charcount\n");

    l = ms_ole_summary_get_long (summary, MS_OLE_SUMMARY_SECURITY, &ret);

    if (ret)
	printf ("Security is %d\n", l);
    else
	printf ("no security\n");

    s = ms_ole_summary_get_short (summary, MS_OLE_SUMMARY_CODEPAGE, &ret);
    if (ret)
	printf ("Codepage is 0x%x (%d)\n", s, s);
    else
	printf ("no codepage\n");

    ms_ole_summary_close (summary);
    ms_ole_destroy (&ole);

    return 0;
}
