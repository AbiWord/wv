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

#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-msole.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-docprop-vector.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static void
cb_print_property (char const *name, GsfDocProp const *prop, FILE * out)
{
  GValue const *val = gsf_doc_prop_get_val  (prop);
  char *tmp;
  
  if (gsf_doc_prop_get_link (prop) != NULL)
    fprintf (out, "\t%s LINKED TO  -> '%s'\n",
	     name, gsf_doc_prop_get_link (prop));
  else
    fprintf (out, "\t%s = ", name);
  
  if (VAL_IS_GSF_DOCPROP_VECTOR ((GValue *)val)) {
    GValueArray *va = gsf_value_get_docprop_varray (val);
    unsigned i;
    
    for (i = 0 ; i < va->n_values; i++) {
      tmp = g_strdup_value_contents (g_value_array_get_nth (va, i));
      if(i != 0)
	g_print(", ");
      fprintf (out, "(%u, %s)", i, tmp);
      g_free (tmp);
    }
  } else {
    tmp = g_strdup_value_contents (val);
    fprintf (out, "%s", tmp);
    g_free (tmp);
  }

  fprintf (out, "\n");
}

static void print_summary_stream (GsfInfile * msole,
				  const char * file_name,
				  const char * stream_name,
				  FILE * out)
{
  GsfInput * stream = gsf_infile_child_by_name (msole, stream_name);
  if (stream != NULL) {
    GsfDocMetaData *meta_data = gsf_doc_meta_data_new ();
    GError    *err = NULL;    

    err = gsf_msole_metadata_read (stream, meta_data);
    if (err != NULL) {
      g_warning ("Error getting metadata for %s->%s: %s", 
		 file_name, stream_name, err->message);
      g_error_free (err);
      err = NULL;
    } else
      gsf_doc_meta_data_foreach (meta_data,
				 (GHFunc) cb_print_property, out);
    
    g_object_unref (meta_data);
    g_object_unref (G_OBJECT (stream));
  }
}

int
main (int argc, char *argv[])
{
  int i;

  if (argc < 2)
      {
	fprintf (stderr, "Usage: wvSummary doc1 [... docN]\n");
	return 1;
      }
  
  wvInit();
  
  for (i = 1 ; i < argc ; i++)
    {      
      GsfInput  *input;
      GsfInfile *msole;
      GError    *err = NULL;

      input = gsf_input_stdio_new (argv[i], &err);

      if(!input)
	{
	  fprintf (stderr, "Problem with getting metadata from %s:%s\n",
		   argv[i], err ? err->message : "");
	  g_error_free (err);
	  continue;
	}

      input = gsf_input_uncompress (input);
      msole = gsf_infile_msole_new (input, &err);
      if(!msole)
	{
	  fprintf (stderr, "Problem with getting metadata from %s:%s\n",
		   argv[i], err ? err->message : "");
	  g_error_free (err);
	  continue;
	}

      fprintf (stdout, "Metadata for %s:\n", argv[i]);      
      print_summary_stream (msole, argv[i], "\05SummaryInformation", stdout);
      print_summary_stream (msole, argv[i], "\05DocumentSummaryInformation", stdout);
      
      g_object_unref (G_OBJECT (msole));
      g_object_unref (G_OBJECT (input));
    }

  wvShutdown();

  return 0;
}
