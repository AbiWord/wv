#ifndef WV_INTERNAL
#define WV_INTERNAL
void wvGetBRC_internal (BRC * abrc, wvStream * infd, U8 * pointer);
void wvGetBRC10_internal (BRC10 * item, wvStream * infd, U8 * pointer);
void wvGetDCS_internal (DCS * item, wvStream * fd, U8 * pointer);
void wvGetDTTM_internal (DTTM *, wvStream * fd, U8 * pointer);
void wvGetNUMRM_internal (NUMRM * item, wvStream * fd, U8 * pointer);
void wvGetSHD_internal (SHD * item, wvStream * fd, U8 * pointer);
void wvGetBRC_internal6 (BRC * abrc, wvStream * infd, U8 * pointer);
void wvGetTBD_internal (TBD * item, wvStream * fd, U8 * pointer);
void wvGetOLST_internal (wvVersion ver, OLST * item, wvStream * fd,
			 U8 * pointer);
void wvGetANLV_internal (ANLV * item, wvStream * fd, U8 * pointer);
int wvGetTC_internal (wvVersion ver, TC * tc, wvStream * infd, U8 * pointer);
void wvGetTLP_internal (TLP * item, wvStream * infd, U8 * pointer);
#endif
