#include "ddbsdk.h"

ErrorNo DDB_StartDevice(TS_Device* device, TS_Service* services, int* serivcesRetNum)
{
	if (0 == device || 0 == services || 0 == serivcesRetNum)
	{
		return DDBSDK_ERRNO_POINTER_INVALID;
	}
	switch(device->deviceType)
	{
	case TS_DMB:
		{

			break;
		}
	case TS_FILE:
		{

			break;
		}
	case TS_SOCKET:
		{

			break;
		}
	}
	return DDBSDK_ERRNO_OK;
}