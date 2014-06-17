#include "../log/LocalLog.h"

#ifndef _DDBSDK_DATA_H_
#define _DDBSDK_DATA_H_

#define PLAINTSBUFSIZE  1000 * 188
#define DMB_LOCK	EnterCriticalSection(&g_cs)
#define DMB_UNLOCK	LeaveCriticalSection(&g_cs)
#define SERVER_TAB_LOCK		EnterCriticalSection(&serviceLockSection)
#define SERVER_TAB_UNLOCK	LeaveCriticalSection(&serviceLockSection)
#define CHECK_IF_START_PACK(TSBUF)   (TSBUF[1]) & 0x40

#define IMPOSSIBLE_PKGLONG   0x1000
#if 1
extern FILE * temp_tsfile;
#endif

//extern CRITICAL_SECTION g_cs;
extern CRITICAL_SECTION serviceLockSection;
extern BYTE  g_bDDBHaveCW;

extern long   Decrypt_data(PBYTE tsBufOut, PBYTE tsBufIn, void* g_OddCW, void* g_EvenCW);

__inline int getAdaptionLen(BYTE * ts_buf)
{
	//载荷前面有调整字段 0x30, 仅有调整字段 0x20
	DWORD wAdaptionLen = 0;
	if(0x30 == (ts_buf[3] & 0x30) || 0x20 == (ts_buf[3] & 0x30))
	{
		wAdaptionLen = ts_buf[4] + 1;
	}
	return wAdaptionLen;
}

class ddbsdk_data
{
public:
	 BYTE plainTsBuffer[PLAINTSBUFSIZE];
	PBYTE plainTsPtrIn;
	PBYTE plainTsPtrOut;
	WORD  emm_pid;
	WORD  ecm_pid;
	WORD  prog_pid;
	long  sizeInBuffer;
	bool* g_bDDBHaveCW;
	enum deDataType dataType;
	HANDLE  hEvent_ts;
	CRITICAL_SECTION g_cs;

	ddbsdk_data() :
		plainTsPtrIn(plainTsBuffer),
		plainTsPtrOut(plainTsBuffer),
		sizeInBuffer(0)
	{
		hEvent_ts = CreateEvent(NULL,FALSE,FALSE,NULL);
		InitializeCriticalSection(&g_cs);
	}

	~ddbsdk_data()
	{
		DeleteCriticalSection(&g_cs);
		CloseHandle(hEvent_ts);
	}

	virtual unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen) { return IMPOSSIBLE_PKGLONG; }
	virtual unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength) { return 1; }

	unsigned long putPlainTsData(PBYTE tsBuf, WORD wPID)
	{
// 		if (wPID == 270)
// 		{
// 			printf("PID=%d\n",wPID);
// 		}
// 		if (wPID == 258)
// 		{
// 			printf("PID=%d\n",wPID);
// 		}
// 		if (wPID == 4112)
// 		{
// 			printf("PID=%d\n",wPID);
// 		}
		
		if (wPID == prog_pid) {
#if 1
			if(((tsBuf[3] & 0xC0) >> 6) == ODD_KEY || ((tsBuf[3] & 0xC0) >> 6) == EVEN_KEY)
			{
				//printf("加密了！\n");
				if(*g_bDDBHaveCW == true) 
				//if(g_bTrafficHaveCW || g_bWeatherHaveCW || g_bFinanceHaveCW || g_bPOIinfoHaveCW || g_bSerFileHaveCW)
				{
					//printf("要解密了\n");
					/* 2. if got CW, decrypt data  */
					Decrypt_data(plainTsPtrIn, tsBuf, dataType);

					DMB_LOCK;
					sizeInBuffer += 188;
					DMB_UNLOCK;
#if 0
	fwrite(plainTsPtrIn, 1, 188, temp_tsfile);
#endif
					if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE)
					{
						plainTsPtrIn += 188;
					}else
					{
						plainTsPtrIn = plainTsBuffer;
					}
					if(sizeInBuffer == 188)
					{	
						DMB_LOCK;
						SetEvent(hEvent_ts);
						DMB_UNLOCK;
					}
				}
			} 
			else
			{
				//printf("没加密！\n");
				memcpy(plainTsPtrIn, tsBuf, 188);
				DMB_LOCK;
				sizeInBuffer += 188;
				DMB_UNLOCK;
				if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE)
				{
					plainTsPtrIn += 188;
				}else
				{
					plainTsPtrIn = plainTsBuffer;
				}
				if(sizeInBuffer == 188)
				{
					DMB_LOCK;
					SetEvent(hEvent_ts);
					DMB_UNLOCK;
				}
			}
