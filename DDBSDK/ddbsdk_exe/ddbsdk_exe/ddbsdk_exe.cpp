
#include <windows.h>
#include "stdafx.h"
#include <stdio.h>
#include "ddbsdk_exe.h"
#include "../log/LocalLog.h"

static bool deviceOpen = false;

extern DWORD 
DDB_Init(
  LPCTSTR pContext,
  DWORD dwBusContext
);

extern BOOL 
DDB_Close(
    DWORD Handle
    );

extern BOOL 
DDB_Deinit(
    DWORD dwContext     // future: pointer to the per disk structure
    );

extern DWORD 
DDB_Open(
    DWORD dwData,
    DWORD dwAccess,
    DWORD dwShareMode
    );

extern BOOL 
DDB_IOControl(
    DWORD Handle,
    DWORD dwIoControlCode,
    PBYTE pInBuf,
    DWORD nInBufSize,
    PBYTE pOutBuf,
    DWORD nOutBufSize,
    PDWORD pBytesReturned
    );

#define DDBSDK_SERVICEFILE_DEBUG_FILE 0

#if DDBSDK_SERVICEFILE_DEBUG_FILE
#include "../ddbsdk_ca/CA_STB_decode.h"
#include "../src/ddbsdk.h"
#include "../ddbsdk_data/ddbsdk_data.h"
extern ddbsdk_t   ddbsdk;
#define TEMPBUFSIZE   188 * 16

extern void DataCallback(void*	    ClientPtr, 
						  UINT32	ServiceHandle, 
						  UINT8*	pBuffer, 
						  UINT32	BufferSize );
#endif

void usagePrint(u32  version)
{
	char * strHelp = {
"ddbsdk test usage: \n\
-f val: set frequency, (default: 1470080000, the other freq 1458096000),\n\
-d val: test private dab data, (val: 0-4\n\
    0: traffic, 1: weather, 2: finance,\n\
    3: PIO info, 4: download file)\n\
-l val: list private dab data, and test one channel,(val:0-4\n\
    0: traffic, 1: weather, 2: finance,\n\
    3: PIO info)\n\
-v val: test video program, (val: 0-1,\n\
    0: first Finance, 1: CCTV-5)\n\
-m val: test audio program, (val: 0-2, 0: classic 947, \n\
    1: e-news, 2: traffic)\n\
-r: register .dll lib, only used for register ddbsdk_drv.dll\n\
-h: help\n\
-s: ddbsdk_drv.dll software version, ca version\n\
-p val: test the program in the list that -l listed,\n\
     (val: list num)\n\
-t : set SMG lab environment, freqency=1456384000, component index=0 \n\
-x : set file_test_mode var, (only used for test)\n"
	};
	printf("ddbsdk version: %x.%x.%x\n%s", ((version >> 16) & 0xFF), ((version >> 8) & 0xFF), (version & 0xFF), strHelp);
}

void TEXT2ASC(LPTSTR    strT, char * strA)
{
	while(* strT != 0) {
		* strA = (char) * strT;
		strA ++;
		strT ++;
	}
}

void getComLineChar(char ** sPtr)
{
	while(*(* sPtr) != '\0') {
		if(*(* sPtr) == '-') {(* sPtr) ++;   break;}
		(* sPtr) ++;
	}
}

u32 getComLineVal(char ** sPtr)
{
	int    cnt = 0;
	unsigned int ret = 0;

	(* sPtr) ++;
	while(*(* sPtr) == ' ') { (* sPtr) ++; }
	while(*(* sPtr) != ' ' && *(* sPtr) != '\0' ) {
		cnt ++;
		ret = ret * 10 + (*(* sPtr) - 0x30);
		(* sPtr) ++;
		if(cnt > 32) break;
	}
	return ret;
}

u16 get_time_t_year(unsigned long time)
{
	return (u16)(1970 + (time / (3600 * 24 * 365.25)));
}

u16 get_time_t_day(unsigned long time)
{
	u16 y = (u16)(time / (3600 * 24 * 365.25));

	return (u16)((time - y * (3600 * 24 * 365.25)) / (3600 * 24) + 1) ;
}

void ddbif_dataListTest(void)
{
	DWORD dwThreadID_1;
	DWORD nParameter_1;
	HANDLE  hThread_1;
	ddbsdk_dmbdataInfo_t    dataInfo = { 0 };
	DWORD BytesReturned = 0;
	int getCount = 0;

	if(deviceOpen == false) {
		//if(file_test_mode == 0) {
			DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
		//} else {
		//	hThread_1 = CreateThread(NULL, 0, thread_sendStreamData, &nParameter_1, 0, &dwThreadID_1);
		//}
		deviceOpen = true;
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_DATAPROGRAM, NULL, 0, (unsigned char *)&dataInfo, sizeof(ddbsdk_dmbdataInfo_t), NULL);

	for(int i = 0; i < dataInfo.ServiceNum; i++)
	{
		printf("-------------program[%d] info:---------------\n", i);
		printf("serviceNo:%d\n",dataInfo.dataInfo[i].serviceNo);
		printf("ServiceIndex:%d\n",dataInfo.dataInfo[i].ServiceIndex);
		printf("data_pid:%d\n",dataInfo.dataInfo[i].data_pid);
		printf("ecm_pid:%d\n",dataInfo.dataInfo[i].ecm_pid);
		printf("emm_pid:%d\n",dataInfo.dataInfo[i].emm_pid);
		printf("stream_type:%x\n",dataInfo.dataInfo[i].stream_type);
		printf("descr:%s\n",dataInfo.dataInfo[i].descr);
		printf("next:%x\n",dataInfo.dataInfo[i].next);
	}
}


unsigned long  thread_sendStreamData(void * param)
{
	FILE * fp;
	int   flen, pos;
	unsigned char * tempTsBuffer;
	
	fp = fopen("\\StorageCard\\TS\\1049.ts", "rb");
//	fp = fopen("\\MMC_Storage\\dump-data.ts", "rb");
#define  TEMPTSBUFSIZE   188 * 16
	tempTsBuffer = (unsigned char *)malloc(TEMPTSBUFSIZE);

	DDB_IOControl(NULL, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL);
	while(1) {
		Sleep(70);
		/* read ts file to test service file download */
		flen = fread(tempTsBuffer, 1, TEMPTSBUFSIZE, fp);

		/* call datacallback to transfer data */
		DDB_IOControl(NULL, IOCTL_DDBSDK_FILEINPUT_TEST, tempTsBuffer, flen, NULL, 0, NULL);

		pos = ftell(fp);
		if(feof(fp)) {
			fseek(fp, 0, SEEK_SET);
		}
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CANCEL_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL);
#undef  TEMPTSBUFSIZE

	free(tempTsBuffer);
	fclose(fp);

	return 0;
}

