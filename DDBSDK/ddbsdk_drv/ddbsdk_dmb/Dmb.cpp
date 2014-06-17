#include "Dmb.h"
#include "sdk\SmsCtrlLib.h"
#ifdef DDBSDK_PLATFORM_GUOWEI_H
#include "sdk\BusDrvIf.h"
#endif
#include <winbase.h>
//#include "../ddbsdk_ca/CA_STB_decode.h"
#include "../log/LocalLog.h"

#define MAX_NUM_DEVICES		4

HANDLE g_hEventInit;
SMSHOSTLIB_ERR_CODES_E g_initRetval;

#define MAX_COMPONENTS_NUM 50
#define MAX_ENSEMBLE_NUM 50


static HANDLE hEventInit;
static HANDLE hEventGetversion;
static HANDLE hEventFoundComponents;
static HANDLE hEventEnsembleChanged;
static HANDLE hEventScanComplete;
static HANDLE hEventGetCombinedComponents;
static HANDLE hEventStopService;
static HANDLE hEventReceptionQuality;
static HANDLE hEventGetStatistics;

static UINT  nRecptionQuality;
static SMSHOSTLIB_STATISTICS_ST  msgInfo;
static SMSHOSTLIB_API_INITLIB_PARAMS_ST initparams;
static SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST components[MAX_COMPONENTS_NUM];
static UINT32 nComponents = MAX_COMPONENTS_NUM;
static UINT32 nReturnedComponents;
static SmsHostApiDeviceData_ST deviceData[MAX_NUM_DEVICES];
static UINT nDevices;

static SmsHostApiDeviceHandle hDevice;
static UINT32 g_nDetectedEnsambles = 0;
//FILE * g_outFile = NULL;
static char g_VersionString[256] = {0};
static SMSHOSTLIB_ENSEMBLE_INFO_ST g_ensambles[MAX_ENSEMBLE_NUM];
//static UINT32 frequency = 1470080000;
static SMSHOSTLIB_FREQ_BANDWIDTH_ET bandwidth = BW_8_MHZ;

UINT32 serviceHandle;
SMSHOSTLIB_ERR_CODES_E retval;

HANDLE hBdDevice;
int first_reset=1;

static SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST components_f[5];

extern void DataCallback(void*	    ClientPtr, 
						  UINT32	ServiceHandle, 
						  UINT8*	pBuffer, 
						  UINT32	BufferSize );

DWORD message_hWnd = 0;

void VALIDATE_RETVAL(DWORD retval,char *strName)
{
}
static DWORD Wait_CallBack(HANDLE eventName, DWORD timeout)
{
	DWORD dwWaitResult = WaitForSingleObject(eventName,timeout); 

	return dwWaitResult;
}