#else 
				memcpy(plainTsPtrIn, tsBuf, 188);
				DMB_LOCK;
				sizeInBuffer += 188;
				DMB_UNLOCK;
				if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE) {
					plainTsPtrIn += 188;
				} else {
					plainTsPtrIn = plainTsBuffer;
				}
				if(sizeInBuffer == 188)
				{
					DMB_LOCK;
					SetEvent(hEvent_ts);
					DMB_UNLOCK;
				}
#endif
			/* overflow here */
			if(sizeInBuffer > PLAINTSBUFSIZE) {
				DMB_LOCK;
				sizeInBuffer = PLAINTSBUFSIZE;
				plainTsPtrOut = plainTsPtrIn;
				DMB_UNLOCK;
			} 
		} else if(wPID == emm_pid) {
			if (wPID != 0)
			{
				EMM_data_got(emm_pid, tsBuf);
			}
		} else if(wPID == ecm_pid) {
			if (wPID != 0)
			{
				ECM_data_got(ecm_pid, tsBuf);
			}
		}
		return 0;
	}

	unsigned long getPlainPkgData(
		PBYTE pOutBuf,
		DWORD nOutBufSize,
		PDWORD pBytesReturned
		)
	{
		int   headerLen;
		unsigned long    bytesRet;
		PBYTE ptr;
		bool  flag = false;
		DWORD wAdaptionLen, pkgLength, leftLength, length;
		BYTE  prev_cc, cc;

		/* 1. waiting for data */
		if(sizeInBuffer <= 0) {
			DMB_LOCK;
			ResetEvent(hEvent_ts);
			DMB_UNLOCK;
			if(WaitForSingleObject(hEvent_ts,  1000) == WAIT_TIMEOUT) {
				printf("没有数据，等待TimeOut\n");
				WriteLog("没有数据，等待TimeOut");
				Sleep(300);
				* pBytesReturned = 0;
				return 0;
			}
		}

		/* 2. check if have enough data for one package */
		while(1) {
			if(plainTsPtrOut[0] != 0x47) {
				printf("Tx Sync error!\n");
				Sleep(1000);
				WriteLog("Tx Sync error!");
				Sleep(300);
				* pBytesReturned = 0;
				return 0;
			}
			/* check if the start package */
			if(CHECK_IF_START_PACK(plainTsPtrOut)) 
				break;

			DMB_LOCK;
			sizeInBuffer -= 188;
			DMB_UNLOCK;
			if(plainTsPtrOut + 188 < plainTsBuffer + PLAINTSBUFSIZE) {
				plainTsPtrOut += 188;
			} else {
				plainTsPtrOut = plainTsBuffer;
			}

			if(sizeInBuffer <= 0) {
				DMB_LOCK;
				ResetEvent(hEvent_ts);
				DMB_UNLOCK;
				if(WaitForSingleObject(hEvent_ts,  1000) == WAIT_TIMEOUT) {
					printf("TimeOut\n");
					WriteLog("没有数据，等待TimeOut");
					Sleep(300);
					* pBytesReturned = 0;
					return 0;
				}
			}
		}
#if 1
		bytesRet = 0;
		ptr = pOutBuf;
		wAdaptionLen = getAdaptionLen(plainTsPtrOut);
		headerLen = 5 + wAdaptionLen;

		/* continuous counter check */
		cc = plainTsPtrOut[3] & 0xf;

		/* total package length check */
		leftLength = pkgLength = getPkgLeng(plainTsPtrOut, headerLen);

		/* 3. if have, transfer it */
		while(1) {
			if(plainTsPtrOut[0] != 0x47) {
				printf("Tx Sync error!\n");
				WriteLog("Tx Sync error!");
				Sleep(1000);
				* pBytesReturned = 0;
				return 0;
			}

			/* check left package length */
			length = 188 - headerLen;
			length = length <= leftLength ? length : leftLength;
			leftLength -= length;

			if(bytesRet + length > nOutBufSize) {
				Sleep(1000);
				printf("too long pkg, overflowed!\n");
				WriteLog("too long pkg, overflowed!\n");
				* pBytesReturned = 0;
				return 0;
			}

			DMB_LOCK;
			memcpy(ptr, &plainTsPtrOut[headerLen], length);
			sizeInBuffer -= 188;
			DMB_UNLOCK;

			if((plainTsPtrOut + 188) < (plainTsBuffer + PLAINTSBUFSIZE)) {
				plainTsPtrOut += 188;
			} else {
				plainTsPtrOut = plainTsBuffer;
			}
			bytesRet += length;
			ptr += length;

			if(pkgLength <= 188 - headerLen) 
				break;
			if(sizeInBuffer <= 0) {
				DMB_LOCK;
				ResetEvent(hEvent_ts);
				DMB_UNLOCK;
				if(leftLength <= 0)
					break;
				if(WaitForSingleObject(hEvent_ts,  5000) == WAIT_TIMEOUT) {
					printf("TimeOut\n");
					WriteLog("没有数据，等待5秒TimeOut");
					Sleep(300);
					* pBytesReturned = 0;
					return 0;
				}
			}
			/* check continuous counter */
			prev_cc = cc;
			cc = plainTsPtrOut[3] & 0xf;
			if(cc != ((prev_cc + 1) & 0xF)) {
				printf("continue counter error!\n");
				WriteLog("continue counter error!\n");
				Sleep(300);
				* pBytesReturned = 0;
				return 0;
			}

			/* check if the start package */
			if(CHECK_IF_START_PACK(plainTsPtrOut)) 
				break;

			if(leftLength <= 0) 
				break;

			wAdaptionLen = getAdaptionLen(plainTsPtrOut);
			headerLen = 4 + wAdaptionLen;
		}

		/* check data integrity */
		if(! checkPkg(pOutBuf, &bytesRet, pkgLength)/* pkgLength != bytesRet && pkgLength > 0 */) {
			printf("error, pkgLen: %d, getBytes: %d\n", pkgLength, bytesRet);
			WriteLog("error, pkgLen: %d, getBytes: %d\n", pkgLength, bytesRet);
			* pBytesReturned = 0;
		} else {
			* pBytesReturned = bytesRet;
		}
#else
		bytesRet = 0;
		ptr = pOutBuf;

		/* continuous counter check */
		cc = plainTsPtrOut[3] & 0xf;

		/* 3. if have, transfer it */
		while(1) {
			if(plainTsPtrOut[0] != 0x47) {
				printf("Tx Sync error!\n");
				WriteLog("Tx Sync error!\n");
				Sleep(1000);
				* pBytesReturned = 0;
				return 0;
			}

			/* check left package length */
			length = 188;

			if(bytesRet + length > nOutBufSize) {
				Sleep(1000);
				printf("too long pkg, overflowed!\n");
				WriteLog("too long pkg, overflowed!\n");
				* pBytesReturned = 0;
				return 0;
			}

			DMB_LOCK;
			memcpy(ptr, &plainTsPtrOut[0], length);
			sizeInBuffer -= 188;
			DMB_UNLOCK;

			if((plainTsPtrOut + 188) < (plainTsBuffer + PLAINTSBUFSIZE)) {
				plainTsPtrOut += 188;
			} else {
				plainTsPtrOut = plainTsBuffer;
			}
			bytesRet += length;
			ptr += length;

			if(sizeInBuffer <= 0) {
				DMB_LOCK;
				ResetEvent(hEvent_ts);
				DMB_UNLOCK;
				if(WaitForSingleObject(hEvent_ts,  5000) == WAIT_TIMEOUT) {
					printf("TimeOut\n");
					WriteLog("没有数据，等待TimeOut");
					* pBytesReturned = 0;
					return 0;
				}
			}
			/* check continuous counter */
			prev_cc = cc;
			cc = plainTsPtrOut[3] & 0xf;
			if(cc != ((prev_cc + 1) & 0xF)) {
				printf("continue counter error!\n");
				WriteLog("continue counter error!\n");
				* pBytesReturned = 0;
				return 0;
			}

			/* check if the start package */
			if(CHECK_IF_START_PACK(plainTsPtrOut)) 
				break;
		}

		* pBytesReturned = bytesRet;
#endif
		return 0;
	}
};


