#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutFILETIME(FILETIME *ft,wvStream *fd)
         {
			 write_32ubit(fd, ft->dwLowDateTime);
			 write_32ubit(fd, ft->dwHighDateTime);
         }