static void CtrlCallback( 
						 void*						ClientContext,	//!< Context of client
						 SMSHOSTLIB_MSG_TYPE_RES_E	MsgType,		//!< Response type	
						 SMSHOSTLIB_ERR_CODES_E		ErrCode,		//!< Response success code
						 void* 						pPayload,		//!< Response payload
						 UINT32						PayloadLen) 	//!< Response payload length
{
	switch(MsgType)
	{
	case SMSHOSTLIB_MSG_INIT_DEVICE_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........INIT_DEVICE_RES \r\n"))); 
		// Device has completed initialization	
		g_initRetval = ErrCode;
		SetEvent(g_hEventInit);
		break;
	case SMSHOSTLIB_SCAN_COMPLETE_IND:
//		RETAILMSG(1, (TEXT("CtrlCallback.........COMPLETE_IND \r\n"))); 
		// The scan has completed
		SetEvent(hEventScanComplete);
		break;
	case SMSHOSTLIB_MSG_START_SERVICE_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........START_SERVICE_RES \r\n"))); 
//		SetEvent(hEventStartService);
		break;
	case SMSHOSTLIB_MSG_STOP_SERVICE_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........STOP_SERVICE_RES \r\n"))); 
		SetEvent(hEventStopService);
		break;
	case SMSHOSTLIB_SCAN_PROGRESS_IND:
//		RETAILMSG(1, (TEXT("CtrlCallback.........SCAN_PROGRESS_IND \r\n"))); 
		//g_FreqList[g_FreqListNum] = *((unsigned long*)pPayload);
//		if(ddbsdk.printDebugOpen) printf(".");
		break;
	case SMSHOSTLIB_MSG_GET_COMBINED_COMPONENTS_INFO_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........OMBINED_COMPONENTS_INFO_RES \r\n"))); 
		SetEvent(hEventGetCombinedComponents);
		break;
	case SMSHOSTLIB_MSG_DEVICE_TERMINATE_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........DEVICE_TERMINATE_RES \r\n"))); 
		//SetEvent(hEventDeviceTerminate);
		break;	
	case SMSHOSTLIB_MSG_GET_ENSEMBLE_INFO_RES:
//		printf("g_nDetectedEnsambles=%d\n",g_nDetectedEnsambles);
		if (g_nDetectedEnsambles > 0)
		{
			// Release the waiters only if there are ensembles detected
			SetEvent(hEventFoundComponents);
		}
		break;
	case SMSHOSTLIB_DAB_ENSEMBLE_CHANGED_IND:
//		RETAILMSG(1, (TEXT("CtrlCallback.........CHANGED_IND \r\n"))); 
		// There was some change in the detected ensembles. It is possible that the
		// ensemble list has been EMPTIED instead of filled, so we will call getEnsembleInfo
		// for a list of ensembles to see if we ensembles were actually detected
		SmsHostApiDabGetEnsembleInfo_Req(hDevice,MAX_ENSEMBLE_NUM,&g_nDetectedEnsambles,g_ensambles);
		SetEvent(hEventEnsembleChanged);
		break;
	case SMSHOSTLIB_MSG_GET_VERSION_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........VERSION_RES \r\n"))); 
		// We got a version info reply, copy it to a global variable
		//memcpy(&g_versionInfo,pPayload,sizeof(SMSHOSTLIB_VERSION_INFO_ST));
		//		memcpy(&g_versionInfo,pPayload,sizeof(SMSHOSTLIB_VERSIONING_ST))
		{
			UINT32 SizeToCopy = sizeof( g_VersionString );
			if ( SizeToCopy >  PayloadLen )
			{
				SizeToCopy = PayloadLen;
			}
			memcpy( g_VersionString, pPayload, SizeToCopy );
		}
		SetEvent(hEventGetversion);
		//SetEvent(hEventGetVersionComplete);
		break;
		
	case SMSHOSTLIB_MSG_GET_RECEPTION_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........RECEPTION_RES \r\n"))); 
		//initRetval = ErrCode;
		nRecptionQuality = *((UINT*)pPayload);
		//printf("nRecptionQuality = %d\r\n", nRecptionQuality);
		SetEvent(hEventReceptionQuality);
		break;
		//////////////////////////////////////////////////////////////////////////
	case SMSHOSTLIB_MSG_TUNE_RES:
//		RETAILMSG(1, (TEXT("CtrlCallback.........TUNE_RES \r\n"))); 
		//SetEvent(hEventTune);
		break;
		//////////////////////////////////////////////////////////////////////////
	case SMSHOSTLIB_MSG_GET_STATISTICS_RES:
		memcpy(&msgInfo,pPayload,sizeof(SMSHOSTLIB_STATISTICS_ST));
		SetEvent(hEventGetStatistics);
		break;

	default:
//		RETAILMSG(1, (TEXT("CtrlCallback.........default \r\n"))); 
		MsgType = MsgType;
		break;
	}	
}
void printfBuf(BYTE *buf, UINT size)
{
	UINT i;
	int c=0;
	for(i=0; i<size; i++)
	{
		c++;
		printf("%.2x ",buf[i]);
		if(c%16 == 0)
			printf("\n");
	}
	printf("\n");
}

#ifndef BUFFER_SIZE
#define BUFFER_SIZE	100
#endif

//extern BYTE *Finance_Info;
extern BYTE* g_byszWriteBuf;
extern BYTE* BUFFER;
//extern CRITICAL_SECTION g_cs;