unsigned long  thread_sendStreamMusicam(void * param)
{
	FILE * fp;
	int   flen, pos;
	unsigned char * tempTsBuffer;
	
	fp = fopen("\\MMC_Storage\\dump-classic_947.mp2", "rb");
//	fp = fopen("\\MMC_Storage\\dump-E-news.mp2", "rb");
#define  TEMPMP2BUFSIZE   384 * 16
	tempTsBuffer = (unsigned char *)malloc(TEMPMP2BUFSIZE);

	DDB_IOControl(NULL, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL);
	while(1) {
		Sleep(330);
		/* read ts file to test service file download */
		flen = fread(tempTsBuffer, 1, TEMPMP2BUFSIZE, fp);

		/* call datacallback to transfer data */
		DDB_IOControl(NULL, IOCTL_DDBSDK_FILEINPUT_TEST, tempTsBuffer, flen, NULL, 0, NULL);

		pos = ftell(fp);
		if(feof(fp)) {
			fseek(fp, 0, SEEK_SET);
		}
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CANCEL_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL);
#undef  TEMPMP2BUFSIZE

	free(tempTsBuffer);
	fclose(fp);

	return 0;
}

unsigned long  thread_sendStreamVideo(void * param)
{
	FILE * fp;
	int   flen, pos;
	unsigned char * tempTsBuffer;
	fp = fopen("\\MMC_Storage\\dump-CCTV5.ts", "rb");
#define  TEMPTSBUFSIZE   188 * 16
	tempTsBuffer = (unsigned char *)malloc(TEMPTSBUFSIZE);

	DDB_IOControl(NULL, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL);
	while(1) {
		Sleep(50);
		/* read ts file to test service file download */
		flen = fread(tempTsBuffer, 1, TEMPTSBUFSIZE, fp);

		/* call datacallback to transfer data */
		DDB_IOControl(NULL, IOCTL_DDBSDK_FILEINPUT_TEST, tempTsBuffer, flen, NULL, 0, NULL);

		pos = ftell(fp);
		if(feof(fp)) {
			fseek(fp, 0, SEEK_SET);
		}
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CANCEL_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL);
#undef  TEMPTSBUFSIZE

	free(tempTsBuffer);
	fclose(fp);

	return 0;
}

unsigned long  thread_getStreamVideo(void * param)
{
	FILE  * f_video = fopen("\\Temp\\video.dat", "wb");
	unsigned char * tempTsBuffer;
	DWORD BytesReturned = 0;

#define  TEMPVIDEOBUFSIZE   20000
	tempTsBuffer = (unsigned char *)malloc(TEMPVIDEOBUFSIZE);

	DWORD testCount = 0, getCount;
	char tempHeader[5] = {0};
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_VIDEO_ONEFRAMEVIDEO, NULL, 0, tempTsBuffer, TEMPVIDEOBUFSIZE, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) 
		{
// 			fwrite(tempHeader, 1, 1, f_video);
// 			fwrite(&getCount, 4, 1, f_video);
			fwrite(tempTsBuffer, 1, getCount, f_video);
		}
		if((testCount & 0x7) == 0) {
			printf("video, count: %d, bytes: %d\n", testCount, getCount);
		}
		if(testCount > 100000) {
			break;
		}
	}
#undef  TEMPVIDEOBUFSIZE

	fclose(f_video);
	return 0;
}

unsigned long  thread_getStreamAudio(void * param)
{
	FILE  * f_audio = fopen("\\Temp\\audio.dat", "wb");
	unsigned char * tempTsBuffer;
	DWORD BytesReturned = 0;

#define  TEMPAUDIOBUFSIZE   1024
	tempTsBuffer = (unsigned char *)malloc(TEMPAUDIOBUFSIZE);

	DWORD testCount = 0, getCount;
	char tempHeader[5] = {0};
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_VIDEO_ONEFRAMEAUDIO, NULL, 0, tempTsBuffer, TEMPAUDIOBUFSIZE, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) 
		{
			fwrite(tempHeader, 1, 5, f_audio);
			fwrite(tempTsBuffer, 1, getCount, f_audio);
		}
		if((testCount & 0x7) == 0) {
			printf("audio, count: %d, bytes: %d\n", testCount, getCount);
		}
		if(testCount > 100000) {
			break;
		}
	}
#undef  TEMPVIDEOBUFSIZE

	fclose(f_audio);
	return 0;
}

void ddbif_videoTest(u32   videoNo)
{
	DWORD dwThreadID_1, dwThreadID_2, dwThreadID_3;
	DWORD nParameter_1, nParameter_2, nParameter_3;
	HANDLE  hThread_1, hThread_2, hThread_3;
	ddbsdk_dmbvideoInfo_t    videoInfo = { 0 };
	DWORD BytesReturned = 0;
	int getCount = 0;

// 	if(deviceOpen == false) {
// 		if(file_test_mode == 0) {
// 			DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
// 		} else {
// 			hThread_1 = CreateThread(NULL, 0, thread_sendStreamVideo, &nParameter_1, 0, &dwThreadID_1);
// 		}
// 		deviceOpen = true;
// 	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_VIDEOPROGRAM, NULL, 0, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), NULL);

	videoInfo.selectServiceNo = videoNo;
	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_VIDEO, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), NULL, 0, NULL);

	/* create thread to get video */
	hThread_2 = CreateThread(NULL, 0, thread_getStreamVideo, &nParameter_2, 0, &dwThreadID_2);

	/* create thread to get audio */
	hThread_3 = CreateThread(NULL, 0, thread_getStreamAudio, &nParameter_3, 0, &dwThreadID_3);

	FILE  * f_pcr = fopen("\\Temp\\pcr.dat", "wb");
	if (f_pcr == NULL)
	{
		printf("Open PCR File Failed!\n");
	}
	while(1) { 
		Sleep(1000);
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_VIDEO_PCR, NULL, 0, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), &BytesReturned);
		printf("pcr: 0x%x%x\n", (u32)(videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base >> 32),
			(u32)videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base);
		if (f_pcr != NULL)
		{
			fwrite(&(videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base), 1, 8, f_pcr);
		}

	}
