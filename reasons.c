#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

ReasonTable reasons[] = 
{
	{ "This is an unsupported (as of yet) pre word 6 doc, sorry\n", 	0 	},
	{ "This is an unsupported (as of yet) word 6 doc\n",				1	},
	{ "This is an unsupported (as of yet) word 7 doc\n",				2 	},
	{ "This in an unsupported (as of yet) encrypted document\n",		3	} 
};

const char *wvReason(int reason)
	{
	return(reasons[reason].m_name);
	}
