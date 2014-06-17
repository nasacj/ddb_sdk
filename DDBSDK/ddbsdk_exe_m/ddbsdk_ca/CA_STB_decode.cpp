// CA_STB_DEMODlg.cpp : implementation file
//

#include <stdio.h>
#include <winbase.h> 

#include "CA_STB_decode.h"
#include "DVTCAS_STBInf.h"
#include "Rijndael.h"
#include "../src/ddbsdk.h"

// #define DMB_LOCK	EnterCriticalSection(&g_cs)
// #define DMB_UNLOCK	LeaveCriticalSection(&g_cs)
// extern CRITICAL_SECTION g_cs;

//HANDLE eventSignal = CreateEvent(NULL, TRUE, TRUE, TEXT("CopyInfoSignal"));
//extern HANDLE readSignal;

extern int copyFile(const char*, const char*);

extern ddbsdk_t   ddbsdk;

//extern BYTE g_OddCW[16];
//extern BYTE g_EvenCW[16];

extern WORD	g_wCurrentEcmPid;

extern BYTE	g_TrafficOddCW[16];	
extern BYTE	g_TrafficEvenCW[16];

extern BYTE	g_FinanceOddCW[16];	
extern BYTE	g_FinanceEvenCW[16];

extern BYTE	g_POIinfoOddCW[16];	
extern BYTE	g_POIinfoEvenCW[16];
	
extern BYTE	g_WeatherOddCW[16];	
extern BYTE	g_WeatherEvenCW[16];

extern BYTE	g_SerFileOddCW[16];	
extern BYTE	g_SerFileEvenCW[16];

extern BYTE	g_CarParkOddCW[16];	
extern BYTE	g_CarParkEvenCW[16];

extern BYTE	g_WeatherFileOddCW[16];	
extern BYTE	g_WeatherFileEvenCW[16];

extern BYTE	g_RoadConditionOddCW[16];	
extern BYTE	g_RoadConditionEvenCW[16];

extern BYTE	g_EmergencyBroadcastOddCW[16];	
extern BYTE	g_EmergencyBroadcastEvenCW[16];

extern BYTE	g_TravelFileOddCW[16];	
extern BYTE	g_TravelFileEvenCW[16];

extern BYTE	g_ExpoFileOddCW[16];	
extern BYTE	g_ExpoFileEvenCW[16];

extern bool	g_bTrafficHaveCW;
extern bool	g_bWeatherHaveCW;
extern bool	g_bFinanceHaveCW;
extern bool	g_bPOIinfoHaveCW;
extern bool	g_bSerFileHaveCW;
extern bool	g_bCarParkHaveCW;
extern bool	g_bWeatherFileHaveCW;
extern bool	g_bRoadConditionHaveCW;
extern bool	g_bEmergencyBroadcastHaveCW;
extern bool	g_bTravelFileHaveCW;
extern bool	g_bExpoFileHaveCW;

BYTE g_byszWriteBuf[188 * 100];
//BYTE* g_byszWriteBuf = (BYTE*)malloc(188*BUFFER_SIZE);
//BYTE* g_byszWriteBuf;
int g_nWriteBufIndex = 0;
//CFile g_plainFile;

extern BYTE * g_BufferForCAS;
#ifndef FOR_CAS_DATA_BUFFER_SIZE
#define FOR_CAS_DATA_BUFFER_SIZE	64*1024
#endif

long CA_Init(void)
{
	FILE * cafile;
	char   capath[256];

	sprintf(capath, "%s%s\\CaFile.data", ddbsdk.ddbsdk_path_main, DDBSDK_MAINPATH_NAME);
	//if((cafile = fopen(capath, "rb")) == NULL) {
	if((cafile = fopen("\\Flash_Storage\\softcard.ca", "rb")) == NULL) {
		if (1 <= copyFile("\\NANDFlash\\softcard.ca", "\\Flash_Storage\\softcard.ca"))
		{
			printf("CA文件还原失败！\n");
			return -1;
		}
		if((cafile = fopen("\\Flash_Storage\\softcard.ca", "rb")) == NULL) 
		{
			printf("CA文件无法打开！\n");
			return -1;
		}

	}

	printf("CA文件成功打开！\n");
	
	g_BufferForCAS = (BYTE *)malloc(FOR_CAS_DATA_BUFFER_SIZE);
	memset(g_BufferForCAS, 0, FOR_CAS_DATA_BUFFER_SIZE);
	fread(g_BufferForCAS, 1, FOR_CAS_DATA_BUFFER_SIZE, cafile);
	fclose(cafile);
	if(!DVTCASTB_Init())
	{
		printf("CA初始化失败！\n");
		printf("尝试还原CA文件\n");
		if (1 <= copyFile("\\NANDFlash\\softcard.ca", "\\Flash_Storage\\softcard.ca"))
		{
			printf("CA文件还原失败！\n");
			free(g_BufferForCAS);
			return -1;
		}
		if((cafile = fopen("\\Flash_Storage\\softcard.ca", "rb")) == NULL) 
		{
			printf("CA文件无法打开！\n");
			free(g_BufferForCAS);
			return -1;
		}
		fread(g_BufferForCAS, 1, FOR_CAS_DATA_BUFFER_SIZE, cafile);
		fclose(cafile);
		if(!DVTCASTB_Init())
		{
			printf("CA初始化失败！\n");
			free(g_BufferForCAS);
			return -1;
		}
	}

	printf("CA初始化成功！\n");

	return 0;
}

long CA_initEcmEmmPid(int m_nEcmPid, int m_nEmmPid)
{
	DVTCASTB_SetCurEcmPid((WORD)m_nEcmPid);
	DVTCASTB_SetEmmPid((WORD)m_nEmmPid);

	return 0;
}