// 	if(file_test_mode == 1) {
// 		CloseHandle(hThread_1);
// 	}
	CloseHandle(hThread_2);
	CloseHandle(hThread_3);

	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_VIDEO, NULL, 0, NULL, 0, NULL);
}


typedef struct _myBufferStru_
{
	PBYTE buffer;
	DWORD size;

}myBufferStru;

unsigned long Thread_test_getTsStream(void* param)
{
	PBYTE buffer;
	DWORD size;

	buffer = ((myBufferStru*) param)->buffer;
	size = ((myBufferStru*) param)->size;

	DWORD BytesReturned = 0;
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	int getCount = 0;
	int tmpCount = 0;
	char tempHeader[5] = {0};

	DDB_IOControl(NULL, IOCTL_DDBSDK_TS_STREAM_OPEN, NULL, 0, NULL, 0, NULL);
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_TS_STREAM_GET, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) 
		{
			FILE  * f_data = fopen("\\StorageCard\\tsStream.ts", "ab");
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		else
		{
			/*
			memset(ca_promt_Message_Weather,0,50); 
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETCA_PROMPTMSG_WEATHER, NULL, 0, ca_promt_Message_Weather, 0, &returnbytes, NULL);
			printf("天气CA_promt_Message:%s   returnbytes:%d\n", ca_promt_Message_Weather, returnbytes);
			*/
		}
		if((testCount & 0x7) == 0)
		{
			printf("test_getTsStream ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
		}
		//if(testCount == 80)break;
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_TS_STREAM_CLOSE, NULL, 0, NULL, 0, NULL);
	return 0;
}

unsigned long Thread_test_RoadConditionsFile(void* param)
{
	BOOL isStartTick = FALSE;
	DWORD testCount = 0;
	DWORD startTime = 0;
	{
		ddbsdk_servFileStatus_t   servStatus = { 0 };
		ddbsdk_servFileInfo_t fileInfo = { 0 };
		int cnt = 0;
		DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_ROADCONDITION_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
		servStatus.selectedFILE_TYPE = 9;
		while(1) {
			Sleep(2000);
			testCount++;
			cnt = 0;
			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_ROADCONDITION_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
			if(servStatus.numDownloading == 0) {
				if(!isStartTick)
				{
					startTime = ::GetTickCount();
					isStartTick = TRUE;
				}
				//if((testCount & 0xffff7) == 0) 
				printf("RoadConditionsFile:no file downloading!\n");
				//WriteLog("RoadConditionsFile:no file downloading!");
			} else {
				//if((testCount & 0xffff7) == 0) 
				printf("RoadConditionsFile:downing number: %d!\n", servStatus.numDownloading);
				//WriteLog("RoadConditionsFile:downing number: %d!", servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
				printf("RoadConditionsFile:downing: %x\n", servStatus.fileID_downing[cnt]);
				//WriteLog("RoadConditionsFile:downing: %x", servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
				//if((testCount & 0xffff7) == 0) 
				printf("RoadConditionsFile:no file finished!\n");
				WriteLog("RoadConditionsFile:no file finished!");
			} else {
				//if((testCount & 0xffff7) == 0)
				printf("RoadConditionsFile:file finished number: %d!\n", servStatus.numFinished);
				//WriteLog("RoadConditionsFile:file finished number: %d!", servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
				//if((testCount & 0xffff7) == 0) 
				printf("RoadConditionsFile:finished: %x\n", servStatus.fileID_finished[cnt]);
				//WriteLog("RoadConditionsFile:finished: %x", servStatus.fileID_finished[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				DWORD pastTime = ::GetTickCount() - startTime;
				printf("RoadConditionsFile:%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
				//WriteLog("RoadConditionsFile:%d类型文件全部下载完成用时：%ld毫秒", servStatus.selectedFILE_TYPE, pastTime);
				break;
			}
#if 0

			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_ROADCONDITION_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);

				DDB_IOControl(NULL, IOCTL_DDBSDK_ROADCONDITION_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\MMC_Storage\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);

				break;
			}
#endif
			break;
		}
		DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_ROADCONDITION_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
	}
	return 0;
}

