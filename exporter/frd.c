#include <stdio.h>
#include <stdlib.h>
#include "wvexporter.h"

void wvPutFRD(FRD *item,wvStream *fd)
        {
        write_16ubit(fd,(U16)item->frd);
        }
