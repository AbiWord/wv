///////////////////////////////////////////////////////////////////////////
//
// DLL functions to get summary info from OLE 2.0 document files.
//
//    Copyright © 1994-1995 Somar Software, All Rights Reserved
//    Send problem reports and comments to 72202.2574@compuserve.com
//    Visit the Somar Software WWW site at http://www.somar.com
//            
// Change Log:
//    V1.5 95/03/25 Handle PID_TOTAL_EDITTIME (time < 1980-01-01)
//	  V1.4 95/02/06 Add 32 bit version of DLL, documentation changes
//    V1.3 94/06/06 Change validity checking, because Excel 5.0 files
//                     are slightly invalid
//    V1.2 94/03/01 Change STGM_SHARE_DENY_WRITE to STGM_SHARE_DENY_NONE, so
//                     info can be obtained for files currently open in WinWord.
//    V1.1 94/01/22 Add wInitStatus parameter to SumInfoInit and SumInfoUninit,
//                     to account for case where CoInitialize already called for  
//                     application.
//                  Ensure SumInfoGetString returns zero terminated string.
//                  Fix minor typos in documentation.
//    V1.0 94/01/20 Initial version.
//                 
// Example of usage:
//     WORD   wInitStatus;
//     HANDLE hSumInfo;
//     char   szTemp[256];
//     LPSTR  szFilePath;
//     WORD   yr, mon, day, hr, min, sec;
//     if (wInitStatus = SumInfoInit()) {
//        ... loop to process files ...
//           szFilePath = ...
//           if (hSumInfo = SumInfoOpenFile(szFilePath)) {
//              if (SumInfoGetString(hSumInfo, PID_TITLE, szTemp, 256)) {
//                 ... do something with szTemp ...
//              }
//              if (SumInfoGetTime(hSumInfo, PID_LASTSAVED, 
//                                 &yr, &mon, &day, &hr, &min, &sec)) {
//                 ... do something with time ...
//              }
//              ...
//              SumInfoCloseFile(hSumInfo);
//           }
//        }
//        SumInfoUninit(wInitStatus);
//     }
//
// Reasons for failure:
//     SumInfoInit:     out of memory
//     SumInfoOpenFile: out of memory
//                      file not found
//                      file is not an OLE 2.0 structured storage file
//                      file does not contain OLE 2.0 summary info
//                      OLE 2.0 summary info is incorrectly formatted
//     SumInfoGet...  : specified property type is not available
//     
///////////////////////////////////////////////////////////////////////////

#define STRICT
#include <windows.h>
#include <memory.h>
#ifdef WIN32
#include <objbase.h>
#else
#include <ole2ver.h>      
#include <storage.h>
#include <compobj.h>
#endif
#pragma hdrstop

// a temporary function was unreferenced and then removed by optimization
// causing warning 4505
#pragma warning(disable:4505) 

///////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define DLLEXPORT __stdcall
#else
#define DLLEXPORT FAR PASCAL __export
#define VT_I4           3
#define VT_LPSTR        30
#define VT_FILETIME     64
#endif

typedef struct _PROPVALUE {
    DWORD vtType;
    union {
        FILETIME      vtTime;  
        LONG          vtLong;
        struct {
            DWORD cBytes;
            char  ch[1];
        } vtBSTR;
    } vtValue;
} PROPVALUE;
typedef PROPVALUE FAR * LPPROPVALUE;

typedef struct _SUMMARYINFO {
    DWORD   cBytes;
    DWORD   cProps;
    struct {
        DWORD propID;
        DWORD dwOffset;
    } aProps[1];
} SUMMARYINFO;
typedef SUMMARYINFO FAR * LPSUMINFO;

LPPROPVALUE FindProperty(HANDLE hSumInfo, DWORD pid);

///////////////////////////////////////////////////////////////////////////
extern "C" WORD DLLEXPORT SumInfoInit()
{
#ifndef WIN32
    DWORD dwVer = CoBuildVersion();
    if (rmm != HIWORD(dwVer)) return 0;
#endif

    HRESULT hr = CoInitialize(NULL);
    SCODE scode = GetScode(hr);
    if (scode == S_OK) return 1;
    if (scode == S_FALSE) return 2;
    return 0;
}

///////////////////////////////////////////////////////////////////////////
extern "C" void DLLEXPORT SumInfoUninit(WORD wInitStatus)
{
#ifndef WIN32
    if (wInitStatus == 1)
#endif
        CoUninitialize();
}   

