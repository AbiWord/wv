#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "wv.h"

extern FILE *erroroutput;
extern wvStream *outputfile;

extern long int cp;
extern long int realcp;

/*
this attempts to parse the HYPERLINK field that ms uses to encode
hyperlink information
*/
U16 *decode_hyperlink(int letter, unsigned long int *swallowcp1, unsigned long int *swallowcp2, U16 **deleteme)
	{
	/* a little state machine then */
	/* 
	1) a hyperlink may or may not start with a space, 
	2) then a word HYPERLINK, though sometimes in another language i believe, 
	3) then another space, 
	4) after this usually theres a link 
	5) followed by an optional final space and an optional 0x01
	but 4 might start with a space then \l then a space then a string then a
	space.
	--
	so we need to get the full len of what we're dealing with, read it all in
	then strip out the HYPERLINK portion, scan for a " \l ", remove it, also
	remove any "0x01" and output the remainder.

	as this eats the 0x14 we have to manually do the > in the calling function.
	*/

	/*if these are zero initialize them from the input*/
	static long int from=-1;
	static long to=-1;
	static int no;
	static U16 *array;
	U16 *begin,*begin2;
	static int state;
	

	error(erroroutput,"incoming, letter is %c %x\n",letter,letter);

	if (from == -1)
		{
		from = *swallowcp1;
		to = *swallowcp2;
		array = (U16 *) malloc (sizeof(U16) * ((to+1)-from));
		if (array==NULL)
			{
			error(erroroutput,"no mem for hyperlink\n");
			exit(-1);
			}
		}

	switch(state)
		{
		case 0:
			state=1;
			break;
		case 1:	
			if (letter == 0x20)
				state=2;
			break;
		case 2:
			if (letter != 0x01)
				array[no++] = letter;
			else 
				{
				state=3;
				if (letter != 0x14)
					break;	
				else
					no--;
				}
			/*let 0x14 fall into the next state*/
		case 3:	
			if ((letter == 0x14) || (letter == 0x15))
				{
				state=0;
				no--;
				if (array[no] == ' ')
					no--;
				array[no+1] = '\0';
				/*
				error(erroroutput,"the current string is <!--%s-->\n",array);
				*/
				begin = array;
				/*analyse this string looking for a backslash l, which for now
				we will assume is always found as a space a blackslask an l and a space*/
				if (array[0] == ' ') 
					if (array[1] == '\\')
						if (array[2] == 'l')
							if (array[3] == ' ')
								{
								begin = array+3;
								if (array[4] == '\"')
									{
									*begin='\"';
									*(begin+1)='#';
									}
								else
									*begin='#';
								}
				error(erroroutput,"the current string is <!--");
				begin2 = begin;
				while (*begin2 != '\0')
					error(erroroutput,"%c",*begin2++);
				error(erroroutput,"-->\n");
					
				from=-1;
				to=-1;
				no=0;
				*deleteme = array;
				return(begin);
				}
			break;	
		}

	return(NULL);
	}

/*
this attempts to parse the REF field that ms uses to encode
crosslink information
*/
U16 *decode_crosslink(int letter,unsigned long int *swallowcp1, unsigned long int *swallowcp2)
	{
	/* a little state machine then */
	/* 
	1) a reference may or may not start with a space, 
	2) then a word PAGEREF, though ill not assume this
	3) then another space
	4) after this theres the name of the bookmark
	5) followed by a space 
	6) there may be a few flags here e.g [\h] [\p] followed by 
	a space.
	7) probably terminating 0x01
	--
	so we need to get the full len of what we're dealing with, read it all in
	then strip out the HYPERLINK portion, scan for a " \l ", remove it, also
	remove any "0x01" and output the remainder.

	as this eats the 0x14 we have to manually do the > in the calling function.
	*/

	static int no=0;
	static int state;
	
	static long int from=-1;
	static long int to=-1;
	static U16 *array;

	if (from == -1)
		{
		from = *swallowcp1;
		to = *swallowcp2;
		error(erroroutput,"a mallocing %d\n",(to+1)-from);
		array = (U16 *) malloc (sizeof(U16) * ((to+1)-from));
		if (array==NULL)
			{
			error(erroroutput,"no mem for hyperlink\n");
			exit(-1);
			}
		}

	switch(state)
		{
		case 0:
			state=1;
			break;
		case 1:
			if (letter == 0x20) 
				state=2;
			break;
		case 2:
			if ((letter != 0x20) && (letter != 0x01) && (letter != 0x14))
				array[no++] = letter;
			else 
				{
				state=0;
				from=-1;
				to=-1;
				array[no] = '\0';
				no=0;
				return(array);
				}
			break;
		}
	return(NULL);
	}