void printMsgInfo(void)
{
	printf("IsRfLocked: %d\n", msgInfo.IsRfLocked);				//!< 0 - not locked, 1 - locked
	printf("IsDemodLocked: %d\n", msgInfo.IsDemodLocked);			//!< 0 - not locked, 1 - locked
	printf("IsExternalLNAOn: %d\n", msgInfo.IsExternalLNAOn);			//!< 0 - external LNA off, 1 - external LNA on
	printf("SNR: %d\n", msgInfo.SNR);						//!< dB
	printf("BER: %d\n", msgInfo.BER);						//!< Post Viterbi BER [1E-5]
	printf("FIB_CRC: %d\n", msgInfo.FIB_CRC);					//!< CRC errors percentage, valid only for DAB
	printf("TS_PER: %d\n", msgInfo.TS_PER);					//!< Transport stream PER, 0xFFFFFFFF indicate N/A, valid only for DVB-T/H
	printf("MFER: %d\n", msgInfo.MFER);					//!< DVB-H frame error rate in percentage, 0xFFFFFFFF indicate N/A, valid only for DVB-H
	printf("RSSI: %d\n", msgInfo.RSSI);					//!< dBm
	printf("InBandPwr: %d\n", msgInfo.InBandPwr);				//!< In band power in dBM
	printf("CarrierOffset: %d\n", msgInfo.CarrierOffset);			//!< Carrier Offset in bin/1024
	printf("Frequency: %d\n", msgInfo.Frequency);				//!< Frequency in Hz
	printf("Bandwidth: %d\n", msgInfo.Bandwidth);				//!< Bandwidth in MHz, valid only for DVB-T/H
	printf("TransmissionMode: %d\n", msgInfo.TransmissionMode);		//!< Transmission Mode, for DAB modes 1-4, for DVB-T/H FFT mode carriers in Kilos
	printf("ModemState: %d\n", msgInfo.ModemState);				//!< from SMSHOSTLIB_DVB_MODEM_STATE_ET , valid only for DVB-T/H
	printf("GuardInterval: %d\n", msgInfo.GuardInterval);			//!< Guard Interval, 1 divided by value , valid only for DVB-T/H
	printf("CodeRate: %d\n", msgInfo.CodeRate);				//!< Code Rate from SMSHOSTLIB_CODE_RATE_ET, valid only for DVB-T/H
	printf("LPCodeRate: %d\n", msgInfo.LPCodeRate);				//!< Low Priority Code Rate from SMSHOSTLIB_CODE_RATE_ET, valid only for DVB-T/H
	printf("Hierarchy: %d\n", msgInfo.Hierarchy);				//!< Hierarchy from SMSHOSTLIB_HIERARCHY_ET, valid only for DVB-T/H
	printf("Constellation: %d\n", msgInfo.Constellation);			//!< Constellation from SMSHOSTLIB_CONSTELLATION_ET, valid only for DVB-T/H
	printf("BurstSize: %d\n", msgInfo.BurstSize);				//!< Current burst size in bytes, valid only for DVB-H
	printf("BurstDuration: %d\n", msgInfo.BurstDuration);			//!< Current burst duration in mSec, valid only for DVB-H
	printf("BurstCycleTime: %d\n", msgInfo.BurstCycleTime);			//!< Current burst cycle time in mSec, valid only for DVB-H
	printf("CalculatedBurstCycleTime: %d\n", msgInfo.CalculatedBurstCycleTime);//!< Current burst cycle time in mSec, as calculated by demodulator, valid only for DVB-H
	printf("NumOfRows: %d\n", msgInfo.NumOfRows);				//!< Number of rows in MPE table, valid only for DVB-H
	printf("NumOfPaddCols: %d\n", msgInfo.NumOfPaddCols);			//!< Number of padding columns in MPE table, valid only for DVB-H
	printf("NumOfPunctCols: %d\n", msgInfo.NumOfPunctCols);			//!< Number of puncturing columns in MPE table, valid only for DVB-H
	printf("ErrorTSPackets: %d\n", msgInfo.ErrorTSPackets);			//!< Number of erroneous transport-stream packets
	printf("TotalTSPackets: %d\n", msgInfo.TotalTSPackets);			//!< Total number of transport-stream packets
	printf("NumOfValidMpeTlbs: %d\n", msgInfo.NumOfValidMpeTlbs);		//!< Number of MPE tables which do not include errors after MPE RS decoding
	printf("NumOfInvalidMpeTlbs: %d\n", msgInfo.NumOfInvalidMpeTlbs);		//!< Number of MPE tables which include errors after MPE RS decoding
	printf("NumOfCorrectedMpeTlbs: %d\n", msgInfo.NumOfCorrectedMpeTlbs);	//!< Number of MPE tables which were corrected by MPE RS decoding
	printf("BERErrorCount: %d\n", msgInfo.BERErrorCount);			//!< Number of errornous SYNC bits.
	printf("BERBitCount: %d\n", msgInfo.BERBitCount);				//!< Total number of SYNC bits.
	printf("SmsToHostTxErrors: %d\n", msgInfo.SmsToHostTxErrors);		//!< Total number of transmission errors.
	printf("PreBER: %d\n", msgInfo.PreBER); 					//!< DAB/T-DMB only: Pre Viterbi BER [1E-5]
	printf("CellId: %d\n", msgInfo.CellId);					//!< TPS Cell ID in bits 15..0, bits 31..16 zero; if set to 0xFFFFFFFF cell_id not yet recovered
	printf("DvbhSrvIndHP: %d\n", msgInfo.DvbhSrvIndHP);			//!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator
	printf("DvbhSrvIndLP: %d\n", msgInfo.DvbhSrvIndLP);			//!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator
	printf("NumMPEReceived: %d\n", msgInfo.NumMPEReceived);			//!< DVB-H, Num MPE section received
}

