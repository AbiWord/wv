#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "wv.h"

extern FILE *erroroutput;
extern FILE *outputfile;
extern long int cp;
extern long int realcp;
extern int insuper;

ATRD *key_atrd;

void decode_annotation(textportions *portions, FILE *main)
	{
	int i,j;
	/*
	search in the first thing for the cp of this
	reference.
	*/
	i=0;
	while(i<portions->andref_no)
		{
		if (portions->andRef[i] == cp)
			{
			error(erroroutput,"found correct anno ref pos at %x, index was %d\n",portions->andRef[i],i);
			if (!insuper)
				fprintf(outputfile,"<sup>");
			fprintf(outputfile,"<a href=\"#");
			j = portions->the_atrd[i].xstUsrInitl[0];
			for (j=1;j<portions->the_atrd[i].xstUsrInitl[0]+1;j++)
                    {
                    /*warning despite the possibility of being 16 bit nos ive done this*/
                    fprintf(outputfile,"%c",portions->the_atrd[i].xstUsrInitl[j]);
                    }
			fprintf(outputfile,"%d\">[",i);
			j = portions->the_atrd[i].xstUsrInitl[0];
			key_atrd[portions->the_atrd[i].ibst].xstUsrInitl[0] = j;
			key_atrd[portions->the_atrd[i].ibst].ibst = portions->the_atrd[i].ibst;
			for (j=1;j<portions->the_atrd[i].xstUsrInitl[0]+1;j++)
                    {
                    /*warning despite the possibility of being 16 bit nos ive done this*/
                    fprintf(outputfile,"%c",portions->the_atrd[i].xstUsrInitl[j]);
					key_atrd[portions->the_atrd[i].ibst].xstUsrInitl[j] = portions->the_atrd[i].xstUsrInitl[j];
                    }
			fprintf(outputfile,"%d]</a>",i+1);

			if (!insuper)
				fprintf(outputfile,"</sup>");
			break;
			}
		i++;
		}

	/*part deux*/
	/*
	now to hold off to the very end any spray them all out, or at the end of every page.
	ill do the latter for the moment
	*/
	if ( (i < portions->andref_no ) && (i < 256) )
		portions->list_annotations[portions->list_anno_no++] = i;
	else if (i >= 256)
		fprintf(erroroutput,"oops silly programmer :-) lost an annotation\n");
	}

#if 0
Xst *extract_authors(FILE *tablefd,U32 fcGrpXstAtnOwners,U32 lcbGrpXstAtnOwners)
	{
	U16 len,i;
	U32 count=0;
	Xst *authorlist=NULL;
	Xst *current=NULL;


	if (lcbGrpXstAtnOwners > 0)
		{
		fprintf(stderr,"offset is %x\n",fcGrpXstAtnOwners);
		fseek(tablefd,fcGrpXstAtnOwners,SEEK_SET);
		authorlist = (Xst*) malloc(sizeof(Xst));

		if (authorlist == NULL)
			{
			fprintf(erroroutput,"not enough mem for annotation group\n");
			return(NULL);
			}

		authorlist->next = NULL;
		authorlist->u16string = NULL;
		authorlist->noofstrings=0;
		current = authorlist;

		while (count < lcbGrpXstAtnOwners)
			{
			len = read_16ubit(tablefd);
			fprintf(stderr,"len is %d\n",len);
			count+=2;
			current->u16string = malloc((len+1) * sizeof(U16));
			authorlist->noofstrings++;
			if (current->u16string == NULL)
				{
				fprintf(erroroutput,"not enough mem for author string of len %d\n",len);
				break;
				}
			for (i=0;i<len;i++)
				{
				current->u16string[i] = read_16ubit(tablefd);
				fprintf(stderr,"char is %c\n",current->u16string[i]);
				count+=2;
				}
			current->u16string[i] = '\0';

			if (count < lcbGrpXstAtnOwners)
				{
				current->next= (Xst*) malloc(sizeof(Xst));
				if (current->next == NULL)
					{
					fprintf(erroroutput,"not enough mem for annotation group\n");
					break;
					}
				current = current->next;
				current->next = NULL;
				current->u16string = NULL;
				}
			}
		}
	return(authorlist);
	}
#endif


/*this finds the beginning of a annotation given a particular cp, adds
the beginning tag and returns the next annotation cp*/
U32 decode_b_annotation(bookmark_limits *l_bookmarks, STTBF *bookmarks)
	{
	int i=0;
	while (i<l_bookmarks->bookmark_b_no)
		{
		if (l_bookmarks->bookmark_b_cps[i] == realcp)
			{
			l_bookmarks->bookmark_b_cps[i] = 0xffff;		/*mark it off the list*/
			fprintf(outputfile,"<a name=\"#an");
			/*
			fprintf(outputfile,"%x",sread_32ubit(bookmarks->extradata[i]+2));
			*/
			fprintf(outputfile,"%x",0x99);		/*i cant remember what i was at here, it was well wrong anyway, 
			i have to get this one right on my next pass through this part of the source*/
			fprintf(outputfile,"\">");
			fprintf(outputfile,"<img src=\"%s/doccommentb.jpg\">",patterndir());
			if (i == l_bookmarks->bookmark_b_no-1)
				return(-1);
			else
				return(l_bookmarks->bookmark_b_cps[i+1]);
			}
		i++;
		}

	i=0;
	while (i<l_bookmarks->bookmark_b_no)
		{
		if ( (l_bookmarks->bookmark_b_cps[i] != 0xffff) && (l_bookmarks->bookmark_b_cps[i] > realcp))
			return(l_bookmarks->bookmark_b_cps[i]);
		i++;
		}

	if ((l_bookmarks->bookmark_b_no) > 0)
		return(l_bookmarks->bookmark_b_cps[0]);
		
	return(-1);
	}

U32 decode_e_annotation(bookmark_limits *l_bookmarks)
	{
	int i=0;
	
	while (i<l_bookmarks->bookmark_e_no)
		{
		if (l_bookmarks->bookmark_e_cps[i] == realcp)
			{
			l_bookmarks->bookmark_e_cps[i] = 0xffff;		/*mark it off the list*/
			fprintf(outputfile,"</A>");
			fprintf(outputfile,"<img src=\"%s/doccommente.jpg\">",patterndir());
			if (i == l_bookmarks->bookmark_e_no-1)
				return(-1);
			else
				return(l_bookmarks->bookmark_e_cps[i+1]);
			}
		i++;
		}

	i=0;
	while (i<l_bookmarks->bookmark_e_no)
		{
		if ((l_bookmarks->bookmark_e_cps[i] != 0xffff) && (l_bookmarks->bookmark_e_cps[i] > realcp))
			return(l_bookmarks->bookmark_e_cps[i]);
		i++;
		}
		
	if ((l_bookmarks->bookmark_e_no) > 0)
		return(l_bookmarks->bookmark_e_cps[0]);
		
	return(-1);
	}
