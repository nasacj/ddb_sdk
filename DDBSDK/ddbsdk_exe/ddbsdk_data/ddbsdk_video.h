

#ifndef _DDBSDK_VIDEO_H_
#define _DDBSDK_VIDEO_H_

#include "ddbsdk_data.h"
#include "../ddbsdk_demux/push_video.h"

extern SLConfig_DR sl_config_dr;

class ddbsdk_dmbVideo : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return (plainTsBuf[headerLen + 4] << 8) + plainTsBuf[headerLen + 5] + 5;
	}
	unsigned long checkPkg(PBYTE plainPkgBuf, unsigned long * retBytes, unsigned long pkgLength)
	{
		if(pkgLength == * retBytes) {
			return 1;
		} else {
			return 0;
		}
	}
#if 0
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

		bytesRet = 0;
		ptr = pOutBuf;
		wAdaptionLen = getAdaptionLen(plainTsPtrOut);
		headerLen = 4 + wAdaptionLen;

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
		return 0;
	}
#else
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
		//leftLength = pkgLength = getPkgLeng(plainTsPtrOut, headerLen);
		pushPacketVideoPES2(&sl_config_dr, plainTsPtrOut, &headerLen, &pkgLength);
		leftLength = pkgLength;

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
#endif
		return 0;
	}
#endif
};




class ddbsdk_dmbAduio : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return (plainTsBuf[headerLen + 3] << 8) + plainTsBuf[headerLen + 4] + 5;
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

class ddbsdk_dmbObjectDr : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return ((plainTsBuf[headerLen + 1] & 0xF) << 8) + plainTsBuf[headerLen + 2] + 3;
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

class ddbsdk_dmbScene : public ddbsdk_data
{
public:
	unsigned long getPkgLeng(PBYTE plainTsBuf, int headerLen)
	{
		return ((plainTsBuf[headerLen + 1] & 0xF) << 8) + plainTsBuf[headerLen + 2] + 3;
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

#endif
