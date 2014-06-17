#ifndef LOCAL_LOG_
#define LOCAL_LOG_


#define _LOG

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

extern void GetCurrentDirectory(TCHAR *pPath, DWORD dwSize);

#ifdef _LOG
extern BOOL InitializeLog(TCHAR *pFileName);
extern void UninitializeLog(void);

extern "C" void WriteLogT(LPSTR lpszFormat, ...);
#define WriteLog WriteLogT
#else
#define WriteLog
#define InitializeLog
#define UninitializeLog()
#endif



#endif 