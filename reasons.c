#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

ReasonTable reasons[] = 
{
	{ "This is an unsupported (as of yet) pre word 6 doc, sorry", 	0 	},
	{ "This is an unsupported (as of yet) word 6 doc",				1	},
	{ "This is an unsupported (as of yet) word 7 doc",				2 	},
	{ "This in an unsupported (as of yet) encrypted document",		3	} 
};

const char *wvReason(int reason)
	{
	return(reasons[reason].m_name);
	}
