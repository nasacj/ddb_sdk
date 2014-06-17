#include "LocalLog.h"

/**
Get the current directory of the application
@param [in/out] strPath The output directory
@return void
*/
void GetCurrentDirectory(TCHAR *pPath, DWORD dwSize)
{ 
    TCHAR szBuf[MAX_PATH] = {0}; 

    GetModuleFileName(NULL,szBuf,sizeof(szBuf)/sizeof(wchar_t)); 

    TCHAR *pFileName = _tcsrchr(szBuf, L'\\');

    _tcsncpy(pPath, szBuf, min(pFileName - szBuf + 1, (int)dwSize));
}


#ifdef _LOG

static CRITICAL_SECTION s_LogCS; // The log file critical section
TCHAR g_szFileName[MAX_PATH] = {0};

/**
    Initialize the log
@param [in] pFileName The log file name, null-terminated string
@return If the function succeeds, the return value is TRUE, otherwise FALSE
*/
BOOL InitializeLog(TCHAR *pFileName)
{
    BOOL bRetVal = FALSE;

    if (pFileName == NULL)
    {
        return bRetVal;
    }

    InitializeCriticalSection(&s_LogCS);

    _tcsncpy(g_szFileName, pFileName, MAX_PATH - 1);

    return bRetVal;
}

/**
    Uninitialize the log
@param none
@return void
*/
void UninitializeLog()
{
    DeleteCriticalSection(&s_LogCS);

  
}

/**
Write log
@param [in] lpszFormat The format string
@return void
*/
void WriteLogT(LPSTR lpszFormat, ...)
{
    DWORD dwWritten = 0;
    static CHAR szMessage[1024 *10] = {0};

    va_list pArgs;
    va_start(pArgs, lpszFormat);
    ::vsprintf(szMessage, lpszFormat, pArgs);
    va_end(pArgs);

    SYSTEMTIME tm;
    GetLocalTime(&tm);
    CHAR szDateTime[32];

    sprintf(szDateTime, "[%04d%02d%02d %02d:%02d:%02d]", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

    EnterCriticalSection(&s_LogCS);

    try
    {
//          wprintf(szDateTime);
//          wprintf(szMessage);
//          wprintf(L"\n");

        TCHAR szPath[MAX_PATH] = {0};
        //GetCurrentDirectory(szPath, MAX_PATH);

		
        _tcscat(szPath, L"\\MMC_Storage\\sdkLog\\");

		//wprintf(szPath);

        CreateDirectory(szPath, NULL);
        HANDLE s_hLogFile = INVALID_HANDLE_VALUE; // The log file handle

        _tcscat(szPath, g_szFileName);    
        s_hLogFile = CreateFile(szPath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, NULL, NULL);
        if (s_hLogFile != INVALID_HANDLE_VALUE)
        {
            SetFilePointer(s_hLogFile,0,0,FILE_END);
        }

        if (s_hLogFile != INVALID_HANDLE_VALUE)
        {
            WriteFile(s_hLogFile, szDateTime, strlen(szDateTime) * sizeof(CHAR), &dwWritten, NULL);
            WriteFile(s_hLogFile, szMessage, strlen(szMessage) * sizeof(CHAR), &dwWritten, NULL);
            WriteFile(s_hLogFile, "\r\n", 2, &dwWritten, NULL);
            //FlushFileBuffers(s_hLogFile);
        }

        if (s_hLogFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(s_hLogFile);
            s_hLogFile = INVALID_HANDLE_VALUE;
        }
    }
    catch(...)
    {
        // Do nothing 
    }
    
    LeaveCriticalSection(&s_LogCS);
}

#endif

