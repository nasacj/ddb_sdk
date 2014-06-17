
#include <stdio.h>
#include <winerror.h>
//#include "../wince500/pkfuncs.h"
#include <windows.h>
#include <winioctl.h>

#include "DVTSTB_CASInf.h"
#include "CA_STB_decode.h"
#include "../src/ddbsdk.h"

extern ddbsdk_t  ddbsdk;

extern "C" BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);
#define IOCTL_HAL_GET_UUID   CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FOR_CAS_DATA_BUFFER_SIZE	64*1024			//�ն�Flash�����ڱ�����Ϣ��������Ҫ����


extern ShowCAPromptMessage CAshowMessage;
extern int errorMessageOccur(int);

//extern ddbsdk_dmbdataInfo_t    dataInfoList;
extern unsigned short getDataECM(u16 pid);

BYTE *  g_BufferForCAS;
//BYTE    g_bDDBHaveCW = 0;
//BYTE	g_OddCW[16];	//�洢��CW
//BYTE	g_EvenCW[16];	//�洢żCW

WORD	g_wCurrentEcmPid = 0;
WORD	g_wTrafficEcmPid = 4112;
WORD	g_wWeatherEcmPid = 4113;
WORD	g_wFinanceEcmPid = 4114;
WORD	g_wPOIinfoEcmPid = 4116;
WORD	g_wSerFileEcmPid = 4117;//WORD	g_wSerFileEcmPid = 4117;
WORD	g_wCarParkEcmPid = 0;
WORD	g_wWeatherFileEcmPid = 0;
WORD	g_wRoadConditionEcmPid = 0;
WORD	g_wEmergencyBroadcastEcmPid = 0;
WORD	g_wTravelFileEcmPid = 0;
WORD	g_wExpoFileEcmPid = 0;

void InitialDataECMPid(void);
BOOL isInitialDataECMPid = 0;


BYTE	g_TrafficOddCW[16];	
BYTE	g_TrafficEvenCW[16];

BYTE	g_FinanceOddCW[16];	
BYTE	g_FinanceEvenCW[16];

BYTE	g_POIinfoOddCW[16];	
BYTE	g_POIinfoEvenCW[16];
	
BYTE	g_WeatherOddCW[16];	
BYTE	g_WeatherEvenCW[16];

BYTE	g_SerFileOddCW[16];	
BYTE	g_SerFileEvenCW[16];

BYTE	g_CarParkOddCW[16];	
BYTE	g_CarParkEvenCW[16];

BYTE	g_WeatherFileOddCW[16];	
BYTE	g_WeatherFileEvenCW[16];

BYTE	g_RoadConditionOddCW[16];	
BYTE	g_RoadConditionEvenCW[16];

BYTE	g_EmergencyBroadcastOddCW[16];	
BYTE	g_EmergencyBroadcastEvenCW[16];

BYTE	g_TravelFileOddCW[16];	
BYTE	g_TravelFileEvenCW[16];

BYTE	g_ExpoFileOddCW[16];	
BYTE	g_ExpoFileEvenCW[16];


bool	g_bTrafficHaveCW = false;
bool	g_bWeatherHaveCW = false;
bool	g_bFinanceHaveCW = false;
bool	g_bPOIinfoHaveCW = false;
bool	g_bSerFileHaveCW = false;
bool	g_bCarParkHaveCW = false;
bool	g_bWeatherFileHaveCW = false;
bool	g_bRoadConditionHaveCW = false;
bool	g_bEmergencyBroadcastHaveCW = false;
bool	g_bTravelFileHaveCW = false;
bool	g_bExpoFileHaveCW = false;


char*	CAPromptMessage_Traffic = NULL;
char*	CAPromptMessage_Weather = NULL;
char*	CAPromptMessage_Finance = NULL;
char*	CAPromptMessage_POIinfo = NULL;
char*	CAPromptMessage_DowFile = NULL;
char*	CAPromptMessage_CarPark = NULL;
char*	CAPromptMessage_WeatherFile = NULL;
char*	CAPromptMessage_RoadCondition = NULL;
char*	CAPromptMessage_EmergencyBroadcast = NULL;
char*	CAPromptMessage_TravelFile = NULL;
char*	CAPromptMessage_ExpoFile = NULL;

