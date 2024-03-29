#include <list>
#include <windows.h>
#include "ddbsdk.h"
#include "..\ddbsdk_dmb\Dmb.h"
#include "..\ddbsdk_ca\CA_STB_decode.h"
#include "..\ddbsdk_ca\DVTCAS_STBInf.h"

#include "..\ddbsdk_data\ddbsdk_cat.h"
#include "..\ddbsdk_data\ddbsdk_data.h"
#include "..\ddbsdk_data\ddbsdk_musicam.h"
#include "..\ddbsdk_data\ddbsdk_patpmt.h"
#include "..\ddbsdk_data\ddbsdk_tdt.h"
#include "..\ddbsdk_data\ddbsdk_video.h"

#include "..\ddbsdk_demux\demux_config.h"
#include "..\ddbsdk_demux\demux_if.h"

#include "..\aes\Aes.h"

#include "../log/LocalLog.h"

#include <stdio.h>
#include <string.h>


using namespace std;

static SLConfig_DR sl_config_dr;

int copyFile(const char*, const char*);
static TCHAR	RegPath[] = L"Drivers\\SianoSPI";
static HANDLE   devHandle = NULL;
extern ddbsdk_servFileTab_t   g_servTab;

//CRITICAL_SECTION g_cs; /*变成各个data的独立维护*/
CRITICAL_SECTION serviceListLockS;
CRITICAL_SECTION serviceLockSection;
CRITICAL_SECTION decryStreamAarrayLockSection;
#define SERVICE_LIST_LOCK		EnterCriticalSection(&serviceListLockS)
#define SERVICE_LIST_UNLOCK		LeaveCriticalSection(&serviceListLockS)
/*list<ddbsdk_decryp_stream*>* aaaaryl;*/
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

extern WORD	g_wTrafficEcmPid;
extern WORD	g_wWeatherEcmPid;
extern WORD	g_wFinanceEcmPid;
extern WORD	g_wPOIinfoEcmPid;
extern WORD	g_wSerFileEcmPid;
extern WORD	g_wCarParkEcmPid;
extern WORD	g_wWeatherFileEcmPid;
extern WORD	g_wRoadConditionEcmPid;
extern WORD	g_wEmergencyBroadcastEcmPid;
extern WORD	g_wTravelFileEcmPid;
extern WORD	g_wExpoFileEcmPid;

extern UINT getSignal();

ShowCAPromptMessage CAshowMessage;

extern char*	CAPromptMessage_Traffic;
extern char*	CAPromptMessage_Weather;
extern char*	CAPromptMessage_Finance;
extern char*	CAPromptMessage_POIinfo;
extern char*	CAPromptMessage_DowFile;

extern BYTE *  g_BufferForCAS;


HANDLE messageMutex = NULL;
HANDLE messageFileMap = NULL;
HANDLE messageEvent = NULL;
int InitMessageMapMutex();
int errorMessageOccur(int errorCode);

ddbsdk_t   ddbsdk = { 0 };

list<ddbsdk_decryp_stream*>* decryp_stream_Array = 0;
ddbsdk_decryp_stream* getNodeFromDecrypStreamArray(u16 prom_pid);
BOOL addNodeFromDecrypStreamArray(ddbsdk_decryp_stream* decrypStream);
int deleteNodeFromDecrypStreamArray(u16 program_pid);
int setCWFromDectrypStreamArray(WORD wEcmPID, BYTE byKeyLen, const unsigned char* szOddKey,const unsigned char* szEvenKey);
int putDataFromDectrypStreamArray(PBYTE buf, u16 pid);

list<ddbsdk_decryp_program_info*>* decryp_program_info_Array = 0;
int setCWFromDectrypProgInfoArray(WORD wEcmPID, BYTE byKeyLen, const unsigned char* szOddKey,const unsigned char* szEvenKey);
int setCWoffFromDectrypProgInfoArray(WORD wEcmPID);
ddbsdk_decryp_program_info* getNodeFromDectryProgInfoArray(u16 prom_pid);
int cleanFromDecrypProgInfoArray();


ddbsdk_dmbdataInfo_t    dataInfoList = { 0 };
BOOL getDataInfoList(ddbsdk_dmbdataInfo_t * dataInfo_p);
u16 getDataECM(u16 pid);
u16 getDataEMM(u16 pid);


bool checkSignal_debug = 0;

UINT* quality;
char** caMessage;

#define START_DMBSERVICE(INDEX, FLAG) \
    startDmbService((INDEX), (FLAG)); ddbsdk.serviceIndexCur = (INDEX);


#define DEBUG_TS_FILE 0

#if DEBUG_TS_FILE
FILE * temp_tsfile;
FILE * temp_tsfile_orig;
#endif

static HANDLE hCheckSignal;
extern UINT getSignal();

