#include <list>
#include <Windows.h>
#include <process.h>
#include "Dmb.h"
#include <stdio.h>
#include <winbase.h>
#include "HG_DAB/DAB_public.h"
#include "HG_DAB/DabWin.h"
#include "../log/LocalLog.h"

#define MAXSERVICENUM	12

SERVINFORM serviceInfoList[MAXSERVICENUM];
int serviceNumber = 0;

extern void DataCallback(void*	    ClientPtr, 
						 UINT32	ServiceHandle, 
						 UINT8*	pBuffer, 
						 UINT32	BufferSize );


static HANDLE getDataThreadH = NULL;
static unsigned int getDataThreadH_ID = 0;
static BOOL isTheadRunning = TRUE;

static BYTE tmpTS[188] = {0};
static int tmpTSsize = 0;

int checkTsBuf(PBYTE inBuf, DWORD inBufSize, PBYTE outBuf, DWORD outBufSize)
{
	PBYTE tempIn = (PBYTE)malloc(tmpTSsize + inBufSize);
	memcpy(tempIn, tmpTS, tmpTSsize);
	memcpy(&tempIn[tmpTSsize], inBuf, inBufSize);

	PBYTE tempIn_Orl = tempIn;
	PBYTE inBuf_Orl = inBuf;
	PBYTE outBuf_Orl = outBuf;

	PBYTE endTempInAdd = tempIn + tmpTSsize + inBufSize;
	PBYTE endInAdd = inBuf + inBufSize;
	PBYTE endOutAdd = outBuf + outBufSize;


	while (tempIn[0] != 0x47 && tempIn <= endTempInAdd)
	{
		tempIn++;
	}
	if (tempIn >= endTempInAdd)
	{
		free(tempIn_Orl);
		tmpTSsize = 0;
		return -1;
	}
	while ((tempIn + 188) < endTempInAdd && (outBuf + 188) <= endOutAdd)
	{
		if (tempIn[188] == 0x47 /*&& inBuf[376] == 0x47*/)
		{
			memcpy(outBuf, tempIn, 188);
			tempIn += 188;
			outBuf += 188;
		}
		else
		{
			tempIn++;
			while(tempIn[0] != 0x47 && tempIn < endTempInAdd)
			{
				tempIn++;
			}
			if (tempIn >= endTempInAdd)
			{
				break;
			}
		}
	}
	int tempSize = endTempInAdd - tempIn;
	memcpy(tmpTS, tempIn, tempSize);
	tmpTSsize = tempSize;
	free(tempIn_Orl);
	return outBuf - outBuf_Orl;
}

static unsigned int __stdcall gainData(void* pram)
{
	
	int bytesRead;
	int littleBufSize = 188*20;
	int tempSize = 188*30;
	PBYTE littleBuf = (PBYTE)malloc(littleBufSize);
	PBYTE tempBuf = (PBYTE)malloc(tempSize);
	while(isTheadRunning)
	{
		bytesRead = DAB_ReadMSC((char*)littleBuf, littleBufSize);
		if (bytesRead > 0)
		{
			bytesRead = checkTsBuf(littleBuf, littleBufSize, tempBuf, tempSize);
			DataCallback(NULL, NULL, tempBuf, bytesRead);
		}
		Sleep(50);
	}
	free(littleBuf);
	return 0;
}



DWORD message_hWnd = 0;