long ECM_data_got(WORD ecm_pid, PBYTE ts_buf)
{

	if(ts_buf[0] != 0x47)
		return -1;

	//仅有调整字段
	if(0x20 == (ts_buf[3] & 0x30))
		return -1;

	//载荷前面有调整字段
	WORD wAdaptionLen = 0;
	if(0x30 == (ts_buf[3] & 0x30))
	{
		wAdaptionLen = ts_buf[4];
	}
	BYTE* pStart = (BYTE*)(&ts_buf[0] + wAdaptionLen + 5);
	int nEcmLen = 0;
	while(pStart < (BYTE*)&ts_buf[0] + 188)
	{
		nEcmLen = ((pStart[1] & 0x0f)<<8) | (pStart[2] & 0xFF);
		nEcmLen += 3;
		if(pStart + nEcmLen > (BYTE*)&ts_buf[0] + 188)
		{
			break;
		}
		if(ecm_pid != g_wCurrentEcmPid)
		{
			DVTCASTB_SetCurEcmPid(ecm_pid);//解扰前要设置PID
			g_wCurrentEcmPid = ecm_pid;
		}
		DVTCASTB_StreamGuardDataGot(DVTCA_STB_ECM_TYPE, ecm_pid, nEcmLen, pStart);
		pStart += nEcmLen;
	}
	return 0;
}

long EMM_data_got(WORD emm_pid, PBYTE ts_buf)
{
	DVTCASTB_SetEmmPid(emm_pid);
	if(ts_buf[0] != 0x47)
		return -1;

	//仅有调整字段
	if(0x20 == (ts_buf[3] & 0x30))
		return -1;

	//载荷前面有调整字段
	WORD wAdaptionLen = 0;
	if(0x30 == (ts_buf[3] & 0x30))
	{
		wAdaptionLen = ts_buf[4];
	}
	BYTE* pStart = (BYTE*)(&ts_buf[0] + wAdaptionLen + 5);
	int nEmmLen = 0;
	while(pStart < (BYTE*)&ts_buf[0] + 188)
	{
		nEmmLen = ((pStart[1] & 0x0f) << 8) | (pStart[2] & 0xFF);
		nEmmLen += 3;
		if(pStart + nEmmLen > (BYTE*)&ts_buf[0] + 188)
		{
			break;
		}
		DVTCASTB_StreamGuardDataGot(DVTCA_STB_EMM_TYPE, emm_pid, nEmmLen, pStart);
		pStart += nEmmLen;
	}
	return 0;
}

long   Decrypt_data(PBYTE tsBufOut, PBYTE tsBufIn, void* g_OddCW, void* g_EvenCW)
{
	CRijndael aes;

	//仅有调整字段
	if(0x20 == (tsBufIn[3] & 0x30))
		return -1;

	//载荷前面有调整字段
	WORD wAdaptionLen = 0;
	if(0x30 == (tsBufIn[3] & 0x30))
	{
		wAdaptionLen = tsBufIn[4];
	}

	int DataLen = (188 - 5 - wAdaptionLen) / 16 * 16;

	switch(tsBufIn[3] & 0xc0)
	{
		//加扰的包才需要解扰
		case 0xc0:	//c0用odd key

			memcpy(tsBufOut, tsBufIn, 188);
			aes.MakeKey((char*)g_OddCW, (char*)g_OddCW, 16, 16);
			aes.Decrypt((char*)(&tsBufIn[0]+4+wAdaptionLen), (char*)&tsBufOut[0]+4+wAdaptionLen, DataLen);
			break;
		case 0x80:	//80用even key

			memcpy(tsBufOut, tsBufIn, 188);
			aes.MakeKey((char*)g_EvenCW, (char*)g_EvenCW, 16, 16);
			aes.Decrypt((char*)(&tsBufIn[0]+4+wAdaptionLen), (char*)&tsBufOut[0]+4+wAdaptionLen, DataLen);
			break;
		default:
			break;
	}
	return 0;
}

long   Decrypt_data(PBYTE tsBufOut, PBYTE tsBufIn, deDataType dataType)
{
	switch(dataType)
	{
	case Traffic:
		return Decrypt_data(tsBufOut, tsBufIn, g_TrafficOddCW, g_TrafficEvenCW);
	case Weather:
		return Decrypt_data(tsBufOut, tsBufIn, g_WeatherOddCW, g_WeatherEvenCW);
	case Finance:
		return Decrypt_data(tsBufOut, tsBufIn, g_FinanceOddCW, g_FinanceEvenCW);
	case POI:
		return Decrypt_data(tsBufOut, tsBufIn, g_POIinfoOddCW, g_POIinfoEvenCW);
	case DownloadFile:
		return Decrypt_data(tsBufOut, tsBufIn, g_SerFileOddCW, g_SerFileEvenCW);
	case CarPark:
		return Decrypt_data(tsBufOut, tsBufIn, g_CarParkOddCW, g_CarParkEvenCW);
	case WeatherFile:
		return Decrypt_data(tsBufOut, tsBufIn, g_WeatherFileOddCW, g_WeatherFileEvenCW);
	case RoadCondition:
		return Decrypt_data(tsBufOut, tsBufIn, g_RoadConditionOddCW, g_RoadConditionEvenCW);
	case EmergencyBroadcast:
		return Decrypt_data(tsBufOut, tsBufIn, g_EmergencyBroadcastOddCW, g_EmergencyBroadcastEvenCW);
	case TravelFile:
		return Decrypt_data(tsBufOut, tsBufIn, g_TravelFileOddCW, g_TravelFileEvenCW);
	case ExpoFile:
		return Decrypt_data(tsBufOut, tsBufIn, g_ExpoFileOddCW, g_ExpoFileEvenCW);
	}
	return -2;
}