extern "C" BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);
#define IOCTL_HAL_GET_UUID   CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
int checkAuthorized()
{
	BYTE inBuffer[16] = {0};
	BYTE outBuffer[16] = {0};
	
	HANDLE localFile = CreateFile(L"\\Flash_storage\\ddbsdk.lic", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	int error = GetLastError();
	if (INVALID_HANDLE_VALUE == localFile)
	{
		copyFile("\\Flash_Storage\\ddbsdk.lic.back", "\\Flash_Storage\\ddbsdk.lic");
		localFile = CreateFile(L"\\Flash_Storage\\ddbsdk.lic", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	
	if (INVALID_HANDLE_VALUE != localFile)
//	FILE* licfile = fopen("\\Flash_Storage\\ddbsdk.lic", "r");
//	if (NULL != licfile)
	{
// 		int bytesReaded = fread(inBuffer, 1, 16, licfile);
// 		int errorM = ferror(licfile);
// 		fclose(licfile);
		DWORD bytesReaded;
		BOOL returnM = ReadFile(localFile, inBuffer, 16, &bytesReaded, NULL);
		CloseHandle(localFile);
		if (16 != bytesReaded && returnM == FALSE)
		{
			return -1;
		} 
		else
		{
			BYTE key[16] = {0};
			BOOL bRetVal;
			DWORD dwBytesReturned;
			bRetVal = KernelIoControl (IOCTL_HAL_GET_UUID, NULL, 0, key, 16, &dwBytesReturned);

			Aes aes(16,key);
			aes.InvCipher(inBuffer,outBuffer);
			for(int i = 0; i < 16; i++)
			{
				if (i != outBuffer[i])
				{
					free(inBuffer);
					free(outBuffer);
					free(key);
					return -1;
				} 
			}
			return 0;
		}
		
	} 
	else
	{
		return -1;
	}

}

BOOL checkDate(unsigned short date)
/* check TDT, to get date */
{
	if (ddbsdk.dataService_open)
	{
		/* set the expire: 2010.06.01 */
		if(ddbsdk.mjdDate > date) {
			//if(ddbsdk.mjdDate > 0xd835) {
			printf("DDBSDK过期！\n");
			stopDmbService();
			ddbsdk.dataService_open = 0;
			//WriteLog("ddbsdk过期");
			errorMessageOccur(0x1003);//ddbsdk未授权
			return 0;
		}
		return TRUE;
	}

	if (!ddbsdk.device_open)
	{
		if(DmbStart(ddbsdk.frequency) != 0) {
			DmbStop();
			printf("dmb start failed!\n");
			errorMessageOccur(0x1);
			return 0;
		}
		//ddbsdk.device_open = 1;
	}
	START_DMBSERVICE(4, ddbsdk.testInSMGLabFlag);

	u8   tempBuf[188];
	unsigned long  retBytes = 0;
	u32   timeOutCount = 0;

	ddbsdk.hTDT = new ddbsdk_tdt;
	ddbsdk.tdt_open = 1;
	((ddbsdk_tdt *)ddbsdk.hTDT)->prog_pid = 20;
	if(ddbsdk.hTDT) {
		while(retBytes <= 0 && timeOutCount < 10) {
			((ddbsdk_tdt *)ddbsdk.hTDT)->getPlainPkgData(tempBuf, 188, &retBytes);
			timeOutCount ++;
		}
	} else
		return 0;

	ddbsdk.tdt_open = 0;
	delete ddbsdk.hTDT;


	if(timeOutCount >= 10) {
		//DmbStop();
		//ddbsdk.device_open = 0;
		stopDmbService();
		DmbStop();
		//ddbsdk.dataService_open = 0;
		//WriteLog("无法获取时间信息");
		printf("无法获取时间信息\n");
		errorMessageOccur(0x2);
		return 0;
	}

	ddbsdk.mjdDate = (tempBuf[3] << 8) | tempBuf[4];
	ddbsdk.tdtTime = (tempBuf[5] << 16) | (tempBuf[6] << 8) | tempBuf[7];

	/* set the expire: 2010.06.01 */
	if(ddbsdk.mjdDate > date) {
		//if(ddbsdk.mjdDate > 0xd835) {
		printf("DDBSDK过期！\n");
		stopDmbService();
		DmbStop();
		//ddbsdk.dataService_open = 0;
		//WriteLog("ddbsdk过期");
		errorMessageOccur(0x1003);//ddbsdk未授权
		return 0;
	}

	stopDmbService();
	DmbStop();
	return TRUE;
}

void mdj_to_utc(unsigned short MJD, ddbsdk_datetime * dateTime)
{

	MJD -= 409;
	BYTE k = 0;
	unsigned short year = MJD / 365;
	unsigned short day = (MJD % 365) - year/4;
	unsigned short month = 1;
	if ((year%4) == 0)
	{
		k=29;
	} 
	else
	{
		k=28;
	}
	if ((day - 31) > 0)	//1月
	{
		month++;
		day -= 31;
		if ((day - k) > 0)	//2月
		{
			month++;
			day -= k;
			if ((day - 31) > 0)	//3月
			{
				month++;
				day -= 31;
				if ((day - 30) > 0)	//4月
				{
					month++;
					day -= 30;
					if ((day - 31) > 0)	//5月
					{
						month++;
						day -= 31;
						if ((day - 30) > 0)	//6月
						{
							month++;
							day -= 30;
							if ((day - 31) > 0)	//7月
							{
								month++;
								day -= 31;
								if ((day - 31) > 0)	//8月
								{
									month++;
									day -= 31;
									if ((day - 30) > 0)	//9月
									{
										month++;
										day -= 30;
										if ((day - 31) > 0)	//10月
										{
											month++;
											day -= 31;
											if ((day - 30) > 0)	//11月
											{
												month++;
												day -= 30;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	year = year + 1860;
	
	dateTime->year = year;
	dateTime->month = month;
	dateTime->day = day;
}


u64 get_dmbVideoPCR(u8 * tsBuf)
{
	s8 adaptation_field_control;
	s16 adaptation_field_length;
	s8 PCR_flag;
	u64   PCR;

	/* check adapt field flag */
	adaptation_field_control = (tsBuf[3] & 0x30) >> 4;
	if(adaptation_field_control >= 2) {
		adaptation_field_length = tsBuf[4];
		if(adaptation_field_length > 0) {
			PCR_flag = tsBuf[5] & 0x10;
			if(PCR_flag != 0) {
				PCR = ((u64)(tsBuf[6]) << 25) | ((u64)(tsBuf[7]) << 17) | 
					((u64)(tsBuf[8]) << 9) | ((u64)(tsBuf[9]) << 1) | 
					(((u64)(tsBuf[10]) & 0x80) >> 7);
				return PCR;
			}
		}
	}
	return 0;
}

void DataCallback(void*	    ClientPtr, 
						  UINT32	ServiceHandle, 
						  UINT8*	pBuffer, 
						  UINT32	BufferSize )
{
	int   leftBytes = BufferSize;
	UINT8 * tsBuf = pBuffer;
//printf("dataCall!\n");

	if(ddbsdk.ts_stream_open) {
		((ddbsdk_ts_stream *)ddbsdk.ts_Stream)->putPlainTsData(tsBuf, BufferSize);
	}

#if DEBUG_TS_FILE
	fwrite(pBuffer, 1, BufferSize, temp_tsfile_orig);
#endif
#if 0
	if(pBuffer[0] != 0x47 || pBuffer[188] != 0x47  || pBuffer[188 * 2] != 0x47) {
		printf("data donnot sync!\n");
		return;
	}
#endif
	TsHeader_t * tsHeader;
	while(leftBytes > 0) {
		if(ddbsdk.musicam_open) {
			((ddbsdk_musicam *)ddbsdk.hMusicam)->putPlainTsData(tsBuf, BufferSize);
			leftBytes = 0;
		} else {

			tsHeader = (TsHeader_t *)tsBuf;
			if(tsBuf[0] != 0x47)
				break;
			WORD  wPID = (tsHeader->pid_5 << 8) + tsHeader->pid_8;

			if(ddbsdk.finance_open) {
				((ddbsdk_finance *)ddbsdk.hFinance)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.serviceFile_open) {
				((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.filedown_open) {
				((ddbsdk_downloadFile *)ddbsdk.hDownloadFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.weather_open) {
				((ddbsdk_weather *)ddbsdk.hWeather)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.traffic_open) {
				((ddbsdk_traffic *)ddbsdk.hTraffic)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.pioinfo_open) {
				((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.carpark_open) {
				((ddbsdk_carPark *)ddbsdk.hCarPark)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.weatherfile_open) {
				((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.roadconditionsfile_open) {
				((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.emergencybroadcastfile_open) {
				((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.travelfile_open) {
				((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.expofile_open) {
				((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.sysfile_open) {
				((ddbsdk_sysFile *)ddbsdk.hSysFile)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.pat_open) {
				((ddbsdk_pat *)ddbsdk.hPAT)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.pmt_open) {
				((ddbsdk_pmt *)ddbsdk.hPMT)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.cat_open) {
				((ddbsdk_cat *)ddbsdk.hCAT)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.videoScene_open) {
				((ddbsdk_dmbScene *)ddbsdk.hDmbScene)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.videoOdr_open) {
				((ddbsdk_dmbObjectDr *)ddbsdk.hDmbOdr)->putPlainTsData(tsBuf, wPID);
			}
			if(ddbsdk.video_open) {
				((ddbsdk_dmbVideo *)ddbsdk.hDmbVideo)->putPlainTsData(tsBuf, wPID);
				((ddbsdk_dmbAduio *)ddbsdk.hDmbAudio)->putPlainTsData(tsBuf, wPID);
				/* check pcr */
				if(wPID == ddbsdk.pcr_pid) {
					u64 pcr = get_dmbVideoPCR(tsBuf);
					ddbsdk.videoProgramPCR = pcr == 0 ? ddbsdk.videoProgramPCR : pcr;
				}
			}
			if(ddbsdk.tdt_open) {
				((ddbsdk_tdt *)ddbsdk.hTDT)->putPlainTsData(tsBuf, wPID);
			}


			if (ddbsdk.decryp_data_ts_open)
			{
				((ddbsdk_decryp_ts_stream*)ddbsdk.hDecrypDataTs)->putPlainTsData(tsBuf, wPID);
			}

			putDataFromDectrypStreamArray(tsBuf, wPID);


			leftBytes -= 188;
			tsBuf += 188;
		}
	}

}

unsigned long  thread_checkSignal(void * param)
{
	while(1) {
		Sleep(3000);
		if(checkSignal_debug) {
			if(ddbsdk.device_open)checkSignal(ddbsdk.frequency);
			checkSignal_debug = 0;
		}
	}

	return 0;
}

void getDownloadFile_Status(PBYTE pInBuf, PBYTE pOutBuf)
{
	ddbsdk_servFileStatus_t * servStatus = (ddbsdk_servFileStatus_t *)pOutBuf;
	ddbsdk_servFileStatus_t * servStatusIn = (ddbsdk_servFileStatus_t *)pInBuf;
	u32   selFILE_TYPE = servStatusIn->selectedFILE_TYPE;

// 			ddbsdk_servFileTab_t * g_servTab = &((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->g_servTab;
// 			WriteLog("WEATHER_STATUS g_servTab:%x",g_servTab);

	int n, Num, j, k;

	//servStatus->numDownloading = g_servTab.num_down;
	//servStatus->numFinished = g_servTab.num_complete;
	servStatus->numDownloading = 0;
	servStatus->numFinished = 0;
	Num = g_servTab.num_complete + g_servTab.num_down;
	Num = Num > MAXLISTFILEID ? MAXLISTFILEID : Num;
	for(n=0,j=0,k=0; n<Num; n++) {
		if(g_servTab.ddbsdk_downFiles[n].synt.FILE_TYPE == selFILE_TYPE) {
			if(g_servTab.ddbsdk_downFiles[n].numDownloaded < g_servTab.ddbsdk_downFiles[n].synt.Block_num) {
				servStatus->numDownloading += 1;
				servStatus->fileID_downing[j] = g_servTab.ddbsdk_downFiles[n].synt.FILE_ID;
				j++;
			} else {
				servStatus->numFinished += 1;
				servStatus->fileID_finished[k] = g_servTab.ddbsdk_downFiles[n].synt.FILE_ID;
				k++;
			}
		}
	}
}

void getDownloadFile_OneFileInfo(PBYTE pInBuf, PBYTE pOutBuf)
{
	ddbsdk_servFileInfo_t * servInfo = (ddbsdk_servFileInfo_t *)pOutBuf;
	ddbsdk_servFileInfo_t * servInfoIn = (ddbsdk_servFileInfo_t *)pInBuf;
	u32   selFILE_ID = servInfoIn->selectedFILE_ID;

	// 		ddbsdk_servFileTab_t * g_servTab = &((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->g_servTab;
	// 		WriteLog("DATA_ONEFILEINFO g_servTab:%x",g_servTab);

	int n, Num;

	Num = g_servTab.num_complete + g_servTab.num_down;
	Num = Num > MAXLISTFILEID ? MAXLISTFILEID : Num;
	for(n=0; n<Num; n++) {
		if(g_servTab.ddbsdk_downFiles[n].synt.FILE_ID == selFILE_ID) {

			servInfo->FILE_LENGTH = g_servTab.ddbsdk_downFiles[n].synt.FILE_LENGTH;
			servInfo->FILE_TYPE = g_servTab.ddbsdk_downFiles[n].synt.FILE_TYPE;
			servInfo->Version = g_servTab.ddbsdk_downFiles[n].synt.Version;
			servInfo->FILE_VALID_ENDDATE = g_servTab.ddbsdk_downFiles[n].synt.FILE_VALID_ENDDATE;
			servInfo->FILE_VALID_ENDTIME = g_servTab.ddbsdk_downFiles[n].synt.FILE_VALID_ENDTIME;
			servInfo->Block_length = g_servTab.ddbsdk_downFiles[n].synt.Block_length;
			servInfo->Block_num = g_servTab.ddbsdk_downFiles[n].synt.Block_num;
			break;
		}
	}
}

BOOL getDownloadFile_OneBlock_open(PBYTE pInBuf, PDWORD pBytesReturned)
{
	ddbsdk_servFileInfo_t * servInfoIn = (ddbsdk_servFileInfo_t *)pInBuf;
	u32   selFILE_ID = servInfoIn->selectedFILE_ID;
	u32   Num, n;
	char  FILEIDPathGet[256];
	char   pathType[9][16] = 
	{
		PREFIX_PATH_SYSFILE, PREFIX_PATH_MEDIA, PREFIX_PATH_AD, PREFIX_PATH_MUSIC,
		PREFIX_PATH_NET, PREFIX_PATH_POIFOOD, PREFIX_PATH_PROGLIST, PREFIX_PATH_LIST, 
		PREFIX_PATH_OTHER
	};
	u32   typeIndex = servInfoIn->FILE_TYPE <= POIFOODFILE ? servInfoIn->FILE_TYPE : 
		servInfoIn->FILE_TYPE == PROGRAMFILE ? 6 :
		servInfoIn->FILE_TYPE == LISTFILE ? 7 : 8;

		// 		ddbsdk_servFileTab_t * g_servTab = &((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->g_servTab;
		// 		WriteLog("GETONEDATABLOCK_FINISHEDFILE_OPEN g_servTab:%x",g_servTab);

		/* not finished file, return Bytes 0 */
		Num = g_servTab.num_complete + g_servTab.num_down;
		Num = Num > MAXLISTFILEID ? MAXLISTFILEID : Num;
		for(n=0; n<Num; n++) {
			if(g_servTab.ddbsdk_downFiles[n].synt.FILE_ID == selFILE_ID) {

				if(g_servTab.ddbsdk_downFiles[n].numDownloaded < g_servTab.ddbsdk_downFiles[n].synt.Block_num) {
					* pBytesReturned = 0;
					servInfoIn->file_get = NULL;
					return FALSE;
				} else {
					break;
				}
			}
		}

		memset(FILEIDPathGet, 0, sizeof(FILEIDPathGet));
		sprintf(FILEIDPathGet, "%s%s%s\\%x.tmp", ddbsdk.ddbsdk_path_main, DDBSDK_MAINPATH_NAME, 
			pathType[typeIndex], selFILE_ID);
		servInfoIn->file_get = fopen(FILEIDPathGet, "rb");
		if (servInfoIn->file_get != NULL)
		{
			return TRUE;
		} 
		else
		{
			return FALSE;
		}

}

static void getVideoObjectDrPid(dvb_program_property_t * cur_prog_info, ddbsdk_videoInfo_t * info)
{
	s32   OD_ES_ID;
	u16   od_pid;

	if(cur_prog_info->iod_dr.es_dr[0].decoder_dr.streamType == 1) 
		cur_prog_info->od_es_num_in_iod = 0;
	if(cur_prog_info->iod_dr.es_dr[1].decoder_dr.streamType == 1)
		cur_prog_info->od_es_num_in_iod = 1;
	
	OD_ES_ID = cur_prog_info->iod_dr.es_dr[cur_prog_info->od_es_num_in_iod].ES_ID;
	if(cur_prog_info->sub_pid[0].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[0].elementary_pid;
	if(cur_prog_info->sub_pid[1].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[1].elementary_pid;
	if(cur_prog_info->sub_pid[2].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[2].elementary_pid;
	if(cur_prog_info->sub_pid[3].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[3].elementary_pid;

	info->odr_pid = od_pid;
}

static void getVideoPesPid(dvb_program_property_t * cur_prog_info, ddbsdk_videoInfo_t * info, u32& video_pid_num, u32&  audio_pid_num)
{
	u32 i;
	u16 video_id = cur_prog_info->object_dr_video->es_dr.ES_ID;
	u16 audio_id = cur_prog_info->object_dr_audio->es_dr.ES_ID;

	for(i=0; i<4; i++) {
		if(video_id == cur_prog_info->sub_pid[i].ES_ID){
			video_pid_num = i;
			break;
		}
	}
	for(i=0; i<4; i++) {
		if(audio_id == cur_prog_info->sub_pid[i].ES_ID){
			audio_pid_num = i;
			break;
		}
	}
	info->video_pid = cur_prog_info->sub_pid[video_pid_num].elementary_pid;
	info->audio_pid = cur_prog_info->sub_pid[audio_pid_num].elementary_pid;
	info->pcr_pid = cur_prog_info->g_pcr_pid;
}

static bool getVideoProgramList(ddbsdk_videoInfo_t * info) 
{
	/* 
	    get PAT / PMT info
		1. start one component,
		2. open pat stream
		3. open pmt stream
		4. list the video, audio, scene, object descriptor pid
	*/
	unsigned char   tempBuf[256] = {0x47, 0x40, 0x00, 0x10, 0x00};
	unsigned long   retBytes, timeOutCount = 0;
	demux_if_t   demux_param = { 0 };
	u32   video_pid_num;
	u32   audio_pid_num;

	/* 1. start one component */
	if(ddbsdk.test_file_mode == 0) {
		if(ddbsdk.serviceIndexCur != info->ServiceIndex) {
			START_DMBSERVICE(info->ServiceIndex, ddbsdk.testInSMGLabFlag);
		}
	}

	Sleep(3000);
	/* 2. open pat stream, and parse it */
	ddbsdk.hPAT = new ddbsdk_pat;
	ddbsdk.pat_open = 1;
	if(ddbsdk.hPAT) {
		((ddbsdk_pat *)ddbsdk.hPAT)->prog_pid = 0;
		while(1) {
			((ddbsdk_pat *)ddbsdk.hPAT)->getPlainPkgData(&tempBuf[5], 188, &retBytes);
			if(retBytes > 0) {
				break;
			} else {
				timeOutCount ++;
			}
			if(timeOutCount > 100) return false;
		}
	} else
		return false;

	paul_Demux(1,  DEMUX_PAT, &demux_param, tempBuf);
	ddbsdk.pat_open = 0;
	delete ddbsdk.hPAT;
	

	/* 3. open pmt stream, and parse it */
	ddbsdk.hPMT = new ddbsdk_pmt;
	ddbsdk.pmt_open = 1;
	if(ddbsdk.hPMT) {
		timeOutCount = 0;
		((ddbsdk_pmt *)ddbsdk.hPMT)->prog_pid = demux_param.program_inf[0].g_pmt_pid;
		while(1) {
			((ddbsdk_pat *)ddbsdk.hPAT)->getPlainPkgData(&tempBuf[5], 188, &retBytes);
			if(retBytes > 0) {
				break;
			} else {
				timeOutCount ++;
			}
			if(timeOutCount > 100) return false;
		}
	} else
		return false;

	paul_Demux(1,  DEMUX_PMT, &demux_param.program_inf[0], tempBuf);
	ddbsdk.pmt_open = 0;
	delete ddbsdk.hPMT;
	

	getVideoObjectDrPid(&demux_param.program_inf[0], info);

	/* parse ISO 14496-1 Sync Layer Object Descriptor */
	ddbsdk.hDmbOdr = new ddbsdk_dmbObjectDr;
	ddbsdk.videoOdr_open = 1;
	if(ddbsdk.hDmbOdr) {
		timeOutCount = 0;
		((ddbsdk_dmbObjectDr *)ddbsdk.hDmbOdr)->prog_pid = info->odr_pid;
		while(1) {
			((ddbsdk_pat *)ddbsdk.hDmbOdr)->getPlainPkgData(&tempBuf[5], 188, &retBytes);
			if(retBytes > 0) {
				break;
			} else {
				timeOutCount ++;
			}
			if(timeOutCount > 100) return false;
		}
	} else
		return false;

	ddbsdk.videoOdr_open = 0;
	delete ddbsdk.hDmbOdr;
	
	paul_Demux(1, DEMUX_14496_SECTION, &demux_param.program_inf[0], tempBuf);

	getVideoPesPid(&demux_param.program_inf[0], info, video_pid_num, audio_pid_num);

	memcpy(&sl_config_dr, &demux_param.program_inf[0].object_dr_video->es_dr.sl_config_dr, sizeof(SLConfig_DR));

	/* copy the avc/h.264 and aac decoder header parameter to user */
	memcpy(&info->aacDecInfo, &demux_param.program_inf[0].AAC_info, sizeof(demux_param.program_inf[0].AAC_info));
//	info->aacDecInfo.bufferSizeDB = demux_param.program_inf[0].iod_dr.es_dr[audio_pid_num].decoder_dr.bufferSizeDB;
//	info->aacDecInfo.maxBitrate = demux_param.program_inf[0].iod_dr.es_dr[audio_pid_num].decoder_dr.maxBitrate;
//	info->aacDecInfo.avgBitrate = demux_param.program_inf[0].iod_dr.es_dr[audio_pid_num].decoder_dr.avgBitrate;

	info->avcDecInfo.configureationVersion = demux_param.program_inf[0].AVC_info.configureationVersion;
	info->avcDecInfo.AVCprofileIndication = demux_param.program_inf[0].AVC_info.AVCprofileIndication;
	info->avcDecInfo.profile_compatibility = demux_param.program_inf[0].AVC_info.profile_compatibility;
	info->avcDecInfo.AVClevelIndication = demux_param.program_inf[0].AVC_info.AVClevelIndication;
	info->avcDecInfo.lengthSizeMinusOne = demux_param.program_inf[0].AVC_info.lengthSizeMinusOne;

	info->avcDecInfo.numOfSequenceParameterSets = demux_param.program_inf[0].AVC_info.numOfSequenceParameterSets;
	info->avcDecInfo.sequenceParameterLength = demux_param.program_inf[0].AVC_info.sequenceParameterLength;
	memcpy(info->avcDecInfo.sps, demux_param.program_inf[0].AVC_info.sps_ptr, demux_param.program_inf[0].AVC_info.sequenceParameterLength);

	info->avcDecInfo.numOfPictureParamterSets = demux_param.program_inf[0].AVC_info.numOfPictureParamterSets;
	info->avcDecInfo.pictureParameterSetLength = demux_param.program_inf[0].AVC_info.pictureParameterSetLength;
	memcpy(info->avcDecInfo.pps, demux_param.program_inf[0].AVC_info.pps_ptr, demux_param.program_inf[0].AVC_info.pictureParameterSetLength);
//	info->avcDecInfo.bufferSizeDB = demux_param.program_inf[0].iod_dr.es_dr[video_pid_num].decoder_dr.bufferSizeDB;
//	info->avcDecInfo.maxBitrate = demux_param.program_inf[0].iod_dr.es_dr[video_pid_num].decoder_dr.maxBitrate;
//	info->avcDecInfo.avgBitrate = demux_param.program_inf[0].iod_dr.es_dr[video_pid_num].decoder_dr.avgBitrate;

	return true;
}

typedef struct programMap_s
{
	u16   program_num;
	u16   stream_pid;

}programMap_t;

static bool demuxSimple_pat(u8 * buffer, programMap_t map[], u32& progNum)
{
	u32   section_len = ((buffer[1] & 0xF) << 8) | buffer[2];
	u8 *  ptr;
	u32   num;

	progNum = (section_len - 5 - 4) / 4;
	ptr = &buffer[8];
	for(num=0; num<progNum; num++) {

		map[num].program_num = (ptr[0] << 8) | ptr[1];
		map[num].stream_pid = ((ptr[2] & 0x1f) << 8) | ptr[3];
		ptr += 4;
	}
	return true;
}

static bool demuxSimple_pmt(u8 * buffer, ddbsdk_dataInfo_t * info, u8& haveCAFlag)
{
	u32   section_len = ((buffer[1] & 0xF) << 8) | buffer[2];
	u8 *  ptr;
	u32   leftLen, length, left_drLen;
	u32   dr_len = ((buffer[10] & 0xF) << 8) | buffer[11];
	u16   CAsystemID;

	/* get ecm pid */
	ptr = &buffer[12];
	left_drLen = dr_len;
	if(dr_len > 0) {
		while(left_drLen) {
			length = ptr[1];
			if(ptr[0] == 0x9) { /* it's CA descriptor */
				CAsystemID = (ptr[2] << 8) | ptr[3];
				if(DVTCASTB_CASIDVerify(CAsystemID) == true) {  /* 0x4ad3: the SUMAVision's soft CA card system */
					info->ecm_pid = ((ptr[4] & 0x1F) << 8) | ptr[5];
					haveCAFlag = 1;
					break;
				}
			}
			left_drLen -= length;
			ptr += length;
		}
	}
	ptr = &buffer[12 + dr_len];
	leftLen = section_len - 12 - dr_len;
	if(leftLen > 0) {
		/* get elementary pid */
		info->stream_type = ptr[0];
		switch(info->stream_type) {
		case 0xdb:
			strcpy(info->descr, "traffic data");
			break;
		case 0xdc:
			strcpy(info->descr, "weather data");
			break;
		case 0xdd:
			strcpy(info->descr, "finance data");
			break;
		case 0xde:
			strcpy(info->descr, "system file data");
			break;
		case 0xdf:
			strcpy(info->descr, "file data");
			break;
		case 0xe1:
			strcpy(info->descr, "PIO info data");
			break;
		case 0xfc:
			strcpy(info->descr, "file list data");
			break;
		}
		info->data_pid = ((ptr[1] & 0x1F) << 8) | ptr[2];
		length = 5 + ptr[4];
		leftLen -= length;
		ptr += length;
	}
	return true;
}

static bool demuxSimple_cat(u8 * buffer, u16& emm_pid)
{
	u32   section_len = ((buffer[1] & 0xF) << 8) | buffer[2];
	u8 *  ptr;
	u32   length, left_drLen;
	u16   CAsystemID;

	/* get ecm pid */
	ptr = &buffer[8];
	left_drLen = section_len - 8;
	if(left_drLen > 0) {
		while(left_drLen) {
			length = ptr[1];
			if(ptr[0] == 0x9) { /* it's CA descriptor */
				CAsystemID = (ptr[2] << 8) | ptr[3];
				if(DVTCASTB_CASIDVerify(CAsystemID) == true) {  /* 0x4ad3: the SUMAVision's soft CA card system */
					emm_pid = ((ptr[4] & 0x1F) << 8) | ptr[5];
					break;
				}
			}
			left_drLen -= length;
			ptr += length;
		}
	}
	return true;
}

static bool getDataProgramList(ddbsdk_dataInfo_t * &dataInfo, u32& ChServiceNum) 
{
	/* 
	    get PAT / PMT info
		1. start one component,
		2. open pat stream
		3. open pmt stream
		4. list the data pid
	*/
	unsigned char   tempBuf[188] = { 0 };
	unsigned long   retBytes, timeOutCount = 0;
	programMap_t   progMap[16];
	u32   progNum, num;
	ddbsdk_dataInfo_t * info = dataInfo, * infoTemp = dataInfo;

	/* 1. start one component */
	info->serviceNo = 0;
	info->ServiceIndex = getDataServerIndex(0);
	if(ddbsdk.test_file_mode == 0) {
		if(ddbsdk.serviceIndexCur != info->ServiceIndex) {
			START_DMBSERVICE(info->ServiceIndex, ddbsdk.testInSMGLabFlag);
		}
	}
	/* 2. open pat stream, and parse it */
	retBytes = 0;
	ddbsdk.hPAT = new ddbsdk_pat;
	ddbsdk.pat_open = 1;
	if(ddbsdk.hPAT) {
		((ddbsdk_pat *)ddbsdk.hPAT)->prog_pid = 0;
		while(1) {
			((ddbsdk_pat *)ddbsdk.hPAT)->getPlainPkgData(tempBuf, 188, &retBytes);
			if(retBytes > 0) {
				break;
			} else {
				timeOutCount ++;
			}
			if(timeOutCount > 10) return false;
		}
	} else
		return false;

	demuxSimple_pat(tempBuf, progMap, progNum);
	delete ddbsdk.hPAT;
	ddbsdk.pat_open = 0;

	/* 3. open pmt stream, and parse it */
	u8 haveCAFlag = 0;
	retBytes = 0;
	ddbsdk.hPMT = new ddbsdk_pmt;
	ddbsdk.pmt_open = 1;
	for(num=0; num<progNum; num++) {
		if(ddbsdk.hPMT) {
			((ddbsdk_pmt *)ddbsdk.hPMT)->prog_pid = progMap[num].stream_pid;
			while(1) {
				((ddbsdk_pat *)ddbsdk.hPAT)->getPlainPkgData(tempBuf, 188, &retBytes);
				if(retBytes > 0) {
					break;
				} else {
					timeOutCount ++;
				}
				if(timeOutCount > 10) return false;
			}
		} else
			return false;

		demuxSimple_pmt(tempBuf, info, haveCAFlag);
		info->serviceNo = num;
		info->ServiceIndex = getDataServerIndex(num);

		info->next = info + 1;
		info = info->next;
	}
	delete ddbsdk.hPMT;
	ddbsdk.pmt_open = 0;

	dataInfo = info - 1;

	info = infoTemp;
	u32   programNum = num;
	u16   emm_pid;
	ChServiceNum = programNum;
	retBytes = 0;
	if(haveCAFlag != 0) { /* if have ECM PID, to check CA for EMM pid */
		ddbsdk.hCAT = new ddbsdk_cat;
		ddbsdk.cat_open = 1;
		if(ddbsdk.hCAT) {
			((ddbsdk_cat *)ddbsdk.hCAT)->prog_pid = 1;
			while(1) {
				((ddbsdk_cat *)ddbsdk.hCAT)->getPlainPkgData(tempBuf, 188, &retBytes);
				if(retBytes > 0) {
					break;
				} else {
					timeOutCount ++;
				}
				if(timeOutCount > 10) return true;
			}
		} else
			return false;

		demuxSimple_cat(tempBuf, emm_pid);

		for(num=0; num<programNum; num++) {
			info->emm_pid = emm_pid;
			info->next = info + 1;
			info = info->next;
		}
		delete ddbsdk.hCAT;
		ddbsdk.cat_open = 0;
	}
	return true;
}


DWORD WINAPI DDB_Init(
  LPCTSTR pContext,
  DWORD dwBusContext
)
{
	if(devHandle)
	{
		return 0;
	}

	InitializeLog(L"ddbsdk_log.log");

	if(0 != InitMessageMapMutex())
		WriteLog("InitMessageMapMutex ERROR!");
	
	if (0 != checkAuthorized())
	{
		ddbsdk.isAuthorized = 0;
		errorMessageOccur(0x1003);
		printf("DDBSDK未授权！\n");
	}
	else
	{
		ddbsdk.isAuthorized = 1;
	}

	//copyFile("\\Flash_Storage\\softcard.ca", "\\Flash_Storage\\softcard.ca.back");
	copyFile("\\Flash_Storage\\ddbsdk.lic", "\\Flash_Storage\\ddbsdk.lic.back");

	InitializeCriticalSection(&serviceLockSection);
	InitializeCriticalSection(&serviceListLockS);
	InitializeCriticalSection(&decryStreamAarrayLockSection);

	decryp_stream_Array = new std::list<ddbsdk_decryp_stream*>;

	ddbsdk.frequency = 1470080000;
#ifdef DDBSDK_PLATFORM_WLD_H
	//-----------------点点通设备的激活方式：-------------------------//
	HANDLE	 hDevSpi = CreateFile(TEXT("SPI1:"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	HANDLE	 hDevSgd = CreateFile(TEXT("SGD1:"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	//printf("%x, %x\n", hDevSpi, hDevSgd);
	if(hDevSpi == INVALID_HANDLE_VALUE || hDevSgd == INVALID_HANDLE_VALUE) {
		devHandle = ActivateDevice(RegPath, 0);
		if(devHandle == NULL) {
			printf("激活SPI失败！\n");
			errorMessageOccur(0x1002);
			return 0;
		}
	} else {
		CloseHandle(hDevSpi);
		CloseHandle(hDevSgd);
		hDevSpi = NULL;
		hDevSgd = NULL;
	}
	Sleep(1000);
#endif
#ifdef DDBSDK_PLATFORM_GUOWEI_H
	//-----------------------国微机设备的激活方式----------------------//
	if ((devHandle = SianoPowerOn()) == NULL)
	{
		errorMessageOccur(0x1002);
		printf("激活SPI失败！\n");
		return 0;
	}
#endif
#ifdef DDBSDK_PLATFORM_MSD_H
	devHandle = (HANDLE)1;
#endif


	/* initiate default ddbsdk path */
	strcpy(ddbsdk.ddbsdk_path_main, DDBSDK_MAINPATH_DEFAULT);
	ddbsdk.mjdDate = 0xffff;
	ddbsdk.tdtTime = 0x00125959;

// 	if (!ddbsdk.isAuthorized)
// 	{
// 		if (!checkDate(0xd835))
// 		{
// 			printf("DDBSDK未授权！并且过期！\n");
// 			return 0;
// 		}
// 	}
	
	printf("init succeed\n");

	return (DWORD)devHandle;/*	return DWORD(1); */


}


BOOL WINAPI
DllEntry(HANDLE hInstDll, DWORD dwReason, LPVOID lpvReserved)
{
	printf("This is DllEntry\n");
    switch ( dwReason ) 
	{
    case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls((HMODULE) hInstDll);
        break;
    }
    return (TRUE);
}

BOOL WINAPI
DDB_Close(
    DWORD Handle
    )
{
	printf("This is DAB_Close\n");

//	if(devHandle) 
//		DeactivateDevice(devHandle);

	

	CloseHandle(hCheckSignal);

#if DEBUG_TS_FILE
	fclose(temp_tsfile);
	fclose(temp_tsfile_orig);
#endif
    return TRUE;
}

BOOL WINAPI
DDB_Deinit(
    DWORD dwContext     // future: pointer to the per disk structure
    )
{
	printf("This is DAB_Deinit:");

	DeleteCriticalSection(&serviceLockSection);
	DeleteCriticalSection(&serviceListLockS);
	DeleteCriticalSection(&decryStreamAarrayLockSection);

	free(g_BufferForCAS);
	delete decryp_stream_Array;

#ifdef DDBSDK_PLATFORM_WLD_H
	if(devHandle) 
		DeactivateDevice(devHandle);
#endif
#ifdef DDBSDK_PLATFORM_GUOWEI_H
	SianoPowerDown();
#endif

	return TRUE;
}

DWORD WINAPI
DDB_Open(
    DWORD dwData,
    DWORD dwAccess,
    DWORD dwShareMode
    )
{
	DWORD dwThreadID;
	DWORD nParameter;
	printf("This is DAB_Open==========>\n");


	if (checkSignal_debug)
	{
		hCheckSignal = CreateThread(NULL, 0, thread_checkSignal, &nParameter, 0, &dwThreadID);
	}
	

#if DEBUG_TS_FILE
	/* open a file to save ts */
	temp_tsfile = fopen("\\temp\\dump.ts", "wb");
	temp_tsfile_orig = fopen("\\temp\\dump_orig.ts", "wb");
#endif
    return DWORD(1);
}

BOOL WINAPI
DDB_IOControl(
    DWORD Handle,
    DWORD dwIoControlCode,
    PBYTE pInBuf,
    DWORD nInBufSize,
    PBYTE pOutBuf,
    DWORD nOutBufSize,
    PDWORD pBytesReturned
    )
{
	//printf("---------DDB_IOControl-----------");
	switch (dwIoControlCode)
	{
	case IOCTL_DDBSDK_RECONFIGURE:
		{
		ddbsdk_reconfigure_t * configParamPtr = (ddbsdk_reconfigure_t *)pInBuf;
		ddbsdk.frequency = configParamPtr->frequency;
		ddbsdk.testInSMGLabFlag = configParamPtr->testInSMGLabFlag;
		if(configParamPtr->ddbsdk_path_main[0] != '\0') {
			strcpy(ddbsdk.ddbsdk_path_main, configParamPtr->ddbsdk_path_main);
		}
		}
		break;
	case IOCTL_DDBSDK_VERSIONINFO:
		{
		unsigned long   caVersion, caCardNo;
		u8    tvsCount = 1;
		u8    entitleCount = 1;
		SDVTCATvsInfo  tvsInfo = { 0 };
		SDVTCAServiceEntitle  entitleInfo = { 0 };
		unsigned long  ret;

		ddbsdk_versionInfo_t * versionParamPtr = (ddbsdk_versionInfo_t *)pOutBuf;
		if(ddbsdk.ca_init_ok == 0) {
			if(CA_Init() != 0) {
				printf("ca init failed!\n");
				WriteLog("ca init failed!");
				errorMessageOccur(0x1001);
				return 0;
			}
			ddbsdk.ca_init_ok = 1;
		}
		memset(versionParamPtr, 0, sizeof(ddbsdk_versionInfo_t));
		versionParamPtr->ddbsdk_version = DDBSDK_VERSION;
		if(DVTCASTB_GetVer(&caVersion) == true) {
			versionParamPtr->ca_version = caVersion;
		} else {
			versionParamPtr->ca_version = 0;
		}
		if(DVTCASTB_GetCardID(&caCardNo) == true) {
			versionParamPtr->ca_card_no = caCardNo;
		} else {
			versionParamPtr->ca_card_no = 0;
		}
		if(DVTCASTB_GetCASManuName(versionParamPtr->ca_ManuName) == false) {
			memset(versionParamPtr->ca_ManuName, 0, sizeof(versionParamPtr->ca_ManuName));
		}
		if(DVTCASTB_GetTvsInfo(&tvsCount, &tvsInfo) == true) {
			strncpy(versionParamPtr->ca_TvsName, "SMG"/* tvsInfo.m_szTVSName */, DVTCA_MAXLEN_TVSNAME);
		}else {
			memset(versionParamPtr->ca_TvsName, 0, sizeof(versionParamPtr->ca_TvsName));
		}
		/*这个抛弃，新增接口
		if((ret = DVTCASTB_GetServiceEntitles(tvsInfo.m_wTVSID, &entitleCount, &entitleInfo)) == DVTCA_OK) {
			u8 * ptr = (u8 *)&entitleInfo;
			versionParamPtr->ca_productID = entitleInfo.m_wProductID;
			versionParamPtr->ca_entitleTime = ptr[2] | (ptr[3] << 8) | (ptr[4] << 16) | (ptr[5] << 24);
			versionParamPtr->ca_startTime = ptr[6] | (ptr[7] << 8) | (ptr[8] << 16) | (ptr[9] << 24);
			versionParamPtr->ca_endTime = ptr[10] | (ptr[11] << 8) | (ptr[12] << 16) | (ptr[13] << 24);
		} else {
			versionParamPtr->ca_entitleTime = 0;
			versionParamPtr->ca_startTime = 0;
			versionParamPtr->ca_endTime = 0;
		}*/
		}
		break;

	case IOCTL_DDBSDK_CA_AUTHORIZED_INFO:
		{
		SDVTCATvsInfo  tvsInfos = { 0 };
		u8    tvsCount = 1;
		SDVTCAServiceEntitle* entitleInfos = (SDVTCAServiceEntitle*)pOutBuf;
		u8  entitleCount = nOutBufSize / (sizeof(SDVTCAServiceEntitle));
		//printf("entitleCount = %d\n", entitleCount);
		unsigned long  ret;
		if(DVTCASTB_GetTvsInfo(&tvsCount, &tvsInfos) == true) 
		{
			if((ret = DVTCASTB_GetServiceEntitles(tvsInfos.m_wTVSID, &entitleCount, entitleInfos)) == DVTCA_OK)
			{
				*pBytesReturned = entitleCount;
				//printf("entitleCount = %d\n", entitleCount);
				return TRUE;
			}
		}
		else
			return FALSE;
		}
		break;

	case IOCTL_DDBSDK_DMB_START_DEVICE:
		if (0 == ddbsdk.device_open)
		{
			if(DmbStart(ddbsdk.frequency) != 0) {
				DmbStop();
				WriteLog("dmb start failed!\n");
				printf("dmb start failed!\n");
				errorMessageOccur(0x1);
				return 0;
			}
			ddbsdk.device_open = 1;
			
		}
		//ddbsdk.device_open_count++;
		break;

	case IOCTL_DDBSDK_DMB_START_SERVICE:
		{
			int serviceNo = *((int *)pInBuf);

			if (ddbsdk.serviceIndexCur != serviceNo)
			{
				if (startDmbService(serviceNo, ddbsdk.testInSMGLabFlag) != 0)
				{
					return FALSE;
				}
				ddbsdk.serviceIndexCur = serviceNo;
			}
		}
		break;

	case IOCTL_DDBSDK_DMB_STOP_SERVICE:
		stopDmbService();
		ddbsdk.serviceIndexCur = -1;
		break;

	case IOCTL_DDBSDK_DMB_STOP_DEVICE:
		DmbStop();
		//ddbsdk.device_open_count = 0;
		ddbsdk.dataService_open = 0;
		ddbsdk.device_open = 0;
		break;

	case IOCTL_DDBSDK_OPEN_DMBDEVICE:

		if (1 == ddbsdk.device_open)
		{
			//ddbsdk.device_open_count++;
			//break;
		}
		else
		{
			/* initiate the DMB fronter */
			if(DmbStart(ddbsdk.frequency) != 0) {
				DmbStop();
				printf("dmb start failed!\n");
				errorMessageOccur(0x1);
				return 0;
			}
			ddbsdk.device_open = 1;
		}

		if (1 == ddbsdk.dataService_open)
		{
			break;
		}

		START_DMBSERVICE(4, ddbsdk.testInSMGLabFlag);
		ddbsdk.serviceIndexCur = 4;

		/* initiate the CA System */
		if(ddbsdk.ca_init_ok == 0) {
		if(CA_Init() != 0) {
			printf("ca init failed!\n");
			errorMessageOccur(0x1001);
				return 0;
			}
		}
		ddbsdk.ca_init_ok = 1;

		/* check TDT, to get date */
		{
			u8   tempBuf[188];
			unsigned long  retBytes = 0;
			u32   timeOutCount = 0;

			ddbsdk.hTDT = new ddbsdk_tdt;
			ddbsdk.tdt_open = 1;
			((ddbsdk_tdt *)ddbsdk.hTDT)->prog_pid = 20;
			if(ddbsdk.hTDT) {
				while(retBytes <= 0 && timeOutCount < 10) {
					((ddbsdk_tdt *)ddbsdk.hTDT)->getPlainPkgData(tempBuf, 188, &retBytes);
					timeOutCount ++;
				}
			} else
			{
				ddbsdk.tdt_open = 0;
				return 0;
			}

			ddbsdk.tdt_open = 0;
			delete ddbsdk.hTDT;


			if(timeOutCount >= 10) {
				//DmbStop();
				//ddbsdk.device_open = 0;
				stopDmbService();
				ddbsdk.dataService_open = 0;
				WriteLog("无法获取时间信息");
				printf("无法获取时间信息\n");
				errorMessageOccur(0x2);
				return 0;
			}

			ddbsdk.mjdDate = (tempBuf[3] << 8) | tempBuf[4];
			ddbsdk.tdtTime = (tempBuf[5] << 16) | (tempBuf[6] << 8) | tempBuf[7];			
		}

		if (!ddbsdk.isAuthorized)
		{
			/* set the expire: 2010.08.01 */
			if(ddbsdk.mjdDate > 0xd871) {
				printf("DDBSDK过期！\n");
				stopDmbService();
				ddbsdk.dataService_open = 0;
				WriteLog("ddbsdk过期");
				return 0;
			}
		}

		
		//ddbsdk.device_open_count++;
		ddbsdk.dataService_open = 1;
		printf("DMB DATA Service启动成功！\n");
		break;

	case IOCTL_DDBSDK_CLOSE_DMBDEVICE:
		if (ddbsdk.carpark_open == 1 || ddbsdk.cat_open == 1 || ddbsdk.emergencybroadcastfile_open == 1 || \
			ddbsdk.filedown_open == 1 || ddbsdk.finance_open == 1 || ddbsdk.pat_open == 1 || ddbsdk.pioinfo_open == 1 || \
			ddbsdk.pmt_open == 1 || ddbsdk.roadconditionsfile_open == 1 || ddbsdk.serviceFile_open == 1 || \
			ddbsdk.tdt_open == 1 || ddbsdk.traffic_open == 1 || ddbsdk.weather_open == 1 || \
			ddbsdk.weatherfile_open == 1)
		{
			return 0;
		} 
		stopDmbService();
		ddbsdk.dataService_open = 0;
		break;

	case IOCTL_DDBSDK_GET_DATETIME:
		{
			/* check TDT, to get date */
			
			u8   tempBuf[188];
			unsigned long  retBytes = 0;
			u32   timeOutCount = 0;

			ddbsdk.hTDT = new ddbsdk_tdt;
			ddbsdk.tdt_open = 1;
			((ddbsdk_tdt *)ddbsdk.hTDT)->prog_pid = 20;
			if(ddbsdk.hTDT) {
				while(retBytes <= 0 && timeOutCount < 10) {
					((ddbsdk_tdt *)ddbsdk.hTDT)->getPlainPkgData(tempBuf, 188, &retBytes);
					timeOutCount ++;
				}
			} else
			{
				ddbsdk.tdt_open = 0;
				return 0;
			}

			ddbsdk.tdt_open = 0;
			delete ddbsdk.hTDT;


			if(timeOutCount >= 10) 
			{
				WriteLog("无法获取时间信息");
				printf("无法获取时间信息\n");
				errorMessageOccur(0x2);
				return 0;
			}

			ddbsdk.mjdDate = (tempBuf[3] << 8) | tempBuf[4];
			ddbsdk.tdtTime = (tempBuf[5] << 16) | (tempBuf[6] << 8) | tempBuf[7];

			ddbsdk_datetime * datetime_out = (ddbsdk_datetime *)pOutBuf;
			
			mdj_to_utc(ddbsdk.mjdDate, datetime_out);

			unsigned char hour = ((ddbsdk.tdtTime & 0xFF0000) >> 16);
			unsigned char minute = ((ddbsdk.tdtTime & 0xFF00) >> 8);
			unsigned char second = ddbsdk.tdtTime & 0xFF;
			hour = ((hour & 0xF0) >> 4) * 10 + (hour & 0x0F);
			minute = ((minute & 0xF0) >> 4) * 10 + (minute & 0x0F);
			second = ((second & 0xF0) >> 4) * 10 + (second & 0x0F);
			datetime_out->hour = hour;
			datetime_out->minute = minute;
			datetime_out->second = second;
		}
		break;

	case IOCTL_DDBSDK_TS_STREAM_OPEN :
		ddbsdk.ts_Stream = new ddbsdk_ts_stream;
		ddbsdk.ts_stream_open = 1;
		break;
	case IOCTL_DDBSDK_TS_STREAM_GET :
		if(ddbsdk.ts_Stream) {
			return ((ddbsdk_ts_stream *)ddbsdk.ts_Stream)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else{
			*pBytesReturned = 0;
			return 0;
		}
			
	case IOCTL_DDBSDK_TS_STREAM_CLOSE :
		ddbsdk.ts_stream_open = 0;
		delete ddbsdk.ts_Stream;
		
		break;


	case IOCTL_DDBSDK_DMBSIGNAL_QUALITY:
		//printf("IOCTL_DDBSDK_DMBSIGNAL_QUALITY\n");
		quality = (UINT*)pOutBuf;
		*quality = getSignal();
		*pBytesReturned = sizeof(UINT);
		break;

	case IOCTL_DDBSDK_GETCA_PROMPTMSG_TRAFFIC:
		strcpy((char*)pOutBuf, CAPromptMessage_Traffic);
		*pBytesReturned = strlen(CAPromptMessage_Traffic);
		break;
	case IOCTL_DDBSDK_GETCA_PROMPTMSG_WEATHER:
		strcpy((char*)pOutBuf, CAPromptMessage_Weather);
		*pBytesReturned = strlen(CAPromptMessage_Weather);
		break;
	case IOCTL_DDBSDK_GETCA_PROMPTMSG_FINANCE:
		strcpy((char*)pOutBuf, CAPromptMessage_Finance);
		*pBytesReturned = strlen(CAPromptMessage_Finance);
		break;
	case IOCTL_DDBSDK_GETCA_PROMPTMSG_POIINFO:
		strcpy((char*)pOutBuf, CAPromptMessage_POIinfo );
		*pBytesReturned = strlen(CAPromptMessage_POIinfo );
		break;
	case IOCTL_DDBSDK_GETCA_PROMPTMSG_DOWNFILE:
		strcpy((char*)pOutBuf, CAPromptMessage_DowFile );
		*pBytesReturned = strlen(CAPromptMessage_DowFile );
		break;

	case IOCTL_DDBSDK_LIST_DATAPROGRAM:
		{
		//ddbsdk_dmbdataInfo_t * dataInfoPtr = (ddbsdk_dmbdataInfo_t *)pOutBuf;
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		memcpy(pOutBuf, &dataInfoList, sizeof(ddbsdk_dmbdataInfo_t));
		break;
		}
	case IOCTL_DDBSDK_OPEN_DATA_XXXX:
		{
		ddbsdk_dmbdataInfo_t * dataInfoPtr = (ddbsdk_dmbdataInfo_t *)pInBuf;
		if(dataInfoPtr->selectServiceNo >= dataInfoPtr->ServiceNum) return 0;
		ddbsdk_dataInfo_t * dataInfo = &dataInfoPtr->dataInfo[dataInfoPtr->selectServiceNo];

		switch(dataInfo->stream_type) {
		case 0xdb:  /* traffic */
			{
			ddbsdk.hTraffic = new ddbsdk_traffic;
			ddbsdk.traffic_open = 1;
			g_wTrafficEcmPid = dataInfo->ecm_pid;
			((ddbsdk_traffic *)ddbsdk.hTraffic)->ecm_pid = dataInfo->ecm_pid;
			((ddbsdk_traffic *)ddbsdk.hTraffic)->emm_pid = dataInfo->emm_pid;
			((ddbsdk_traffic *)ddbsdk.hTraffic)->prog_pid = dataInfo->data_pid;
			((ddbsdk_traffic *)ddbsdk.hTraffic)->g_bDDBHaveCW = &g_bTrafficHaveCW;
			((ddbsdk_traffic *)ddbsdk.hTraffic)->dataType = Traffic;
			CA_initEcmEmmPid(((ddbsdk_traffic *)ddbsdk.hTraffic)->ecm_pid,
				((ddbsdk_traffic *)ddbsdk.hTraffic)->emm_pid);
			}
			break;
		case 0xdc:  /* weather */
			{
			ddbsdk.hWeather = new ddbsdk_weather;
			ddbsdk.weather_open = 1;
			g_wWeatherEcmPid  = dataInfo->ecm_pid;
			((ddbsdk_weather *)ddbsdk.hWeather)->ecm_pid = dataInfo->ecm_pid;
			((ddbsdk_weather *)ddbsdk.hWeather)->emm_pid = dataInfo->emm_pid;
			((ddbsdk_weather *)ddbsdk.hWeather)->prog_pid = dataInfo->data_pid;
			((ddbsdk_weather *)ddbsdk.hWeather)->g_bDDBHaveCW = &g_bWeatherHaveCW;
			((ddbsdk_weather *)ddbsdk.hWeather)->dataType = Weather;
			CA_initEcmEmmPid(((ddbsdk_weather *)ddbsdk.hWeather)->ecm_pid,
				((ddbsdk_weather *)ddbsdk.hWeather)->emm_pid);
			}
			break;
		case 0xdd:  /* finance */
			{
			ddbsdk.hFinance = new ddbsdk_finance;
			ddbsdk.finance_open = 1;
			g_wFinanceEcmPid = dataInfo->ecm_pid;
			((ddbsdk_finance *)ddbsdk.hFinance)->ecm_pid = dataInfo->ecm_pid;
			((ddbsdk_finance *)ddbsdk.hFinance)->emm_pid = dataInfo->emm_pid;
			((ddbsdk_finance *)ddbsdk.hFinance)->prog_pid = dataInfo->data_pid;
			((ddbsdk_finance *)ddbsdk.hFinance)->g_bDDBHaveCW = &g_bFinanceHaveCW;
			((ddbsdk_finance *)ddbsdk.hFinance)->dataType = Finance;
			CA_initEcmEmmPid(((ddbsdk_finance *)ddbsdk.hFinance)->ecm_pid,
				((ddbsdk_finance *)ddbsdk.hFinance)->emm_pid);
			}
			break;
		case 0xde:  /* system file */
			
			break;
		case 0xdf:  /* file */
			
			break;
		case 0xe1:  /* PIO info */
			{
			ddbsdk.hPIOInfo = new ddbsdk_pioInfo;
			ddbsdk.pioinfo_open = 1;
			g_wPOIinfoEcmPid = dataInfo->ecm_pid;
			((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->ecm_pid = dataInfo->ecm_pid;
			((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->emm_pid = dataInfo->emm_pid;
			((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->prog_pid = dataInfo->data_pid;
			((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->g_bDDBHaveCW = &g_bPOIinfoHaveCW;
			((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->dataType = POI;
			CA_initEcmEmmPid(((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->ecm_pid,
				((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->emm_pid);
			}
			break;
		case 0xfc:  /* file list */
			
			break;
		}
		}
		break;
	case IOCTL_DDBSDK_GET_DATA_XXXX:
		{
		ddbsdk_dmbdataInfo_t * dataInfoPtr = (ddbsdk_dmbdataInfo_t *)pInBuf;
		if(dataInfoPtr->selectServiceNo >= dataInfoPtr->ServiceNum) return 0;
		ddbsdk_dataInfo_t * dataInfo = &dataInfoPtr->dataInfo[dataInfoPtr->selectServiceNo];

		switch(dataInfo->stream_type) {
		case 0xdb:
			if(ddbsdk.hTraffic) {
				return ((ddbsdk_traffic *)ddbsdk.hTraffic)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
			} else
				return 0;
			break;
		case 0xdc:
			if(ddbsdk.hWeather) {
				return ((ddbsdk_weather *)ddbsdk.hWeather)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
			} else
				return 0;
			break;
		case 0xdd:
			if(ddbsdk.hFinance) {
				return ((ddbsdk_finance *)ddbsdk.hFinance)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
			} else
				return 0;
			break;
		case 0xde:
			
			break;
		case 0xdf:
			
			break;
		case 0xe1:
			if(ddbsdk.hPIOInfo) {
				return ((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
			} else
				return 0;
			break;
		case 0xfc:
			
			break;
		}
		}
		break;
	case IOCTL_DDBSDK_CLOSE_DATA_XXXX:
		{

			ddbsdk_dmbdataInfo_t * dataInfoPtr = (ddbsdk_dmbdataInfo_t *)pInBuf;
		if(dataInfoPtr->selectServiceNo >= dataInfoPtr->ServiceNum) return 0;
		ddbsdk_dataInfo_t * dataInfo = &dataInfoPtr->dataInfo[dataInfoPtr->selectServiceNo];

		switch(dataInfo->stream_type) {
		case 0xdb:
			delete ddbsdk.hTraffic;
			ddbsdk.traffic_open = 0;
			break;
		case 0xdc:
			delete ddbsdk.hWeather;
			ddbsdk.weather_open = 0;
			break;
		case 0xdd:
			delete ddbsdk.hFinance;
			ddbsdk.finance_open = 0;
			break;
		case 0xde:
			
			break;
		case 0xdf:
			
			break;
		case 0xe1:
			delete ddbsdk.hPIOInfo;
			ddbsdk.pioinfo_open = 0;
			break;
		case 0xfc:
			
			break;
		}
		}
		break;

	

	case IOCTL_DDBSDK_LIST_VIDEOPROGRAM:
		{
		ddbsdk_dmbvideoInfo_t * videoInfoPtr = (ddbsdk_dmbvideoInfo_t *)pOutBuf;
		u32   num;
		ddbsdk_videoInfo_t * videoInfo = &videoInfoPtr->videoInfo[0];
		videoInfoPtr->ServiceNum = getVideoServNum();
		for(num=0; num<videoInfoPtr->ServiceNum; num++) {
			videoInfo = &videoInfoPtr->videoInfo[num];
			videoInfo->serviceNo = num;
			videoInfo->ServiceIndex = getVideoServerIndex(num);
			videoInfo->ecm_pid = 0x1fff;
			videoInfo->emm_pid = 0x1fff;
			getVideoDescr(videoInfo->descr, num);

			getVideoProgramList(videoInfo);

			videoInfo->next = videoInfo + 1;
			videoInfo = videoInfo->next;

			/* if memory not enough, return error */
		}
		videoInfo->next = NULL;
		}
		break;
	case IOCTL_DDBSDK_OPEN_VIDEO:
		{
		ddbsdk_dmbvideoInfo_t * videoInfoPtr = (ddbsdk_dmbvideoInfo_t *)pInBuf;
		if(videoInfoPtr->selectServiceNo >= videoInfoPtr->ServiceNum) return 0;
		ddbsdk_videoInfo_t * videoInfo = &videoInfoPtr->videoInfo[videoInfoPtr->selectServiceNo];
		ddbsdk.pcr_pid = videoInfo->pcr_pid;

		ddbsdk.hDmbVideo = new ddbsdk_dmbVideo;
		ddbsdk.hDmbAudio = new ddbsdk_dmbAduio;

		if(ddbsdk.test_file_mode == 0) {
			if(ddbsdk.serviceIndexCur != videoInfo->ServiceIndex) {
				START_DMBSERVICE(videoInfo->ServiceIndex, ddbsdk.testInSMGLabFlag);
			}
		}
		ddbsdk.video_open = 1;
		((ddbsdk_dmbVideo *)ddbsdk.hDmbVideo)->prog_pid = videoInfo->video_pid;
		((ddbsdk_dmbAduio *)ddbsdk.hDmbAudio)->prog_pid = videoInfo->audio_pid;
		}
		break;
	case IOCTL_DDBSDK_GET_VIDEO_ONEFRAMEVIDEO:
		if(ddbsdk.hDmbVideo) {
			return ((ddbsdk_dmbVideo *)ddbsdk.hDmbVideo)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else
			return 0;

		break;
	case IOCTL_DDBSDK_GET_VIDEO_ONEFRAMEAUDIO:
		if(ddbsdk.hDmbAudio) {
			return ((ddbsdk_dmbAduio *)ddbsdk.hDmbAudio)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else
			return 0;

		break;
	case IOCTL_DDBSDK_GET_VIDEO_PCR:
		{
		ddbsdk_dmbvideoInfo_t * videoInfoPtr = (ddbsdk_dmbvideoInfo_t *)pOutBuf;

		videoInfoPtr->videoInfo[videoInfoPtr->selectServiceNo].pcr_base = ddbsdk.videoProgramPCR;
		}
		break;
	case IOCTL_DDBSDK_CLOSE_VIDEO:
		delete ddbsdk.hDmbVideo;
		delete ddbsdk.hDmbAudio;
		delete ddbsdk.hDmbScene;
		delete ddbsdk.hDmbOdr;
		ddbsdk.video_open = 0;
		break;
	case IOCTL_DDBSDK_CHANGE_VIDEO_CHAL:
		{
		ddbsdk_dmbvideoInfo_t * videoInfoPtr = (ddbsdk_dmbvideoInfo_t *)pInBuf;
		if(videoInfoPtr->selectServiceNo >= videoInfoPtr->ServiceNum) return 0;
		}
		break;

	case IOCTL_DDBSDK_LIST_MUSICAMPROGRAM:
		{
		ddbsdk_dmbmusicamInfo_t * musicamInfoPtr = (ddbsdk_dmbmusicamInfo_t *)pOutBuf;
		u32   num;
		ddbsdk_musicamInfo_t * musicamInfo = &musicamInfoPtr->musicamInfo[0];
		musicamInfoPtr->ServiceNum = getMusicamServNum();
		for(num=0; num<musicamInfoPtr->ServiceNum; num++) {
			musicamInfo->serviceNo = num;
			musicamInfo->ServiceIndex = getMusicamServerIndex(num);
			getMusicamDescr(musicamInfo->descr, num);
			musicamInfo->next = musicamInfo + 1;
			musicamInfo = musicamInfo->next;

			/* if memory not enough, return error */

		}
		musicamInfo->next = NULL;
		}
		break;
	case IOCTL_DDBSDK_OPEN_MUSICAM:
		{
		ddbsdk_dmbmusicamInfo_t * musicamInfoPtr = (ddbsdk_dmbmusicamInfo_t *)pInBuf;
		if(musicamInfoPtr->selectServiceNo >= musicamInfoPtr->ServiceNum) return 0;
		ddbsdk_musicamInfo_t * musicamInfo = &musicamInfoPtr->musicamInfo[musicamInfoPtr->selectServiceNo];
		ddbsdk.hMusicam = new ddbsdk_musicam;
		((ddbsdk_musicam *)ddbsdk.hMusicam)->componentIndex = musicamInfo->ServiceIndex;
		if(ddbsdk.test_file_mode == 0) {
			if(ddbsdk.serviceIndexCur != ((ddbsdk_musicam *)ddbsdk.hMusicam)->componentIndex) {
				START_DMBSERVICE(((ddbsdk_musicam *)ddbsdk.hMusicam)->componentIndex, 
					ddbsdk.testInSMGLabFlag);
			}
		}
		ddbsdk.musicam_open = 1;
		}
		break;
	case IOCTL_DDBSDK_GET_MUSICAM_DATA:
		if(ddbsdk.hMusicam) {
			return ((ddbsdk_musicam *)ddbsdk.hMusicam)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else
			return 0;
		break;
	case IOCTL_DDBSDK_CHANGE_MUSICAM_CHAL:
		{
		ddbsdk_dmbmusicamInfo_t * musicamInfoPtr = (ddbsdk_dmbmusicamInfo_t *)pInBuf;
		if(musicamInfoPtr->selectServiceNo >= musicamInfoPtr->ServiceNum) return 0;
		ddbsdk_musicamInfo_t * musicamInfo = &musicamInfoPtr->musicamInfo[musicamInfoPtr->selectServiceNo];

		((ddbsdk_musicam *)ddbsdk.hMusicam)->componentIndex = musicamInfo->ServiceIndex;
		if(ddbsdk.serviceIndexCur != ((ddbsdk_musicam *)ddbsdk.hMusicam)->componentIndex) {
			START_DMBSERVICE(((ddbsdk_musicam *)ddbsdk.hMusicam)->componentIndex, 
				ddbsdk.testInSMGLabFlag);
		}
		}
		break;
	case IOCTL_DDBSDK_CLOSE_MUSICAM:
		delete ddbsdk.hMusicam;
		ddbsdk.musicam_open = 0;
		break;

	case IOCTL_DDBSDK_OPEN_DATA_DOWNLOADFILE:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		/* initiate to start serviceFIle download service */
		ddbsdk.hServiceFile = new ddbsdk_serviceFile;
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->ecm_pid = getDataECM(264);//4117;//4118;
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->emm_pid = 4097;
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->prog_pid = 264;//270
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->g_bDDBHaveCW = &g_bSerFileHaveCW;
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->dataType = DownloadFile;
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->downHandle = ddbsdk.hServiceFile;
		WriteLog("ddbsdk.hServiceFile->downHandle:%x", ddbsdk.hServiceFile);
		CA_initEcmEmmPid(((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->ecm_pid,
			((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->emm_pid);
		ddbsdk.serviceFile_open = 1;
		if(ddbsdk.hServiceFile) {
			((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->initServiceFileData();
		}
		break;

	case IOCTL_DDBSDK_OPEN_WEATHER_DOWNLOADFILE:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		/* initiate to start serviceFIle download service */
		ddbsdk.hWeatherFile = new ddbsdk_serviceFile;
		((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->ecm_pid = getDataECM(263);//4117;//4118;
		((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->emm_pid = 4097;
		((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->prog_pid = 263;//270
		((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->g_bDDBHaveCW = &g_bWeatherFileHaveCW;
		((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->dataType = WeatherFile;
		((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->downHandle = ddbsdk.hWeatherFile;
		WriteLog("ddbsdk.hWeatherFile->downHandle:%x", ddbsdk.hWeatherFile);
		CA_initEcmEmmPid(((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->ecm_pid,
			((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->emm_pid);
		ddbsdk.weatherfile_open = 1;
		if(ddbsdk.hWeatherFile) {
			((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->initServiceFileData();
		}
		break;

	case IOCTL_DDBSDK_OPEN_ROADCONDITION_DOWNLOADFILE:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		/* initiate to start serviceFIle download service */
		ddbsdk.hRoadConditionFile = new ddbsdk_serviceFile;
		((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->ecm_pid = getDataECM(262);//4117;//4118;
		((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->emm_pid = 4097;
		((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->prog_pid = 262;//270
		((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->g_bDDBHaveCW = &g_bRoadConditionHaveCW;
		((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->dataType = RoadCondition;
		((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->downHandle = ddbsdk.hRoadConditionFile;
		WriteLog("ddbsdk.hRoadConditionFile->downHandle:%x", ddbsdk.hRoadConditionFile);
		CA_initEcmEmmPid(((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->ecm_pid,
			((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->emm_pid);
		ddbsdk.roadconditionsfile_open = 1;
		if(ddbsdk.hRoadConditionFile) {
			((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->initServiceFileData();
		}
		break;

	case IOCTL_DDBSDK_OPEN_EMERGENCYBRO_DOWNLOADFILE:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		/* initiate to start serviceFIle download service */
		ddbsdk.hEmergencyBroadcastFile = new ddbsdk_serviceFile;
		((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->ecm_pid = getDataECM(270);//4117;//4118;
		((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->emm_pid = 4097;
		((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->prog_pid = 270;//270
		((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->g_bDDBHaveCW = &g_bEmergencyBroadcastHaveCW;
		((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->dataType = EmergencyBroadcast;
		((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->downHandle = ddbsdk.hEmergencyBroadcastFile;
		WriteLog("ddbsdk.hEmergencyBroadcastFile->downHandle:%x", ddbsdk.hEmergencyBroadcastFile);
		CA_initEcmEmmPid(((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->ecm_pid,
			((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->emm_pid);
		ddbsdk.emergencybroadcastfile_open = 1;
		if(ddbsdk.hEmergencyBroadcastFile) {
			((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->initServiceFileData();
		}
		break;

	case IOCTL_DDBSDK_OPEN_TRAVELFILE_DOWNLOADFILE:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		/* initiate to start serviceFIle download service */
		ddbsdk.hTravelFile = new ddbsdk_serviceFile;
		((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->ecm_pid = getDataECM(267);//4117;//4118;
		((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->emm_pid = 4097;
		((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->prog_pid = 267;//270
		((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->g_bDDBHaveCW = &g_bTravelFileHaveCW;
		((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->dataType = TravelFile;
		((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->downHandle = ddbsdk.hTravelFile;
		WriteLog("ddbsdk.hhTravelFile->downHandle:%x", ddbsdk.hTravelFile);
		CA_initEcmEmmPid(((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->ecm_pid,
			((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->emm_pid);
		ddbsdk.travelfile_open = 1;
		if(ddbsdk.hTravelFile) {
			((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->initServiceFileData();
		}
		break;

	case IOCTL_DDBSDK_OPEN_EXPOFILE_DOWNLOADFILE:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		/* initiate to start serviceFIle download service */
		ddbsdk.hExpoFile = new ddbsdk_serviceFile;
		((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->ecm_pid = getDataECM(266);//4117;//4118;
		((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->emm_pid = 4097;
		((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->prog_pid = 266;//270
		((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->g_bDDBHaveCW = &g_bExpoFileHaveCW;
		((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->dataType = ExpoFile;
		((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->downHandle = ddbsdk.hExpoFile;
		WriteLog("ddbsdk.hExpoFile->downHandle:%x", ddbsdk.hExpoFile);
		CA_initEcmEmmPid(((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->ecm_pid,
			((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->emm_pid);
		ddbsdk.expofile_open = 1;
		if(ddbsdk.hExpoFile) {
			((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->initServiceFileData();
		}
		break;

	case IOCTL_DDBSDK_CLOSE_DATA_DOWNLOADFILE:
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->deinitServiceFileData();
		delete ddbsdk.hServiceFile;
		ddbsdk.hServiceFile = 0;
		ddbsdk.serviceFile_open = 0;
		//g_bSerFileHaveCW = false;
		break;

	case IOCTL_DDBSDK_CLOSE_WEATHER_DOWNLOADFILE:
		((ddbsdk_serviceFile *)ddbsdk.hWeatherFile)->deinitServiceFileData();
		delete ddbsdk.hWeatherFile;
		ddbsdk.hWeatherFile = 0;
		ddbsdk.weatherfile_open = 0;
		//g_bWeatherFileHaveCW = false;
		break;

	case IOCTL_DDBSDK_CLOSE_ROADCONDITION_DOWNLOADFILE:
		((ddbsdk_serviceFile *)ddbsdk.hRoadConditionFile)->deinitServiceFileData();
		delete ddbsdk.hRoadConditionFile;
		ddbsdk.hRoadConditionFile = 0;
		ddbsdk.roadconditionsfile_open = 0;
		//g_bRoadConditionHaveCW = false;
		break;

	case IOCTL_DDBSDK_CLOSE_EMERGENCYBRO_DOWNLOADFILE:
		((ddbsdk_serviceFile *)ddbsdk.hEmergencyBroadcastFile)->deinitServiceFileData();
		delete ddbsdk.hEmergencyBroadcastFile;
		ddbsdk.hEmergencyBroadcastFile = 0;
		ddbsdk.emergencybroadcastfile_open = 0;
		//g_bEmergencyBroadcastHaveCW = false;
		break;

	case IOCTL_DDBSDK_CLOSE_TRAVELFILE_DOWNLOADFILE:
		ddbsdk.travelfile_open = 0;
		((ddbsdk_serviceFile *)ddbsdk.hTravelFile)->deinitServiceFileData();
		delete ddbsdk.hTravelFile;
		ddbsdk.hTravelFile = 0;
		//g_bTravelFileHaveCW = false;
		break;

	case IOCTL_DDBSDK_CLOSE_EXPOFILE_DOWNLOADFILE:
		ddbsdk.expofile_open = 0;
		((ddbsdk_serviceFile *)ddbsdk.hExpoFile)->deinitServiceFileData();
		delete ddbsdk.hExpoFile;
		ddbsdk.hExpoFile = 0;
		//g_bExpoFileHaveCW = false;
		break;
	
	case IOCTL_DDBSDK_LIST_DATA_DOWNLOADINGFILESTATUS:
		{
			getDownloadFile_Status(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_LIST_WEATHER_DOWNLOADINGFILESTATUS:
		{
			getDownloadFile_Status(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_LIST_ROADCONDITION_DOWNLOADINGFILESTATUS:
		{
			getDownloadFile_Status(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_LIST_EMERGENCYBRO_DOWNLOADINGFILESTATUS:
		{
			getDownloadFile_Status(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_LIST_TRAVELFILE_DOWNLOADINGFILESTATUS:
		{
			getDownloadFile_Status(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_LIST_EXPOFILE_DOWNLOADINGFILESTATUS:
		{
			getDownloadFile_Status(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_GET_DATA_ONEFILEINFO:
		{
			getDownloadFile_OneFileInfo(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_GET_WEATHER_ONEFILEINFO:
		{
			getDownloadFile_OneFileInfo(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_GET_ROADCONDITION_ONEFILEINFO:
		{
			getDownloadFile_OneFileInfo(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_GET_EMERGENCYBRO_ONEFILEINFO:
		{
			getDownloadFile_OneFileInfo(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_GET_TRAVELFILE_ONEFILEINFO:
		{
			getDownloadFile_OneFileInfo(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_GET_EXPOFILE_ONEFILEINFO:
		{
			getDownloadFile_OneFileInfo(pInBuf, pOutBuf);
		}
		break;

	case IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_OPEN:
		{
			return getDownloadFile_OneBlock_open(pInBuf, pBytesReturned);
		}
		break;

	case IOCTL_DDBSDK_WEATHER_GETONEDATABLOCK_FINISHEDFILE_OPEN:
		{
			return getDownloadFile_OneBlock_open(pInBuf, pBytesReturned);
				
		}
		break;

	case IOCTL_DDBSDK_ROADCONDITION_GETONEDATABLOCK_FINISHEDFILE_OPEN:
		{
			return getDownloadFile_OneBlock_open(pInBuf, pBytesReturned);
		}
		break;

	case IOCTL_DDBSDK_EMERGENCYBRO_GETONEDATABLOCK_FINISHEDFILE_OPEN:
		{
			return getDownloadFile_OneBlock_open(pInBuf, pBytesReturned);
		}
		break;

	case IOCTL_DDBSDK_TRAVELFILE_GETONEDATABLOCK_FINISHEDFILE_OPEN:
		{
			return getDownloadFile_OneBlock_open(pInBuf, pBytesReturned);
		}
		break;

	case IOCTL_DDBSDK_EXPOFILE_GETONEDATABLOCK_FINISHEDFILE_OPEN:
		{
			return getDownloadFile_OneBlock_open(pInBuf, pBytesReturned);
		}
		break;


	case IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE:
		{
		ddbsdk_servFileInfo_t * servInfoIn = (ddbsdk_servFileInfo_t *)pInBuf;
		if(servInfoIn->file_get != NULL) {
			/* buffer to transfer donot enough */
			if(nOutBufSize < servInfoIn->Block_length) {
				* pBytesReturned = 0;
				return TRUE;
			} else {
				u32   readLen;
				readLen = fread(pOutBuf, 1, servInfoIn->Block_length, servInfoIn->file_get);
				//readLen = fread(pOutBuf, 1, servInfoIn->FILE_LENGTH, servInfoIn->file_get);
				* pBytesReturned = readLen;
			}
		} else {
			memset(pOutBuf, 0, nOutBufSize);
			* pBytesReturned = 0;
		}
		}
		break;
	
	case IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE:
		{
		ddbsdk_servFileInfo_t * servInfoIn = (ddbsdk_servFileInfo_t *)pInBuf;
		fclose(servInfoIn->file_get);
		}
		break;

	case IOCTL_DDBSDK_GET_ONE_FINISHEDFILE:
		{
			ddbsdk_servFileInfo_t * servInfoIn = (ddbsdk_servFileInfo_t *)pInBuf;
			u32   selFILE_ID = servInfoIn->selectedFILE_ID;
			u32   Num, n;
			char  FILEIDPathGet[256];
			FILE* tmpFile;
			char   pathType[9][16] = 
			{
				PREFIX_PATH_SYSFILE, PREFIX_PATH_MEDIA, PREFIX_PATH_AD, PREFIX_PATH_MUSIC,
				PREFIX_PATH_NET, PREFIX_PATH_POIFOOD, PREFIX_PATH_PROGLIST, PREFIX_PATH_LIST, 
				PREFIX_PATH_OTHER
			};
			u32   typeIndex = servInfoIn->FILE_TYPE <= POIFOODFILE ? servInfoIn->FILE_TYPE : 
				servInfoIn->FILE_TYPE == PROGRAMFILE ? 6 :
				servInfoIn->FILE_TYPE == LISTFILE ? 7 : 8;

/*			ddbsdk_servFileTab_t * g_servTab = &((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->g_servTab;*/

				/* not finished file, return Bytes 0 */
				Num = g_servTab.num_complete + g_servTab.num_down;
				Num = Num > MAXLISTFILEID ? MAXLISTFILEID : Num;
				for(n=0; n<Num; n++) {
					if(g_servTab.ddbsdk_downFiles[n].synt.FILE_ID == selFILE_ID) {

						if(g_servTab.ddbsdk_downFiles[n].numDownloaded < g_servTab.ddbsdk_downFiles[n].synt.Block_num) {
							* pBytesReturned = 0;
							return TRUE;
						} else {
							break;
						}
					}
				}

				memset(FILEIDPathGet, 0, sizeof(FILEIDPathGet));
				sprintf(FILEIDPathGet, "%s%s%s\\%x.tmp", ddbsdk.ddbsdk_path_main, DDBSDK_MAINPATH_NAME, 
					pathType[typeIndex], selFILE_ID);
				tmpFile = fopen(FILEIDPathGet, "rb");

				if(tmpFile != NULL) {
					/* buffer to transfer donot enough */
					if(nOutBufSize < servInfoIn->Block_length) {
						* pBytesReturned = 0;
						return TRUE;
					} else {
						u32   readLen;
						readLen = fread(pOutBuf, 1, servInfoIn->FILE_LENGTH, tmpFile);
						* pBytesReturned = readLen;
					}
				} else {
					memset(pOutBuf, 0, nOutBufSize);
					* pBytesReturned = 0;
				}
				fclose(tmpFile);		
		}
		break;

	case IOCTL_DDBSDK_DATA_PID_OPEN:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;

		{
			u16 pid = *pInBuf;
			ddbsdk_decryp_stream* decryp_stream = new ddbsdk_decryp_stream;
			decryp_stream->prog_pid = pid;
			decryp_stream->ecm_pid = getDataECM(pid);
			decryp_stream->emm_pid = getDataEMM(pid);
			return addNodeFromDecrypStreamArray(decryp_stream);
		}
		break;

	case IOCTL_DDBSDK_DATA_PID_GET:
		{
			u16 pid = *pInBuf;
			ddbsdk_decryp_stream* decryp_stream = getNodeFromDecrypStreamArray(pid);
			if (decryp_stream != 0)
			{
				decryp_stream->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
			}
			else
			{
				*pBytesReturned = 0;
				return 0;
			}	
		}
		break;

	case IOCTL_DDBSDK_DATA_PID_CLOSE:
		{
			u16 pid = *pInBuf;
			deleteNodeFromDecrypStreamArray(pid);
		}
		break;

	case IOCTL_DDBSDK_DATA_TS_OPEN:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;

		{
			if (decryp_program_info_Array != 0)
			{
				return 0;
			}
			decryp_program_info_Array = new list<ddbsdk_decryp_program_info*>;
			for (int i = 0; i < dataInfoList.ServiceNum; i++)
			{
				ddbsdk_decryp_program_info * prog_info = new ddbsdk_decryp_program_info;
				prog_info->prog_pid = dataInfoList.dataInfo[i].data_pid;
				prog_info->ecm_pid = dataInfoList.dataInfo[i].ecm_pid;
				prog_info->emm_pid = dataInfoList.dataInfo[i].emm_pid;
				decryp_program_info_Array->push_back(prog_info);
			}
			ddbsdk.hDecrypDataTs = new ddbsdk_decryp_ts_stream;
			ddbsdk.decryp_data_ts_open = 1;
			
		}
		break;

	case IOCTL_DDBSDK_DATA_TS_GET:
		{
			if (decryp_program_info_Array == 0)
			{
				*pBytesReturned = 0;
				return 0;
			}
			if (ddbsdk.hDecrypDataTs)
			{
				((ddbsdk_decryp_ts_stream*)ddbsdk.hDecrypDataTs)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
			}
		}
		break;

	case IOCTL_DDBSDK_DATA_TS_CLOSE:
		{
			ddbsdk.decryp_data_ts_open = 0;
			Sleep(100);
			delete ddbsdk.hDecrypDataTs;
			cleanFromDecrypProgInfoArray();
		}
		break;

	case IOCTL_DDBSDK_OPEN_DATA_FINANCE :
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		ddbsdk.hFinance = new ddbsdk_finance;
		ddbsdk.finance_open = 1;
		((ddbsdk_finance *)ddbsdk.hFinance)->ecm_pid = getDataECM(260);//4114;
		((ddbsdk_finance *)ddbsdk.hFinance)->emm_pid = 4097;
		((ddbsdk_finance *)ddbsdk.hFinance)->prog_pid = 260;
		((ddbsdk_finance *)ddbsdk.hFinance)->g_bDDBHaveCW = &g_bFinanceHaveCW;
		((ddbsdk_finance *)ddbsdk.hFinance)->dataType = Finance;
		CA_initEcmEmmPid(((ddbsdk_finance *)ddbsdk.hFinance)->ecm_pid,
			((ddbsdk_finance *)ddbsdk.hFinance)->emm_pid);
		//printf("ddbsdk.hFinance->g_bDDBHaveCW = %x",((ddbsdk_finance *)ddbsdk.hFinance)->g_bDDBHaveCW);
		break;
	case IOCTL_DDBSDK_GET_DATA_FINANCE :
		if(ddbsdk.hFinance) {
			return ((ddbsdk_finance *)ddbsdk.hFinance)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else
			return 0;
	case IOCTL_DDBSDK_CLOSE_DATA_FINANCE :
		delete ddbsdk.hFinance;
		ddbsdk.finance_open = 0;
		break;

	case IOCTL_DDBSDK_OPEN_DATA_PIOINFO :
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		ddbsdk.hPIOInfo = new ddbsdk_pioInfo;
		ddbsdk.pioinfo_open = 1;
		((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->ecm_pid = getDataECM(265);//4116;
		((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->emm_pid = 4097;
		((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->prog_pid = 265;
		((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->g_bDDBHaveCW = &g_bPOIinfoHaveCW;
		((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->dataType = POI;
		CA_initEcmEmmPid(((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->ecm_pid,
			((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->emm_pid);
		break;
	case IOCTL_DDBSDK_GET_DATA_PIOINFO :
		if(ddbsdk.hPIOInfo) {
			return ((ddbsdk_pioInfo *)ddbsdk.hPIOInfo)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else 
			return 0;
		break;
	case IOCTL_DDBSDK_CLOSE_DATA_PIOINFO :
		delete ddbsdk.hPIOInfo;
		ddbsdk.pioinfo_open = 0;
		//g_bPOIinfoHaveCW = false;
		break;

	case IOCTL_DDBSDK_OPEN_DATA_TRAFFIC :
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		ddbsdk.hTraffic = new ddbsdk_traffic;
		ddbsdk.traffic_open = 1;
		((ddbsdk_traffic *)ddbsdk.hTraffic)->ecm_pid = getDataECM(258);//4112;
		((ddbsdk_traffic *)ddbsdk.hTraffic)->emm_pid = 4097;
		((ddbsdk_traffic *)ddbsdk.hTraffic)->prog_pid = 258;
		((ddbsdk_traffic *)ddbsdk.hTraffic)->g_bDDBHaveCW = &g_bTrafficHaveCW;
		((ddbsdk_traffic *)ddbsdk.hTraffic)->dataType = Traffic;
		CA_initEcmEmmPid(((ddbsdk_traffic *)ddbsdk.hTraffic)->ecm_pid,
			((ddbsdk_traffic *)ddbsdk.hTraffic)->emm_pid);
		break;
	case IOCTL_DDBSDK_GET_DATA_TRAFFIC :
		if(ddbsdk.hTraffic) {
			return ((ddbsdk_traffic *)ddbsdk.hTraffic)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else
			return 0;
		break;
	case IOCTL_DDBSDK_CLOSE_DATA_TRAFFIC :
		delete ddbsdk.hTraffic;
		ddbsdk.traffic_open = 0;
		//g_bTrafficHaveCW = false;
		break;

	case IOCTL_DDBSDK_OPEN_DATA_WEATHER :
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		ddbsdk.hWeather = new ddbsdk_weather;
		ddbsdk.weather_open = 1;
		((ddbsdk_weather *)ddbsdk.hWeather)->ecm_pid = getDataECM(259);//4113;
		((ddbsdk_weather *)ddbsdk.hWeather)->emm_pid = 4097;
		((ddbsdk_weather *)ddbsdk.hWeather)->prog_pid = 259;
		((ddbsdk_weather *)ddbsdk.hWeather)->g_bDDBHaveCW = &g_bWeatherHaveCW;
		((ddbsdk_weather *)ddbsdk.hWeather)->dataType =Weather;
		CA_initEcmEmmPid(((ddbsdk_weather *)ddbsdk.hWeather)->ecm_pid,
			((ddbsdk_weather *)ddbsdk.hWeather)->emm_pid);
		//CA_initEcmEmmPid(((ddbsdk_traffic *)ddbsdk.hTraffic)->ecm_pid,
			//((ddbsdk_traffic *)ddbsdk.hTraffic)->emm_pid);
		break;
	case IOCTL_DDBSDK_GET_DATA_WEATHER :
		if(ddbsdk.hWeather) {
			return ((ddbsdk_weather *)ddbsdk.hWeather)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else 
			return 0;
		break;
	case IOCTL_DDBSDK_CLOSE_DATA_WEATHER :
		delete ddbsdk.hWeather;
		ddbsdk.weather_open = 0;
		//g_bWeatherHaveCW = false;
		break;
	case IOCTL_DDBSDK_OPEN_DATA_CARPARK:
		SERVICE_LIST_LOCK;
		if(!ddbsdk.haveDataList)
		{
			BOOL isGetDataInfoList = getDataInfoList(&dataInfoList);
			if (TRUE != isGetDataInfoList)
			{
				SERVICE_LIST_UNLOCK;
				return 0;
			}
			ddbsdk.haveDataList = TRUE;
		}
		SERVICE_LIST_UNLOCK;
		ddbsdk.hCarPark = new ddbsdk_carPark;
		((ddbsdk_carPark *)ddbsdk.hCarPark)->ecm_pid = getDataECM(257);//4113;
		((ddbsdk_carPark *)ddbsdk.hCarPark)->emm_pid = 4097;
		((ddbsdk_carPark *)ddbsdk.hCarPark)->prog_pid = 257;
		((ddbsdk_carPark *)ddbsdk.hCarPark)->g_bDDBHaveCW = &g_bCarParkHaveCW;
		((ddbsdk_carPark *)ddbsdk.hCarPark)->dataType =CarPark;
		CA_initEcmEmmPid(((ddbsdk_carPark *)ddbsdk.hCarPark)->ecm_pid,
			((ddbsdk_carPark *)ddbsdk.hCarPark)->emm_pid);
		ddbsdk.carpark_open = 1;
		break;

	case IOCTL_DDBSDK_GET_DATA_CARPARK:
		if(ddbsdk.hCarPark) {
			return ((ddbsdk_carPark *)ddbsdk.hCarPark)->getPlainPkgData(pOutBuf, nOutBufSize, pBytesReturned);
		} else 
			return 0;
		break;

	case IOCTL_DDBSDK_CLOSE_DATA_CARPARK:
		delete ddbsdk.hCarPark;
		ddbsdk.carpark_open = 0;
		//g_bCarParkHaveCW = false;
		break;

	case IOCTL_DDBSDK_FILEINPUT_TEST:
		DataCallback(NULL, 0, pInBuf, nInBufSize);
		break;
	case IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE:
		/* initiate the CA System */
		if(ddbsdk.ca_init_ok == 0) {
			if(CA_Init() != 0) {
				printf("ca init failed!\n");
				return 0;
			}
		}
		ddbsdk.ca_init_ok = 1;
		ddbsdk.test_file_mode = 1;
		break;
	case IOCTL_DDBSDK_CANCEL_FILEINPUT_TEST_MODE:
		ddbsdk.test_file_mode = 0;
		break;
	default:
		printf("donot surport IOCTL!\n");
		return 0;
	}
//printf("total: %d\n", bytesRet);
    return TRUE;
}


DWORD WINAPI DDB_Read(DWORD Handle, LPVOID pBuffer, DWORD dwNumBytes)
{
	printf("This is DAB_Read\n");

	return DWORD(1);;
}
DWORD WINAPI DDB_Write(DWORD Handle, LPCVOID pBuffer, DWORD dwNumBytes)
{
	printf("This is DAB_Write\n");
	return DWORD(1);;
}
DWORD WINAPI DDB_Seek(DWORD Handle, long lDistance, DWORD dwMoveMethod)
{
	printf("This is DAB_Seek\n");
	return DWORD(1);;
}

void WINAPI DDB_PowerUp(void)
{
	printf("This is DAB_PowerUp\n");
	return;
}

void WINAPI
DDB_PowerDown(void)
{
	printf("This is DAB_PowerDown\n");
	return;
}


BOOL getDataInfoList(ddbsdk_dmbdataInfo_t * dataInfoPtr)
{
	u32   num, ChNum, ServiceNum, ChServiceNum;
	ddbsdk_dataInfo_t * dataInfo = &dataInfoPtr->dataInfo[0];
	ChNum = getDataServNum();
	ServiceNum = 0;
	for(num=0; num<ChNum; num++) {
		bool isGot = getDataProgramList(dataInfo, ChServiceNum);
		if (!isGot)
		{
			printf("获取数据节目单失败！\n");
			return FALSE;
		}
		ServiceNum = ChServiceNum;
		dataInfo = dataInfo->next;

		/* if memory not enough, return error */

	}
	dataInfoPtr->ServiceNum = ServiceNum;
	dataInfo->next = NULL;
	return TRUE;
}

u16 getDataECM(u16 pid)
{
	for(unsigned int i = 0; i < dataInfoList.ServiceNum; i++)
	{
		if(dataInfoList.dataInfo[i].data_pid == pid)
			return dataInfoList.dataInfo[i].ecm_pid;
	}
	return 0;
}

u16 getDataEMM(u16 pid)
{
	for(unsigned int i = 0; i < dataInfoList.ServiceNum; i++)
	{
		if(dataInfoList.dataInfo[i].data_pid == pid)
			return dataInfoList.dataInfo[i].emm_pid;
	}
	return 0;
}

void InitCAPromptMessage(ShowCAPromptMessage func)
{
	printf("func = %x\n", func);
	CAshowMessage = func;
}


int InitMessageMapMutex()
{
	messageEvent = CreateEvent(NULL,FALSE,FALSE,L"Global\\DDBSDK_MESSAGE_EVENT");
	messageMutex = CreateMutex(NULL, FALSE, L"Global\\DDBSDK_MESSAGE_MUTEX");
	messageFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(unsigned int), L"Global\\DDBSDK_MESSAGE_FILEMAP");
	if(messageEvent == NULL) printf("messageEvent == NULL!\n");
	if(messageMutex == NULL) printf("messageMutex == NULL!\n");
	if(messageFileMap == NULL) printf("messageFileMap == NULL!\n");
	if(messageEvent == NULL || messageMutex == NULL || messageFileMap == NULL) return -1;
	
	return 0;
}


int errorMessageOccur(int errorCode)
{
	LPCVOID pVoid = NULL;
	if (WAIT_OBJECT_0 == WaitForSingleObject(messageMutex, INFINITE))
	{
		 pVoid= MapViewOfFile(messageFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	}

	if(pVoid == NULL)
	{
		printf("MapViewOfFile FAILED!\n");
		return -1;
	}

	if(pVoid != NULL)
	{
		*((unsigned int*)pVoid) = errorCode;
		UnmapViewOfFile(pVoid);
	}
	ReleaseMutex(messageMutex);
	SetEvent(messageEvent);

	return 0;
}


#define COPY_FILE_BUF_SIZE    256
int copyFile(const char* inFile, const char* outFile)
{
	FILE *in_file, *out_file;
	char data[COPY_FILE_BUF_SIZE];
	size_t bytes_in, bytes_out;
	long len = 0;

	if ( (in_file = fopen(inFile, "rb")) == NULL )
	{
		return 1;
	}

	if ( (out_file = fopen(outFile, "wb")) == NULL )
	{
		return 2;
	}

	while ( (bytes_in = fread(data, 1, COPY_FILE_BUF_SIZE, in_file)) > 0 )
	{
		bytes_out = fwrite(data, 1, bytes_in, out_file);
		if ( bytes_in != bytes_out )
		{
			return 3;
		}
		len += bytes_out;
	}

	fclose(in_file);
	fclose(out_file);

	return 0;
}

ddbsdk_decryp_stream* getNodeFromDecrypStreamArray(u16 prom_pid)
{
	EnterCriticalSection(&decryStreamAarrayLockSection);
	if (decryp_stream_Array->size() == 0)
	{
		LeaveCriticalSection(&decryStreamAarrayLockSection);
		return 0;
	}
	list<ddbsdk_decryp_stream*>::iterator iter;
	for (iter = decryp_stream_Array->begin(); iter != decryp_stream_Array->end(); ++iter)
	{
		if ((*iter)->prog_pid == prom_pid)
		{
			LeaveCriticalSection(&decryStreamAarrayLockSection);
			return (*iter);
		}
	}
	LeaveCriticalSection(&decryStreamAarrayLockSection);
	return 0;
}

BOOL addNodeFromDecrypStreamArray(ddbsdk_decryp_stream* decrypStream)
{
	if (getNodeFromDecrypStreamArray(decrypStream->prog_pid) != 0)
	{
		delete decrypStream;
		return FALSE;
	}
	else
	{
		EnterCriticalSection(&decryStreamAarrayLockSection);
		decryp_stream_Array->push_back(decrypStream);
		LeaveCriticalSection(&decryStreamAarrayLockSection);
		return TRUE;
	}
	
	
}

int deleteNodeFromDecrypStreamArray(u16 program_pid)
{
	EnterCriticalSection(&decryStreamAarrayLockSection);
	if (decryp_stream_Array->size() == 0)
	{
		LeaveCriticalSection(&decryStreamAarrayLockSection);
		return 0;
	}
	list<ddbsdk_decryp_stream*>::iterator iter;
	for (iter = decryp_stream_Array->begin(); iter != decryp_stream_Array->end(); ++iter)
	{
		if ((*iter)->prog_pid == program_pid)
		{
			delete (*iter);
			iter = decryp_stream_Array->erase(iter);
 			LeaveCriticalSection(&decryStreamAarrayLockSection);
 			return 0;
		}
	}
	LeaveCriticalSection(&decryStreamAarrayLockSection);
	return 0;
}

int setCWFromDectrypStreamArray(WORD wEcmPID, BYTE byKeyLen, const unsigned char* szOddKey,const unsigned char* szEvenKey)
{
	EnterCriticalSection(&decryStreamAarrayLockSection);
	if (decryp_stream_Array->size() == 0)
	{
		LeaveCriticalSection(&decryStreamAarrayLockSection);
		return 0;
	}
	list<ddbsdk_decryp_stream*>::iterator iter;
	for (iter = decryp_stream_Array->begin(); iter != decryp_stream_Array->end(); ++iter)
	{
		if ((*iter)->ecm_pid == wEcmPID)
		{
			memcpy((*iter)->g_OddCW, szOddKey, byKeyLen);
			memcpy((*iter)->g_EvenCW, szEvenKey, byKeyLen);
			(*iter)->isHaveCW = true;
			LeaveCriticalSection(&decryStreamAarrayLockSection);
			return 0;
		}
	}
	LeaveCriticalSection(&decryStreamAarrayLockSection);
	return 0;
}

int putDataFromDectrypStreamArray(PBYTE buf, u16 pid)
{
	EnterCriticalSection(&decryStreamAarrayLockSection);
	if (decryp_stream_Array->size() == 0)
	{
		LeaveCriticalSection(&decryStreamAarrayLockSection);
		return 0;
	}
	list<ddbsdk_decryp_stream*>::iterator iter;
	for (iter = decryp_stream_Array->begin(); iter != decryp_stream_Array->end(); ++iter)
	{
		(*iter)->putPlainTsData(buf, pid);
	}
	LeaveCriticalSection(&decryStreamAarrayLockSection);
	return 0;
}

int setCWFromDectrypProgInfoArray(WORD wEcmPID, BYTE byKeyLen, const unsigned char* szOddKey,const unsigned char* szEvenKey)
{
	if (decryp_program_info_Array == 0)
	{
		return 0;
	}
	if (decryp_program_info_Array->size() == 0)
	{
		return 0;
	}
	list<ddbsdk_decryp_program_info*>::iterator iter;
	for(iter = decryp_program_info_Array->begin(); iter != decryp_program_info_Array->end(); ++iter)
	{
		if ((*iter)->ecm_pid == wEcmPID)
		{
			//printf("设置解扰器==>   wEcmPID = %d\n",wEcmPID);
			memcpy((*iter)->g_OddCW, szOddKey, byKeyLen);
			memcpy((*iter)->g_EvenCW, szEvenKey, byKeyLen);
			(*iter)->isHaveCW = true;
			return 0;
		}
	}
	return 0;
}

int setCWoffFromDectrypProgInfoArray(WORD wEcmPID)
{
	if (decryp_program_info_Array == 0)
	{
		return 0;
	}
	if (decryp_program_info_Array->size() == 0)
	{
		return 0;
	}
	list<ddbsdk_decryp_program_info*>::iterator iter;
	for(iter = decryp_program_info_Array->begin(); iter != decryp_program_info_Array->end(); ++iter)
	{
		if ((*iter)->ecm_pid == wEcmPID)
		{
			(*iter)->isHaveCW = false;
			return 0;
		}
	}
	return 0;
}

ddbsdk_decryp_program_info* getNodeFromDectryProgInfoArray(u16 prom_pid)
{
	if (decryp_program_info_Array->size() == 0)
	{
		return 0;
	}
	list<ddbsdk_decryp_program_info*>::iterator iter;
	for (iter = decryp_program_info_Array->begin(); iter != decryp_program_info_Array->end(); ++iter)
	{
		if ((*iter)->prog_pid == prom_pid || (*iter)->ecm_pid == prom_pid || (*iter)->emm_pid == prom_pid)
		{
			return (*iter);
		}
	}
	return 0;
}

int cleanFromDecrypProgInfoArray()
{
	if (decryp_program_info_Array->size() == 0)
	{
		return 0;
	}
	list<ddbsdk_decryp_program_info*>::iterator iter;
	for (iter = decryp_program_info_Array->begin(); iter != decryp_program_info_Array->end(); ++iter)
	{
		delete (*iter);
	}
	decryp_program_info_Array->clear();
	delete decryp_program_info_Array;
	decryp_program_info_Array = 0;
	return 0;
}
