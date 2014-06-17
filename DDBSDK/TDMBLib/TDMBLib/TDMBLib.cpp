#include "TDMBLib.h"
#include "ddbsdk/ddbsdk_if.h"
#include "log/LocalLog.h"

#define TS_BUFFER_SIZE			188*30

SETGDCALLBACKFUNC DataCallBackFun = NULL;
BOOL	isSysInitialized = FALSE;
PBYTE	TS_Buffer = NULL;
HANDLE	getDataTs_Thread = NULL;
DWORD	getDataTs_Thread_ID = 0;
BOOL	isGetDataTsThreadRunning = FALSE;

extern HANDLE hDDB_sdklib;

static bool getCAID(DWORD *cardID)
{
	ddbsdk_versionInfo_t  verInfo;
	BOOL resul = DDB_IOControl(NULL, IOCTL_DDBSDK_VERSIONINFO, NULL, 0, (unsigned char *)&verInfo, sizeof(verInfo), NULL);
	if (resul == FALSE)
	{
		return false;
	}
	*cardID = verInfo.ca_card_no;
	return true;
}

static bool OpenDecrypDataTS()
{
	BOOL bResult = DDB_IOControl(NULL, IOCTL_DDBSDK_DATA_TS_OPEN, NULL, 0, NULL, 0, NULL);
	if (FALSE == bResult)
	{
		return false;
	}
	return true;
}

static unsigned int getSignalValue()
{
	UINT signal = 100;
	DWORD returnbytes;
	DDB_IOControl(NULL, IOCTL_DDBSDK_DMBSIGNAL_QUALITY, NULL, 0, (PBYTE)&signal, sizeof(UINT), &returnbytes);
	return signal;
}

static unsigned long thread_getDataTs(void * param)
{
	WriteLog("线程thread_getDataTs开始...\n");
	DWORD BytesReturned = 0;
	PBYTE buffer = TS_Buffer;
	DWORD size = TS_BUFFER_SIZE;
	while(isGetDataTsThreadRunning)
	{
		DDB_IOControl(NULL, IOCTL_DDBSDK_DATA_TS_GET, NULL, 0, buffer, size, &BytesReturned);
		if (BytesReturned > 0)
		{
			if (DataCallBackFun != NULL)
			{
				DataCallBackFun(buffer, BytesReturned, NULL);
			}
		}
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_DATA_TS_CLOSE, NULL, 0, buffer, size, &BytesReturned);
	WriteLog("线程thread_getDataTs退出，并且DATA_TS_CLOSE...\n");
	return 0;
}

void TDMB_SetGDCallbackFunc(SETGDCALLBACKFUNC callBackFun)
{
	DataCallBackFun = callBackFun;
}

bool TDMB_Init(HWND hWnd,unsigned int MODE)
{
	if (isSysInitialized)
	{
		return false;
	}
	if (DDB_Init(NULL, 0) == 0)
	{
		return false;
	}
// 	if (DDB_Open() != 1)
// 	{
// 		return false;
// 	}
	if (ddbif_openDmbDevice() == DDBIF_ERROR)
	{
		return false;
	}
	TS_Buffer = (PBYTE)malloc(TS_BUFFER_SIZE);
	if (TS_Buffer == NULL)
	{
		return false;
	}
	isSysInitialized = TRUE;
	return true;
}

bool TDMB_InitDataDown()
{
	if (!isSysInitialized)
	{
		return false;
	}
	if (!OpenDecrypDataTS())
	{
		return false;
	}
	isGetDataTsThreadRunning = TRUE;
	getDataTs_Thread = CreateThread(NULL, 0, thread_getDataTs, NULL, 0, &getDataTs_Thread_ID);
	if (getDataTs_Thread == NULL)
	{
		isGetDataTsThreadRunning = false;
		return false;
	}
	return true;
}

bool TDMB_UnInit()
{
	if (!isSysInitialized)
	{
		return false;
	}
	isGetDataTsThreadRunning = FALSE;
	WaitForSingleObject(getDataTs_Thread, INFINITE);
	CloseHandle(getDataTs_Thread);
	ddbif_closeDmbDevice();
	ddbif_closeDdbsdk();
	free(TS_Buffer);
	TS_Buffer = NULL;
	getDataTs_Thread = NULL;
	getDataTs_Thread_ID = 0;
	DataCallBackFun = NULL;
	isSysInitialized = FALSE;
	return true;
}

unsigned int TDMB_GetSignalSNR()
{
	if (!isSysInitialized)
	{
		return 0xFF;
	}
	return getSignalValue();
}

bool DVTCASTB_GetCardID(DWORD *pdwCardID)
{
	if (!isSysInitialized)
	{
		return false;
	}
	return getCAID(pdwCardID);
}
