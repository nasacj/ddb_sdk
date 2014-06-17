
#include "stdafx.h"
#include "ddbsdk_if.h"
//#include <BaseTyps.h>
//#include "LocalLog.h"

static HANDLE	 hDDB_sdklib = NULL;
static PBYTE     tmpBuf_Finance = NULL;
static PBYTE     tmpBuf_Traffic = NULL;
static PBYTE     tmpBuf_Weather = NULL;
static HANDLE    ddbsdkDevice = NULL;  


char ca_promt_Message_Traffic[50];
char ca_promt_Message_Weather[50];
char ca_promt_Message_Finance[50];
char ca_promt_Message_POIinfo[50];
char ca_promt_Message_DownFile[50];


HANDLE messageMutex = NULL;
HANDLE messageFileMap = NULL;
HANDLE messageEvent = NULL;
int InitMessageMapMutex();
unsigned int getErrorCode();

bool deviceOpen = false;
static int  file_test_mode = 0;


#define BUFFFERSIZE 8192
//#pragma comment( lib, "shdocvw.lib")


int ddbif_register()
{
	HANDLE	 hDDB0 = CreateFile(TEXT("DDB1:"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hDDB0 != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(hDDB0);
		hDDB0 = NULL;
		TRACE(L"Don't need to Register\n");
		return DDBIF_SUCCEED;
	}

	HKEY key;
	HKEY hSoftKey = NULL;
	unsigned long reglog = 0;
	int ret;
	BYTE   Path[128] = { 0 };
	/*LPBYTE   dllPath;*/
	WCHAR*   dllPath;
	

	if(-1 == InitMessageMapMutex())
	{
		printf("InitMessageMapMutex ERROR!\n");
	}
	
#if 1
	/* check if /Windows/ddbsdk_drv.dll exist */
	if((ret = GetFileAttributes(TEXT("\\Flash_Storage\\$msd$\\app\\ddbsdk_drv.dll"))) != -1) {
		dllPath = TEXT("\\Flash_Storage\\$msd$\\app\\ddbsdk_drv.dll");
	} else if ((ret = GetFileAttributes(TEXT("\\StorageCard\\ddbsdk\\ddbsdk_drv.dll"))) != -1) {
		dllPath = TEXT("\\StorageCard\\ddbsdk\\ddbsdk_drv.dll");
	} else {
		printf("cannot find file ddbsdk_drv.dll.\n");
		return DDBIF_ERROR;
	}
#else
	/* check if /Windows/ddbsdk_drv.dll exist */
	if((ret = GetFileAttributes(TEXT("\\Windows\\ddbsdk_drv.dll"))) != -1) {
		dllPath = (LPBYTE)TEXT("ddbsdk_drv.dll");
	} else if ((ret = GetFileAttributes(TEXT("\\StorageCard\\ddbsdk_drv.dll"))) != -1) {
		dllPath = (LPBYTE)TEXT("\\StorageCard\\ddbsdk_drv.dll");
	} else {
		printf("cannot find file ddbsdk_drv.dll.\n");
		return DDBIF_ERROR;
	}

#endif
	/* 1. if donot register the DDBSDK, register it! */
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Drivers\\BuiltIn\\DDBSDK"), 0, KEY_WRITE|KEY_READ, &hSoftKey) != ERROR_SUCCESS) {
		printf("RegCreateKeyEx\n");
		RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Drivers\\BuiltIn\\DDBSDK"),0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &reglog); 
		{
			DWORD dValue1 = 1;
			DWORD dValue0 = 0;
			//RegSetValueEx(key, TEXT("Prefix"), 0, REG_SZ, (LPBYTE)TEXT("DDB\0"), sizeof("DDB\0"));
			//RegSetValueEx(key, TEXT("DLL"), 0, REG_SZ, dllPath, 124);
			RegSetValueEx(key, TEXT("Prefix"), 0, REG_SZ, (LPBYTE)TEXT("DDB"), (wcslen(TEXT("DDB"))+1)*sizeof(wchar_t));
			RegSetValueEx(key, TEXT("DLL"), 0, REG_SZ, (LPBYTE)dllPath, (wcslen(dllPath)+1)*sizeof(wchar_t));
			RegSetValueEx(key, TEXT("Order"), 0, REG_DWORD, (LPBYTE) &dValue0, sizeof(dValue0));
			RegSetValueEx(key, TEXT("Index"), 0, REG_DWORD, (LPBYTE) &dValue1, sizeof(dValue1));
		}
	}
	RegCloseKey(key); 

	/* 2. if donot activate the DDBSDK, activate it! */
	HANDLE	 hDDB = CreateFile(TEXT("DDB1:"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hDDB == INVALID_HANDLE_VALUE) {
		ddbsdkDevice = ActivateDeviceEx(TEXT("Drivers\\BuiltIn\\DDBSDK"), NULL, 0, NULL);
		int error = GetLastError();
		if(ddbsdkDevice == NULL) {
			return DDBIF_ERROR;
		}
	} else {
		CloseHandle(hDDB);
		hDDB = NULL;
	}
	printf("registe ddbsdk_drv.dll succeed.\n");
	return DDBIF_SUCCEED;
}

int ddbif_openDdbsdk(void)
{
	hDDB_sdklib = CreateFile(TEXT("DDB1:"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hDDB_sdklib == NULL) {
		printf("open ddbsdk -- failed!\n");
		return DDBIF_ERROR;
	} else {
		printf("open a device -- succeed!\n");
	}
	return DDBIF_SUCCEED;
}

int ddbif_closeDdbsdk(void)
{
	//Sleep(3000);
	if(hDDB_sdklib) {
		CloseHandle(hDDB_sdklib);
		hDDB_sdklib = NULL;
	}
	
	if (ddbsdkDevice)
	{
		DeactivateDevice(ddbsdkDevice);
		CloseHandle(ddbsdkDevice);
		ddbsdkDevice = NULL;
	}
	

	return DDBIF_SUCCEED;
}

#if 0
int ddbif_misc(ddbif_misc_e   misc)
{
	switch(misc) {
	case DDBIF_SET_FREQUENCY:
		break;
	case DDBIF_
		break;
	}

}
#endif



int ddbif_openDmbDevice(void)
{
	

	BOOL bResult;

	bResult = DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMB_START_DEVICE, NULL , 0, NULL, 0, NULL, NULL);
	
	if(bResult == NULL) {
		printf("open ddb device -- failed!\n");
		return DDBIF_ERROR;
	} else {
		deviceOpen = true;
		printf("open ddb device -- succeed!\n");
	}

	return DDBIF_SUCCEED;
}

int ddbif_closeDmbDevice(void)
{
	if (deviceOpen)
	{
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMB_STOP_DEVICE, NULL, 0, NULL, 0, NULL, NULL);
	}
	return DDBIF_SUCCEED;
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

unsigned long  thread_sendStreamData(void * param)
{
	FILE * fp;
	int   flen, pos;
	unsigned char * tempTsBuffer;
	
	fp = fopen("\\StorageCard\\dump-withCA.ts", "rb");
//	fp = fopen("\\StorageCard\\dump-data.ts", "rb");
#define  TEMPTSBUFSIZE   188 * 16
	tempTsBuffer = (unsigned char *)malloc(TEMPTSBUFSIZE);

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		Sleep(200);
		/* read ts file to test service file download */
		flen = fread(tempTsBuffer, 1, TEMPTSBUFSIZE, fp);

		/* call datacallback to transfer data */
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_FILEINPUT_TEST, tempTsBuffer, flen, NULL, 0, NULL, NULL);

		pos = ftell(fp);
		if(feof(fp)) {
			fseek(fp, 0, SEEK_SET);
		}
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CANCEL_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL, NULL);
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
	
	fp = fopen("\\StorageCard\\dump-classic_947.mp2", "rb");
