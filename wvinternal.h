#ifndef WV_INTERNAL
#define WV_INTERNAL
void wvGetBRC_internal(BRC *abrc,FILE *infd,U8 *pointer);
void wvGetBRC10_internal(BRC10 *item,FILE *infd,U8 *pointer);
void wvGetDCS_internal(DCS *item,FILE *fd, U8 *pointer);
void wvGetDTTM_internal(DTTM *,FILE *fd,U8 *pointer);
void wvGetNUMRM_internal(NUMRM *item,FILE *fd,U8 *pointer);
void wvGetSHD_internal(SHD *item,FILE *fd,U8 *pointer);
#endif