extern ddbsdk_decryp_program_info* getNodeFromDectryProgInfoArray(u16 prom_pid);

class ddbsdk_decryp_ts_stream : public ddbsdk_data
{
public:
	
// 	CRITICAL_SECTION exitLocker;
// 
// 	ddbsdk_decryp_ts_stream():ddbsdk_data()
// 	{
// 		InitializeCriticalSection(&exitLocker);
// 	}
// 
// 	~ddbsdk_decryp_ts_stream()
// 	{
// 		EnterCriticalSection(&exitLocker);
// 		LeaveCriticalSection(&exitLocker);
// 		DeleteCriticalSection(&exitLocker);
// 	}

	unsigned long putPlainTsData(PBYTE tsBuf, WORD wPID)
	{
		if (wPID != 0)
		{
			ddbsdk_decryp_program_info* prog_info = 0;
			prog_info = getNodeFromDectryProgInfoArray(wPID);
			if (prog_info != 0)
			{
				return putPlainTsData(tsBuf, wPID, prog_info);
			}
		}
		
		memcpy(plainTsPtrIn, tsBuf, 188);
		DMB_LOCK;
		sizeInBuffer += 188;
		DMB_UNLOCK;
		if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE)
		{
			plainTsPtrIn += 188;
		}else
		{
			plainTsPtrIn = plainTsBuffer;
		}
		if(sizeInBuffer == 188)
		{
			DMB_LOCK;
			SetEvent(hEvent_ts);
			DMB_UNLOCK;
		}