unsigned long Thread_test_EmergencyFile(void* param)
{
	BOOL isStartTick = FALSE;
	DWORD testCount = 0;
	DWORD startTime = 0;
	{
		ddbsdk_servFileStatus_t   servStatus = { 0 };
		ddbsdk_servFileInfo_t fileInfo = { 0 };
		int cnt = 0;
		DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_EMERGENCYBRO_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
		servStatus.selectedFILE_TYPE = 127;
		while(1) {
			Sleep(2000);
			testCount++;
			cnt = 0;
			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_EMERGENCYBRO_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
			if(servStatus.numDownloading == 0) {
				if(!isStartTick)
				{
					startTime = ::GetTickCount();
					isStartTick = TRUE;
				}
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:no file downloading!\n");
				WriteLog("EmergencyFile:no file downloading!");
			} else {
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:downing number: %d!\n", servStatus.numDownloading);
				WriteLog("EmergencyFile:downing number: %d!", servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
				printf("EmergencyFile:downing: %x\n", servStatus.fileID_downing[cnt]);
				WriteLog("EmergencyFile:downing: %x", servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:no file finished!\n");
				WriteLog("EmergencyFile:no file finished!");
			} else {
				//if((testCount & 0xffff7) == 0)
				printf("EmergencyFile:file finished number: %d!\n", servStatus.numFinished);
				WriteLog("EmergencyFile:file finished number: %d!", servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:finished: %x\n", servStatus.fileID_finished[cnt]);
				WriteLog("EmergencyFile:finished: %x", servStatus.fileID_finished[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				DWORD pastTime = ::GetTickCount() - startTime;
				printf("EmergencyFile:%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
				//WriteLog("EmergencyFile:%d类型文件全部下载完成用时：%ld毫秒", servStatus.selectedFILE_TYPE, pastTime);
				break;
			}
#if 0

			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_EMERGENCYBRO_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);

				DDB_IOControl(NULL, IOCTL_DDBSDK_EMERGENCYBRO_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\MMC_Storage\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);

				break;
			}
#endif
			break;
		}
		DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_EMERGENCYBRO_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
	}
	return 0;
}



unsigned long _thread_show_typeFile(void* param)
{
	int fileType = *(int *)param;

	BOOL isStartTick = FALSE;
	DWORD testCount = 0;
	DWORD startTime = 0;

	ddbsdk_servFileStatus_t   servStatus = { 0 };
	ddbsdk_servFileInfo_t fileInfo = { 0 };
	int cnt = 0;

	servStatus.selectedFILE_TYPE = fileType;

	while(1) {
		Sleep(2000);
		testCount++;
		cnt = 0;
		DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_DATA_DOWNLOADINGFILESTATUS, 
			(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
			(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
		if(servStatus.numDownloading == 0) {
			if(!isStartTick)
			{
				startTime = ::GetTickCount();
				isStartTick = TRUE;
			}
			//if((testCount & 0xffff7) == 0) 
			printf("FileType[%d]:no file downloading!\n",servStatus.selectedFILE_TYPE);
			//WriteLog("WeatherFile:no file downloading!");
		} else {
			//if((testCount & 0xffff7) == 0) 
			printf("FileType[%d]:downing number: %d!\n", servStatus.selectedFILE_TYPE, servStatus.numDownloading);
			//WriteLog("WeatherFile:downing number: %d!", servStatus.numDownloading);
		}
// 		while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
// 			printf("WeatherFile:downing: %x\n", servStatus.fileID_downing[cnt]);
// 			//WriteLog("WeatherFile:downing: %x", servStatus.fileID_downing[cnt]);
// 			cnt ++;
// 		}
		if(servStatus.numFinished == 0) {
			//if((testCount & 0xffff7) == 0) 
			printf("FileType[%d]:no file finished!\n",servStatus.selectedFILE_TYPE);
			//WriteLog("WeatherFile:no file finished!");
		} else {
			//if((testCount & 0xffff7) == 0)
			printf("FileType[%d]:file finished number: %d!\n", servStatus.selectedFILE_TYPE, servStatus.numFinished);
			//WriteLog("WeatherFile:file finished number: %d!", servStatus.numFinished);
		}
// 		cnt = 0;
// 		while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
// 			//if((testCount & 0xffff7) == 0) 
// 			printf("WeatherFile:finished: %x\n", servStatus.fileID_finished[cnt]);
// 			//WriteLog("WeatherFile:finished: %x", servStatus.fileID_finished[cnt]);
// 			cnt ++;
//		}
		if(servStatus.numFinished && servStatus.numDownloading == 0) {
			DWORD pastTime = ::GetTickCount() - startTime;
			printf("DownloadFile:%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
			//WriteLog("WeatherFile:%d类型文件全部下载完成用时：%ld毫秒", servStatus.selectedFILE_TYPE, pastTime);
			break;
		}
		break;
	}
	return 0;
}

unsigned long Thread_test_WeatherFile(void* param)
{
	BOOL isStartTick = FALSE;
	DWORD testCount = 0;
	DWORD startTime = 0;
	{
		ddbsdk_servFileStatus_t   servStatus = { 0 };
		ddbsdk_servFileInfo_t fileInfo = { 0 };
		int cnt = 0;
		DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_WEATHER_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
		servStatus.selectedFILE_TYPE = 14;
		while(1) {
			Sleep(2000);
			testCount++;
			cnt = 0;
			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_WEATHER_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
			if(servStatus.numDownloading == 0) {
				if(!isStartTick)
				{
					startTime = ::GetTickCount();
					isStartTick = TRUE;
				}
				//if((testCount & 0xffff7) == 0) 
				printf("WeatherFile:no file downloading!\n");
				//WriteLog("WeatherFile:no file downloading!");
			} else {
				//if((testCount & 0xffff7) == 0) 
				printf("WeatherFile:downing number: %d!\n", servStatus.numDownloading);
				//WriteLog("WeatherFile:downing number: %d!", servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
				printf("WeatherFile:downing: %x\n", servStatus.fileID_downing[cnt]);
				//WriteLog("WeatherFile:downing: %x", servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
				//if((testCount & 0xffff7) == 0) 
				printf("WeatherFile:no file finished!\n");
				//WriteLog("WeatherFile:no file finished!");
			} else {
				//if((testCount & 0xffff7) == 0)
				printf("WeatherFile:file finished number: %d!\n", servStatus.numFinished);
				//WriteLog("WeatherFile:file finished number: %d!", servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
				//if((testCount & 0xffff7) == 0) 
				printf("WeatherFile:finished: %x\n", servStatus.fileID_finished[cnt]);
				//WriteLog("WeatherFile:finished: %x", servStatus.fileID_finished[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				DWORD pastTime = ::GetTickCount() - startTime;
				printf("WeatherFile:%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
				//WriteLog("WeatherFile:%d类型文件全部下载完成用时：%ld毫秒", servStatus.selectedFILE_TYPE, pastTime);
				break;
			}
#if 0

			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_WEATHER_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);
				
				DDB_IOControl(NULL, IOCTL_DDBSDK_WEATHER_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\MMC_Storage\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}
				
				free(temp_fileBuf);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);

				break;
			}
#endif
			break;
		}
		DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_WEATHER_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
	}
	return 0;
}

unsigned long Thread_test_downloadFile(void* param)
{
	BOOL isStartTick = FALSE;
	DWORD testCount = 0;
	DWORD startTime = 0;
	{

		DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);

		int type5 = 5;
		int type8 = 8;
		int type10 = 10;
		int type11 = 11;
		int type12 = 12;
		int type15 = 15;
		int type19 = 19;
		
		DWORD filetype_5_H_ID = 0;
		DWORD filetype_8_H_ID = 0;
		DWORD filetype_10_H_ID = 0;
		DWORD filetype_11_H_ID = 0;
		DWORD filetype_12_H_ID = 0;
		DWORD filetype_15_H_ID = 0;
		DWORD filetype_19_H_ID = 0;
		
		HANDLE filetype_5_H = CreateThread(NULL, 0, _thread_show_typeFile, &type5, 0, &filetype_5_H_ID);
		HANDLE filetype_8_H = CreateThread(NULL, 0, _thread_show_typeFile, &type8, 0, &filetype_8_H_ID);
		HANDLE filetype_10_H = CreateThread(NULL, 0, _thread_show_typeFile, &type10, 0, &filetype_10_H_ID);
		HANDLE filetype_11_H = CreateThread(NULL, 0, _thread_show_typeFile, &type11, 0, &filetype_11_H_ID);
		HANDLE filetype_12_H = CreateThread(NULL, 0, _thread_show_typeFile, &type12, 0, &filetype_12_H_ID);
		HANDLE filetype_15_H = CreateThread(NULL, 0, _thread_show_typeFile, &type15, 0, &filetype_15_H_ID);
		HANDLE filetype_19_H = CreateThread(NULL, 0, _thread_show_typeFile, &type19, 0, &filetype_19_H_ID);

		WaitForSingleObject(filetype_5_H, INFINITE);
		WaitForSingleObject(filetype_8_H, INFINITE);
		WaitForSingleObject(filetype_10_H, INFINITE);
		WaitForSingleObject(filetype_11_H, INFINITE);
		WaitForSingleObject(filetype_12_H, INFINITE);
		WaitForSingleObject(filetype_15_H, INFINITE);
		WaitForSingleObject(filetype_19_H, INFINITE);
// 		ddbsdk_servFileStatus_t   servStatus = { 0 };
// 		ddbsdk_servFileInfo_t fileInfo = { 0 };
// 		int cnt = 0;
//		servStatus.selectedFILE_TYPE = 12;
// 		while(1) {
// 			Sleep(2000);
// 			testCount++;
// 			cnt = 0;
// 			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_DATA_DOWNLOADINGFILESTATUS, 
// 				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
// 				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
// 			if(servStatus.numDownloading == 0) {
// 				if(!isStartTick)
// 				{
// 					startTime = ::GetTickCount();
// 					isStartTick = TRUE;
// 				}
// 				//if((testCount & 0xffff7) == 0) 
// 				printf("no file downloading!\n");
// 				//WriteLog("no file downloading!");
// 			} else {
// 				//if((testCount & 0xffff7) == 0) 
// 					printf("downing number: %d!\n", servStatus.numDownloading);
// 					//WriteLog("downing number: %d!", servStatus.numDownloading);
// 			}
// 			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
// 				printf("downing: %x\n", servStatus.fileID_downing[cnt]);
// 				//WriteLog("downing: %x", servStatus.fileID_downing[cnt]);
// 				cnt ++;
// 			}
// 			if(servStatus.numFinished == 0) {
// 				//if((testCount & 0xffff7) == 0) 
// 					printf("no file finished!\n");
// 					//WriteLog("no file finished!");
// 			} else {
// 				//if((testCount & 0xffff7) == 0)
// 					printf("file finished number: %d!\n", servStatus.numFinished);
// 					//WriteLog("file finished number: %d!", servStatus.numFinished);
// 			}
// 			cnt = 0;
// 			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
// 				//if((testCount & 0xffff7) == 0) 
// 					printf("finished: %x\n", servStatus.fileID_finished[cnt]);
// 					//WriteLog("finished: %x", servStatus.fileID_finished[cnt]);
// 				cnt ++;
// 			}
// 			if(servStatus.numFinished && servStatus.numDownloading == 0) {
// 				DWORD pastTime = ::GetTickCount() - startTime;
// 				printf("%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
// 				//WriteLog("%d类型文件全部下载完成用时：%ld毫秒", servStatus.selectedFILE_TYPE, pastTime);
// 				break;
// 			}
#if 0

			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);
				/*
				DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\MMC_Storage\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}*/
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_ONE_FINISHEDFILE, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
				if(bytesreturned > 0)
				{
					df = fopen("\\temp\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}
				free(temp_fileBuf);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);

				break;
			}
#endif
//		}
		DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
		}
	return 0;
}

unsigned long Thread_test_TravelFile(void* param)
{
	BOOL isStartTick = FALSE;
	DWORD testCount = 0;
	DWORD startTime = 0;
	{
		ddbsdk_servFileStatus_t   servStatus = { 0 };
		ddbsdk_servFileInfo_t fileInfo = { 0 };
		int cnt = 0;
		DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_TRAVELFILE_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
		servStatus.selectedFILE_TYPE = 22;
		while(1) {
			Sleep(2000);
			testCount++;
			cnt = 0;
			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_TRAVELFILE_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
			if(servStatus.numDownloading == 0) {
				if(!isStartTick)
				{
					startTime = ::GetTickCount();
					isStartTick = TRUE;
				}
				//if((testCount & 0xffff7) == 0) 
				printf("TravelFile:no file downloading!\n");
				//WriteLog("TravelFile:no file downloading!");
			} else {
				//if((testCount & 0xffff7) == 0) 
				printf("TravelFile:downing number: %d!\n", servStatus.numDownloading);
				//WriteLog("TravelFile:downing number: %d!", servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
				printf("TravelFile:downing: %x\n", servStatus.fileID_downing[cnt]);
				//WriteLog("TravelFile:downing: %x", servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
				//if((testCount & 0xffff7) == 0) 
				printf("TravelFile:no file finished!\n");
				//WriteLog("TravelFile:no file finished!");
			} else {
				//if((testCount & 0xffff7) == 0)
				printf("TravelFile:file finished number: %d!\n", servStatus.numFinished);
				//WriteLog("TravelFile:file finished number: %d!", servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
				//if((testCount & 0xffff7) == 0) 
				printf("TravelFile:finished: %x\n", servStatus.fileID_finished[cnt]);
				//WriteLog("TravelFile:finished: %x", servStatus.fileID_finished[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				DWORD pastTime = ::GetTickCount() - startTime;
				printf("TravelFile:%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
				//WriteLog("EmergencyFile:%d类型文件全部下载完成用时：%ld毫秒", servStatus.selectedFILE_TYPE, pastTime);
				//break;
			}
#if 0

			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_TRAVELFILE_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);

				DDB_IOControl(NULL, IOCTL_DDBSDK_TRAVELFILE_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\MMC_Storage\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);

				break;
			}
#endif
			break;
		}
		DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_TRAVELFILE_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
	}
	return 0;
}

unsigned long Thread_test_ExpoFile(void* param)
{
	BOOL isStartTick = FALSE;
	DWORD testCount = 0;
	DWORD startTime = 0;
	{
		DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_EXPOFILE_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);

		int type5 = 28;
		int type8 = 29;
		int type10 = 30;
		int type11 = 31;
 		int type12 = 27;
//  	int type15 = 28;
//  	int type19 = 29;

		DWORD filetype_5_H_ID = 0;
		DWORD filetype_8_H_ID = 0;
		DWORD filetype_10_H_ID = 0;
		DWORD filetype_11_H_ID = 0;
 		DWORD filetype_12_H_ID = 0;
//  	DWORD filetype_15_H_ID = 0;
//  	DWORD filetype_19_H_ID = 0;

		HANDLE filetype_5_H = CreateThread(NULL, 0, _thread_show_typeFile, &type5, 0, &filetype_5_H_ID);
		HANDLE filetype_8_H = CreateThread(NULL, 0, _thread_show_typeFile, &type8, 0, &filetype_8_H_ID);
		HANDLE filetype_10_H = CreateThread(NULL, 0, _thread_show_typeFile, &type10, 0, &filetype_10_H_ID);
		HANDLE filetype_11_H = CreateThread(NULL, 0, _thread_show_typeFile, &type11, 0, &filetype_11_H_ID);
 		HANDLE filetype_12_H = CreateThread(NULL, 0, _thread_show_typeFile, &type12, 0, &filetype_12_H_ID);
// 		HANDLE filetype_15_H = CreateThread(NULL, 0, _thread_show_typeFile, &type15, 0, &filetype_15_H_ID);
//  	HANDLE filetype_19_H = CreateThread(NULL, 0, _thread_show_typeFile, &type19, 0, &filetype_19_H_ID);

		WaitForSingleObject(filetype_5_H, INFINITE);
		WaitForSingleObject(filetype_8_H, INFINITE);
		WaitForSingleObject(filetype_10_H, INFINITE);
		WaitForSingleObject(filetype_11_H, INFINITE);
 		WaitForSingleObject(filetype_12_H, INFINITE);
//  	WaitForSingleObject(filetype_15_H, INFINITE);
//  	WaitForSingleObject(filetype_19_H, INFINITE);
// 		ddbsdk_servFileStatus_t   servStatus = { 0 };
// 		ddbsdk_servFileInfo_t fileInfo = { 0 };
// 		int cnt = 0;
// 		DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_EXPOFILE_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
// 		servStatus.selectedFILE_TYPE = 127;
// 		while(1) {
// 			Sleep(2000);
// 			testCount++;
// 			cnt = 0;
// 			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_EXPOFILE_DOWNLOADINGFILESTATUS, 
// 				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
// 				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
// 			if(servStatus.numDownloading == 0) {
// 				if(!isStartTick)
// 				{
// 					startTime = ::GetTickCount();
// 					isStartTick = TRUE;
// 				}
// 				//if((testCount & 0xffff7) == 0) 
// 				printf("ExpoFile:no file downloading!\n");
// 				//WriteLog("EmergencyFile:no file downloading!");
// 			} else {
// 				//if((testCount & 0xffff7) == 0) 
// 				printf("ExpoFile:downing number: %d!\n", servStatus.numDownloading);
// 				//WriteLog("EmergencyFile:downing number: %d!", servStatus.numDownloading);
// 			}
// 			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
// 				printf("ExpoFile:downing: %x\n", servStatus.fileID_downing[cnt]);
// 				//WriteLog("EmergencyFile:downing: %x", servStatus.fileID_downing[cnt]);
// 				cnt ++;
// 			}
// 			if(servStatus.numFinished == 0) {
// 				//if((testCount & 0xffff7) == 0) 
// 				printf("ExpoFile:no file finished!\n");
// 				//WriteLog("EmergencyFile:no file finished!");
// 			} else {
// 				//if((testCount & 0xffff7) == 0)
// 				printf("ExpoFile:file finished number: %d!\n", servStatus.numFinished);
// 				//WriteLog("EmergencyFile:file finished number: %d!", servStatus.numFinished);
// 			}
// 			cnt = 0;
// 			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
// 				//if((testCount & 0xffff7) == 0) 
// 				printf("ExpoFile:finished: %x\n", servStatus.fileID_finished[cnt]);
// 				//WriteLog("EmergencyFile:finished: %x", servStatus.fileID_finished[cnt]);
// 				cnt ++;
// 			}
// 			if(servStatus.numFinished && servStatus.numDownloading == 0) {
// 				DWORD pastTime = ::GetTickCount() - startTime;
// 				printf("ExpoFile:%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
// 				//WriteLog("EmergencyFile:%d类型文件全部下载完成用时：%ld毫秒", servStatus.selectedFILE_TYPE, pastTime);
// 				break;
// 			}
#if 0

			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_EXPOFILE_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);

				DDB_IOControl(NULL, IOCTL_DDBSDK_EXPOFILE_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\MMC_Storage\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);

				break;
			}
#endif
		//}
		DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_EXPOFILE_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
	}
	return 0;
}