//	fp = fopen("\\StorageCard\\dump-E-news.mp2", "rb");
#define  TEMPMP2BUFSIZE   384 * 16
	tempTsBuffer = (unsigned char *)malloc(TEMPMP2BUFSIZE);

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		Sleep(330);
		/* read ts file to test service file download */
		flen = fread(tempTsBuffer, 1, TEMPMP2BUFSIZE, fp);

		/* call datacallback to transfer data */
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_FILEINPUT_TEST, tempTsBuffer, flen, NULL, 0, NULL, NULL);

		pos = ftell(fp);
		if(feof(fp)) {
			fseek(fp, 0, SEEK_SET);
		}
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CANCEL_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL, NULL);
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
	fp = fopen("\\StorageCard\\dump-CCTV5.ts", "rb");
#define  TEMPTSBUFSIZE   188 * 16
	tempTsBuffer = (unsigned char *)malloc(TEMPTSBUFSIZE);

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		Sleep(50);
		/* read ts file to test service file download */
		flen = fread(tempTsBuffer, 1, TEMPTSBUFSIZE, fp);

		/* call datacallback to transfer data */
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_FILEINPUT_TEST, tempTsBuffer, flen, NULL, 0, NULL, NULL);

		pos = ftell(fp);
		if(feof(fp)) {
			fseek(fp, 0, SEEK_SET);
		}
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CANCEL_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL, NULL);
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
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_VIDEO_ONEFRAMEVIDEO, NULL, 0, tempTsBuffer, TEMPVIDEOBUFSIZE, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) 
		{
			fwrite(tempHeader, 1, 5, f_video);
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
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_VIDEO_ONEFRAMEAUDIO, NULL, 0, tempTsBuffer, TEMPAUDIOBUFSIZE, &BytesReturned, NULL);
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



/*
unsigned long Thread_test_getCAMessage(void*  param)
{
	char* caMessage[50];
	DWORD returnbytes;

	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_CA_PROMPTMESSAGE, NULL, 0, caMessage, 0, &returnbytes, NULL);
		printf("CA_Message:%s   returnbytes:%d\n", caMessage, returnbytes);
		Sleep(5000);
	}
	return 0;
}*/
unsigned long Thread_test_getErrorCode(void* param)
{
	while(TRUE)
	{
		unsigned int errorCode = getErrorCode();
		switch(errorCode)
		{
		case CA_INTIAL_ERROR:
			printf("%s\n", "CA初始化失败");break;
		case DMB_START_FAILED:
			printf("%s\n", "DMB设备开启失败");break;
		case OPENDEVICE_GET_TDT_TIMEOUT:
			printf("%s\n", "opendevice获取时间超时");break;
		case REGSET_DMBDEVICE_FIALD:
			printf("%s\n", "注册DMB设备失败");break;
		case DDBSDK_NOT_AUTHORIZED:
			printf("%s\n", "DDBSDK未授权");break;
		case TRAFFIC_DATA_CA_UNAUTH:
			printf("%s\n", "交通数据CA未授权");break;
		case WEATHER_DATA_CA_UNAUTH:
			printf("%s\n", "气数据CA未授权");break;
		case FINANCE_DATA_CA_UNAUTH:
			printf("%s\n", "财经数据CA未授权");break;
		case POIINFO_DATA_CA_UNAUTH:
			printf("%s\n", "影院数据CA未授权");break;
		case DOWNFILE_DATA_CA_UNAUTH:
			printf("%s\n", "文件下载CA未授权");break;
		}
	}
	
	return 0;
}
unsigned long Thread_test_getSignal(void* param)
{
	UINT signal = 0;
	DWORD returnbytes;

	while(1) {
		signal = 100;
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMBSIGNAL_QUALITY, NULL, 0, &signal, sizeof(UINT), &returnbytes, NULL);
		printf("DMBSIGNAL_QUALITY:%d   returnbytes:%d\n", signal,returnbytes);
		Sleep(3000);
	}
	return 0;
}

