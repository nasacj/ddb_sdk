#ifndef DDBSDK_TSDEVICE_H_
#define DDBSDK_TSDEVICE_H_

#include "BaseIO.h"
#include <list>

using namespace std;

typedef struct _DDBSDK_DAB_SERVICE_
{
	U8	serviceLable[32];
	U8	EnsembleName[32];
	U8	SubChannleId;
	U16	ServiceType;
	U16	BitRateKhz;
	U8	SrvCharSet;
}DAB_Service;


Interface TsDevice
{
public:
	virtual int openDevice(char* pram, U32 timeout) = 0;
	virtual list<DAB_Service>*	getSevices() = 0;
	virtual InputStream* getInputStream(DAB_Service*) = 0;
	virtual int closeDevice() = 0;

	virtual ~TsDevice(){};
};



#endif