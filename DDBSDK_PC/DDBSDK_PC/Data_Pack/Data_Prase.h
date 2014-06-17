#ifndef DDBSDK_DATAPRASE_H_
#define DDBSDK_DATAPRASE_H_

#include "Data_Pack.h"
#include "../dmb/TsDevic.h"
#include "../dmb/HGDABDevice.h"
#include "../TS_Sturct/TS_Tables.h"

typedef struct DDBSDK_CRYP_PID_PROGRAM_INFO_
{
	U16		pid;
	U8		type;
	U16		ecm_pid;
	U16		emm_pid;
	U8		oddcw[16];
	U8		evencw[16];
	U8		isHaveCW;
}CrypProgram;

class TsStreamCollector
{
public:
	int startDevice(char* pram, U32 timeout);
	list<DAB_Service>* getDabServiceInfos();
	int startService(DAB_Service*);
	int startPid(U16 pid);
	int getPidData(BYTE_P, U32* bytesRet);
 
private:
	TsDevice* tsDevice;
	InputStream* tsDeviceInputStream;
	list<DAB_Service>* dabServiceInfos;
	list<I_DataPack*>* dataPackList;
};




#endif