unsigned long Thread_test_downloadGetStatus(void* param)
{
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	BOOL isStartTick = FALSE;

	DWORD startTime = 0;
	ddbsdk_servFileStatus_t   servStatus = { 0 };
	ddbsdk_servFileInfo_t fileInfo = { 0 };
	int cnt = 0;

	servStatus.selectedFILE_TYPE = *((int*)param);
		while(1) {
			Sleep(2000);
			cnt = 0;
			testCount ++;
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_DATA_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL, NULL);
			if(servStatus.numDownloading == 0) {
				printf("TYPE:%d:no file downloading!\n",servStatus.selectedFILE_TYPE);
				/*
				memset(ca_promt_Message_DownFile,0,50); 
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETCA_PROMPTMSG_DOWNFILE, NULL, 0, ca_promt_Message_DownFile, 0, &returnbytes, NULL);
				printf("文件下载CA_promt_Message:%s   returnbytes:%d\n", ca_promt_Message_DownFile, returnbytes);
				*/
			} else {
				if(!isStartTick)
				{
					startTime = ::GetTickCount();
					isStartTick = TRUE;
				}
				printf("TYPE:%d:file downing number: %d!\n", servStatus.selectedFILE_TYPE, servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
				printf("TYPE:%d:downing file: %x\n", servStatus.selectedFILE_TYPE, servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
				printf("TYPE:%d:no file finished!\n", servStatus.selectedFILE_TYPE);
			} else {
				printf("TYPE:%d:finished number: %d!\n", servStatus.selectedFILE_TYPE,servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
				printf("TYPE:%d:finished file: %x\n", servStatus.selectedFILE_TYPE, servStatus.fileID_finished[cnt]);
				cnt ++;
			}


			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				DWORD pastTime = ::GetTickCount() - startTime;
				printf("%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
				break;
			}
		
#if 0
			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, NULL);

				/*
				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, NULL, NULL);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\TEMP\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}*/

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.FILE_LENGTH);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_ONE_FINISHEDFILE, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned, NULL);
				if(bytesreturned > 0)
				{
					df = fopen("\\temp\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);

				break;
			}
#endif
			//if(testCount == 80)break;
		}
		return 0;
}
unsigned long Thread_test_downloadFileGetStatus(void* param)
{
	HANDLE thread_1;
	DWORD dwThreadID_1;
	int filetype_1 = 11;

	HANDLE thread_2;
	DWORD dwThreadID_2;
	int filetype_2 = 15;
	{
	
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		
		thread_1 = CreateThread(NULL, 0, Thread_test_downloadGetStatus, &filetype_1, 0, &dwThreadID_1);
		thread_2 = CreateThread(NULL, 0, Thread_test_downloadGetStatus, &filetype_2, 0, &dwThreadID_2);


		::WaitForSingleObject(thread_1, INFINITE);
		::WaitForSingleObject(thread_2, INFINITE);
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
	}
	return 0;
}
unsigned long Thread_test_downloadFile(void* param)
{
	DWORD returnbytes = 0;
	DWORD testCount = 0;
	BOOL isStartTick = FALSE;

	{
		DWORD startTime = 0;
		ddbsdk_servFileStatus_t   servStatus = { 0 };
		ddbsdk_servFileInfo_t fileInfo = { 0 };
		int cnt = 0;
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		servStatus.selectedFILE_TYPE = 5;
		while(1) {
			Sleep(2000);
			cnt = 0;
			testCount ++;
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_DATA_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL, NULL);
			if(servStatus.numDownloading == 0) {
//				if((testCount & 0xfff7) == 0) 
				//WriteLog("no file downloading!\n");
				printf("no file downloading!\n");
				/*
				memset(ca_promt_Message_DownFile,0,50); 
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETCA_PROMPTMSG_DOWNFILE, NULL, 0, ca_promt_Message_DownFile, 0, &returnbytes, NULL);
				printf("文件下载CA_promt_Message:%s   returnbytes:%d\n", ca_promt_Message_DownFile, returnbytes);
				*/
			} else {
				if(!isStartTick)
				{
					startTime = ::GetTickCount();
					isStartTick = TRUE;
				}
//				if((testCount & 0xfff7) == 0) 
				//WriteLog("file downing number: %d!\n", servStatus.numDownloading);
				printf("file downing number: %d!\n", servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
//				if((testCount & 0xfff7) == 0) 
				//WriteLog("downing file: %x\n", servStatus.fileID_downing[cnt]);
				printf("downing file: %x\n", servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
//				if((testCount & 0xfff7) == 0) 
				//WriteLog("no file finished!\n");
				printf("no file finished!\n");
			} else {
//				if((testCount & 0xfff7) == 0)
				//WriteLog("finished number: %d!\n", servStatus.numFinished);
				printf("finished number: %d!\n", servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
//				if((testCount & 0xfff7) == 0) 
				//WriteLog("finished file: %x\n", servStatus.fileID_finished[cnt]);
				printf("finished file: %x\n", servStatus.fileID_finished[cnt]);
				cnt ++;
			}


			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				DWORD pastTime = ::GetTickCount() - startTime;
				//WriteLog("%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
				printf("%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
				//break;
			}
		
#if 0
			/////////////// get one file that have finished ///////////////
			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;
				DWORD bytesreturned;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, NULL);

				
				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, NULL, NULL);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\TEMP\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.FILE_LENGTH);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_ONE_FINISHEDFILE, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned, NULL);
				if(bytesreturned > 0)
				{
					df = fopen("\\temp\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);

				break;
			}
#endif
			//if(testCount == 80)break;
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		}
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
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_ROADCONDITION_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		servStatus.selectedFILE_TYPE = 9;
		while(1) {
			Sleep(2000);
			testCount++;
			cnt = 0;
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_ROADCONDITION_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL, NULL);
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
				//WriteLog("RoadConditionsFile:no file finished!");
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
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_ROADCONDITION_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);

				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_ROADCONDITION_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL ,NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned, NULL);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\StorageCard\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DeviceIoControl(hDDB_sdklibL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);

				break;
			}
#endif
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_ROADCONDITION_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
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
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_EMERGENCYBRO_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		servStatus.selectedFILE_TYPE = 127;
		while(1) {
			Sleep(2000);
			testCount++;
			cnt = 0;
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_EMERGENCYBRO_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL, NULL);
			if(servStatus.numDownloading == 0) {
				if(!isStartTick)
				{
					startTime = ::GetTickCount();
					isStartTick = TRUE;
				}
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:no file downloading!\n");
				//WriteLog("EmergencyFile:no file downloading!");
			} else {
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:downing number: %d!\n", servStatus.numDownloading);
				//WriteLog("EmergencyFile:downing number: %d!", servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < servStatus.numDownloading) {
				printf("EmergencyFile:downing: %x\n", servStatus.fileID_downing[cnt]);
				//WriteLog("EmergencyFile:downing: %x", servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:no file finished!\n");
				//WriteLog("EmergencyFile:no file finished!");
			} else {
				//if((testCount & 0xffff7) == 0)
				printf("EmergencyFile:file finished number: %d!\n", servStatus.numFinished);
				//WriteLog("EmergencyFile:file finished number: %d!", servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < servStatus.numFinished) {
				//if((testCount & 0xffff7) == 0) 
				printf("EmergencyFile:finished: %x\n", servStatus.fileID_finished[cnt]);
				//WriteLog("EmergencyFile:finished: %x", servStatus.fileID_finished[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished && servStatus.numDownloading == 0) {
				DWORD pastTime = ::GetTickCount() - startTime;
				printf("EmergencyFile:%d类型文件全部下载完成用时：%ld毫秒\n", servStatus.selectedFILE_TYPE, pastTime);
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
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_EMERGENCYBRO_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);

				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_EMERGENCYBRO_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\StorageCard\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}

				free(temp_fileBuf);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);

				break;
			}
#endif
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_EMERGENCYBRO_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
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
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_WEATHER_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		servStatus.selectedFILE_TYPE = 12;
		while(1) {
			Sleep(2000);
			testCount++;
			cnt = 0;
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_WEATHER_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL, NULL);
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
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_WEATHER_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, NULL);
				
				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);
				
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_WEATHER_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesreturned);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\StorageCard\\file.dat", "wb");
					fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}
				
				free(temp_fileBuf);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);
				
				break;
			}
#endif
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_WEATHER_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
	}
	return 0;
}