extern int setCWFromDectrypStreamArray(WORD wEcmPID, BYTE byKeyLen, const unsigned char* szOddKey,const unsigned char* szEvenKey);
extern int setCWFromDectrypProgInfoArray(WORD wEcmPID, BYTE byKeyLen, const unsigned char* szOddKey,const unsigned char* szEvenKey);
extern int setCWoffFromDectrypProgInfoArray(WORD wEcmPID);

#if 0
HRESULT DVTSTBCA_GetUUid(BYTE *pbyUUid, unsigned short *pwLen)
{
	char szUUID[16];
	memset(szUUID, 0 , 16);
	memcpy(pbyUUid, szUUID, 16);
	return 0;
}
#else
HRESULT DVTSTBCA_GetUUid(BYTE *pbyUUid, unsigned short *pwLen)
{
#if 0
//	char szUUID[16] = {0xa8, 0x25, 0x0b, 0x81, 0x4c, 0xf4, 0x49, 0x89,
//		0x97, 0x49, 0x42, 0xae, 0xf3, 0x16, 0xb3, 0xf3};
	char szUUID[16] = {0xC1, 0xB1, 0xC6, 0x49, 0x58, 0xA8, 0xEA, 0x0C, 0x92, 0x64, 0xB4, 0x75, 0x77, 0x6A, 0x9E, 0x9A};
	memcpy(pbyUUid, szUUID, 16);
	return 0;
#endif

#if 1
	GUID myUUID;
	BOOL bRetVal;
	DWORD dwBytesReturned;
	bRetVal = KernelIoControl (IOCTL_HAL_GET_UUID, NULL, 0, &myUUID, 
                             sizeof (myUUID), &dwBytesReturned);
	memcpy(pbyUUid, &myUUID, dwBytesReturned);
	printf("%x %x %x %x %x %x %x %x\n", pbyUUid[0], pbyUUid[1], pbyUUid[2], pbyUUid[3], pbyUUid[4], pbyUUid[5], pbyUUid[6], pbyUUid[7]);
	printf("%x %x %x %x %x %x %x %x\n", pbyUUid[8], pbyUUid[9], pbyUUid[10], pbyUUid[11], pbyUUid[12], pbyUUid[13], pbyUUid[14], pbyUUid[15]);
	WriteLog("%x %x %x %x %x %x %x %x", pbyUUid[0], pbyUUid[1], pbyUUid[2], pbyUUid[3], pbyUUid[4], pbyUUid[5], pbyUUid[6], pbyUUid[7]);
	WriteLog("%x %x %x %x %x %x %x %x", pbyUUid[8], pbyUUid[9], pbyUUid[10], pbyUUid[11], pbyUUid[12], pbyUUid[13], pbyUUid[14], pbyUUid[15]);

	return 0;
#endif

}
#endif
/*++
���ܣ�����ն˷����CAS��flash�ռ������ַ�ʹ�С�����ֽ�Ϊ��λ����
������
	ppStartAddr:			�����з����CAS��flash�ռ�Ŀ�ʼ��ַ��
	lSize:					��������з����CAS��flash�ռ�Ĵ�С��
--*/
HRESULT DVTSTBCA_GetDataBufferAddr(long* lSize,char ** ppStartAddr)
{
	HRESULT result = 0;
	*lSize = FOR_CAS_DATA_BUFFER_SIZE;
	*ppStartAddr = (char*)g_BufferForCAS;
	return result;
}