		/* overflow here */
		if(sizeInBuffer > PLAINTSBUFSIZE) {
			DMB_LOCK;
			sizeInBuffer = PLAINTSBUFSIZE;
			plainTsPtrOut = plainTsPtrIn;
			DMB_UNLOCK;
		}

		return 0;
	}

	unsigned long putPlainTsData(PBYTE tsBuf, WORD wPID, ddbsdk_decryp_program_info *program_info)
	{
		//EnterCriticalSection(&exitLocker);
		if (wPID == program_info->prog_pid)
		{
			if(((tsBuf[3] & 0xC0) >> 6) == ODD_KEY || ((tsBuf[3] & 0xC0) >> 6) == EVEN_KEY)
			{
				//printf("加密了！\n");
				if(program_info->isHaveCW == true) 
				{
// 					if (wPID == 258)
// 					{
// 						printf("");
// 					}
					//printf("要解密了\n");
					/* 2. if got CW, decrypt data  */
					Decrypt_data(plainTsPtrIn, tsBuf, program_info->g_OddCW, program_info->g_EvenCW);

					DMB_LOCK;
					sizeInBuffer += 188;
					DMB_UNLOCK;

					if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE)
					{
						plainTsPtrIn += 188;
					}else
					{
						plainTsPtrIn = plainTsBuffer;
					}
					if(sizeInBuffer == 188)
					{	
						DMB_LOCK;
						SetEvent(hEvent_ts);
						DMB_UNLOCK;
					}
				}
			} 
			else
			{
				//printf("没加密！\n");
				memcpy(plainTsPtrIn, tsBuf, 188);
				DMB_LOCK;
				sizeInBuffer += 188;
				DMB_UNLOCK;
				if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE)
				{
					plainTsPtrIn += 188;
				}else
				{
					plainTsPtrIn = plainTsBuffer;
				}
				if(sizeInBuffer == 188)
				{
					DMB_LOCK;
					SetEvent(hEvent_ts);
					DMB_UNLOCK;
				}
			}

			/* overflow here */
			if(sizeInBuffer > PLAINTSBUFSIZE) {
				DMB_LOCK;
				sizeInBuffer = PLAINTSBUFSIZE;
				plainTsPtrOut = plainTsPtrIn;
				DMB_UNLOCK;
			} 
		} else if(wPID == program_info->emm_pid) {
			if (wPID != 0)
			{
				EMM_data_got(wPID, tsBuf);
			}
		} else if(wPID == program_info->ecm_pid) {
			if (wPID != 0)
			{
				ECM_data_got(wPID, tsBuf);
			}
		}
		//LeaveCriticalSection(&exitLocker);
		return 0;
	}


	unsigned long getPlainPkgData(
		PBYTE pOutBuf,
		DWORD nOutBufSize,
		PDWORD pBytesReturned
		)
	{
		DWORD bytesRet = 0;

		if (nOutBufSize == 0)
		{
			* pBytesReturned = 0;
			return 0;
		}

		/* 1. waiting for data */
		if(sizeInBuffer <= 0) {
			DMB_LOCK;
			ResetEvent(hEvent_ts);
			DMB_UNLOCK;
			if(WaitForSingleObject(hEvent_ts,  1000) == WAIT_TIMEOUT) {
				printf("ts流没有数据，等待TimeOut\n");
				Sleep(300);
				* pBytesReturned = 0;
				return 0;
			}
		}

		/* 2. if have, transfer it */
		DMB_LOCK;
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
		if (sizeInBuffer < 0)
		{
			int tt = 0;
		}
		* pBytesReturned = bytesRet;
		DMB_UNLOCK;

		return 0;
	}

};