///////////////////////////////////////////////////////////////////////////
extern "C" HANDLE DLLEXPORT SumInfoOpenFile(LPSTR szPath)
{
    BOOL            bResult = FALSE;
    LPSUMINFO       lpSumInfo;
    DWORD           i;       
    DWORD           dwBytesInSection;
    HRESULT         hr;
    ULONG           ulBytesRead;                      
    LARGE_INTEGER   li;                 
    LPSTREAM        pIStream;
    LPSTORAGE       pIStorage;
    HGLOBAL         hglb = NULL;

    struct {
        WORD     byteOrder;
        WORD     wFormat;
        WORD     osVersion1;
        WORD     osVersion2;
        CLSID    classId;
        DWORD    cSections;
    } PropHeader;
    struct {
        DWORD dwords[4];
        DWORD dwOffset;
    } FIDAndOffset;
#ifdef WIN32
    WCHAR            wzPath[MAX_PATH+1];

    if (mbstowcs(wzPath, szPath, MAX_PATH) == -1)
        return NULL;
    wzPath[MAX_PATH] = 0; // ensure null termination

    hr = StgOpenStorage(wzPath,
                        NULL, STGM_READ | STGM_SHARE_DENY_NONE | STGM_PRIORITY,
                        NULL, 0, &pIStorage);
    if (FAILED(hr)) return NULL;

    hr = pIStorage->OpenStream(L"\005SummaryInformation", NULL,
                        STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pIStream);
#else
    hr = StgOpenStorage(szPath,
                        NULL, STGM_READ | STGM_SHARE_DENY_NONE | STGM_PRIORITY,
                        NULL, 0, &pIStorage);
    if (FAILED(hr)) return NULL;

    hr = pIStorage->OpenStream("\005SummaryInformation", NULL,
                        STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pIStream);

#endif
    if (FAILED(hr)) goto ReleaseStorage;

    LISet32(li, 0);
    hr = pIStream->Seek(li, STREAM_SEEK_SET, NULL);
    if (hr != NOERROR) goto ReleaseStream;

    hr = pIStream->Read(&PropHeader, 28, &ulBytesRead);
    if (hr != NOERROR || ulBytesRead != 28) goto ReleaseStream;

    if (PropHeader.byteOrder != 0xFFFE) goto ReleaseStream;
    if (PropHeader.wFormat != 0) goto ReleaseStream;

    for (i = 0; i < PropHeader.cSections; i++) {
        hr = pIStream->Read(&FIDAndOffset, 20, &ulBytesRead);
        if (hr != NOERROR || ulBytesRead != 20) goto ReleaseStream;
        if (FIDAndOffset.dwords[0] == 0XF29F85E0 && 
            FIDAndOffset.dwords[1] == 0X10684FF9 &&
            FIDAndOffset.dwords[2] == 0X000891AB &&
            FIDAndOffset.dwords[3] == 0XD9B3272B) break;
    }
    if (i >= PropHeader.cSections) goto ReleaseStream;

    LISet32(li, FIDAndOffset.dwOffset);
    hr = pIStream->Seek(li, STREAM_SEEK_SET, NULL);
    if (hr != NOERROR) goto ReleaseStream;

    hr = pIStream->Read(&dwBytesInSection, 4, &ulBytesRead);
    if (hr != NOERROR || ulBytesRead != 4) goto ReleaseStream;
                                            
    hglb = GlobalAlloc(GPTR, dwBytesInSection);
    if (hglb == NULL) goto ReleaseStream; 
    lpSumInfo = (LPSUMINFO) GlobalLock(hglb);
    if (lpSumInfo == NULL) goto Free;

    hr = pIStream->Seek(li, STREAM_SEEK_SET, NULL);
    if (hr != NOERROR) goto Unlock;

    hr = pIStream->Read(lpSumInfo, dwBytesInSection, &ulBytesRead);
    if (hr != NOERROR || ulBytesRead > dwBytesInSection) goto Unlock;
    // tbd: dwBytesInSection is long by 4 bytes for some Excel 5.0 files, so just
    // check that ulBytesRead is <= dwBytesInSection and not that they are equal

    GlobalUnlock(hglb);

    goto ReleaseStream;  

Unlock:
    GlobalUnlock(hglb);

Free:   
    GlobalFree(hglb);
    hglb = NULL;

ReleaseStream:                   
    pIStream->Release();

ReleaseStorage:
    pIStorage->Release();

    return hglb;       
}

///////////////////////////////////////////////////////////////////////////
extern "C" void DLLEXPORT SumInfoCloseFile(HANDLE hSumInfo)
{
    GlobalFree(hSumInfo);
}