unsigned long Thread_test_getWeahter(void* param)
{
	PBYTE buffer;
	DWORD size;

	buffer = ((myBufferStru*) param)->buffer;
	size = ((myBufferStru*) param)->size;

	DWORD BytesReturned = 0;
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	int getCount = 0;
	int tmpCount = 0;
	char tempHeader[5] = {0};

	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_WEATHER, NULL, 0, NULL, 0, NULL);
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_WEATHER, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) 
		{
			FILE  * f_data = fopen("\\MMC_Storage\\temp\\weather.dat", "ab");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		else
		{
			/*
			memset(ca_promt_Message_Weather,0,50); 
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETCA_PROMPTMSG_WEATHER, NULL, 0, ca_promt_Message_Weather, 0, &returnbytes, NULL);
			printf("天气CA_promt_Message:%s   returnbytes:%d\n", ca_promt_Message_Weather, returnbytes);
			*/
		}
		if((testCount & 0x7) == 0)
		{
			printf("test_getWeahter ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
		}
		//if(testCount == 80)break;
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_WEATHER, NULL, 0, NULL, 0, NULL);
	return 0;
}
unsigned long Thread_test_getPOIinfo(void* param)
{
	PBYTE buffer;
	DWORD size;

	buffer = ((myBufferStru*) param)->buffer;
	size = ((myBufferStru*) param)->size;

	DWORD BytesReturned = 0;
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	int getCount = 0;
	int tmpCount = 0;
	char tempHeader[5] = {0};

	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_PIOINFO, NULL, 0, NULL, 0, NULL);
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_PIOINFO, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\MMC_Storage\\temp\\POI.dat", "ab");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		if((testCount & 0x7) == 0)
		{
			printf("test_getPOIinfo ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
		}
		//if(testCount == 80)break;
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_PIOINFO, NULL, 0, NULL, 0, NULL);
	return 0;
}

