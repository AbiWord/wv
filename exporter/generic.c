#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutChar(wvStream *fd,U8 chartype, U16 ch)
{
     if (chartype == 1)
		write_8ubit(fd, (U8)ch);
     else
		write_16ubit(fd, ch);
}
