#ifndef WVEXPORTER_H
#define WVEXPORTER_H

#include "wv.h"
#include "ms-ole-summary.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This is our exportation abstraction layer.  
 * Each wvDocument maps to one file,
 * which streams can be opened within.
 */
typedef MsOle wvDocument;

  typedef struct {
    /* consider all of this data to be private */    

    /* our toplevel OLE Document */
    wvDocument *ole;

    /* the OLE streams within the toplevel OLE document */
    wvStream     *documentStream;
    wvStream     *table1Stream;
    wvStream     *table0Stream;
    wvStream     *dataStream;
    MsOleSummary *summaryStream;
    MsOleSummary *docSummaryStream;

    /* more accounting structures to come later */

    FIB fib;
    wvVersion ver;
  } wvExporter;

  int wvExporter_queryVersionSupported(wvVersion v);
  wvExporter * wvExporter_create(const char *filename);
  wvExporter * wvExporter_create_version(const char *filename, wvVersion v);
  void wvExporter_close(wvExporter *exp);
  void wvExporter_summaryPutString(wvExporter *exp, U32 field, const char *str);
  void wvExporter_summaryPutLong(wvExporter *exp, U32 field, U32 l);
  void wvExporter_summaryPutTime(wvExporter *exp, U32 field, time_t *t);

  size_t wvExporter_writeChars(wvExporter *exp, const char *chars);
  size_t wvExporter_writeBytes(wvExporter *exp, size_t sz, size_t nmemb,
			       const void *bytes);

  /* */
wvStream* wvStream_new(wvDocument* ole_file, const char* name);
int wvStream_write(void *ptr, size_t size, size_t nmemb, wvStream *in);
int write_32ubit(wvStream *in, U32 out);
int write_16ubit(wvStream *in, U16 out);
int write_8ubit(wvStream *in, U8 out);

void wvInitFIBForExport(FIB *item);
void wvPutFIB(FIB *item, wvStream *fd);
void wvPutDOP(wvVersion ver, DOP *item, wvStream *fd);
void wvPutANLD(wvVersion ver,ANLD *item,wvStream *fd);
void wvPutANLV(ANLV *item,wvStream *fd);
void wvPutANLV(ANLV *item,wvStream *fd);
void wvPutASUMYI(ASUMYI *asu,wvStream *fd);
void wvPutATRD(ATRD *item, wvStream *fd);
void wvPutBKD(BKD *item,wvStream *fd);
void wvPutBKF(BKF *item,wvStream *fd);
void wvPutBKL(BKL *item,wvStream *fd);
void wvPutBRC10(BRC *item, wvStream *fd);
void wvPutBRC6(BRC *item, wvStream *fd);
void wvPutBRC(BRC *item, wvStream *fd);
void wvPutBTE(BTE *bte,wvStream *fd);
void wvPutBX(BX *item, U8 *page, U16 *pos);
void wvPutBX6(BX *item, U8 *page, U16 *pos);
void wvPutDCS(DCS *item,wvStream *fd);
void wvPutDOGRID(DOGRID *dog,wvStream *fd);
void wvPutCOPTS(COPTS *item,wvStream *fd);
void wvPutDOPTYPOGRAPHY(DOPTYPOGRAPHY *dopt,wvStream *fd);
void wvPutDTTM(DTTM *item,wvStream *fd);
void wvUnixToDTTM(struct tm * src, DTTM * dest);
void wvPutFBSE(FBSE *item, wvStream *fd);
void wvPutFDOA(FDOA *item,wvStream *fd);
void wvPutFFN6(FFN *item, wvStream *fd);
void wvPutFFN(FFN *item, wvStream *fd);
void wvPutFILETIME(FILETIME *ft,wvStream *fd);
void wvPutFLD(FLD *item,wvStream *fd);
void wvPutFONTSIGNATURE(FONTSIGNATURE *fs,wvStream *fd);
void wvPutPANOSE(PANOSE *item,wvStream *fd);
void wvPutFAnchor(FAnchor *item,wvStream *fd);
void wvPutFOPTE(FOPTE *afopte,wvStream *fd);
void wvPutFOPTEArray(FOPTE **fopte,MSOFBH *msofbh,wvStream *fd);
void wvPutFRD(FRD *item,wvStream *fd);
void wvPutFSPA(FSPA *item,wvStream *fd);
void wvPutFTXBXS(FTXBXS *item,wvStream *fd);
void wvPutChar(wvStream *fd,U8 chartype, U16 ch);
void wvPutLFO(LFO *item,wvStream *fd);
void wvPutLSPD(LSPD *item, wvStream *fd);
void wvPutLSTF(LSTF *item,wvStream *fd);
void wvPutLVL(LVL *item, wvStream *fd);
void wvPutLVLF(LVLF *item,wvStream *fd);
void wvPutNUMRM(NUMRM *item, wvStream *fd);
void wvPutOLST(OLST *item, wvStream *fd);
void wvPutPCD(PCD *item, wvStream *fd);
void wvPutPGD(PGD *item, wvStream *fd);
void wvPutPHE6(PHE *item, wvStream *fd);
void wvPutPHE(PHE *item, wvStream *fd);
void wvPutPRM(PRM *item, wvStream *fd);
void wvPutRR(RR *item, wvStream *fd);
void wvPutRS(RS *item, wvStream *fd);
void wvPutSED(SED *item, wvStream *fd);
void wvPutSEPX(wvVersion ver,SEPX *item,wvStream *fd);
void wvPutSHD(SHD *item, wvStream *fd);
void wvPutPropHeader(PropHeader *header,wvStream *file);
void wvPutFIDAndOffset(FIDAndOffset *fid,wvStream *file);
void wvPutSummaryInfo(SummaryInfo *si,wvStream *file,U32 offset);
void wvPutTBD(TBD *item, wvStream *fd);
void wvPutTC(TC *tc, wvStream *fd);
void wvPutTLP(TLP *item, wvStream *fd);
void wvPutWKB(WKB *item,wvStream *fd);
void wvPutSTSHI(STSHI *item, U16 cbSTSHI, wvStream *fd);
void wvPutSTD(STD *item, U16 len, wvStream *fd);
void wvPutSTSH(STSH *item, U16 cbStshi, wvStream *fd);

#ifdef __cplusplus
}
#endif
    
#endif /* WVEXPORTER_H */