///////////////////////////////////////////////////////////////////////////
extern "C" BOOL DLLEXPORT SumInfoGetString(HANDLE hSumInfo,
                                           DWORD  pid,
                                           LPSTR  lpStr,
                                           WORD   cbStr)
{
    LPPROPVALUE lpProp = FindProperty(hSumInfo, pid);
    if (lpProp == NULL) return FALSE;
    if (lpProp->vtType != VT_LPSTR) return FALSE;
    int len = (int) lpProp->vtValue.vtBSTR.cBytes;
    if (len > cbStr) len = cbStr;
    if (len <= 0) {
        *(lpStr) = '\0';
    }
    else {
        lstrcpyn(lpStr, lpProp->vtValue.vtBSTR.ch, len);
        *(lpStr + len - 1) = '\0'; // len includes terminating null
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
extern "C" BOOL DLLEXPORT SumInfoGetLong(HANDLE       hSumInfo,
                                                   DWORD        pid,
                                                   LPLONG       lpLong)
{
    LPPROPVALUE lpProp = FindProperty(hSumInfo, pid);
    if (lpProp == NULL) return FALSE;
    if (lpProp->vtType != VT_I4) return FALSE;
    *lpLong = lpProp->vtValue.vtLong;
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
extern "C" BOOL DLLEXPORT SumInfoGetTime(HANDLE    hSumInfo,
                                                   DWORD        pid,
                                                   LPWORD       yr,
                                                   LPWORD       mon,
                                                   LPWORD       day,
                                                   LPWORD       hr,
                                                   LPWORD       min,
                                                   LPWORD       sec)
{                                                       
    struct {
        unsigned int day : 5;
        unsigned int mon : 4;
        unsigned int yr  : 7;
    } DosDate;
    struct {
        unsigned int sec2 : 5;
        unsigned int min  : 6;
        unsigned int hr   : 5;
    } DosTime; 
    BOOL fBefore1980;

    #define JAN1980_HIGH 0x01A8E79F
    #define JAN1980_LOW  0xE1D58000

    LPPROPVALUE lpProp = FindProperty(hSumInfo, pid);
    if (lpProp == NULL) return FALSE;
    if (lpProp->vtType != VT_FILETIME) return FALSE;

        // If time < Jan 1, 1980, then add FILETIME of 1980/01/01 00:00:00.
        // This is necessary for CoFileTimeToDosDateTime to work.
    if (lpProp->vtValue.vtTime.dwHighDateTime < JAN1980_HIGH) {
        lpProp->vtValue.vtTime.dwLowDateTime += JAN1980_LOW;
        lpProp->vtValue.vtTime.dwHighDateTime += JAN1980_HIGH;
        if (lpProp->vtValue.vtTime.dwLowDateTime < JAN1980_LOW)
            lpProp->vtValue.vtTime.dwHighDateTime++; // overflow
        fBefore1980 = TRUE;
    }
    else fBefore1980 = FALSE;

    if (!CoFileTimeToDosDateTime(&lpProp->vtValue.vtTime,
                                 (LPWORD) &DosDate, (LPWORD) &DosTime))
        return FALSE;

    *yr  = (WORD) (DosDate.yr + 1980);
    *mon = (WORD) DosDate.mon;
    *day = (WORD) DosDate.day;
    *hr  = (WORD) DosTime.hr;
    *min = (WORD) DosTime.min;
    *sec = (WORD) (DosTime.sec2 * 2);

    if (fBefore1980) {
            // Suppose edittime is actually 1 day, 3 hours.
            // Then Y/M/D will be 1980/1/2 at this point.
            // Which should be tranlated to 0/0/1.
            // The code below also handles cases where edittime > 1 month or even 1 year.
        *yr = *yr - 1980;
        *mon = *mon - 1;
        *day = *day - 1;
        if (*mon == 0 && *yr > 0) {
            (*yr)--;
            *mon = 12;
        }
        if (*day == 0 && *mon > 0) {
            (*mon)--;
            switch (*mon) {
                case  1: *day = 31; break;
                case  2: *day = 28; break;
                case  3: *day = 31; break;
                case  4: *day = 30; break;
                case  5: *day = 31; break;
                case  6: *day = 30; break;
                case  7: *day = 31; break;
                case  8: *day = 31; break;
                case  9: *day = 30; break;
                case 10: *day = 31; break;
                case 11: *day = 30; break;
                case 12: *day = 31; break;
            }
        }
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
LPPROPVALUE FindProperty(HANDLE hSumInfo, DWORD pid)
{
    LPPROPVALUE lpProp;
    DWORD i;
    LPSUMINFO lpSumInfo = (LPSUMINFO) GlobalLock(hSumInfo);
    if (lpSumInfo == NULL) return FALSE;
    for (i = 0; i < lpSumInfo->cProps; i++) {
        if (lpSumInfo->aProps[i].propID == pid) {
            lpProp = (LPPROPVALUE) ((LPBYTE) lpSumInfo + lpSumInfo->aProps[i].dwOffset);
            GlobalUnlock(hSumInfo);
            return lpProp;
        }
    }                      
    GlobalUnlock(hSumInfo);
    return NULL;
}
