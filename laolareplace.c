/*
The interface to wvOLEDecode now has
  int OLEdecode(char *filename, FILE **mainfd, FILE **tablefd0, FILE 
**tablefd1,FILE **data,FILE **summary)	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "wv.h"
#include "oledecod.h"

extern FILE*erroroutput;

pps_entry *stream_tree;
U32 root_stream;

void wvOLEFree(void)
	{
	/* need to free all the allocated memory */
	freeOLEtree (stream_tree);
	}


int wvOLEDecode(FILE *input, FILE **mainfd, FILE **tablefd0, FILE **tablefd1,FILE **data,FILE **summary)
	{
	int result;
	U32 stream;

	result = OLEdecode (input, &stream_tree, &root_stream, 1);
	if (result == 0)
		{
		for (stream = stream_tree[root_stream].dir; stream != 0xffffffff; stream = stream_tree[stream].next)
			{
			if (stream_tree[stream].type != 1 && stream_tree[stream].level == 1)
				{
				if (!(strcmp(stream_tree[stream].name,"WordDocument")))
					{
					*mainfd = fopen(stream_tree[stream].filename,"rb");
					}
				else if (!(strcmp(stream_tree[stream].name,"1Table")))
					{
					*tablefd1 = fopen(stream_tree[stream].filename,"rb");
					}
				else if (!(strcmp(stream_tree[stream].name,"0Table")))
					{
					*tablefd0 = fopen(stream_tree[stream].filename,"rb");
					}
				else if (!(strcmp(stream_tree[stream].name,"Data")))
					{
					*data = fopen(stream_tree[stream].filename,"rb");
					}
				else if (!(strcmp(stream_tree[stream].name,"\005SummaryInformation")))
					{
					*summary = fopen(stream_tree[stream].filename,"rb");
					}
				}
			}
		}
	switch(result)
		{
		case 5:
			wvError(("OLE file appears to be corrupt, unable to extract streams\n"));
			break;
		}

	return(result);
	}


pps_entry *myfind(char *idname,U32 start_entry)
	{
	pps_entry *ret=NULL;
	U32 entry;
	for (entry = start_entry; entry != 0xffffffffUL; entry = stream_tree[entry].next)
		{
		wvTrace(("%s %s\n",stream_tree[entry].name,idname));
		if (!(strcmp(idname,stream_tree[entry].name)))
			return(&(stream_tree[entry]));
		if (stream_tree[entry].type == 2)
			{
			wvTrace(("FILE %02lx %5ld %s\n", stream_tree[entry].ppsnumber, stream_tree[entry].size, stream_tree[entry].name));
			}
		else
			{
			wvTrace(("DIR  %02lx %s\n", stream_tree[entry].ppsnumber, stream_tree[entry].name));
			ret = myfind(idname,stream_tree[entry].dir);
			if (ret)
				return(ret);
			}
		}
	return(ret);
	}

pps_entry *wvFindObject(S32 id)
	{
	char idname[64];
	sprintf(idname,"_%ld",id);
	return(myfind(idname,0));
	}