long DmbStart(int   frequency)
{
	int   timeOutCount = 0;
//	RETAILMSG(1, (TEXT("--------------> DmbStart >----------\r\n"))); 

	//InitializeCriticalSection(&g_cs);

	g_hEventInit = CreateEvent(NULL,FALSE,FALSE,NULL);

	// basic setup
	hEventInit = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventGetversion = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventFoundComponents = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventEnsembleChanged = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventScanComplete = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventGetCombinedComponents = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventStopService = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventReceptionQuality = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventGetStatistics = CreateEvent(NULL,FALSE,FALSE,NULL);

	// Init the control library
	initparams.Size = sizeof(SMSHOSTLIB_API_INITLIB_PARAMS_ST);
	initparams.pCtrlCallback = CtrlCallback;
	initparams.pDataCallback = DataCallback;
	initparams.CommType = SMSHOSTLIB_COMM_SPI;
	initparams.pCommParam = NULL;
	initparams.WorkingDirectory = "\\temp";
	initparams.IsSupportMultInstances = TRUE;
	retval = SmsHostApiLibInit(&initparams);
	if (retval == SMSHOSTLIB_ERR_OK) 
	{ 
//		RETAILMSG(1, (TEXT("--- SmsHostApiLibInit : Success.\r\n"))); 
	} 
	else 
	{               
//		RETAILMSG(1, (TEXT("--- SmsHostApiLibInit : FAIL! error code: 0x%08X \r\n"),retval));
		//MessageBox(NULL,TEXT("SmsHostApiLibInit failed!"),NULL,MB_OK);
		return -1;
	}	
	
	// Enumerate connected devices and choose the first one
	retval = SmsHostApiGetDevicesList(MAX_NUM_DEVICES,&nDevices,deviceData);
	if (retval == SMSHOSTLIB_ERR_OK) 
	{ 
//		RETAILMSG(1, (TEXT("--- SmsHostApiGetDevicesList : Success.\r\n")));
//		RETAILMSG(1,(L"Found %d connected devices\n",nDevices));
	} 
	else 
	{ 
//		RETAILMSG(1, (TEXT("--- SmsHostApiGetDevicesList : FAIL! \r\n")));
//		RETAILMSG(1, (TEXT("--->>> Error code: 0x%08X \r\n"),retval));
		//	MessageBox(NULL,TEXT("SmsHostApiGetDevicesList failed!"),NULL,MB_OK);
		return -1;
	}

	// Init the device
	retval = SmsHostApiDeviceInit_Req(deviceData[0].DeviceName,SMSHOSTLIB_DEVMD_DAB_TDMB,12000000,NULL,&hDevice);
#if 1
	//if (retval == SMSHOSTLIB_ERR_OK) 
	// Check initial return value, if this check is OK it still
	// doesn't mean that the initialization is OK
	if (retval == SMSHOSTLIB_ERR_OK)
	{
//		RETAILMSG(1, (TEXT("--- SmsHostApiDeviceInit_Req : Success.\n"))); 
	} 
	else 
	{ 
//		RETAILMSG(1, (TEXT("--- SmsHostApiDeviceInit_Req : FAIL!\n"))); 
//		RETAILMSG(1, (TEXT("--->>> Error code: 0x%08X \r\n"),retval));
		//	MessageBox(NULL,TEXT("SmsHostApiDeviceInit_Req failed!"),NULL,MB_OK);
		return -1;
	}
#endif

	Wait_CallBack(g_hEventInit,3000);

	if (g_initRetval != SMSHOSTLIB_ERR_OK)
	{

//		RETAILMSG(1, (TEXT("---Error initializing device(0x%08x) !!!!!!!!! \r\n"),g_initRetval)); 
		return -1;
	}

//	RETAILMSG(1, (TEXT("--------------< DmbStart Sucessfully >----------\r\n"))); 


	// Get firmware version 
	retval = SmsHostApiGetVersion_Req( hDevice );
	Wait_CallBack(hEventGetversion,10000);
//	if(ddbsdk.printDebugOpen) printf( "Device version: %s\n", g_VersionString );

	/////////////////////////////////////////////////////////////////
	BOOL bDoScan=FALSE;
	if (bDoScan)
	{
		// Scan and search for services
		SMSHOSTLIB_SCAN_RANGE_ST range;
		range.BottomFreq = 209936000;
		range.UpFreq = 239280000;
		range.Gap = 8000000;
		retval = SmsHostApiScanStart_Req(hDevice,
			ANALYSE_SRV_INFO,
			BW_8_MHZ,SCAN_PARAM_RANGE,
			&range);
		VALIDATE_RETVAL(retval,"Scan");
		Wait_CallBack(hEventScanComplete,30000);
		//cout << endl;
	}
	else
	{
		// Tune the SMS on a single frequency	
		retval = SmsHostApiTune_Req(hDevice,frequency,bandwidth);
		VALIDATE_RETVAL(retval,"Tune");
	}

	// Wait for ensembles to have been found
	Wait_CallBack(hEventFoundComponents,5000);

	// Get the list of all ensembles, services and components in one big flat array.
	// The alternative way of doing this is calling:
	// 1. SmsHostApiDabGetEnsembleInfo_Req 
	// 2. SmsHostApiDabGetServices_Req,
	// 3. SmsHostApiDabGetComponentsInfo_Req
	retval = SmsHostApiDabGetCombinedComponentsInfo_Req(hDevice,nComponents,&nReturnedComponents,components);
	VALIDATE_RETVAL(retval,"GetComponents");
	Wait_CallBack(hEventGetCombinedComponents, 1000);

	//cout << "Got " << nReturnedComponents << " components." << endl;
//	if(ddbsdk.printDebugOpen) printf("Got:%d components\n",nReturnedComponents);

	while(1) {
		//Sleep(1000);
		retval = SmsHostApiDabGetCombinedComponentsInfo_Req(hDevice,nComponents,&nReturnedComponents,components);
		VALIDATE_RETVAL(retval,"GetComponents");
		Wait_CallBack(hEventGetCombinedComponents, 1000);

//		if(ddbsdk.printDebugOpen) printf("Got:%d components\n",nReturnedComponents);
		WriteLog("Got:%d components\n",nReturnedComponents);
		if(nReturnedComponents == components->EnsembleInfo.NumOfService && nReturnedComponents > 0) break;
		timeOutCount ++;
		if(timeOutCount > 1) {
			return -1;
		}
	}

	for (UINT32 i = 0; i< nReturnedComponents; i++)
	{
		if (0x0000c016 == components[i].ServiceInfo.SID)
		{
			memcpy(&components_f[0], &components[i], sizeof(SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST));
		}
		if (0x0000c002 == components[i].ServiceInfo.SID)
		{
			memcpy(&components_f[1], &components[i], sizeof(SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST));
		}
		if (0xf0c00040 == components[i].ServiceInfo.SID)
		{
			memcpy(&components_f[2], &components[i], sizeof(SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST));
		}
		if (0xf0c00006 == components[i].ServiceInfo.SID)
		{
			memcpy(&components_f[3], &components[i], sizeof(SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST));
		}
		if (0xf0c00030 == components[i].ServiceInfo.SID)
		{
			memcpy(&components_f[4], &components[i], sizeof(SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST));
		}
	}

	// Start one of the components
	//components[2].ServiceInfo.SID = 0xf0c00030;
	serviceHandle = NULL;
#if 0
	UINT32 index = programIndex;

	if(ddbsdk.printDebugOpen) printf("Request No.%d Stream~~~~~~", index);

	UINT32 EIdx = components[index].EnsembleIdx;
	UINT32 ServiceID = components[index].ServiceInfo.SID;
	UINT32 SCIdS = components[index].ComponentInfo.SCIdS;
	//UINT32 serviceHandle;  //变到全局去了
	retval = SmsHostApiDabStartService_Req(hDevice,EIdx,ServiceID,SCIdS,&serviceHandle);
	VALIDATE_RETVAL(retval,"StartService");
	if(SMSHOSTLIB_ERR_OK != retval)
	{
		return -1;
	}
#endif
	retval = SmsHostApiGetReceptionQuality_Req(hDevice);
	Wait_CallBack(hEventReceptionQuality,10000);
//	if(ddbsdk.printDebugOpen) printf("Qu: %d\n", nRecptionQuality);

	retval = SmsHostApiGetStatistics_Req(hDevice);
	Wait_CallBack(hEventGetStatistics, 10000);

//	if(ddbsdk.printDebugOpen) printMsgInfo();
#if 0
	while(1) {
		Sleep(1000);
		retval = SmsHostApiGetReceptionQuality_Req(hDevice);
		Wait_CallBack(hEventReceptionQuality,10000);
		printf("Qu: %d\n", nRecptionQuality);

		retval = SmsHostApiGetStatistics_Req(hDevice);
		Wait_CallBack(hEventGetStatistics, 10000);
		printf("IsRfLocked: %d\n", msgInfo.IsRfLocked);				//!< 0 - not locked, 1 - locked
		printf("IsDemodLocked: %d\n", msgInfo.IsDemodLocked);			//!< 0 - not locked, 1 - locked
	}
#endif
	return 0;
}