/*++
���ܣ���ȡ�������ն�flash�е���Ϣ��
������
	pStartAddr:				Ҫ��ȡ�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪ��ȡ�������ֵ�����Ϊʵ�ʶ�ȡ�Ĵ�С��
	pData:					���������ݡ�
--*/
HRESULT DVTSTBCA_ReadDataBuffer(const char * pStartAddr,long * plDataLen,unsigned char* pData)
{
	HRESULT result = 0;
	if(*plDataLen<0)
		return S_FALSE;
	if((DWORD)pStartAddr < (DWORD)g_BufferForCAS 
	|| (DWORD)pStartAddr >= (DWORD)g_BufferForCAS + FOR_CAS_DATA_BUFFER_SIZE)
	{
		return -1;
	}
	if((DWORD)pStartAddr + (*plDataLen) - (DWORD)g_BufferForCAS > FOR_CAS_DATA_BUFFER_SIZE)
	{
		*plDataLen = FOR_CAS_DATA_BUFFER_SIZE - ((DWORD)pStartAddr - (DWORD)g_BufferForCAS);
	}
	try
	{
		memcpy(pData,pStartAddr,*plDataLen);
	}
	catch(...)
	{
		NULL;
	}
	return result;
}

//extern CString GetAppPath();
/*++
���ܣ����ն˵Ĵ洢�ռ�д��Ϣ��
������
	lStartAddr:				Ҫд�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪд�����ݵĳ��ȣ����Ϊд���ʵ�ʳ��ȡ�
	pData:					Ҫд�����ݡ�
--*/
HRESULT DVTSTBCA_WriteDataBuffer(const char * pStartAddr,long * plDataLen,const unsigned char* pData)
{
	HRESULT result = 0;
	if(*plDataLen<0)
		return -1;
	if((DWORD)pStartAddr < (DWORD)g_BufferForCAS
	|| (DWORD)pStartAddr >= (DWORD)g_BufferForCAS + FOR_CAS_DATA_BUFFER_SIZE)
	{
		return -1;
	}
	if((DWORD)pStartAddr + (*plDataLen) - (DWORD)g_BufferForCAS > FOR_CAS_DATA_BUFFER_SIZE)
	{
		*plDataLen = FOR_CAS_DATA_BUFFER_SIZE - ((DWORD)pStartAddr - (DWORD)g_BufferForCAS);
	}
	try
	{
		memcpy((void*)pStartAddr,pData,*plDataLen);
		FILE * cafile;
		char   capath[256];

		//sprintf(capath, "%s%s\\CaFile.data", ddbsdk.ddbsdk_path_main, DDBSDK_MAINPATH_NAME);
		if((cafile = fopen("\\Flash_Storage\\softcard.ca", "wb")) == NULL)
		{
			return -1;
		}
		fwrite(g_BufferForCAS, 1, FOR_CAS_DATA_BUFFER_SIZE, cafile);
		fclose(cafile);
	}
	catch(...)
	{
		NULL;
	}
	return result;
}