unsigned long putPlainTsData(PBYTE tsBuf, UINT32 bufferSize);
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
	
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_TS_STREAM_OPEN, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_TS_STREAM_GET, NULL, 0, buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) 
		{
			FILE  * f_data = fopen("\\StorageCard\\temp\\tsStream.ts", "ab");
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
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_TS_STREAM_CLOSE, NULL, 0, NULL, 0, NULL, NULL);
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

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_WEATHER, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_WEATHER, NULL, 0, buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) 
		{
			FILE  * f_data = fopen("\\StorageCard\\temp\\weather.dat", "ab");
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
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_WEATHER, NULL, 0, NULL, 0, NULL, NULL);
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

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_PIOINFO, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_PIOINFO, NULL, 0, buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\StorageCard\\temp\\pioinfo.dat", "ab");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		else
		{
			/*
			memset(ca_promt_Message_POIinfo,0,50); 
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETCA_PROMPTMSG_POIINFO, NULL, 0, ca_promt_Message_POIinfo, 0, &returnbytes, NULL);
			printf("影院CA_promt_Message:%s   returnbytes:%d\n", ca_promt_Message_POIinfo, returnbytes);
			*/
		}
		if((testCount & 0x7) == 0)
		{
			printf("test_getPOIinfo ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
		}
		//if(testCount == 80)break;
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_PIOINFO, NULL, 0, NULL, 0, NULL, NULL);
	return 0;
}

unsigned long Thread_test_getTrafficData(void* param)
{
	PBYTE buffer;
	DWORD size;

	buffer = ((myBufferStru*) param)->buffer;
	size = ((myBufferStru*) param)->size;

	DWORD BytesReturned = 0;
	DWORD testCount = 0;
	DWORD returnbytes = 0;
	int getCount = 0;
	int tmpCount = 0;
	char tempHeader[5] = {0};


	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_TRAFFIC, NULL, 0, buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\StorageCard\\temp\\traffic.dat", "ab");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		else
		{
			/*
			memset(ca_promt_Message_Traffic,0,50);
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETCA_PROMPTMSG_TRAFFIC, NULL, 0, ca_promt_Message_Traffic, 0, &returnbytes, NULL);
			printf("交通CA_promt_Message:%s   returnbytes:%d\n", ca_promt_Message_Traffic, returnbytes);
			*/
		}
		if((testCount & 0x7) == 0) {
			printf("test_getTrafficData ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
		}
		//if(testCount == 160)	break;
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL, NULL);

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
	
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_CARPARK, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_CARPARK, NULL, 0, buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\StorageCard\\temp\\carpark.dat", "ab");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		if((testCount & 0x7) == 0) {
			printf("test_getCarPark ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
			//if(testCount == 80)break;
		}
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_CARPARK, NULL, 0, NULL, 0, NULL, NULL);
	
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

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_FINANCE, NULL, 0, NULL, 0, NULL, NULL);
	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_FINANCE, NULL, 0, buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0)
		{
			FILE  * f_data = fopen("\\StorageCard\\temp\\finance.dat", "wb");
			fwrite(tempHeader, 1, 5, f_data);
			fwrite(buffer, 1, getCount, f_data);
			fclose(f_data);
			tmpCount++;
		}
		else
		{
			/*
			memset(ca_promt_Message_Finance,0,50);
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETCA_PROMPTMSG_FINANCE, NULL, 0, ca_promt_Message_Finance, 0, &returnbytes, NULL);
			printf("财经CA_promt_Message:%s   returnbytes:%d\n", ca_promt_Message_Finance, returnbytes);
			*/
		 }
		if((testCount & 0x7) == 0) {
			printf("test_getFinanceDat ==> count: %d, bytes: %d, tmpCount: %d\n", testCount, getCount, tmpCount);
		}
		//if(testCount == 160)break;
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_FINANCE, NULL, 0, NULL, 0, NULL, NULL);
	
	return 0;
}

void ddbif_reconfig(u32 freq)
{
	ddbsdk_reconfigure_t param;
	param.frequency = freq; /* 14563840000(LAB) / 1470080000 */
	param.testInSMGLabFlag = 0;
	strcpy(param.ddbsdk_path_main, "\\StorageCard");
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_RECONFIGURE, (unsigned char *)&param, sizeof(ddbsdk_reconfigure_t), NULL, 0, NULL, NULL);
	printf("set freq: %d \n", param.frequency);
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL, NULL);
	deviceOpen = true;
	printf("----------open device!--------------\n");
}


void ddbif_dataTest(u32 dataType, PBYTE buffer, DWORD size)
{
	DWORD dwThreadID_1;
	DWORD nParameter_1;
	HANDLE  hThread_1;
	DWORD BytesReturned = 0;
	int getCount = 0;

	if(deviceOpen == false) {
		if(file_test_mode == 0) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL, NULL);
		} else {
			hThread_1 = CreateThread(NULL, 0, thread_sendStreamData, &nParameter_1, 0, &dwThreadID_1);
		}
		deviceOpen = true;
	}

	switch(dataType) {
	case 0:  /* traffic */
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL, NULL);
		{
		FILE  * f_data = fopen("\\Temp\\traffic.dat", "wb");
		DWORD testCount = 0;
		while(1) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_TRAFFIC, NULL, 0, buffer, size, &BytesReturned, NULL);
			getCount = BytesReturned;
			testCount ++;
			if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
			if((testCount & 0x7) == 0) {
				printf("count: %d, bytes: %d\n", testCount, getCount);
			}
			//if(testCount == 160)break;
		}
		fclose(f_data);
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL, NULL);
		break;
	case 1:  /* weather */
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_WEATHER, NULL, 0, NULL, 0, NULL, NULL);
		{
		FILE  * f_data = fopen("\\Temp\\weather.dat", "wb");
		DWORD testCount = 0;
		while(1) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_WEATHER, NULL, 0, buffer, size, &BytesReturned, NULL);
			getCount = BytesReturned;
			testCount ++;
			if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
			if((testCount & 0x7) == 0) {
				printf("天气数据count: %d, bytes: %d\n", testCount, getCount);
			}
			//if(testCount == 160)break;
		}
		fclose(f_data);
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_WEATHER, NULL, 0, NULL, 0, NULL, NULL);
		break;
	case 2:  /* finance */
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_FINANCE, NULL, 0, NULL, 0, NULL, NULL);
		{
		FILE  * f_data = fopen("\\Temp\\finance.dat", "wb");
		DWORD testCount = 0;
		while(1) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_FINANCE, NULL, 0, buffer, size, &BytesReturned, NULL);
			getCount = BytesReturned;
			testCount ++;
			if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
			if((testCount & 0x7) == 0) {
				printf("财经数据count: %d, bytes: %d\n", testCount, getCount);
			}
			//if(testCount == 160)break;
		}
		fclose(f_data);
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_FINANCE, NULL, 0, NULL, 0, NULL, NULL);
		break;
	case 3:  /* PIO */
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_PIOINFO, NULL, 0, NULL, 0, NULL, NULL);
		{
		FILE  * f_data = fopen("\\Temp\\pioinfo.dat", "wb");
		DWORD testCount = 0;
		while(1) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_PIOINFO, NULL, 0, buffer, size, &BytesReturned, NULL);
			getCount = BytesReturned;
			testCount ++;
			if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
			if((testCount & 0x7) == 0) {
				printf("影院数据count: %d, bytes: %d\n", testCount, getCount);
			}
			//if(testCount == 160)break;
		}
		fclose(f_data);
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_PIOINFO, NULL, 0, NULL, 0, NULL, NULL);
		break;
	case 4:  /* File */
		{
		DWORD testCount = 0;
		ddbsdk_servFileStatus_t   servStatus = { 0 };
		ddbsdk_servFileInfo_t fileInfo = { 0 };
		int cnt = 0;
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		servStatus.selectedFILE_TYPE = CDMUSICFILE;
		while(1) {
			Sleep(2000);
			testCount++;
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_DATA_DOWNLOADINGFILESTATUS, 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
				(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL, NULL);
			if(servStatus.numDownloading == 0) {
				printf("no file downloading!\n");
			} else {
				printf("downing file number: %d!\n", servStatus.numDownloading);
			}
			while(servStatus.fileID_downing[cnt] != 0 && cnt < 8) {
				printf("downing file: %x\n", servStatus.fileID_downing[cnt]);
				cnt ++;
			}
			if(servStatus.numFinished == 0) {
				printf("no file finished!\n");
			} else {
				printf("finished file number: %d!\n", servStatus.numFinished);
			}
			cnt = 0;
			while(servStatus.fileID_finished[cnt] != 0 && cnt < 8) {
				printf("finished file: %x\n", servStatus.fileID_finished[cnt]);
				cnt ++;
			}

			/* get one file that have finished */
			if(servStatus.numFinished) {
				u8 *  temp_fileBuf, * temp_filePtr;
				u32   n;
				FILE * df;

				fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
					(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, NULL);

				temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);
				if(fileInfo.file_get != NULL) {
					for(n=0; n<fileInfo.Block_num; n++) {
						DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, NULL, NULL);
						temp_filePtr += fileInfo.Block_length;
					}
					df = fopen("\\Temp\\file.dat", "wb");
					if(df == NULL)
						printf("open \\Temp\\file.dat error!\n");
					else
						fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
					fclose(df);
				}
				free(temp_fileBuf);
				DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL, NULL);

				break;
			}
			//if(testCount == 160)break;
		}
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL, NULL);
		}
		break;
	}
}

