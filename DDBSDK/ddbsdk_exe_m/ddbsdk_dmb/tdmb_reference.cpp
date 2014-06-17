// Reference code for Siano SMS T-DMB interface
// July 2007, Siano Mobile Silicon

#include <tchar.h>
#include <windows.h>
#include "sdk/SmsCtrlLib.h"
#include <fstream>
#include <iostream>

using namespace std;

#define MAX_COMPONENTS_NUM 50
#define MAX_ENSEMBLE_NUM 50


HANDLE hEventInit;
HANDLE hEventGetversion;
HANDLE hEventFoundComponents;
HANDLE hEventEnsembleChanged;
HANDLE hEventScanComplete;
HANDLE hEventGetCombinedComponents;
HANDLE hEventStopService;
SmsHostApiDeviceHandle hDevice;
UINT32 g_nDetectedEnsambles = 0;
FILE * g_outFile = NULL;
char g_VersionString[256] = {0};


SMSHOSTLIB_ENSEMBLE_INFO_ST g_ensambles[MAX_ENSEMBLE_NUM];

void DataCallback(void*	ClientPtr, 
				  UINT32	ServiceHandle, 
				  UINT8*	pBuffer, 
				  UINT32	BufferSize );

void CallbackFunction( 
					  void*						ClientContext,	
					  SMSHOSTLIB_MSG_TYPE_RES_E	MsgType,		
					  SMSHOSTLIB_ERR_CODES_E		ErrCode,	
					  void* 						pPayload,	
					  UINT32						PayloadLen);


#define VALIDATE_RETVAL(retval,strName) \
	if (retval == SMSHOSTLIB_ERR_OK) \
{ \
	cout << strName << ": Success." << endl; \
} \
	else \
{ \
	cout << strName << ": FAIL!" << endl; \
	return 1; \
}

#define WAIT_CALLBACK(eventName,timeout) \
	dwWaitResult = WaitForSingleObject(eventName,timeout); \
	if (dwWaitResult != WAIT_OBJECT_0) \
{ \
	cout << "ERROR: Timeout!" << endl; \
	return 1; \
}


int _tmain(int argc, _TCHAR* argv[])
{
	SMSHOSTLIB_ERR_CODES_E retval;
	DWORD dwWaitResult;
	boolean bDoScan = false;//true;
	int k=0;

//for test
//while (1)
{
	k++;
	cout << "--------------"<<k<<"--------------------------\n";
	
	// basic setup
	hEventInit = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventGetversion = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventFoundComponents = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventEnsembleChanged = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventScanComplete = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventGetCombinedComponents = CreateEvent(NULL,FALSE,FALSE,NULL);
	hEventStopService = CreateEvent(NULL,FALSE,FALSE,NULL);

	// Init the control library
	SMSHOSTLIB_API_INITLIB_PARAMS_ST initparams;
	initparams.Size = sizeof(SMSHOSTLIB_API_INITLIB_PARAMS_ST);
	initparams.pCtrlCallback = CallbackFunction;
	initparams.pDataCallback = DataCallback;
	initparams.CommType = SMSHOSTLIB_COMM_USB;
	initparams.pCommParam = NULL;
	initparams.WorkingDirectory = "c:\\temp";
	initparams.IsSupportMultInstances = TRUE;
	retval = SmsHostApiLibInit(&initparams);
	VALIDATE_RETVAL(retval,"InitLib");

	// Enumerate connected devices and choose the first one
	UINT nDevices;
	SmsHostApiDeviceData_ST deviceData;
	retval = SmsHostApiGetDevicesList(1,&nDevices,&deviceData);
	VALIDATE_RETVAL(retval,"GetDevicesList");
	cout << nDevices << " device(s) returned." << endl;

	// Init the device
	retval = SmsHostApiDeviceInit_Req(deviceData.DeviceName,SMSHOSTLIB_DEVMD_DAB_TDMB,12000000,NULL,&hDevice);
	VALIDATE_RETVAL(retval,"InitDevice");
	WAIT_CALLBACK(hEventInit,10000);

	// Get firmware version 
	retval = SmsHostApiGetVersion_Req( hDevice );
	VALIDATE_RETVAL(retval,"GetVersion");
	WAIT_CALLBACK(hEventGetversion,10000);

	printf( "Device version: %s\n", g_VersionString );

#if 1

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
		WAIT_CALLBACK(hEventScanComplete,30000);
		cout << endl;
	}
	else
	{
		// Tune the SMS on a single frequency	
		UINT32 frequency = 1470080000;
		SMSHOSTLIB_FREQ_BANDWIDTH_ET bandwidth = BW_8_MHZ;
		retval = SmsHostApiTune_Req(hDevice,frequency,bandwidth);
		VALIDATE_RETVAL(retval,"Tune");
	}

	// Wait for ensembles to have been found
	WAIT_CALLBACK(hEventFoundComponents,5000);

	// Get the list of all ensembles, services and components in one big flat array.
	// The alternative way of doing this is calling:
	// 1. SmsHostApiDabGetEnsembleInfo_Req 
	// 2. SmsHostApiDabGetServices_Req,
	// 3. SmsHostApiDabGetComponentsInfo_Req
	UINT32 nComponents = MAX_COMPONENTS_NUM;
	UINT32 nReturnedComponents;
	SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST components[MAX_COMPONENTS_NUM];
	retval = SmsHostApiDabGetCombinedComponentsInfo_Req(hDevice,nComponents,&nReturnedComponents,components);
	VALIDATE_RETVAL(retval,"GetComponents");
	WAIT_CALLBACK(hEventGetCombinedComponents,1000);

	cout << "Got " << nReturnedComponents << " components." << endl;


	// Prepare an output file
	g_outFile = fopen("dump.ts","wb");

	// Start one of the components
	UINT32 index = 1;
	UINT32 EIdx = components[index].EnsembleIdx;
	UINT32 ServiceID = components[index].ServiceInfo.SID;
	UINT32 SCIdS = components[index].ComponentInfo.SCIdS;
	UINT32 serviceHandle;
	retval = SmsHostApiDabStartService_Req(hDevice,EIdx,ServiceID,SCIdS,&serviceHandle);
	VALIDATE_RETVAL(retval,"StartService");

	// Sleep for some time, meanwhile dumping transport stream 
	// data through our data callback function
	int nSecondsSleep = 30;
	cout << "Sleeping " << nSecondsSleep << " seconds..." << endl;
	Sleep(nSecondsSleep * 1000);
	cout << "\n";

	// Stop the service
	retval = SmsHostApiDabStopService_Req(hDevice,serviceHandle);
	VALIDATE_RETVAL(retval,"StopService");
	WAIT_CALLBACK(hEventStopService,1000);

	// Clean up
	fclose(g_outFile);
	g_outFile = NULL;
#endif

//Added by Alice for test
	SmsHostApiDeviceTerminate_Req( hDevice );
	printf("device teminated.\n");


	SmsHostApiLibTerminate();
	CloseHandle(hEventStopService);
	CloseHandle(hEventInit);
	CloseHandle(hEventFoundComponents);
	CloseHandle(hEventEnsembleChanged);
	CloseHandle(hEventScanComplete);
	CloseHandle(hEventGetCombinedComponents);
	CloseHandle(hEventGetversion);	
}
	return 0;
}