unsigned long  startDmbService(int  index, int testInSMGLabFlag)
{
	if(! testInSMGLabFlag) {
	WriteLog("Request No.%d Stream~~~~~~\n", index);

		if(serviceHandle != NULL) {
			retval = SmsHostApiDabStopService_Req(hDevice,serviceHandle);
			VALIDATE_RETVAL(retval,"StopService");
			Wait_CallBack(hEventStopService,1000);
		}
	UINT32 EIdx = components_f[index].EnsembleIdx;
	UINT32 ServiceID = components_f[index].ServiceInfo.SID;
	UINT32 SCIdS = components_f[index].ComponentInfo.SCIdS;
	//UINT32 serviceHandle;  //变到全局去了
	retval = SmsHostApiDabStartService_Req(hDevice,EIdx,ServiceID,SCIdS,&serviceHandle);
	VALIDATE_RETVAL(retval,"StartService");
		Sleep(2000);
	if(SMSHOSTLIB_ERR_OK != retval)
	{
		return -1;
	}
	} else {  /* only for test in SMG Lab */
		// Start one of the components
		UINT32 index2 = 0;
		UINT32 EIdx = components[index2].EnsembleIdx;
		UINT32 ServiceID = 0xf0c00030;//components[index2].ServiceInfo.SID;
		UINT32 SCIdS = components[index2].ComponentInfo.SCIdS;

		retval = SmsHostApiDabStartService_Req(hDevice,EIdx,ServiceID,SCIdS,&serviceHandle);
//		Wait_CallBack(hEventStartService, 8000);
		VALIDATE_RETVAL(retval,"StartService");
	}
	return 0;
}

