#ifndef DDBSDK_DMB_HGDABDEVICE_H_
#define DDBSDK_DMB_HGDABDEVICE_H_

#include <Windows.h>
#include <process.h>
#include "TsDevic.h"
#include "HG_DAB/DAB_public.h"
#include "HG_DAB/DabWin.h"
#include <vector>

class HGDABDevice : implements TsDevice
{
public:
	HGDABDevice();
	int openDevice(char* pram, U32 timeout);
	list<DAB_Service>*	getSevices();
	InputStream* getInputStream(DAB_Service*);
	int closeDevice();

	~HGDABDevice();

private:
	int timeout;
	int serviceNum;
	Buffer* dataBuffer;
	vector<SERVINFORM> serviceInfoArray;

	class HGDABImputStream : implements InputStream
	{
	public:
		HGDABImputStream(Buffer*);
		int readBytes(__in BYTE_P buffer, __in U32 bufferSize, __out U32* bytesReturned);
		void close();
		
		~HGDABImputStream();

	private:
		Buffer* tsDataBuffer;
	};
};

#endif