unsigned long Thread_test_getTrafficData(void* param)
{
	PBYTE buffer;
	DWORD size;

	buffer = ((myBufferStru*) param)->buffer;
	size = ((myBufferStru*) param)->size;

	DWORD BytesReturned = 0;
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	int getCount = 0;
	int tmpCount = 0;
	char tempHeader[5] = {0};

	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL);
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_TRAFFIC, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\MMC_Storage\\temp\\traffic.dat", "ab");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		if((testCount & 0x7) == 0) {
			printf("test_getTrafficData ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
		if(testCount > 20)break;
		}
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL);

	return 0;
}

unsigned long Thread_test_getCarPark(void* param)
{
	PBYTE buffer;
	DWORD size;

	buffer = ((myBufferStru*) param)->buffer;
	size = ((myBufferStru*) param)->size;

	DWORD BytesReturned = 0;
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	int getCount = 0;
	int tmpCount = 0;
	char tempHeader[5] = {0};

	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_CARPARK, NULL, 0, NULL, 0, NULL);
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_CARPARK, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\MMC_Storage\\temp\\carpark.dat", "ab");
			//fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		if((testCount & 0x7) == 0) {
			printf("test_getCarPark ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
			//if(testCount == 80)break;
		}
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_CARPARK, NULL, 0, NULL, 0, NULL);

	return 0;
}

