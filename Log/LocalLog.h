#ifndef LOCAL_LOG_
#define LOCAL_LOG_


#define _LOG

extern void GetCurrentDirectory(TCHAR *pPath, DWORD dwSize);

#ifdef _LOG
extern BOOL InitializeLog(TCHAR *pFileName);
extern void UninitializeLog(void);

extern void WriteLogT(LPTSTR lpszFormat, ...);
#define WriteLog WriteLogT
#else
#define WriteLog
#define InitializeLog
#define UninitializeLog()
#endif



#endif 