void ddbif_dataListTest(u32  dataType, PBYTE buffer, DWORD size)
{
	DWORD dwThreadID_1;
	DWORD nParameter_1;
	HANDLE  hThread_1;
	ddbsdk_dmbdataInfo_t    dataInfo = { 0 };
	DWORD BytesReturned = 0;
	int getCount = 0;

	if(deviceOpen == false) {
		if(file_test_mode == 0) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL, NULL);
		} else {
			hThread_1 = CreateThread(NULL, 0, thread_sendStreamData, &nParameter_1, 0, &dwThreadID_1);
		}
		deviceOpen = true;
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_DATAPROGRAM, NULL, 0, (unsigned char *)&dataInfo, sizeof(ddbsdk_dmbdataInfo_t), NULL, NULL);

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

	dataInfo.selectServiceNo = dataType;
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DATA_XXXX, (unsigned char *)&dataInfo, sizeof(ddbsdk_dmbdataInfo_t), NULL, 0, NULL, NULL);
	{
	FILE  * f_data = fopen("\\Temp\\data.dat", "wb");
	DWORD testCount = 0;
	while(1) {
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATA_XXXX, (unsigned char *)&dataInfo, sizeof(ddbsdk_dmbdataInfo_t), buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
		if((testCount & 0x7) == 0) {
			printf("count: %d, bytes: %d\n", testCount, getCount);
		}
		if(testCount > 160) {
			break;
		}
	}
	fclose(f_data);
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_DATA_XXXX, NULL, 0, NULL, 0, NULL, NULL);
}

void ddbif_videoTest(u32   videoNo)
{
	DWORD dwThreadID_1, dwThreadID_2, dwThreadID_3;
	DWORD nParameter_1, nParameter_2, nParameter_3;
	HANDLE  hThread_1, hThread_2, hThread_3;
	ddbsdk_dmbvideoInfo_t    videoInfo = { 0 };
	DWORD BytesReturned = 0;
	int getCount = 0;

	if(deviceOpen == false) {
		if(file_test_mode == 0) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL, NULL);
		} else {
			hThread_1 = CreateThread(NULL, 0, thread_sendStreamVideo, &nParameter_1, 0, &dwThreadID_1);
		}
		deviceOpen = true;
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_VIDEOPROGRAM, NULL, 0, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), NULL, NULL);

	videoInfo.selectServiceNo = videoNo;
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_VIDEO, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), NULL, 0, NULL, NULL);

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
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_VIDEO_PCR, NULL, 0, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), &BytesReturned, NULL);
		printf("pcr: 0x%x%x\n", (u32)(videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base >> 32),
			(u32)videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base);
		if (f_pcr != NULL)
		{
			fwrite(&(videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base), 1, 8, f_pcr);
		}
		
	}
	if(file_test_mode == 1) {
		CloseHandle(hThread_1);
	}
	CloseHandle(hThread_2);
	CloseHandle(hThread_3);

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_VIDEO, NULL, 0, NULL, 0, NULL, NULL);
}

void ddbif_musicamTest(u32  selectNo, PBYTE buffer, DWORD size)
{
	DWORD dwThreadID_1;
	DWORD nParameter_1;
	HANDLE  hThread_1;
	DWORD BytesReturned = 0;
	int getCount = 0;

	ddbsdk_dmbmusicamInfo_t    musicamInfo = { 0 };
	if(deviceOpen == false) {
		if(file_test_mode == 0) {
			DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL, NULL);
		} else {
			hThread_1 = CreateThread(NULL, 0, thread_sendStreamMusicam, &nParameter_1, 0, &dwThreadID_1);
		}
		deviceOpen = true;
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_LIST_MUSICAMPROGRAM, NULL, 0, (unsigned char *)&musicamInfo, sizeof(ddbsdk_dmbmusicamInfo_t), NULL, NULL);

	musicamInfo.selectServiceNo = selectNo;
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_MUSICAM, (unsigned char *)&musicamInfo, sizeof(ddbsdk_dmbmusicamInfo_t), NULL, 0, NULL, NULL);
	{
	FILE  * f_data = fopen("\\Temp\\musicam.dat", "wb");
NEXT_MUSICAM_INDEX:
	DWORD testCount = 0;
	while(1) {
		Sleep(10);
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_MUSICAM_DATA, NULL, 0, buffer, size, &BytesReturned, NULL);
		getCount = BytesReturned;
		testCount ++;
		if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
		if((testCount & 0x7) == 0) {
			printf("count: %d, bytes: %d\n", testCount, getCount);
		}

		if(testCount > 160) {
			break;
		}
	}
	musicamInfo.selectServiceNo ++;
	if(musicamInfo.selectServiceNo >= musicamInfo.ServiceNum)
		musicamInfo.selectServiceNo = 0;
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CHANGE_MUSICAM_CHAL, (unsigned char *)&musicamInfo, sizeof(ddbsdk_dmbmusicamInfo_t), NULL, 0, NULL, NULL);
	printf("change to channel %d\n", musicamInfo.selectServiceNo);
	goto NEXT_MUSICAM_INDEX;

	fclose(f_data);
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CLOSE_MUSICAM, NULL, 0, NULL, 0, NULL, NULL);
}