class ddbsdk_ts_stream : public ddbsdk_data
{
public:
	unsigned long putPlainTsData(PBYTE tsBuf, UINT32 bufferSize)
	{
		DMB_LOCK;
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
		if(sizeInBuffer >= 188)
		{
			/*DMB_LOCK;*/
			SetEvent(hEvent_ts);
			/*DMB_UNLOCK;*/
		}

		/* overflow here */
		if(sizeInBuffer > PLAINTSBUFSIZE) {
			printf("存储ts数据超出buffer大小\n");
			/*DMB_LOCK;*/
			sizeInBuffer = PLAINTSBUFSIZE;
			plainTsPtrOut = plainTsPtrIn;
			/*DMB_UNLOCK;*/
		}
		DMB_UNLOCK;

		return 0;
	}

	unsigned long getPlainPkgData(
		PBYTE pOutBuf,
		DWORD nOutBufSize,
		PDWORD pBytesReturned
		)
	{
		DWORD bytesRet = 0;

		if (nOutBufSize == 0)
		{
			* pBytesReturned = 0;
			return 0;
		}

		/* 1. waiting for data */
		if(sizeInBuffer <= 0) {
			DMB_LOCK;
			ResetEvent(hEvent_ts);
			DMB_UNLOCK;
			if(WaitForSingleObject(hEvent_ts,  1000) == WAIT_TIMEOUT) {
				printf("ts流没有数据，等待TimeOut\n");
				Sleep(300);
				* pBytesReturned = 0;
				return 0;
			}
		}

		/* 2. if have, transfer it */
		DMB_LOCK;
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
		if (sizeInBuffer < 0)
		{
			int tt = 0;
		}
		* pBytesReturned = bytesRet;
		DMB_UNLOCK;

		return 0;
	}
};