unsigned long  stopDmbService()
{
	if(serviceHandle != NULL) {
		retval = SmsHostApiDabStopService_Req(hDevice,serviceHandle);
		VALIDATE_RETVAL(retval,"StopService");
		Wait_CallBack(hEventStopService,1000);
		serviceHandle = NULL;
		return retval;
	}
	return 0;
}

void dmbGetServiceNum(unsigned int * ServiceNum)
{
	* ServiceNum = components->EnsembleInfo.NumOfService;
}
int getMusicamServNum()
{
	return 3;
}

int getMusicamServerIndex(int num)
{
	switch(num) {
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		return 5;
	}
	return 0;
}

void getMusicamDescr(char * descr, int num)
{
	switch(num) {
	case 0:
		strncpy(descr, (const char *)&components[0].ServiceInfo.pLabel[0], sizeof(components[0].ServiceInfo.pLabel));
		break;
	case 1:
		strncpy(descr, (const char *)&components[1].ServiceInfo.pLabel[0], sizeof(components[1].ServiceInfo.pLabel));
		break;
	case 2:
		strncpy(descr, (const char *)&components[5].ServiceInfo.pLabel[0], sizeof(components[5].ServiceInfo.pLabel));
		break;
	}
}

int getVideoServNum()
{
	return 2;
}

int getVideoServerIndex(int num)
{
	switch(num) {
	case 0:
		return 3;
	case 1:
		return 4;
	}
	return 3;
}

void getVideoDescr(char * descr, int num)
{
	switch(num) {
	case 0:
		strcpy(descr, (const char *)&components[3].ServiceInfo.pLabel[0]);
		break;
	case 1:
		strcpy(descr, (const char *)&components[4].ServiceInfo.pLabel[0]);
		break;
	}
}

int getDataServNum()
{
	return 1;
}

int getDataServerIndex(int num)
{
	return 4;
}