void VALIDATE_RETVAL(DWORD retval,char *strName)
{if (retval == 0) \
{ \
	//cout << strName << ": Success." << endl; 
	printf("%s:Success.\n",strName);
} \
	else \
{ \
	//cout << strName << ": FAIL!" << endl; 
	printf("%s:FAIL!\n",strName);
	return ;\
}
}
static DWORD Wait_CallBack(HANDLE eventName, DWORD timeout)
{
	DWORD dwWaitResult = WaitForSingleObject(eventName,timeout); 
	if (dwWaitResult != WAIT_OBJECT_0) 
	{ printf("ERROR : Timeout!\n");
//RETAILMSG(1, (TEXT("----------------------->>> ERROR : Timeout!\n"))); 
	}
	else
	{ printf("Successful..........\n");
		//RETAILMSG(1, (TEXT("----------------------->>>  WAIT_CALLBACK WAIT_OBJECT_0!\r\n"))); 
	}
	return dwWaitResult;
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

// #ifndef BUFFER_SIZE
// #define BUFFER_SIZE	100
// #endif
// 
// //extern BYTE *Finance_Info;
// extern BYTE* g_byszWriteBuf;
// extern BYTE* BUFFER;
// //extern CRITICAL_SECTION g_cs;



void U8ToUnicode(char * u8a) 
{ 
	//UTF8 to Unicode 
	//由于中文直接复制过来会成乱码，编译器有时会报错，故采用16进制形式 
	char* szU8 = "abcd1234\xe4\xbd\xa0\xe6\x88\x91\xe4\xbb\x96\x00"; 
	szU8 = u8a;
	//预转换，得到所需空间的大小 
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0); 
	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间 
	wchar_t* wszString = new wchar_t[wcsLen + 1]; 
	//转换 
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen); 
	//最后加上'\0' 
	wszString[wcsLen] = '\0'; 
	//unicode版的MessageBox API 

	//写文本同ansi to unicode 
} 

long DmbStart(int   frequency)
{
	if (TRUE == DAB_OpenDevice(NULL))
	{
		serviceNumber = DAB_Scan(frequency);
		if (serviceNumber <= 0)
		{
			return -1;
		}
		for (int i = 0; i < serviceNumber; i++)
		{
			serviceInfoList[i] = DAB_GetInfo(i);
		}
		return 0;
	}
	return -1;
}

unsigned long  startDmbService(int  index, int testInSMGLabFlag)
{
	if (!testInSMGLabFlag)
	{
		//int playstatus = DAB_SelService((BYTE)index);
		int playstatus = DAB_SelService(3);
		
	}
	else
	{
		int playstatus = DAB_SelService(0);
	}
	getDataThreadH = (HANDLE)_beginthreadex( NULL, 0, gainData, NULL, 0, &(getDataThreadH_ID) );
	return 0;
}

unsigned long  stopDmbService()
{
	isTheadRunning = FALSE;
	DWORD waitstatus = WaitForSingleObject(getDataThreadH, 5000);
	if (waitstatus != WAIT_OBJECT_0)
	{
		return -1;
	}
	return 0;
}

void dmbGetServiceNum(unsigned int * ServiceNum)
{
	*ServiceNum = serviceNumber;
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
		//strncpy(descr, (const char *)&components[0].ServiceInfo.pLabel[0], sizeof(components[0].ServiceInfo.pLabel));
		break;
	case 1:
		//strncpy(descr, (const char *)&components[1].ServiceInfo.pLabel[0], sizeof(components[1].ServiceInfo.pLabel));
		break;
	case 2:
		//strncpy(descr, (const char *)&components[5].ServiceInfo.pLabel[0], sizeof(components[5].ServiceInfo.pLabel));
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
		//strcpy(descr, (const char *)&components[3].ServiceInfo.pLabel[0]);
		break;
	case 1:
		//strcpy(descr, (const char *)&components[4].ServiceInfo.pLabel[0]);
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
// 	SmsHostApiGetReceptionQuality_Req(hDevice);
// 	Wait_CallBack(hEventReceptionQuality,1000);
	return 0;
}
void  checkSignal(int  frequency) 
{
	
}

void DmbStop(void)
{
	if (isTheadRunning == TRUE)
	{
		isTheadRunning = FALSE;
		DWORD waitstatus = WaitForSingleObject(getDataThreadH, 5000);
	}
	DAB_CloseDevice();

}



