#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "wv.h"
/*
Released under GPL, written by Caolan.McNamara@ul.ie.

Copyright (C) 1998,1999 
	Caolan McNamara

Real Life: Caolan McNamara           *  Doing: MSc in HCI
Work: Caolan.McNamara@ul.ie          *  Phone: +353-86-8790257
URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
How would you have done it?
*/

void usage(void)
	{
	printf("Usage: wvGraphicConvert filename.in filename.out\n");
	exit(-1);
	}

/* messy extern */
extern int external_knowledge_0x01;
extern char *outputfilename;

int main(int argc,char **argv)
	{
	FILE *input,*out;
	int ret=0;
	PICF apicf;
	int count;

	/* messy structs */
	fbse_list *pic_list;
	fbse_list *tpic_list;
	fsp_list *afsp_list;
	fsp_list *tfsp_list;
	fopte_list *tfopte_list;
	
	wvInitError();

	if (argc < 3)
		usage();

	input = fopen(argv[1],"rb");
	outputfilename = argv[2];

	wvGetPICF(&apicf,input,0);

	/* 
	we are clean as far as here, after this we get messy, and need
	to be cleaned up
	*/
	count=68;
	wvTrace(("count would be %d\n",apicf.lcb));
	out = tmpfile();
    for (;count<apicf.lcb;count++)
        fputc(getc(input),out);
    rewind(out);

	wvError(("here now\n"));

    external_knowledge_0x01 = 1;    /*no delay streams in use*/
    afsp_list = wvParseEscher(&pic_list,0,(apicf.lcb)-68,out,out);
    external_knowledge_0x01 = 0;    /*reenable delay streams (not necessary)*/

	wvError(("here now\n"));

	/* mad dance */
	fclose(out);

	apicf.rgb = NULL;

    if (pic_list == NULL)
        wvError(("rats\n"));
    else
        {
        apicf.rgb = (S8 *)malloc(strlen(pic_list->filename)+1);
        strcpy(apicf.rgb,pic_list->filename);
        }

    while (afsp_list != NULL)
        {
        while (afsp_list->afopte_list != NULL)
            {
            tfopte_list = afsp_list->afopte_list;
            afsp_list->afopte_list = afsp_list->afopte_list->next;
            free(tfopte_list);
            }
        tfsp_list = afsp_list;
        afsp_list = afsp_list->next;
        free(tfsp_list);
        }

    while (pic_list != NULL)
        {
        tpic_list = pic_list;
        pic_list = pic_list->next;
        free(tpic_list);
        }

	fclose(input);
	printf("output saved in %s\n",apicf.rgb);
	return(ret);
	}