#if 0
int getSmsDmbLibVersion(void)
{
	SmsHostApiGetVersion_Req(hDevice);
	Wait_CallBack(hEventGetversion, 10000);
	return 0;  //g_VersionString;
}
#endif
UINT getSignal()
{
	SmsHostApiGetReceptionQuality_Req(hDevice);
	Wait_CallBack(hEventReceptionQuality,1000);
	return nRecptionQuality;
}
void  checkSignal(int  frequency) 
{
	retval = SmsHostApiGetReceptionQuality_Req(hDevice);
	Wait_CallBack(hEventReceptionQuality,10000);
//	if(ddbsdk.printDebugOpen) printf("Qu: %d\n", nRecptionQuality);

	retval = SmsHostApiGetStatistics_Req(hDevice);
	Wait_CallBack(hEventGetStatistics, 10000);
#if 0
	if(ddbsdk.printDebugOpen) {
		printf("IsRfLocked: %d\n", msgInfo.IsRfLocked);				//!< 0 - not locked, 1 - locked
		printf("IsDemodLocked: %d\n", msgInfo.IsDemodLocked);			//!< 0 - not locked, 1 - locked
		printf("SNR: %d\n", msgInfo.SNR);						//!< dB
		printf("BER: %d\n", msgInfo.BER);						//!< Post Viterbi BER [1E-5]
	}
#endif
/*
	retval = SmsHostApiDabGetCombinedComponentsInfo_Req(hDevice,nComponents,&nReturnedComponents,components);
	Wait_CallBack(hEventGetCombinedComponents, 1000);
	printf("Got:%d components\n",nReturnedComponents);
*/
#if 1
//	if(nRecptionQuality < RECEPTION_QUALITY_FAIL) {
		SmsHostApiDabStopService_Req(hDevice,serviceHandle);
		Wait_CallBack(hEventStopService,1000);
		SmsHostApiLibTerminate();
//		while(1) {
//			Sleep(1000);
//			retval = SmsHostApiGetReceptionQuality_Req(hDevice);
//			Wait_CallBack(hEventReceptionQuality,10000);
//			printf("Qu: %d\n", nRecptionQuality);
//			if(nRecptionQuality >= RECEPTION_QUALITY_FAIL) 
//				break;
//		}
		retval = SmsHostApiLibInit(&initparams);
		SmsHostApiGetDevicesList(MAX_NUM_DEVICES,&nDevices,deviceData);
		SmsHostApiDeviceInit_Req(deviceData[0].DeviceName,SMSHOSTLIB_DEVMD_DAB_TDMB,12000000,NULL,&hDevice);
		Wait_CallBack(g_hEventInit,3000);
		SmsHostApiTune_Req(hDevice,frequency,bandwidth);
		Wait_CallBack(hEventFoundComponents,5000);
		retval = SmsHostApiDabGetCombinedComponentsInfo_Req(hDevice,nComponents,&nReturnedComponents,components);
		Wait_CallBack(hEventGetCombinedComponents, 1000);
//		if(ddbsdk.printDebugOpen) printf("Got:%d components\n",nReturnedComponents);
		UINT32 index = 2;
		UINT32 EIdx = components[index].EnsembleIdx;
		UINT32 ServiceID = components[index].ServiceInfo.SID;
		UINT32 SCIdS = components[index].ComponentInfo.SCIdS;
		retval = SmsHostApiDabStartService_Req(hDevice,EIdx,ServiceID,SCIdS,&serviceHandle);
//	}
#endif
}

void DmbStop(void)
{
//	RETAILMSG(1, (TEXT("-------------- DmbStop ----------\r\n"))); 
//	if(ddbsdk.printDebugOpen) printf("-------------- DmbStop ----------\n");


	// Stop the service
	retval = SmsHostApiDabStopService_Req(hDevice,serviceHandle);
//	if(ddbsdk.printDebugOpen) VALIDATE_RETVAL(retval,"StopService");
	Wait_CallBack(hEventStopService,1000);

	//DeleteCriticalSection(&g_cs);
	// Deinit
	SmsHostApiLibTerminate();
	CloseHandle(g_hEventInit);

//	if(ddbsdk.printDebugOpen) printf("device teminated.\n");


//	SmsHostApiLibTerminate();
	CloseHandle(hEventStopService);
	CloseHandle(hEventInit);
	CloseHandle(hEventFoundComponents);
	CloseHandle(hEventEnsembleChanged);
	CloseHandle(hEventScanComplete);
	CloseHandle(hEventGetCombinedComponents);
	CloseHandle(hEventGetversion);	

}