void ddbif_versionTest()
{
	ddbsdk_versionInfo_t  verInfo;
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_VERSIONINFO, NULL, 0, (unsigned char *)&verInfo, sizeof(verInfo), NULL, NULL);
	printf("ddbsdk version: %x.%x.%x\n", (verInfo.ddbsdk_version >> 16) & 0xFF, 
		(verInfo.ddbsdk_version >> 8) & 0xFF, verInfo.ddbsdk_version & 0xFF);
	printf("CA version: %x.%x.%x.%x\n", (verInfo.ca_version >> 24) & 0xFF, 
		(verInfo.ca_version >> 16) & 0xFF, (verInfo.ca_version >> 8) & 0xFF, verInfo.ca_version & 0xFF);
	printf("CA Info: cardNo-%d, manuName-%s, tvsName-%s\n", verInfo.ca_card_no, verInfo.ca_ManuName, verInfo.ca_TvsName, NULL);

	
	
	DWORD returnbytes;
	SDVTCAServiceEntitle* ServiceEntitle = (SDVTCAServiceEntitle*)calloc(sizeof(SDVTCAServiceEntitle),10);
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_CA_AUTHORIZED_INFO, NULL, 0, ServiceEntitle, sizeof(SDVTCAServiceEntitle)*10, &returnbytes, NULL);

	WORD 	ProductID = 0;										//产品ID。
	DWORD	EntitleTime = 0;									//授权时间，time_t格式。
	DWORD	StartTime = 0;										//开始时间，time_t格式。
	DWORD	EndTime = 0;
	for(int i = 0; i < (returnbytes); i++)
	{
		PBYTE ptr = PBYTE(ServiceEntitle);
												//结束时间，time_t格式。
		ProductID = ptr[35*i+0] | (ptr[35*i+1] << 8);
		EntitleTime = ptr[35*i+2] | (ptr[35*i+3] << 8) | (ptr[35*i+4] << 16) | (ptr[35*i+5] << 24);
		StartTime = ptr[35*i+6] | (ptr[35*i+7] << 8) | (ptr[35*i+8] << 16) | (ptr[35*i+9] << 24);
		EndTime = ptr[35*i+10] | (ptr[35*i+11] << 8) | (ptr[35*i+12] << 16) | (ptr[35*i+13] << 24);
		/*
		ProductID = ptr[0] | (ptr[1] << 8);
		EntitleTime = ptr[2] | (ptr[3] << 8) | (ptr[4] << 16) | (ptr[5] << 24);
		StartTime = ptr[6] | (ptr[7] << 8) | (ptr[8] << 16) | (ptr[9] << 24);
		EndTime = ptr[10] | (ptr[11] << 8) | (ptr[12] << 16) | (ptr[13] << 24);*/
		printf("-----------------------------------------------\n");
		printf("Product_ID: %d\n  entitleTime-%d年第%d天, startTime-%d年第%d天, endTime-%d年第%d天\n",
		ProductID,
		get_time_t_year(EntitleTime), get_time_t_day(EntitleTime),
		get_time_t_year(StartTime), get_time_t_day(StartTime),
		get_time_t_year(EndTime), get_time_t_day(EndTime));
	}
}

void ddbif_smgLabReconfig()
{
	ddbsdk_reconfigure_t param;
	UINT size = sizeof(ddbsdk_reconfigure_t);
	ZeroMemory(&param, size);
	param.frequency = 1456384000;
	param.testInSMGLabFlag = 1;
	strcpy(param.ddbsdk_path_main, "\\StorageCard");
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_RECONFIGURE, (unsigned char *)&param, sizeof(ddbsdk_reconfigure_t), NULL, 0, NULL, NULL);
	printf("set freq: %d \n", param.frequency);
	BOOL openResult = DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL, NULL);
	if (openResult)
	{
		deviceOpen = true;
		printf("----------open device!--------------\n");
	} 
	else
	{
		printf("----------open device FAIL!--------------\n");
	}
	
}

void ddbif_fileTestReconfig()
{
	file_test_mode = 1;
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL, NULL);
}

#if 0
void ddbif_printErrMsg(unsigned long ErrMsgNo)
{
	char * str;
	switch(ErrMsgNo)
	{
	case DDBIF_MSG_MORETHANTWO:
		str = "donnot support more than two service at the same time now!";
		break;
	case DDBIF_MSG_NOTFOUNDDEV:
		str = "donnot find the driver file: *.dll!";
		break;
	case DDBIF_MSG_OPENERROR:
		str = "failed to open device!";
		break;
	case DDBIF_MSG_NOTFOUNDSERVICE:
		str = "cannot find the service you wanted!";
		break;
	case DDBIF_MSG_FILENOTFINISHED:
		str = "cannot get file that downloading!";
		break;
	case DDBIF_MSG_MEMORYTOOSMALL:
		str = "too small memory allocated!";
		break;
	case DDBIF_MSG_READTOOLOW:
		str = "too low read speed, the data overflowed!";
		break;
	case DDBIF_MSG_FAULTMEDIATYPE:
		str = "please select correct media process MACRO type: data, video, or musicam!";
		break;
	default:
		str = "unknown!";
		break;
	}
	//将str显示出来
	printf("%s\n", str);
}
#endif


void test_MultiGetData(void)
{
	DWORD size;
	PBYTE buffer1;
	PBYTE buffer2;
	PBYTE buffer3;
	PBYTE buffer4;
	PBYTE buffer5;
	PBYTE buffer6;
	
 	/* 1. register the DDBSDK 
	if(ddbif_register() == DDBIF_ERROR) {
		printf("register error!\n");
		return;
	}*/

	//ddbif_openDdbsdk( );
	//ddbif_smgLabReconfig();
	//ddbif_openDmbDevice( );


	if (!deviceOpen)
	{
		Sleep(1000);
		return;
	}

	ddbsdk_datetime dateTime;
	if (DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_GET_DATETIME, NULL, 0, (PBYTE)&dateTime, sizeof(ddbsdk_datetime), NULL, NULL))
	{
		printf("%d年%d月%d日 - %d:%d:%d\n",dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, dateTime.second);

	}

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
	myBufferInfo6.buffer = buffer5;
	myBufferInfo6.size = size;

	//ddbif_dataListTest(0, buffer1, size);

	HANDLE hThread_0, hThread_1, hThread_2, hThread_3, hThread_4, hThread_5, hThread_6, hThread_7, hThread_8, hThread_9, hThread_10, hThread_11, hThread_12;
	//HANDLE ThreadObjects[6];
	DWORD dwThreadID_0, dwThreadID_1, dwThreadID_2, dwThreadID_3, dwThreadID_4, dwThreadID_5, dwThreadID_6, dwThreadID_7, dwThreadID_8, dwThreadID_9, dwThreadID_10, dwThreadID_11, dwThreadID_12;
	
	//hThread_0 = CreateThread(NULL, 0, thread_sendStreamData, NULL, 0, &dwThreadID_0);
	
