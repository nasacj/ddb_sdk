

#ifndef _DDBSDK_MUSICAM_H_
#define _DDBSDK_MUSICAM_H_

#define PLAINTSBUFSIZE_MUSICAM  100 * 384

#define DMB_LOCK	EnterCriticalSection(&g_cs)
#define DMB_UNLOCK	LeaveCriticalSection(&g_cs)

class ddbsdk_musicam
{
public:
	BYTE plainTsBuffer[PLAINTSBUFSIZE_MUSICAM];
	PBYTE plainTsPtrIn;
	PBYTE plainTsPtrOut;
	long  componentIndex;
	long  sizeInBuffer;
	HANDLE  hEvent_musicam;
	CRITICAL_SECTION g_cs;

	ddbsdk_musicam() :
		plainTsPtrIn(plainTsBuffer),
		plainTsPtrOut(plainTsBuffer),
		sizeInBuffer(0)
	{
		hEvent_musicam = CreateEvent(NULL,FALSE,FALSE,NULL);
		InitializeCriticalSection(&g_cs);
	}

	~ddbsdk_musicam()
	{
		DeleteCriticalSection(&g_cs);
		CloseHandle(hEvent_musicam);
	}

	unsigned long putPlainTsData(PBYTE tsBuf, int length)
	{
		if(tsBuf[0] != 0xFF || ((tsBuf[1] & 0xFE) != 0xFC)) return 0;
		if(plainTsPtrIn + length < plainTsBuffer + PLAINTSBUFSIZE_MUSICAM) {
			memcpy(plainTsPtrIn, tsBuf, length);
			plainTsPtrIn += length;
		} else {
			int copyLen = ((plainTsBuffer + PLAINTSBUFSIZE_MUSICAM) - plainTsPtrIn);
			memcpy(plainTsPtrIn, tsBuf, copyLen);
			if(length - copyLen > 0)
				memcpy(plainTsBuffer, tsBuf + copyLen, length - copyLen);
			plainTsPtrIn = plainTsBuffer + length - copyLen;
		}

		DMB_LOCK;
		if(sizeInBuffer <= 0)SetEvent(hEvent_musicam);
		sizeInBuffer += length;
		DMB_UNLOCK;

		/* overflow here */
		if(sizeInBuffer > PLAINTSBUFSIZE_MUSICAM) {
			DMB_LOCK;
			sizeInBuffer = PLAINTSBUFSIZE;
			plainTsPtrOut = plainTsPtrIn;
			DMB_UNLOCK;
		}
		return 0;
	}

	unsigned long getPlainPkgData(
		PBYTE pOutBuf,
		DWORD nOutBufSize,
		PDWORD pBytesReturned
		)
	{
		/* 1. waiting for data */
		if(sizeInBuffer <= 0) {
			DMB_LOCK;
			ResetEvent(hEvent_musicam);
			DMB_UNLOCK;
			if(WaitForSingleObject(hEvent_musicam,  1000) == WAIT_TIMEOUT) {
				printf("TimeOut\n");
				* pBytesReturned = 0;
				return 0;
			}

		}
		if((plainTsPtrOut[0] == 0xFF) && ((plainTsPtrOut[1] & 0xFE) == 0xFC)) {

//			int br_tab[16] = {0,  32000,  48000, 56000, 64000,  80000,  96000, 112000,
//				128000, 160000, 192000, 224000, 256000, 320000, 384000, 0};
			int sr_tab[4] = {44100, 48000, 32000, 0};
			int br_tab[16] = {0,  32,  48, 56, 64,  80,  96, 112,
				128, 160, 192, 224, 256, 32, 384, 0};

			int index = (plainTsPtrOut[2] & 0xF0) >> 4;
			int bitrate = br_tab[index];
			int index2 = (plainTsPtrOut[2] & 0x0C) >> 2;
			int samplerate = sr_tab[index2];
//			int framelen = bitrate * 144 / samplerate;
			int framelen = bitrate * 144 / 48;
			if(sizeInBuffer >= framelen) {
				if(plainTsPtrOut + framelen < plainTsBuffer + PLAINTSBUFSIZE_MUSICAM) {
					memcpy(pOutBuf, plainTsPtrOut, framelen);
					plainTsPtrOut += framelen;
				} else {
					int copyLen = ((plainTsBuffer + PLAINTSBUFSIZE_MUSICAM) - plainTsPtrOut);
					memcpy(pOutBuf, plainTsPtrOut, copyLen);
					if(framelen - copyLen > 0)
						memcpy(pOutBuf + copyLen, plainTsBuffer, framelen - copyLen);
					plainTsPtrOut = plainTsBuffer + framelen - copyLen;
				}
				DMB_LOCK;
				sizeInBuffer -= framelen;
				DMB_UNLOCK;
				* pBytesReturned = framelen;
				return 0;
			}
		}
		return 0;
	}
};


#endif
