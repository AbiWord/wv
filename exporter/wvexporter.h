#ifndef WVEXPORTER_H
#define WVEXPORTER_H

#include "../wv.h"

#ifdef __cplusplus
extern "C" {
#endif


/* This is our exportation abstraction layer.  Each wvDocument maps to one file,
 * which streams can be opened within.
 */
typedef MsOle wvDocument;

wvDocument* wvDocument_create(const char* fileName);
wvStream* wvStream_new(wvDocument* ole_file, const char* name);
int write_32ubit(wvStream *in, U32 out);
int write_16ubit(wvStream *in, U16 out);
int write_8ubit(wvStream *in, U8 out);
int wvStream_write(void *ptr, size_t size, size_t nmemb, wvStream *in);


void wvInitFIBForExport(FIB *item);
int wvPutFIB(FIB *item, wvStream *fd);



#ifdef __cplusplus
}
#endif
    
#endif /* WVEXPORTER_H */

