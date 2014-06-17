#include <windows.h>
#include <winioctl.h>
#include "rapi.h"

extern "C" BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);
#define IOCTL_HAL_GET_UUID   CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)


extern "C"
{
	__declspec(dllexport) int GetUUID(DWORD cbInput,
		BYTE* pInput, DWORD* pcbOutput, BYTE** ppOutput,
		IRAPIStream* pStream);
}
