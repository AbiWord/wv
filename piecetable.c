#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "wv.h"

extern FILE *erroroutput;

int get_piecetable(FILE *in,U32 **rgfc,U32 **avalrgfc,U16 **sprm,U32 *clxcount)
	{
	U32 lcb;
	int nopieces=0;
	int i;
	U32 aval;

	lcb = read_32ubit(in);
	(*clxcount)+=(lcb+4);
	nopieces = (lcb-4)/12;
	error(erroroutput,"lcb is %ld, theres %d pieces\n",lcb,nopieces);
	
	*rgfc = (U32 *) malloc((nopieces +1) * sizeof(U32));
	*avalrgfc = (U32 *) malloc((nopieces) * sizeof(U32));
	*sprm = (U16 *)  malloc((nopieces) * sizeof(U16));

	if ( ((*rgfc) == NULL) || ((*avalrgfc) == NULL) || ((*sprm) == NULL) )
		{
		fprintf(erroroutput,"aborting due lack to lack of memory\n");
		exit(-1);
		}

	for(i=0;i<nopieces+1;i++)
		{
		(*rgfc)[i] = read_32ubit(in);
		error(erroroutput," array entry is %x\n",(*rgfc)[i]);
		}

	for(i=0;i<nopieces;i++)
		{
		aval = read_16ubit(in);
		(*avalrgfc)[i] = read_32ubit(in);
		if ((*avalrgfc)[i] & 0x40000000UL)
			{
			error(erroroutput,"mpiece: current piece is %d file offsets of pieces are (%x)\n",i,((*avalrgfc)[i]&0xbfffffffUL)/2);
			error(erroroutput,"mpiece: end of pieces is (%x)\n",((*avalrgfc)[i]&0xbfffffffUL)/2+(*rgfc)[i+1]-(*rgfc)[i]);
			}
		else
			{
			error(erroroutput,"mpiece: current piece is %d file offsets of pieces are (%x)\n",i,(*avalrgfc)[i]);
			error(erroroutput,"mpiece: end of pieces is (%x)\n",(*avalrgfc)[i]+(*rgfc)[i+1]-(*rgfc)[i]);
			}
		(*sprm)[i] = read_16ubit(in);
		error(erroroutput,"TOUGH: the sprm referenced here is %d\n",(*sprm)[i]);
		if ((*sprm)[i] & 0x01)
			error(erroroutput,"sprm varient 2\n");
		else
			{
			error(erroroutput,"sprm varient 1, isprm is %x, val is %d\n",((*sprm)[i]&0x00fe)>>1,((*sprm)[i]&0xff00)>>8);
			}
		}
	error(erroroutput,"NOPIECES is %d\n",nopieces);
	return(nopieces);
	}

/*
query_piece takes a piecetable, and a given character position. 
nextpiece is modified to contain the file position of the nextpiece
	in the list
flag_8_16 is modified to be 1 if the piece that querycp is in is one 
	that consists of 8 bit chars, and 0 if the piece is 16 bit chars 
	(the usual). 
the index of the piece that querycp belongs to is returned.
*/
int query_piece_cp(U32 *rgfc,U32* avalrgfc,int nopieces,U32 querycp,U32 *nextpiececp,int *flag_8_16)
	{
	int i=0;

    while(i<nopieces)
        {
        if (rgfc[i+1] > querycp)
            {
			if (nextpiececp != NULL)
            	*nextpiececp = rgfc[i+1];
            if (!(avalrgfc[i] & 0x40000000UL))
				if (flag_8_16 != NULL)
                	*flag_8_16=1;
            break;
            }
        i++;
        }
	return(i);
	}


/*
does the same as query_piece_cp, only it seeks fd to the beginning of the piece that
querycp is in, and returns the position that we've seeked to.
*/
int query_piece_cp_seek(U32 *rgfc,U32* avalrgfc,int nopieces,long int querycp,U32 *nextpiececp,int *flag_8_16,FILE *fd)
	{
	int index = query_piece_cp(rgfc,avalrgfc,nopieces,querycp,nextpiececp,flag_8_16);
	int thisfc;

	if (avalrgfc[index] & 0x40000000UL)
		thisfc = (avalrgfc[index]&0xbfffffffUL)/2;
	else
		thisfc = avalrgfc[index];
	fseek(fd,thisfc,SEEK_SET);

	return(thisfc);
	}