#ifdef DDBSDK_PLATFORM_GUOWEI_H
HANDLE SianoPowerOn(void)
{
	WCHAR tmpStr[300];
	WCHAR line[256];
	WCHAR standard[10];
	FILE* pf;
	BOOL found;
	UINT32 standardLen;
	WCHAR utilsPath[250];
	DWORD fileSize;
	PBYTE pBuff;

	found = FALSE;

	hBdDevice = CreateFile(TEXT("SPI1:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if (hBdDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	/* open Config.txt and extract current inp name */

	wcscpy(utilsPath,TEXT("\\Windows\\"));

	wcscpy(tmpStr,utilsPath);
	wcscat(tmpStr,TEXT("Config.txt"));

	pf = _wfopen(tmpStr,TEXT("rt"));
	if (!pf)
	{
		return FALSE;
	}

	// search for start of [Standard] section
	while (fgetws(line, 256, pf) != NULL)
	{
		if (_wcsnicmp(line,TEXT("[Standard]"),10) == 0)
		{
			found = TRUE;
			break;
		}
	}

	if ((!found) || (fgetws(standard, 10, pf) == NULL))
	{
		fclose(pf);
		return FALSE;
	}

	/* remove the \n */
	standard[wcslen(standard) - 1] = '\0';
	standardLen = wcslen(standard);
	if ((_wcsnicmp(standard,TEXT("DVBH"),4) != 0) && (_wcsnicmp(standard,TEXT("DVB-H"),5) != 0) &&
		(_wcsnicmp(standard,TEXT("TDMB"),4) != 0) && (_wcsnicmp(standard,TEXT("T-DMB"),5) != 0) &&
		(_wcsnicmp(standard,TEXT("DVBT"),4) != 0) && (_wcsnicmp(standard,TEXT("DVB-T"),5) != 0))
	{
		fclose(pf);
		return FALSE;
	}

	found = FALSE;
	fseek(pf,0,SEEK_SET);
	// search for start of [INP] section
	while (fgetws(line, 256, pf) != NULL)
	{
		if (_wcsnicmp(line,TEXT("[INP]"),5) == 0)
		{
			found = TRUE;
			break;
		}
	}

	if (!found)
	{
		fclose(pf);
		return FALSE;
	}

	found = FALSE;
	// search for INP for Standard
	while (fgetws(line, 256, pf) != NULL)
	{
		if (_wcsnicmp(line,standard,standardLen) == 0)
		{
			found = TRUE;
			break;
		}
	}

	if (!found)
	{
		fclose(pf);
		return FALSE;
	}

	/* remove the \n */
	line[wcslen(line) - 1] = '\0';

	// cat the "Config.txt" form the tmpStr
	tmpStr[wcslen(utilsPath)] = '\0';

	// append the inp name
	wcscat(tmpStr,line + standardLen + 1);

	fclose(pf);

	/* read inp to memory */

	//DBGMSG(ZONE_INIT, (TEXT("SmsGenDrv: download file: %s\r\n"),tmpStr));

	pf = _wfopen(tmpStr,TEXT("rb"));
	if (pf == NULL)
	{
		// error open file
		return FALSE;
	}

	fseek(pf,0,SEEK_END);
	fileSize = ftell(pf);
	fseek(pf,0,SEEK_SET);

	pBuff = (PBYTE)LocalAlloc(LPTR, fileSize);
	fread(pBuff,1,fileSize,pf);

	fclose(pf);
#if 0
	if (DeviceIoControl(hBdDevice,SIANO_BD_IOCTL_POWERDOWN_DEVICE,pBuff,fileSize,
		NULL,0,NULL,NULL) == FALSE)
	{
		return FALSE;
	}
	Sleep(200);
#endif
	if (DeviceIoControl(hBdDevice,SIANO_BD_IOCTL_POWERUP_DEVICE,pBuff,fileSize,
		NULL,0,NULL,NULL) == FALSE)
	{
		return FALSE;
	}
	Sleep(100);

	if (first_reset)
	{
		if (DeviceIoControl(hBdDevice,SIANO_BD_IOCTL_RESET_HIGH_DEVICE,pBuff,fileSize,
			NULL,0,NULL,NULL) == FALSE)
		{
			return FALSE;
		}
		Sleep(200);
		/* write FW buffer to the card */
		if (DeviceIoControl(hBdDevice,SIANO_BD_IOCTL_WRITE_FW_BUFF_TO_DEVICE,pBuff,fileSize,
			NULL,0,NULL,NULL) == FALSE)
		{
			return FALSE;
		}
		//first_reset = 0;
	}

	LocalFree(pBuff);

	return hBdDevice;
}



VOID SianoPowerDown(void)
{
	SMSHOSTLIB_ERR_CODES_E retval;
	DWORD fileSize;
	PBYTE pBuff = NULL;

	if (DeviceIoControl(hBdDevice,SIANO_BD_IOCTL_RESET_LOW_DEVICE,pBuff,fileSize,
		NULL,0,NULL,NULL) == FALSE)
	{
		return;
	}
	Sleep(200);

	if (DeviceIoControl(hBdDevice,SIANO_BD_IOCTL_POWERDOWN_DEVICE,pBuff,fileSize,
		NULL,0,NULL,NULL) == FALSE)
	{
		return;
	}
	CloseHandle(hBdDevice);
	hBdDevice = NULL;
}
#endif
