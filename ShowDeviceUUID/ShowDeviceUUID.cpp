// ShowDeviceUUID.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <stdio.h>
#include <types.h>
#include <WINIOCTL.H>
extern "C" __declspec(dllimport) 
BOOL KernelIoControl(
  DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, 
  LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned
);

//#define IOCTL_HAL_GET_DEVICEID CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_UUID   CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
 	BYTE *pbyUUid;
	GUID myUUID;
	pbyUUid = (BYTE*)&myUUID;
	DWORD dwBytesReturned;
	KernelIoControl (IOCTL_HAL_GET_UUID, NULL, 0, &myUUID, 
                             sizeof (myUUID), &dwBytesReturned);
	memcpy(pbyUUid, &myUUID, dwBytesReturned);
	printf("dwBytesReturned = %d\n", dwBytesReturned);
	printf("sizeof(GUID) = %d\n", sizeof(GUID));
	for(int i=0; i<sizeof(GUID); i++)
	{
		printf("%.2x ", pbyUUid[i]);
	}

	int d;
	scanf("%d", &d);
	return 0;
}