// 	hThread_1	= CreateThread(NULL, 0, Thread_test_getTrafficData, &myBufferInfo1, 0, &dwThreadID_1);
// 	hThread_2	= CreateThread(NULL, 0, Thread_test_getPOIinfo, &myBufferInfo2, 0, &dwThreadID_2);
// 	hThread_3	= CreateThread(NULL, 0, Thread_test_getFinanceData, &myBufferInfo3, 0, &dwThreadID_3);
// 	hThread_4	= CreateThread(NULL, 0, Thread_test_getWeahter, &myBufferInfo4, 0, &dwThreadID_4);
// 	hThread_5	= CreateThread(NULL, 0, Thread_test_downloadFile, NULL, 0, &dwThreadID_5);
//  hThread_6	= CreateThread(NULL, 0, Thread_test_getSignal, NULL, 0, &dwThreadID_6);
// 	hThread_7	= CreateThread(NULL, 0, Thread_test_getErrorCode, NULL, 0, &dwThreadID_7);
// 	hThread_8	= CreateThread(NULL, 0, Thread_test_getCarPark, &myBufferInfo5, 0, &dwThreadID_8);
// 	//hThread_5	= CreateThread(NULL, 0, Thread_test_downloadFileGetStatus, NULL, 0, &dwThreadID_5);
// 	hThread_9	= CreateThread(NULL, 0, Thread_test_WeatherFile, NULL, 0, &dwThreadID_9);
// 	hThread_10	= CreateThread(NULL, 0, Thread_test_EmergencyFile, NULL, 0, &dwThreadID_10);
// 	hThread_11	= CreateThread(NULL, 0, Thread_test_RoadConditionsFile, NULL, 0, &dwThreadID_11);
 	hThread_12	= CreateThread(NULL, 0, Thread_test_getTsStream, &myBufferInfo6, 0, &dwThreadID_12);
	
	


	WaitForSingleObject (hThread_0,INFINITE);
	WaitForSingleObject (hThread_1,INFINITE);
	WaitForSingleObject (hThread_2,INFINITE);
	WaitForSingleObject (hThread_3,INFINITE);
	WaitForSingleObject (hThread_4,INFINITE);
	WaitForSingleObject (hThread_5,INFINITE);
	WaitForSingleObject (hThread_6,INFINITE);
	WaitForSingleObject (hThread_7,INFINITE);
	WaitForSingleObject (hThread_8,INFINITE);
	WaitForSingleObject (hThread_9,INFINITE);
	WaitForSingleObject (hThread_10,INFINITE);
	WaitForSingleObject (hThread_11,INFINITE);
	WaitForSingleObject (hThread_12,INFINITE);


	//::WaitForMultipleObjects(4,ThreadObjects,TRUE,INFINITE);

//	for(int i = 0; i < 6; i++)
//		CloseHandle(ThreadObjects[i]);

	CloseHandle(hThread_1);
	CloseHandle(hThread_2);
	CloseHandle(hThread_3);
	CloseHandle(hThread_4);
	CloseHandle(hThread_5);
	CloseHandle(hThread_6);

	free(buffer1);
	free(buffer2);
	free(buffer3);
	free(buffer4);

	ddbif_closeDmbDevice( );
	ddbif_closeDdbsdk( );

}


int InitMessageMapMutex()
{
	/*
	messageEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Global\\DDBSDK_MESSAGE_EVENT");
	messageMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"Global\\DDBSDK_MESSAGE_MUTEX");
	messageFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Global\\DDBSDK_MESSAGE_FILEMAP");
	*/
	messageEvent = CreateEvent(NULL,FALSE,FALSE,L"Global\\DDBSDK_MESSAGE_EVENT");
	messageMutex = CreateMutex(NULL, FALSE, L"Global\\DDBSDK_MESSAGE_MUTEX");
	messageFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(unsigned int), L"Global\\DDBSDK_MESSAGE_FILEMAP");
	if(messageEvent == NULL) printf("messageEvent == NULL!\n");
	if(messageMutex == NULL) printf("messageMutex == NULL!\n");
	if(messageFileMap == NULL) printf("messageFileMap == NULL!\n");
	if(messageEvent == NULL || messageMutex == NULL || messageFileMap == NULL) return -1;
	
	return 0;
}


unsigned int getErrorCode()
{
	LPCVOID pVoid = NULL;
	unsigned int errorC = -1;
	if (WAIT_OBJECT_0 == WaitForSingleObject(messageEvent, INFINITE))
	{
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
			errorC = *((unsigned int*)pVoid);
			UnmapViewOfFile(pVoid);
		}	
		ReleaseMutex(messageMutex);
	}
	return errorC;
}



#define PLAINTSBUFSIZE 600*188//408*188
//BYTE	plainTsBuffer[PLAINTSBUFSIZE];
PBYTE	plainTsBuffer = NULL;
PBYTE	plainTsPtrIn = plainTsBuffer;
PBYTE	plainTsPtrOut = plainTsBuffer;
long  sizeInBuffer = 0;
CRITICAL_SECTION getTsLock_CS;
HANDLE storeTsDataThread_H = NULL;
DWORD storeTsData_threadID = 0;
BOOL isStoreTsDataThreadRunning = TRUE;

int resetTsBuffer()
{
	if (NULL != plainTsBuffer)
	{
		free(plainTsBuffer);
		plainTsBuffer = NULL;
	}
	plainTsBuffer = (PBYTE)malloc(PLAINTSBUFSIZE);
	plainTsPtrIn = plainTsBuffer;
	plainTsPtrOut = plainTsBuffer;
	sizeInBuffer = 0;
	return 0;
}

int freeTsBuffer()
{
	if (NULL != plainTsBuffer)
	{
		free(plainTsBuffer);
		plainTsBuffer = NULL;
		sizeInBuffer = 0;
		plainTsPtrIn = plainTsBuffer;
		plainTsPtrOut = plainTsBuffer;
	}
	return 0;
}

int stopStoreTsThread()
{
	if (0 != storeTsDataThread_H)
	{
		isStoreTsDataThreadRunning = FALSE;
		WaitForSingleObject(storeTsDataThread_H, 5000);
		CloseHandle(storeTsDataThread_H);
		storeTsDataThread_H = 0;
	}
	return 0;
}

