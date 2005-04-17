/*
   Sample using OLEdecode
   Copyright (C) 1998  Roberto Arturo Tena Sanchez

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published  by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */
/*
  Roberto Arturo Tena Sanchez <arturo@directmail.org>
  Fco. del Paso y T. 398 C-1. Col. Jardin Balbuena.
  CP. 15900, Mexico, D.F.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <oledecod.h>


int
main (int argc, char **argv)
{
  int result;
  pps_entry *stream_tree;
  U32 root_stream;
  U32 stream;

  if (argc != 2)
  {
    fprintf (stderr, "OLEdecode demo.\nUsage: demo FILE.\n");
    return 1;
  }

  result = OLEdecode (argv[1], &stream_tree, &root_stream, 0);

  fprintf (stderr, "OLEdecode output = %d\n", result);
  perror (argv[1]);

  if (result != 0)
    return 1;

  printf ("Top level no directory streams:\n");
  /* travel through the top level no directory streams,
     just follows next field and ignore type 1 fileds */
  for (stream = stream_tree[root_stream].dir;
       stream != 0xffffffff;
       stream = stream_tree[stream].next)
    {
      if (stream_tree[stream].type != 1 && stream_tree[stream].level == 1)
	printf ("%s\n", stream_tree[stream].name);
    }

  /* need to free all the allocated memory */
  freeOLEtree (stream_tree);

  return 0;
}