unsigned long Thread_test_getFinanceData(void* param)
{
	PBYTE buffer;
	DWORD size;

	buffer = ((myBufferStru*) param)->buffer;
	size = ((myBufferStru*) param)->size;

	DWORD BytesReturned = 0;
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	int getCount = 0;
	int tmpCount = 0;
	char tempHeader[5] = {0};

	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_FINANCE, NULL, 0, NULL, 0, NULL);
	while(1) {
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_FINANCE, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\MMC_Storage\\temp\\finance.dat", "ab");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		if((testCount & 0x7) == 0) {
			printf("test_getFinanceDat ==> count: %d, bytes: %d\n", testCount, getCount);
		//if(testCount == 80)break;
		}
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_FINANCE, NULL, 0, NULL, 0, NULL);

	return 0;
}

unsigned long Thread_test_getSignal(void* param)
{
	UINT signal = 0;
	DWORD returnbytes;

	while(1) {
		signal = 100;
		DDB_IOControl(NULL, IOCTL_DDBSDK_DMBSIGNAL_QUALITY, NULL, 0, (PBYTE)&signal, sizeof(UINT), &returnbytes);
		printf("DMBSIGNAL_QUALITY:%d   returnbytes:%d\n", signal,returnbytes);
		Sleep(3000);
	}
	return 0;
}

void ddbif_smgLabReconfig()
{
	ddbsdk_reconfigure_t param;
	UINT size = sizeof(ddbsdk_reconfigure_t);
	ZeroMemory(&param, size);
	param.frequency = 1456384000;
	param.testInSMGLabFlag = 1;
	//strcpy(param.ddbsdk_path_main, "\\MMC_Storage");
	DDB_IOControl(NULL, IOCTL_DDBSDK_RECONFIGURE, (unsigned char *)&param, sizeof(ddbsdk_reconfigure_t), NULL, 0, NULL);
	printf("set freq: %d \n", param.frequency);
	BOOL openresult = DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
	if (!openresult)
	{
		deviceOpen = false;
		printf("----------open device FAIL!--------------\n");
	} 
	else
	{
		deviceOpen = true;
		printf("----------open device!--------------\n");
	}
	
}

extern "C" BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);
#define IOCTL_HAL_GET_UUID   CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
void getUUID()
{
	BYTE pbyUUid[16];
	GUID myUUID;
	BOOL bRetVal;
	DWORD dwBytesReturned;
	bRetVal = KernelIoControl (IOCTL_HAL_GET_UUID, NULL, 0, &myUUID, 
		sizeof (myUUID), &dwBytesReturned);
	memcpy(pbyUUid, &myUUID, dwBytesReturned);
	printf("%x %x %x %x %x %x %x %x\n", pbyUUid[0], pbyUUid[1], pbyUUid[2], pbyUUid[3], pbyUUid[4], pbyUUid[5], pbyUUid[6], pbyUUid[7]);
	printf("%x %x %x %x %x %x %x %x\n", pbyUUid[8], pbyUUid[9], pbyUUid[10], pbyUUid[11], pbyUUid[12], pbyUUid[13], pbyUUid[14], pbyUUid[15]);
// 	WriteLog("%x %x %x %x %x %x %x %x", pbyUUid[0], pbyUUid[1], pbyUUid[2], pbyUUid[3], pbyUUid[4], pbyUUid[5], pbyUUid[6], pbyUUid[7]);
// 	WriteLog("%x %x %x %x %x %x %x %x", pbyUUid[8], pbyUUid[9], pbyUUid[10], pbyUUid[11], pbyUUid[12], pbyUUid[13], pbyUUid[14], pbyUUid[15]);
}

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	DWORD size;
	PBYTE buffer1;
	PBYTE buffer2;
	PBYTE buffer3;
	PBYTE buffer4;
	PBYTE buffer5;
	PBYTE buffer6;
	//bool debug_exit = false;


	getUUID();

	DDB_Init(NULL, 0);
	
	DDB_Open(0, 0, 0);
RETRY:
#if 0
	if (DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL))
	//if (DDB_IOControl(NULL, IOCTL_DDBSDK_DMB_START_DEVICE, NULL, 0, NULL, 0, NULL))
	{
		deviceOpen = true;
	}
	
/*	ddbif_smgLabReconfig();*/
	if (!deviceOpen)
	{
		return 0;
	} 

#endif
// 	Sleep(5000);
// 	int serviceNo = 4;
// 	if (TRUE != DDB_IOControl(NULL, IOCTL_DDBSDK_DMB_START_SERVICE, (PBYTE)&serviceNo, 0, NULL, 0, NULL))
// 	{
// 		return 0;
// 	}

	//ddbif_dataListTest();
#if 0
	ddbsdk_datetime dateTime;
	if (DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATETIME, NULL, 0, (PBYTE)&dateTime, sizeof(ddbsdk_datetime), NULL))
	{
		printf("%d年%d月%d日 - %d:%d:%d\n",dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, dateTime.second);
	}