/*++
���ܣ�CA�����ô˺������ý�����������ǰ���ڼ���һ���ڵ�CW�͸���������
������
	wEcmPID:				CW������ECMPID��
	szOddKey:				��CW�����ݡ�
	szEvenKey:				żCW�����ݡ�
	byKeyLen:				CW�ĳ��ȡ�
--*/
HRESULT DVTSTBCA_SetDescrCW(WORD wEcmPID, BYTE byKeyLen, const unsigned char* szOddKey,const unsigned char* szEvenKey)
{
	setCWFromDectrypStreamArray(wEcmPID, byKeyLen, szOddKey, szEvenKey);
	setCWFromDectrypProgInfoArray(wEcmPID, byKeyLen, szOddKey, szEvenKey);


	if(!isInitialDataECMPid)
	{
		InitialDataECMPid();
		isInitialDataECMPid = TRUE;
	}

//	printf("���ý�����\n");
//	memcpy(g_OddCW, szOddKey, byKeyLen);
//	memcpy(g_EvenCW, szEvenKey, byKeyLen);
//	g_bDDBHaveCW = 1;
	//printf("���ý�����==>   wEcmPID = %d\n",wEcmPID);
	if(wEcmPID == g_wTrafficEcmPid)
	{
		g_bTrafficHaveCW = true;
		CAPromptMessage_Traffic = NULL;
		memcpy(g_TrafficOddCW, szOddKey, byKeyLen);
		memcpy(g_TrafficEvenCW, szEvenKey, byKeyLen);
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wWeatherEcmPid)
	{
		g_bWeatherHaveCW = true;
		CAPromptMessage_Weather = NULL;
		memcpy(g_WeatherOddCW, szOddKey, byKeyLen);
		memcpy(g_WeatherEvenCW, szEvenKey, byKeyLen);
		//g_bTrafficHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wFinanceEcmPid)
	{
		//printf("g_bFinanceHaveCW address = %x",&g_bFinanceHaveCW);
		g_bFinanceHaveCW = true;
		CAPromptMessage_Finance = NULL;
		memcpy(g_FinanceOddCW, szOddKey, byKeyLen);
		memcpy(g_FinanceEvenCW, szEvenKey, byKeyLen);
		//g_bTrafficHaveCW = false;
		//g_bWeatherHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wPOIinfoEcmPid)
	{
		g_bPOIinfoHaveCW = true;
		CAPromptMessage_POIinfo = NULL;
		memcpy(g_POIinfoOddCW, szOddKey, byKeyLen);
		memcpy(g_POIinfoEvenCW, szEvenKey, byKeyLen);
		//g_bTrafficHaveCW = false;
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wSerFileEcmPid)
	{
		//printf("g_bSerFileHaveCW address = %x\n",&g_bSerFileHaveCW);
		g_bSerFileHaveCW = true;
		CAPromptMessage_DowFile = NULL;
		memcpy(g_SerFileOddCW, szOddKey, byKeyLen);
		memcpy(g_SerFileEvenCW, szEvenKey, byKeyLen);
		//g_bTrafficHaveCW = false;
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wCarParkEcmPid)
	{
		g_bCarParkHaveCW = true;
		CAPromptMessage_CarPark = NULL;
		memcpy(g_CarParkOddCW, szOddKey, byKeyLen);
		memcpy(g_CarParkEvenCW, szEvenKey, byKeyLen);
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wWeatherFileEcmPid)
	{
		g_bWeatherFileHaveCW = true;
		CAPromptMessage_WeatherFile = NULL;
		memcpy(g_WeatherFileOddCW, szOddKey, byKeyLen);
		memcpy(g_WeatherFileEvenCW, szEvenKey, byKeyLen);
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wRoadConditionEcmPid)
	{
		g_bRoadConditionHaveCW = true;
		CAPromptMessage_RoadCondition = NULL;
		memcpy(g_RoadConditionOddCW, szOddKey, byKeyLen);
		memcpy(g_RoadConditionEvenCW, szEvenKey, byKeyLen);
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wEmergencyBroadcastEcmPid)
	{
		g_bEmergencyBroadcastHaveCW = true;
		CAPromptMessage_EmergencyBroadcast = NULL;
		memcpy(g_EmergencyBroadcastOddCW, szOddKey, byKeyLen);
		memcpy(g_EmergencyBroadcastEvenCW, szEvenKey, byKeyLen);
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wTravelFileEcmPid)
	{
		g_bTravelFileHaveCW = true;
		CAPromptMessage_TravelFile = NULL;
		memcpy(g_TravelFileOddCW, szOddKey, byKeyLen);
		memcpy(g_TravelFileEvenCW, szEvenKey, byKeyLen);
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	if(wEcmPID == g_wEmergencyBroadcastEcmPid)
	{
		g_bExpoFileHaveCW = true;
		CAPromptMessage_ExpoFile = NULL;
		memcpy(g_ExpoFileOddCW, szOddKey, byKeyLen);
		memcpy(g_ExpoFileEvenCW, szEvenKey, byKeyLen);
		//g_bWeatherHaveCW = false;
		//g_bFinanceHaveCW = false;
		//g_bPOIinfoHaveCW = false;
		//g_bSerFileHaveCW = false;
		return 0;
	}
	return 0;
	//������������g_OddCW��g_EvenCW����ҵ�����ݣ�
	//���TS���ſ��Ʊ�־λ��11��szOddKey�����Ϊ10��szEvenKey
}

void praseMessage(char** datatypeMsg, BYTE byMesageNo)
{

	switch(byMesageNo)
	{
	case DVTCA_TVS_NOT_FOUND:
		*datatypeMsg = "the ISP key donnot exist!";
		break;
	case DVTCA_KEY_NOT_FOUND:
		*datatypeMsg = "the ISP does not exist!";
		break;
	case DVTCA_DECRYPT_FAILURE:
		*datatypeMsg = "encrypted program!";
		break;
	case DVTCA_NO_ENTITLE:
		*datatypeMsg = "donnot buy this program!";
		break;
	case DVTCA_SC_IS_FORBIDDEN:
		*datatypeMsg = "forbidden card!";
		break;
	case DVTCA_DATA_INVALID:
		*datatypeMsg = "cannot receive program!";
		break;
	default:
		*datatypeMsg = "unknown!";
		break;
	}
}

//��ʾ��ʾ��Ϣ
void  DVTSTBCA_ShowPromptMessage(BYTE byMesageNo)
{
	if(!isInitialDataECMPid)
	{
		InitialDataECMPid();
		isInitialDataECMPid = TRUE;
	}
	/*
	if(CAshowMessage != NULL)
	{
		printf("CAshowMessage = %x\n",CAshowMessage);
		//CAshowMessage(byMesageNo);
	}
	else
	{
		printf("CAshowMessage == NULL!\n");
	}
*/

	int setCWoffFromDectrypProgInfoArray(g_wCurrentEcmPid);

	if(g_wCurrentEcmPid == g_wTrafficEcmPid)
	{
		g_bTrafficHaveCW = false;
		praseMessage(&CAPromptMessage_Traffic, byMesageNo);
		printf("traffic_CA:%s\n",CAPromptMessage_Traffic);
		errorMessageOccur(0x2001);
	}
	else if(g_wCurrentEcmPid == g_wWeatherEcmPid)
	{
		g_bWeatherHaveCW = false;
		praseMessage(&CAPromptMessage_Weather, byMesageNo);
		printf("weather_CA:%s\n",CAPromptMessage_Weather);
		errorMessageOccur(0x2002);
	}
	else if(g_wCurrentEcmPid == g_wFinanceEcmPid)
	{
		g_bFinanceHaveCW = false;
		praseMessage(&CAPromptMessage_Finance, byMesageNo);
		printf("finance_CA:%s\n",CAPromptMessage_Finance);
		errorMessageOccur(0x2003);
	}
	else if(g_wCurrentEcmPid == g_wPOIinfoEcmPid)
	{
		g_bPOIinfoHaveCW = false;
		praseMessage(&CAPromptMessage_POIinfo, byMesageNo);
		printf("poi_CA:%s\n",CAPromptMessage_POIinfo);
		errorMessageOccur(0x2004);
	}
	else if(g_wCurrentEcmPid == g_wSerFileEcmPid)
	{
		g_bSerFileHaveCW = false;
		praseMessage(&CAPromptMessage_DowFile, byMesageNo);
		printf("file_CA:%s\n",CAPromptMessage_DowFile);
		errorMessageOccur(0x2005);
	}
	else if(g_wCurrentEcmPid == g_wCarParkEcmPid)
	{
		g_bCarParkHaveCW = false;
		praseMessage(&CAPromptMessage_CarPark, byMesageNo);
		printf("CarPark_CA:%s\n",CAPromptMessage_CarPark);
		//errorMessageOccur(0x2005);
	}
	else if(g_wCurrentEcmPid == g_wWeatherFileEcmPid)
	{
		g_bWeatherFileHaveCW = false;
		praseMessage(&CAPromptMessage_WeatherFile, byMesageNo);
		printf("WeatherFile_CA:%s\n",CAPromptMessage_WeatherFile);
		//errorMessageOccur(0x2005);
	}
	else if(g_wCurrentEcmPid == g_wRoadConditionEcmPid)
	{
		g_bRoadConditionHaveCW = false;
		praseMessage(&CAPromptMessage_RoadCondition, byMesageNo);
		printf("RoadCondition_CA:%s\n",CAPromptMessage_RoadCondition);
		//errorMessageOccur(0x2005);
	}
	else if(g_wCurrentEcmPid == g_wEmergencyBroadcastEcmPid)
	{
		g_bEmergencyBroadcastHaveCW = false;
		praseMessage(&CAPromptMessage_EmergencyBroadcast, byMesageNo);
		printf("EmergencyBroadcast_CA:%s\n",CAPromptMessage_EmergencyBroadcast);
		//errorMessageOccur(0x2005);
	}
	else if(g_wCurrentEcmPid == g_wTravelFileEcmPid)
	{
		g_bTravelFileHaveCW = false;
		praseMessage(&CAPromptMessage_TravelFile, byMesageNo);
		printf("TravelFile_CA:%s\n",CAPromptMessage_TravelFile);
		//errorMessageOccur(0x2005);
	}
	else if(g_wCurrentEcmPid == g_wEmergencyBroadcastEcmPid)
	{
		g_bExpoFileHaveCW = false;
		praseMessage(&CAPromptMessage_ExpoFile, byMesageNo);
		printf("EmergencyBroadcast_CA:%s\n",CAPromptMessage_ExpoFile);
		//errorMessageOccur(0x2005);
	}
/*
	//char * str;
	switch(byMesageNo)
	{
	case DVTCA_TVS_NOT_FOUND:
		CAPromptMessage = "the ISP key donnot exist!";
		break;
	case DVTCA_KEY_NOT_FOUND:
		CAPromptMessage = "the ISP does not exist!";
		break;
	case DVTCA_DECRYPT_FAILURE:
		CAPromptMessage = "encrypted program!";
		break;
	case DVTCA_NO_ENTITLE:
		CAPromptMessage = "donnot buy this program!";
		break;
	case DVTCA_SC_IS_FORBIDDEN:
		CAPromptMessage = "forbidden card!";
		break;
	case DVTCA_DATA_INVALID:
		CAPromptMessage = "cannot receive program!";
		break;
	default:
		CAPromptMessage = "unknown!";
		break;
	}
*/
	//��str��ʾ����
	//printf("%s\n", str);
//	printf("%s\n", CAPromptMessage);
	//printf("CAPromptMessage address:%x\n", &CAPromptMessage);
	//printf("CAPromptMessage =:%x\n", CAPromptMessage);
	
}

/*++
���ܣ���ӡ������Ϣ��
������
	pszMsg:					������Ϣ���ݡ�
--*/
void DVTSTBCA_AddDebugMsg(const char *pszMsg)
{
	char * str = "ca module: ";
	//��������Ϣ��ʾ����
	//......
	printf("%s%s\n", str, pszMsg);
}

void DVTSTBCA_HidePromptMessage(void)
{
	//�����ʾ��Ϣ
}

void InitialDataECMPid(void)
{
	g_wTrafficEcmPid = getDataECM(258);
	g_wWeatherEcmPid = getDataECM(259);
	g_wFinanceEcmPid = getDataECM(260);
	g_wPOIinfoEcmPid = getDataECM(265);
	g_wSerFileEcmPid = getDataECM(264);//WORD	g_wSerFileEcmPid = 4117;

	g_wCarParkEcmPid = getDataECM(257);
	g_wWeatherFileEcmPid = getDataECM(263);
	g_wRoadConditionEcmPid = getDataECM(262);
	g_wEmergencyBroadcastEcmPid = getDataECM(270);

	g_wTravelFileEcmPid = getDataECM(267);
	g_wExpoFileEcmPid = getDataECM(266);
}