/// Control callback
void CallbackFunction( 
					  void*						ClientContext,	//!< Context of client
					  SMSHOSTLIB_MSG_TYPE_RES_E	MsgType,		//!< Response type	
					  SMSHOSTLIB_ERR_CODES_E		ErrCode,		//!< Response success code
					  void* 						pPayload,		//!< Response payload
					  UINT32						PayloadLen) 	//!< Response payload length
{
	switch(MsgType)
	{
	case SMSHOSTLIB_MSG_INIT_DEVICE_RES:
		// Device has completed initialization
		SetEvent(hEventInit);
		break;
	case SMSHOSTLIB_SCAN_COMPLETE_IND:
		// The scan has completed
		SetEvent(hEventScanComplete);
		break;
	case SMSHOSTLIB_MSG_STOP_SERVICE_RES:
		SetEvent(hEventStopService);
		break;
	case SMSHOSTLIB_SCAN_PROGRESS_IND:
		// Scan is in progress
		cout << ".";
		break;
	case SMSHOSTLIB_MSG_GET_COMBINED_COMPONENTS_INFO_RES:
		SetEvent(hEventGetCombinedComponents);
		break;
	case SMSHOSTLIB_MSG_TUNE_RES:
		break;
	case SMSHOSTLIB_MSG_GET_VERSION_RES:		//added by alice
		{
			UINT32 SizeToCopy = sizeof( g_VersionString );
			if ( SizeToCopy >  PayloadLen )
			{
				SizeToCopy = PayloadLen;
			}
			memcpy( g_VersionString, pPayload, SizeToCopy );
		}
		SetEvent(hEventGetversion);
		break;
	case SMSHOSTLIB_MSG_GET_ENSEMBLE_INFO_RES:
		if (g_nDetectedEnsambles > 0)
		{
			// Release the waiters only if there are ensembles detected
			SetEvent(hEventFoundComponents);
		}
		break;
	case SMSHOSTLIB_DAB_ENSEMBLE_CHANGED_IND:
		// There was some change in the detected ensembles. It is possible that the
		// ensemble list has been EMPTIED instead of filled, so we will call getEnsembleInfo
		// for a list of ensembles to see if we ensembles were actually detected
		SmsHostApiDabGetEnsembleInfo_Req(hDevice,MAX_ENSEMBLE_NUM,&g_nDetectedEnsambles,g_ensambles);
		SetEvent(hEventEnsembleChanged);
		break;
	}	
}


// Callback function for data
void DataCallback(void*	ClientPtr, 
				  UINT32	ServiceHandle, 
				  UINT8*	pBuffer, 
				  UINT32	BufferSize )
{
	if (g_outFile != NULL)
	{
		fwrite(pBuffer,1,BufferSize,g_outFile);
		cout << ".";
	}
}