class ddbsdk_decryp_stream : public ddbsdk_data
{
public:
	BYTE	g_OddCW[16];	
	BYTE	g_EvenCW[16];
	bool	isHaveCW;
	CRITICAL_SECTION exitLocker;

	ddbsdk_decryp_stream():ddbsdk_data()
	{
		InitializeCriticalSection(&exitLocker);
	}

	~ddbsdk_decryp_stream()
	{
		EnterCriticalSection(&exitLocker);
		LeaveCriticalSection(&exitLocker);
		DeleteCriticalSection(&exitLocker);
	}

	unsigned long putPlainTsData(PBYTE tsBuf, WORD wPID)
	{
		EnterCriticalSection(&exitLocker);
		if (wPID == prog_pid)
		{
			if(((tsBuf[3] & 0xC0) >> 6) == ODD_KEY || ((tsBuf[3] & 0xC0) >> 6) == EVEN_KEY)
			{
				//printf("加密了！\n");
				if(this->isHaveCW == true) 
				{
					//printf("要解密了\n");
					/* 2. if got CW, decrypt data  */
					Decrypt_data(plainTsPtrIn, tsBuf, this->g_OddCW, this->g_EvenCW);

					DMB_LOCK;
					sizeInBuffer += 188;
					DMB_UNLOCK;

					if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE)
					{
						plainTsPtrIn += 188;
					}else
					{
						plainTsPtrIn = plainTsBuffer;
					}
					if(sizeInBuffer == 188)
					{	
						DMB_LOCK;
						SetEvent(hEvent_ts);
						DMB_UNLOCK;
					}
				}
			} 
			else
			{
				//printf("没加密！\n");
				memcpy(plainTsPtrIn, tsBuf, 188);
				DMB_LOCK;
				sizeInBuffer += 188;
				DMB_UNLOCK;
				if(plainTsPtrIn + 188 < plainTsBuffer + PLAINTSBUFSIZE)
				{
					plainTsPtrIn += 188;
				}else
				{
					plainTsPtrIn = plainTsBuffer;
				}
				if(sizeInBuffer == 188)
				{
					DMB_LOCK;
					SetEvent(hEvent_ts);
					DMB_UNLOCK;
				}
			}

			/* overflow here */
			if(sizeInBuffer > PLAINTSBUFSIZE) {
				DMB_LOCK;
				sizeInBuffer = PLAINTSBUFSIZE;
				plainTsPtrOut = plainTsPtrIn;
				DMB_UNLOCK;
			} 
		} else if(wPID == emm_pid) {
			if (wPID != 0)
			{
				EMM_data_got(emm_pid, tsBuf);
			}
		} else if(wPID == ecm_pid) {
			if (wPID != 0)
			{
				ECM_data_got(ecm_pid, tsBuf);
			}
		}
		LeaveCriticalSection(&exitLocker);
		return 0;
	}

	unsigned long getPlainPkgData(
		PBYTE pOutBuf,
		DWORD nOutBufSize,
		PDWORD pBytesReturned
		)
	{
		DWORD bytesRet = 0;

		if (nOutBufSize == 0)
		{
			* pBytesReturned = 0;
			return 0;
		}

		/* 1. waiting for data */
		if(sizeInBuffer <= 0) {
			DMB_LOCK;
			ResetEvent(hEvent_ts);
			DMB_UNLOCK;
			if(WaitForSingleObject(hEvent_ts,  1000) == WAIT_TIMEOUT) {
				printf("ts流没有数据，等待TimeOut\n");
				Sleep(300);
				* pBytesReturned = 0;
				return 0;
			}
		}

		/* 2. if have, transfer it */
		DMB_LOCK;
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
		if (sizeInBuffer < 0)
		{
			int tt = 0;
		}
		* pBytesReturned = bytesRet;
		DMB_UNLOCK;

		return 0;
	}
};

class ddbsdk_finance : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return ((plainTsBuf[headerLen + 1] & 0xF) << 8) + plainTsBuf[headerLen + 2];
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		if(pkgLength == * retBytes) {
			return 1;
		} else {
			return 0;
		}
	}
};

