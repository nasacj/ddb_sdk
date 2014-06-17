
#include <stdlib.h>
#include "HGDABDevice.h"
#include "../jscpp.io/BaseIOImp/SyncCircleBuffer.h"
#include "../ddbsdk.h"

static HANDLE getDataThreadH = NULL;
static unsigned int getDataThreadH_ID = 0;
static int isTheadRunning = 1;
static unsigned int __stdcall gainData(void* pram)
{
	Buffer* buffer = (Buffer*)pram;
	int bytesRead;
	int littleBufSize = 188*30;
	BYTE_P littleBuf = (BYTE_P)malloc(littleBufSize);
	while(isTheadRunning)
	{
		bytesRead = DAB_ReadMSC((char*)littleBuf, littleBufSize);
		if (bytesRead > 0)
		{
			buffer->store(littleBuf, bytesRead);
		}
		
	}
	free(littleBuf);
	return 0;
}

HGDABDevice::HGDABDevice()
{

}

int HGDABDevice::openDevice(char* pram, U32 timeout)
{
	if (TRUE == DAB_OpenDevice(NULL))
	{
		unsigned int freq = (unsigned int)atol(pram);
		this->serviceNum = DAB_Scan(freq);
		if (serviceNum <= 0)
		{
			this->timeout = timeout;
			return DDBSDK_ERRNO_DEVICE_OPEN_FAIL;
		}
		
	}
	return DDBSDK_ERRNO_DEVICE_OPEN_FAIL;
}

list<DAB_Service>*	HGDABDevice::getSevices()
{
	if (this->serviceNum > 0)
	{
		list<DAB_Service>* servicesList = new list<DAB_Service>;
		for (int i = 0; i < serviceNum; i++)
		{
			SERVINFORM serviceInof = DAB_GetInfo(i);
			DAB_Service dabService;
			memcpy(dabService.serviceLable, serviceInof.serviceLable, sizeof(dabService.serviceLable));
			memcpy(dabService.EnsembleName, serviceInof.EnsembleName, sizeof(dabService.EnsembleName));
			dabService.SubChannleId = serviceInof.SubChannleId;
			dabService.ServiceType = serviceInof.ServiceType;
			dabService.BitRateKhz = serviceInof.BitRateKhz;
			dabService.SrvCharSet = serviceInof.ServiceType;
			servicesList->push_back(dabService);
			this->serviceInfoArray.push_back(serviceInof);
		}
		return servicesList;
	}
	return 0;
}

InputStream* HGDABDevice::getInputStream(DAB_Service* dabServiceInfo)
{
	int seriveNumLocation = -1;
	for(int lok = 0; lok < serviceInfoArray.size(); lok++)
	{
		seriveNumLocation++;
		if (!strcmp((char*)&(serviceInfoArray[lok].serviceLable[0]), (char*)&(dabServiceInfo->serviceLable[0])))
		{
			DAB_SelService(seriveNumLocation);
			this->dataBuffer = new SyncCircleBuffer(1000*188, this->timeout);
			assert(this->dataBuffer);
			getDataThreadH = (HANDLE)_beginthreadex( NULL, 0, gainData, (void*)(this->dataBuffer), 0, &(getDataThreadH_ID) );
			return new HGDABDevice::HGDABImputStream(this->dataBuffer);
		}
	}

	return 0;
}

int HGDABDevice::closeDevice()
{
	DAB_CloseDevice();
	return 0;
}


HGDABDevice::~HGDABDevice()
{
	
}

HGDABDevice::HGDABImputStream::HGDABImputStream(Buffer * buffer)
{
	tsDataBuffer = buffer;
}

int HGDABDevice::HGDABImputStream::readBytes(__in BYTE_P buffer, __in U32 bufferSize, __out U32* bytesReturned)
{
	tsDataBuffer->read(buffer, bufferSize, bytesReturned);
	return 0;
}
void HGDABDevice::HGDABImputStream::close()
{
	isTheadRunning = 0;
	WaitForSingleObject(getDataThreadH, INFINITE);
	delete tsDataBuffer;

}

HGDABDevice::HGDABImputStream::~HGDABImputStream()
{

}
