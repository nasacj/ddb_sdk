// GetUUID_DLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "GetUUID_DLL.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


__declspec(dllexport) int GetUUID(DWORD cbInput,
		BYTE* pInput, DWORD* pcbOutput, BYTE** ppOutput,
		IRAPIStream* pStream)
{
	GUID myUUID;
	BOOL bRetVal;
	DWORD dwBytesReturned;
	bRetVal = KernelIoControl (IOCTL_HAL_GET_UUID, NULL, 0, &myUUID, 
                             sizeof (myUUID), &dwBytesReturned);
	*ppOutput = (BYTE*)LocalAlloc(LPTR, dwBytesReturned);

	if (bRetVal == FALSE || *ppOutput == NULL)
	{
		*pcbOutput = 0;
		return 1;
	}
	else
	{
		memcpy(*ppOutput, &myUUID, dwBytesReturned);
		*pcbOutput = dwBytesReturned;
		return 0;
	}
	
}