class ddbsdk_pioInfo : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return IMPOSSIBLE_PKGLONG;
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		int cinuma_num = plainPkgBuf[4];
		int film_num;
		int total_len = 5;
		int n, m;
		for(n=0; n<cinuma_num; n++) {
			total_len += 3;
			film_num = plainPkgBuf[total_len - 1];
			for(m=0; m<film_num; m++) {
				total_len += 1;
				total_len += plainPkgBuf[total_len - 1]; 
				if(total_len > IMPOSSIBLE_PKGLONG) goto CHECK_EXIT;
				total_len += 2;
				total_len += 1;
				total_len += plainPkgBuf[total_len - 1];
				if(total_len > IMPOSSIBLE_PKGLONG) goto CHECK_EXIT;
				total_len += 1;
				total_len += plainPkgBuf[total_len - 1];
				if(total_len > IMPOSSIBLE_PKGLONG) goto CHECK_EXIT;
				total_len += 1;
				total_len += plainPkgBuf[total_len - 1] * 3;
				if(total_len > IMPOSSIBLE_PKGLONG) goto CHECK_EXIT;
			}
		}
CHECK_EXIT:
		if(total_len > IMPOSSIBLE_PKGLONG) {
			return 0;
		} else {
			* retBytes = total_len;
			return 1;
		}
	}
};

class ddbsdk_sysFile : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return IMPOSSIBLE_PKGLONG;
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		return 1;
	}
};

class ddbsdk_downloadFile : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return IMPOSSIBLE_PKGLONG;
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		return 1;
	}
};

class ddbsdk_traffic : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		if((plainTsBuf[headerLen] & 0xF) == 0x1) {
			int Num = plainTsBuf[headerLen + 4];  /* 匝道信息 */
			return Num * 2 + 5;
		} else if((plainTsBuf[headerLen] & 0xF) == 0x2) {
				int Num = plainTsBuf[headerLen + 5];  /* 停车场信息 */
				return Num * 4 + 6;
			}
			else {
				int Num = plainTsBuf[headerLen + 2];  /* 一般交通信息 */
				return Num * 2 + 5;
			}
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		if(pkgLength == * retBytes) {
			return 1;
		} else {
			return 0;
		}
	}
};

class ddbsdk_carPark : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		int Num = plainTsBuf[headerLen + 5];  /* 停车场信息 */
		return Num * 4 + 6;
// 		int Num = (plainTsBuf[headerLen + 2] << 4) | (plainTsBuf[headerLen + 3] >> 4);  /* 停车场信息 */
// 		return (Num) * 184 + 183;
		
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		if(pkgLength == * retBytes) {
			return 1;
		} else {
			return 0;
		}
	}
};

class ddbsdk_weather : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		//return plainTsBuf[headerLen + 1];
		return 188-5;
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		if(pkgLength == * retBytes) {
			return 1;
		} else {
			return 0;
		}
	}
};

typedef struct ddbsdk_servFileData_s
{
	unsigned ext_service_id : 4;
	unsigned service_id :     4;
	unsigned user_def :       8;
	unsigned file_id1 :       8;
	unsigned file_id2 :       8;

	unsigned file_id3 :       8;
	unsigned file_id4 :       8;
	unsigned file_type :      8;
	unsigned version :        8;

	unsigned file_length1 :   8;
	unsigned file_length2 :   8;
	unsigned file_length3 :   8;
	unsigned file_length4 :   8;

	unsigned file_valid_enddate1 : 8;
	unsigned file_valid_enddate2 : 8;
	unsigned file_valid_endtime1 : 8;
	unsigned file_valid_endtime2 : 8;

	unsigned reserved :           4;
	unsigned block_length_type :  4;

	unsigned block_total_num1 :    8;
	unsigned block_total_num2 :    8;
	unsigned cur_block_num1 :      8;
	unsigned cur_block_num2 :      8;

}ddbsdk_servFileData_t;

