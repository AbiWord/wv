/* just for now, testing purposes */
#define DEBUG 1

#include "wvexporter.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

/* document stream names */
#define DOCUMENT_STREAM "WordDocument"
#define TABLE1_STREAM    "1Table"
#define TABLE0_STREAM    "0Table"
#define DATA_STREAM     "Data"

/* normally text begins 128 bytes after the FIB */
#define TXT_OFFSET (long)128

#define ASSERT_STREAM(s) \
if(!s) { \
  wvError(("Error creating %s stream\n", #s));\
}

/**
 * Returns >= 1 if we can export to version # @v
 * Of the MSWord format or 0 if not
 *
 * Currently Supported Versions: WORD 8
 *
 * @v - version
 */
int 
wvExporter_queryVersionSupported(version v)
{
  switch((int)v){
  case WORD8:
    return 1;
  default:
    return 0;
  }
}

/**
 * Creates a MSWord exporter object, hopefully with the
 * Version @v. If version @v isn't supported, return NULL
 *
 * @filename - file on disk to create 
 * @v - version of Word Format to create. Only WORD8 is supported
 * @returns <code>NULL</code> on error, valid wvExporter on success
 */
wvExporter *
wvExporter_create_version(const char *filename, version v)
{
  wvExporter *exp = NULL;
  MsOle *ole = NULL;

  if(!wvExporter_queryVersionSupported(v))
    {
      wvError(("wvExporter: unsupported version %d", (int)v));
      return NULL;
    }

  if(filename == NULL) {
    wvError(("Error: file name can't be null\n"));
    return NULL;
  }

  /* first allocate the exporter object, initialized to 0's */  
  exp = (wvExporter *)calloc(1, sizeof(wvExporter));
  if(!exp) {
    wvError(("Error allocating memory for the exporter\n"));
    return NULL;
  }

  if(ms_ole_create((MsOle**)(&ole), filename) != MS_OLE_ERR_OK)
    {
      wvError(("Error creating OLE docfile %s\n", filename)); 
      free(ole);
      free(exp);
      return NULL;
    }

  wvTrace(("Created OLE\n"));
  exp->ole = (wvDocument *)ole;
  
  /* now to initialize the streams */
  
  exp->documentStream = wvStream_new(ole, DOCUMENT_STREAM);
  ASSERT_STREAM(exp->documentStream);
  
  exp->table1Stream   = wvStream_new(ole, TABLE1_STREAM);
  ASSERT_STREAM(exp->table1Stream);
  
  exp->table0Stream   = wvStream_new(ole, TABLE0_STREAM);
  ASSERT_STREAM(exp->table0Stream);
  
  exp->dataStream = wvStream_new(ole, DATA_STREAM);
  ASSERT_STREAM(exp->dataStream);

  exp->summaryStream  = ms_ole_summary_create(ole);
  ASSERT_STREAM(exp->summaryStream);

  wvTrace(("Created all relevant OLE streams\n"));
  
  /* initialize the FIB and put it into the document stream
   * this is fine to do, since we're going to rewind the
   * stream, and put an updated FIB in the stream on
   * wvExporter_close() anyway
   */
  wvInitFIBForExport(&(exp->fib));
  wvPutFIB(&(exp->fib), exp->documentStream);
  wvTrace(("Initial FIB inserted, proceeding with export\n"));
    
  /* normally offset 128 bytes after the FIB, but who am I to care? */
  exp->fib.fcMin = wvStream_tell(exp->documentStream);

  exp->ver = v;
  return exp;
}

/**
 * Creates a MSWord 97 (WORD8) exporter object
 *
 * @filename - file on disk to create 
 * @returns <code>NULL</code> on error, valid wvExporter on success
 */
wvExporter * 
wvExporter_create(const char *filename)
{
  return wvExporter_create_version(filename, WORD8);
}

/**
 * Closes and saves the MSWord97 document
 *
 */
void
wvExporter_close(wvExporter *exp)
{
  if(exp == NULL) {
    wvError(("Exporter can't be null\n"));
    return;
  }

  /* rewind and put the updated FIB in its proper place */
  
  /* last character's position + 1 */
  exp->fib.cbMac = wvStream_tell(exp->documentStream) + 1;
  exp->fib.ccpText = exp->fib.cbMac - exp->fib.fcMin;

  wvStream_rewind(exp->documentStream);
  wvPutFIB(&(exp->fib), exp->documentStream);
  wvTrace(("Re-inserted FIB into document\n"));

  /* close the summary stream since we're using libole2's
   * mechanism 
   */
  ms_ole_summary_close(exp->summaryStream);
  wvTrace(("Closed summary stream\n"));

  /* i hate wvOLEFree... */
  /* close all of the streams */
  wvOLEFree();

  /* close the document */
  ms_ole_destroy(&(exp->ole));
  wvTrace(("Closed all of the streams and OLE\n"));

  free(exp);
  exp = NULL;

  wvTrace(("Word97 Document Written!\n"));
}

/*********************************************************************/

/**
 * wvExporter_summaryPutString
 *
 * @exp - a valid wvExporter created by wvExporter_create()
 * @field - summary stream id key PID_XXX from "wv.h"
 * @str - string to put into the summary stream
 *
 * If field isn't a valid value or isn't valid for the given
 * type (i.e. string), this function just noops
 */
void 
wvExporter_summaryPutString(wvExporter *exp, U32 field, const char *str)
{
  MsOleSummaryPID p = 0;

  if(exp == NULL){
    wvError(("Exporter can't be null\n"));
    return;
  }
  if(str == NULL){
    wvError(("String can't be null\n"));
    return;
  }

  switch(field)
    {
      /* summary stream */
      case PID_TITLE:      p = MS_OLE_SUMMARY_TITLE;      break;
      case PID_SUBJECT:    p = MS_OLE_SUMMARY_SUBJECT;    break;
      case PID_AUTHOR:     p = MS_OLE_SUMMARY_AUTHOR;     break;
      case PID_KEYWORDS:   p = MS_OLE_SUMMARY_KEYWORDS;   break;
      case PID_COMMENTS:   p = MS_OLE_SUMMARY_COMMENTS;   break;
      case PID_TEMPLATE:   p = MS_OLE_SUMMARY_TEMPLATE;   break;
      case PID_LASTAUTHOR: p = MS_OLE_SUMMARY_LASTAUTHOR; break;
      case PID_REVNUMBER:  p = MS_OLE_SUMMARY_REVNUMBER;  break;
      case PID_APPNAME:    p = MS_OLE_SUMMARY_APPNAME;    break;
    default:
      return;
    }

  ms_ole_summary_set_string(exp->summaryStream, p, (const gchar *)str);
  wvTrace(("Summary set String %d: %s\n", p, str));
}

/**
 * wvExporter_summaryPutLong
 *
 * @exp - a valid wvExporter created by wvExporter_create()
 * @field - summary stream id key PID_XXX from "wv.h"
 * @l - long value
 *
 * If field isn't a valid value or isn't valid for the given
 * type (i.e. long), this function just noops
 */
void
wvExporter_summaryPutLong(wvExporter *exp, U32 field, U32 l)
{
  MsOleSummaryPID p = 0;

  if(exp == NULL){
    wvError(("Exporter can't be null\n"));
    return;
  }

  switch(field)
    {
      /* summary stream */
    case PID_PAGECOUNT: p = MS_OLE_SUMMARY_PAGECOUNT; break;  
    case PID_WORDCOUNT: p = MS_OLE_SUMMARY_WORDCOUNT; break;
    case PID_CHARCOUNT: p = MS_OLE_SUMMARY_CHARCOUNT; break;
    case PID_SECURITY:  p = MS_OLE_SUMMARY_SECURITY;  break;
    case PID_THUMBNAIL: p = MS_OLE_SUMMARY_THUMBNAIL; break;

    default:
      return;
    }

  ms_ole_summary_set_long(exp->summaryStream, p, (guint32)l);
  wvTrace(("Summary set long %d: %d\n", p, l));
}

/**
 * wvExporter_summaryPutTime
 *
 * @exp - a valid wvExporter created by wvExporter_create()
 * @field - summary stream id key PID_XXX from "wv.h"
 * @t - UNIX time_t value
 *
 * If @field isn't a valid value or isn't valid for the given
 * type (i.e. time_t), this function just noops
 */
void
wvExporter_summaryPutTime(wvExporter *exp, U32 field, time_t *t)
{
  MsOleSummaryPID p = 0;

  if(exp == NULL)
    return;

  switch(field)
    {
      /* summary stream only */
    case PID_TOTAL_EDITTIME: p = MS_OLE_SUMMARY_TOTAL_EDITTIME;  break;
    case PID_LASTPRINTED:    p = MS_OLE_SUMMARY_LASTPRINTED;     break;
    case PID_CREATED:        p = MS_OLE_SUMMARY_CREATED;         break;
    case PID_LASTSAVED:      p = MS_OLE_SUMMARY_LASTSAVED;       break;
    default:
      return;
    }

  /* noop now until i can convert to GTimeVals */
  wvTrace(("Summary set Time not implemented yet: %s\n", asctime(gmtime(t))));
  return;
}

/*********************************************************************/

/**
 * wvExporter_writeChars
 *
 * If you're worried, use wvExporter_writeBytes instead.
 *
 * Writes the string @chars to the Word document @exp
 *
 * @returns number of chars written
 */
size_t
wvExporter_writeChars(wvExporter *exp, const char *chars)
{
  if(exp == NULL) {
    wvError(("Exporter can't be NULL\n"));
    return 0;
  }
  if(chars == NULL) {
    wvError(("I won't write a NULL string\n"));
    return 0;
  }

  return wvExporter_writeBytes(exp, sizeof(char), strlen(chars), 
			       (void *)chars);
}

/**
 * wvExporter_writeBytes
 *
 * Should be UTF-safe
 *
 * Writes @nmemb members from the array of @bytes of 
 * in size @sz chunks to the @exp word document
 *
 * @returns number of bytes written
 */
size_t
wvExporter_writeBytes(wvExporter *exp, size_t sz, size_t nmemb,
		      const void *bytes)
{
  int nwr = 0;

  if(exp == NULL) {
    wvError(("Exporter can't be NULL\n"));
    return 0;
  }
  if(sz == 0) {
    wvError(("Attempting to write an array of zero size items? WTF?\n"));
    return 0;
  }
  if(nmemb == 0) {
    /* not so bad I guess */
    wvTrace(("Zero bytes passed to writeBytes\n"));
    return 0;
  }
  if(bytes == 0) {
    /* TODO: is this an error? */
    wvTrace(("NULL array passed to writeBytes\n"));
    return 0;
  }

  /* write the bytes and update the FIB */
  nwr = wvStream_write((void *)bytes, sz, nmemb, exp->documentStream);
  exp->fib.fcMac = wvStream_tell(exp->documentStream) + 1;

  wvTrace(("Wrote %d byte(s)\n", nwr));
  return nwr;
}