unsigned long putPlainTsData(PBYTE tsBuf, UINT32 bufferSize)
{
	if (0 == bufferSize || NULL == plainTsBuffer)
	{
		return 0;
	}

	EnterCriticalSection(&getTsLock_CS);
	sizeInBuffer += bufferSize;

	if(plainTsPtrIn + bufferSize <= plainTsBuffer + PLAINTSBUFSIZE)
	{
		memcpy(plainTsPtrIn, tsBuf, bufferSize);
		plainTsPtrIn += bufferSize;
	}
	else
	{
		UINT32 middleBytes = plainTsBuffer + PLAINTSBUFSIZE - plainTsPtrIn;
		UINT32 leftBytes = bufferSize - middleBytes;
		memcpy(plainTsPtrIn, tsBuf, middleBytes);
		plainTsPtrIn = plainTsBuffer;
		memcpy(plainTsPtrIn, &tsBuf[middleBytes], bufferSize);
		plainTsPtrIn += leftBytes;
	}
// 	if(sizeInBuffer >= 188)
// 	{
// 		/*DMB_LOCK;*/
// 		SetEvent(hEvent_ts);
// 		/*DMB_UNLOCK;*/
// 	}

	/* overflow here */
	if(sizeInBuffer > PLAINTSBUFSIZE) {
		printf("存储ts数据超出buffer大小\n");
		/*DMB_LOCK;*/
		sizeInBuffer = PLAINTSBUFSIZE;
		plainTsPtrOut = plainTsPtrIn;
		/*DMB_UNLOCK;*/
	}
	LeaveCriticalSection(&getTsLock_CS);

	return 0;
}

unsigned long _threadStoreTsData(void * Pram)
{
	TRACE(L"存储TS流数据Buffer Thread.....START\n");
	//BOOL isThreadRunning = *((BOOL *)Pram);
	const int size = 60*188;
	DWORD BytesReturned = 0;
	BYTE buffer[size];
	memset(buffer, 0, size);
	InitializeCriticalSection(&getTsLock_CS);
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_TS_STREAM_OPEN, NULL, 0, NULL, 0, NULL, NULL);//Close in ddbif_StopVideoService()
	while (isStoreTsDataThreadRunning)
	{
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_TS_STREAM_GET, NULL, 0, buffer, size, &BytesReturned, NULL);
		putPlainTsData(buffer, BytesReturned);
		/*memset(buffer, 0, size);*/
		Sleep(100);
		TRACE(L"_threadStoreTsData: %ld\n", BytesReturned);
	}
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_TS_STREAM_OPEN, NULL, 0, NULL, 0, NULL, NULL);
	DeleteCriticalSection(&getTsLock_CS);
	TRACE(L"存储TS流数据Buffer Thread.....EXIT\n");
	return 0;
}

extern volatile BOOL isStartDeviceThreadRunning;
unsigned long ddif_ThreadStartDMBDevice(void * Pram)
{
	BOOL bResult = FALSE;

	while(!bResult && isStartDeviceThreadRunning)
	{
		printf("开设备...\n");
		bResult = DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMB_START_DEVICE, NULL , 0, NULL, 0, NULL, NULL);
	}
	if (bResult)
	{
		deviceOpen = true;
	}
	printf("...开设备的线程退出！\n");
	return 0;
}

int ddbif_threadGetTsData(PBYTE pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned)
{
	DWORD bytesRet = 0;

	if (nOutBufSize <= 0 || NULL == plainTsBuffer)
	{
		* pBytesReturned = 0;
		return 0;
	}

	/* 1. waiting for data */
//	if(sizeInBuffer < 128*188) {
	if(sizeInBuffer < 0) {
		* pBytesReturned = 0;
		return 0;
	}

	/* 2. if have, transfer it */
	EnterCriticalSection(&getTsLock_CS);
	if(sizeInBuffer <= nOutBufSize) 
	{
		bytesRet = sizeInBuffer;
	}
	else
	{
		bytesRet = nOutBufSize;
	}

	//Copy the data
	if((plainTsPtrOut + bytesRet) <= (plainTsBuffer + PLAINTSBUFSIZE))
	{
		memcpy(pOutBuf, plainTsPtrOut, bytesRet);
		plainTsPtrOut += bytesRet;
	} 
	else 
	{
		DWORD middleBytes = plainTsBuffer + PLAINTSBUFSIZE - plainTsPtrOut;
		DWORD leftBytes = bytesRet - middleBytes;
		memcpy(pOutBuf, plainTsPtrOut, middleBytes);
		plainTsPtrOut = plainTsBuffer;
		memcpy(&pOutBuf[middleBytes], plainTsPtrOut, leftBytes);
		plainTsPtrOut += leftBytes;
	}

	sizeInBuffer -= bytesRet;

	* pBytesReturned = bytesRet;
	LeaveCriticalSection(&getTsLock_CS);;

	return 0;
}


int ddbif_startVideoService(int programeNo)
{
	if (!deviceOpen)
	{
		TRACE(L"DMB设备未启动\n");
		return 1;
	}
	int proNo = programeNo;

	//关闭存储TS线程
	stopStoreTsThread();

	//重置Buffer
	resetTsBuffer();

	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMB_STOP_SERVICE, NULL, 0, NULL, 0, NULL, NULL);
	TRACE(L"关闭流\n");
	Sleep(100);
	
	if (TRUE != DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMB_START_SERVICE, (PBYTE)&programeNo , 0, NULL, 0, NULL, NULL))
	{
		TRACE(L"启动流：%d--->失败！\n", proNo);
		return 1;
	}
	TRACE(L"启动流：%d\n", proNo);
	if (!storeTsDataThread_H)
	{
		isStoreTsDataThreadRunning = TRUE;
		storeTsDataThread_H = CreateThread(NULL, 0, _threadStoreTsData, NULL, 0, &storeTsData_threadID);
	}
	
	return 0;
}

int ddbif_StopVideoService(void)
{
	TRACE(L"停止音视频\n");
	isStoreTsDataThreadRunning = FALSE;
	if (storeTsDataThread_H)
	{
		WaitForSingleObject(storeTsDataThread_H, 5000);
		CloseHandle(storeTsDataThread_H);
		storeTsDataThread_H = 0;
	}
	freeTsBuffer();
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_TS_STREAM_CLOSE, NULL, 0, NULL, 0, NULL, NULL);
	DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMB_STOP_SERVICE, NULL, 0, NULL, 0, NULL, NULL);
	return 0;
}

BOOL ddbif_StopDabDevice()
{
	return DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMB_STOP_DEVICE, NULL, 0, NULL, 0, NULL, NULL);
}

int ddbif_getSignalValue(void)
{
	int signalV = 0;
	DWORD returnbytes;
	if (deviceOpen == true)
	{
		DeviceIoControl(hDDB_sdklib, IOCTL_DDBSDK_DMBSIGNAL_QUALITY, NULL, 0, &signalV, sizeof(UINT), &returnbytes, NULL);
	}
	return signalV;
}