#define   PREFIX_PATH_SYSFILE   "\\0_sys"
#define   PREFIX_PATH_MEDIA     "\\1_multimedia"
#define   PREFIX_PATH_AD        "\\2_ad"
#define   PREFIX_PATH_MUSIC     "\\3_music"
#define   PREFIX_PATH_NET       "\\4_net"
#define   PREFIX_PATH_POIFOOD   "\\5_poiFood"
#define   PREFIX_PATH_PROGLIST  "\\0xfd_progList"
#define   PREFIX_PATH_LIST      "\\0xfe_list"
#define   PREFIX_PATH_OTHER     "\\other"

#define MAXNUMDOWN          8192
#define BLOCKDOWNBUFSIZE    5 * 1024
typedef struct ddbsdk_servFileDownStatus_1024_s
{
	unsigned long       numDown;    /* 0-9: counter of data buffer */
	unsigned char       blockStatus[4096];
	unsigned int        data_pos[BLOCKDOWNBUFSIZE / 256];
	unsigned char       data[BLOCKDOWNBUFSIZE];

}ddbsdk_servFileDownStatus_1024_t;

typedef struct ddbsdk_servFile_syntax_s
{
	u32   FILE_ID;
	u32   FILE_LENGTH;
	 u8   FILE_TYPE;
	 u8   Version;
	u16   FILE_VALID_ENDDATE;
	u16   FILE_VALID_ENDTIME;
	u16   Block_length;
	u16   Block_num;
	u16   Cur_block_num;
}ddbsdk_servFile_syntax_t;

typedef struct ddbsdk_servFile_s
{
	ddbsdk_servFile_syntax_t   synt;
	u32    statFilePos;      /* index to the filedown_status.log */
	u32    numDownloaded;    /* if data finished, it's equal Block_num - 1*/
	u16	   pid;
	void * pStatus;
}ddbsdk_servFile_t;

typedef struct ddbsdk_servFileTab_s
{
	int num_down;
	int num_complete;
	ddbsdk_servFile_t   ddbsdk_downFiles[MAXNUMDOWN];
}ddbsdk_servFileTab_t;

class ddbsdk_serviceFile : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		int leftLen, pkgLenFlag, pkgLen, fileLen;
		int cur_block_num, block_num;
		int lengthTab[3] = {256, 512, 1024};

		pkgLenFlag = (plainTsBuf[headerLen + 16] & 0xF0) >> 4;
		pkgLen = pkgLenFlag < 3 ?  lengthTab[pkgLenFlag] : 0;
		fileLen = (plainTsBuf[headerLen + 8] << 24) | 
			(plainTsBuf[headerLen + 9] << 16) | 
			(plainTsBuf[headerLen + 10] << 8) | 
			(plainTsBuf[headerLen + 11]);
		block_num = (plainTsBuf[headerLen + 17] << 8) | 
			(plainTsBuf[headerLen + 18]);
		cur_block_num = (plainTsBuf[headerLen + 19] << 8) | 
			(plainTsBuf[headerLen + 20]);

		if(block_num < cur_block_num) return 0;
		leftLen = fileLen - cur_block_num * pkgLen;

		return (leftLen > pkgLen ? pkgLen : leftLen) + 26;
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, int retBytes, int pkgLength)
	{
		if(pkgLength == retBytes) {
			return 1;
		} else {
			return 0;
		}
	}

	//ddbsdk_servFileTab_t   g_servTab;
	HANDLE downHandle;
	BOOL isThreadRunning;
	HANDLE hCheckServiceFile;
	PBYTE g_ddbsdk_servFileBuffer;
	unsigned long initServiceFileData(void);
	unsigned long deinitServiceFileData(void);
	

	ddbsdk_serviceFile():
		downHandle(0),
		g_ddbsdk_servFileBuffer(0),
		isThreadRunning(1),
		hCheckServiceFile(0)
	{
		//memset(&(this->g_servTab), 0 ,sizeof(ddbsdk_servFileTab_t));
	}

};

#endif