#endif
/*	ddbif_videoTest(1);*/

	/* allocate memory for data transfer */
	size = 100 * 1024;
	buffer1 = (PBYTE)malloc(size);
	memset(buffer1, 0, size);
	buffer2 = (PBYTE)malloc(size);
	memset(buffer2, 0, size);
	buffer3 = (PBYTE)malloc(size);
	memset(buffer3, 0, size);
	buffer4 = (PBYTE)malloc(size);
	memset(buffer4, 0, size);
	buffer5 = (PBYTE)malloc(size);
	memset(buffer5, 0, size);
	buffer6 = (PBYTE)malloc(size);
	memset(buffer6, 0, size);

	myBufferStru myBufferInfo1, myBufferInfo2, myBufferInfo3, myBufferInfo4, myBufferInfo5, myBufferInfo6;
	myBufferInfo1.buffer = buffer1;
	myBufferInfo1.size = size;
	myBufferInfo2.buffer = buffer2;
	myBufferInfo2.size = size;
	myBufferInfo3.buffer = buffer3;
	myBufferInfo3.size = size;
	myBufferInfo4.buffer = buffer4;
	myBufferInfo4.size = size;
	myBufferInfo5.buffer = buffer5;
	myBufferInfo5.size = size;
	myBufferInfo6.buffer = buffer6;
	myBufferInfo6.size = size;

	HANDLE hThread_0, hThread_1, hThread_2, hThread_3, hThread_4, hThread_5, hThread_6, hThread_7, hThread_8, hThread_9, hThread_10, hThread_11, hThread_12, hThread_13;
	DWORD dwThreadID_0, dwThreadID_1, dwThreadID_2, dwThreadID_3, dwThreadID_4, dwThreadID_5, dwThreadID_6, dwThreadID_7, dwThreadID_8, dwThreadID_9, dwThreadID_10, dwThreadID_11, dwThreadID_12, dwThreadID_13;

	hThread_0 = CreateThread(NULL, 0, thread_sendStreamData, NULL, 0, &dwThreadID_0);
	Sleep(100);

	hThread_1 = CreateThread(NULL, 0, Thread_test_getTrafficData, &myBufferInfo1, 0, &dwThreadID_1);
	//hThread_2 = CreateThread(NULL, 0, Thread_test_getPOIinfo, &myBufferInfo2, 0, &dwThreadID_2);
	//hThread_3 = CreateThread(NULL, 0, Thread_test_getFinanceData, &myBufferInfo3, 0, &dwThreadID_3);
	//hThread_4 = CreateThread(NULL, 0, Thread_test_getWeahter, &myBufferInfo4, 0, &dwThreadID_4);
	//hThread_5 = CreateThread(NULL, 0, Thread_test_downloadFile, NULL, 0, &dwThreadID_5);
	//hThread_6 = CreateThread(NULL, 0, Thread_test_getSignal, NULL, 0, &dwThreadID_6);
	//hThread_7 = CreateThread(NULL, 0, Thread_test_getCarPark, &myBufferInfo5, 0, &dwThreadID_7);
	hThread_8 = CreateThread(NULL, 0, Thread_test_WeatherFile, NULL, 0, &dwThreadID_8);
	hThread_9 = CreateThread(NULL, 0, Thread_test_EmergencyFile, NULL, 0, &dwThreadID_9);
	hThread_10 = CreateThread(NULL, 0, Thread_test_RoadConditionsFile, NULL, 0, &dwThreadID_10);
	//hThread_11 = CreateThread(NULL, 0, Thread_test_getTsStream, &myBufferInfo6, 0, &dwThreadID_11);
	hThread_12 = CreateThread(NULL, 0, Thread_test_ExpoFile, NULL, 0, &dwThreadID_12);
	hThread_13 = CreateThread(NULL, 0, Thread_test_TravelFile, NULL, 0, &dwThreadID_13);

// 	Sleep(15000);
// 	if (TRUE != DDB_IOControl(NULL, IOCTL_DDBSDK_DMB_STOP_SERVICE, NULL, 0, NULL, 0, NULL))
// 	{
// 		return 0;
// 	}
// 	serviceNo = 3;
// 	if (TRUE != DDB_IOControl(NULL, IOCTL_DDBSDK_DMB_START_SERVICE, (PBYTE)&serviceNo, 0, NULL, 0, NULL))
// 	{
// 		return 0;
// 	}
/*
	WaitForSingleObject (eventSignal1,INFINITE);
	WaitForSingleObject (eventSignal2,INFINITE);
	WaitForSingleObject (eventSignal3,INFINITE);
	WaitForSingleObject (eventSignal4,INFINITE);
	WaitForSingleObject (eventSignal5,INFINITE);
*/
// 	Sleep(10000);
// 	DDB_IOControl(NULL, IOCTL_DDBSDK_SUSPEND_ALL_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
// 	Sleep(10*1000);
// 	DDB_IOControl(NULL, IOCTL_DDBSDK_RESUM_ALL_DOWMLOADFILE, NULL, 0, NULL, 0, NULL);

 	//WaitForSingleObject (hThread_0,INFINITE);
 	WaitForSingleObject (hThread_1,INFINITE);
// 	WaitForSingleObject (hThread_2,INFINITE);
// 	WaitForSingleObject (hThread_3,INFINITE);
// 	WaitForSingleObject (hThread_4,INFINITE);
// 	WaitForSingleObject (hThread_5,INFINITE);
// 	WaitForSingleObject (hThread_6,INFINITE);
// 	WaitForSingleObject (hThread_7,INFINITE);
	WaitForSingleObject (hThread_8,INFINITE);
 	WaitForSingleObject (hThread_9,INFINITE);
 	WaitForSingleObject (hThread_10,INFINITE);
// 	WaitForSingleObject (hThread_11,INFINITE);
	WaitForSingleObject (hThread_12,INFINITE);
	WaitForSingleObject (hThread_13,INFINITE);

	if (!DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DMBDEVICE, NULL, 0, NULL, 0, NULL))
	{
		printf("DMBDevice CLose 失败！\n");
	}
	//goto RETRY;


	free(buffer1);
	free(buffer2);
	free(buffer3);
	free(buffer4);
	free(buffer5);
	free(buffer6);

	
	DDB_Close(NULL);
	DDB_Deinit(0);

//	Sleep(2000);

	return 0;
}