#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"
#include "ms-ole.h"

int
wvOLEDecode (wvParseStruct * ps, char *path, wvStream ** mainfd, wvStream ** tablefd0,
	     wvStream ** tablefd1, wvStream ** data, wvStream ** summary)
{
    MsOle *ole_file = NULL;
    int result = 5;

    if (ms_ole_open (&ole_file, path) == MS_OLE_ERR_OK)
      {
	  MsOleStream **temp_stream;
	  temp_stream = (MsOleStream **) wvMalloc (sizeof (MsOleStream *));

	  ps->ole_file = ole_file;

	  wvTrace (("Opened VFS\n"));
	  if (ms_ole_stream_open
	      (temp_stream, ole_file, "/", "WordDocument",
	       'r') != MS_OLE_ERR_OK)
	    {
		*mainfd = NULL;
		wvTrace (("Opening \"WordDocument\" stream\n"));
	    }
	  else
	    {
		wvTrace (("Opened \"WordDocument\" stream\n"));
		wvStream_libole2_create (mainfd, *temp_stream);
	    }
	  if (ms_ole_stream_open (temp_stream, ole_file, "/", "1Table", 'r')
	      != MS_OLE_ERR_OK)
	    {
		*tablefd1 = NULL;
		wvTrace (("Opening \"1Table\" stream\n"));
	    }
	  else
	    {
		wvTrace (("Opened \"1Table\" stream\n"));
		wvStream_libole2_create (tablefd1, *temp_stream);
	    }
	  if (ms_ole_stream_open (temp_stream, ole_file, "/", "0Table", 'r')
	      != MS_OLE_ERR_OK)
	    {
		*tablefd0 = NULL;
		wvTrace (("Opening \"0Table\" stream\n"));
	    }
	  else
	    {
		wvTrace (("Opened \"0Table\" stream\n"));
		wvStream_libole2_create (tablefd0, *temp_stream);
	    }
	  if (ms_ole_stream_open (temp_stream, ole_file, "/", "Data", 'r') !=
	      MS_OLE_ERR_OK)
	    {
		*data = NULL;
		wvTrace (("Opening \"Data\" stream\n"));
	    }
	  else
	    {
		wvTrace (("Opened \"Data\" stream\n"));
		wvStream_libole2_create (data, *temp_stream);
	    }
	  if (ms_ole_stream_open
	      (temp_stream, ole_file, "/", "\005SummaryInformation",
	       'r') != MS_OLE_ERR_OK)
	    {
		*summary = NULL;
		wvTrace (("Opening \"\\005SummaryInformation\" stream\n"));
	    }
	  else
	    {
		wvTrace (("Opened \"\\005SummaryInformation\" stream\n"));
		wvStream_libole2_create (summary, *temp_stream);
	    }
	  wvFree (temp_stream);
	  result = 0;
      }
    return (result);
}
