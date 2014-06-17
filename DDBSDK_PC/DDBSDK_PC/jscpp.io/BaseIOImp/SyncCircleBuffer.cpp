
#include <string.h>
#include <stdlib.h>
#include "Locker.h"
#include "WinEvent.h"
#include <stdio.h>
#include "SyncCircleBuffer.h"
#include "LocalLog.h"

SyncCircleBuffer::SyncCircleBuffer()
{
	SyncCircleBuffer(512, 0);
}
SyncCircleBuffer::SyncCircleBuffer(U32 bufferSize, U32 outtime)
{
	this->bufferSize = bufferSize;
	this->timeout = outtime;
	this->buffer = (BYTE_P)malloc(this->bufferSize);
	this->locker = new WinLocker();
	this->eventSignal = new WinEvent();
	assert(this->buffer);
	assert(this->locker);
	assert(this->eventSignal);
	this->sizeInBuffer = 0;
	this->buffer_in = buffer;
	this->buffer_out = buffer;
}
SyncCircleBuffer::~SyncCircleBuffer()
{
	this->bufferSize = 0;
	free(this->buffer);
	delete this->locker;
	delete this->eventSignal;
	this->buffer = 0;
	this->sizeInBuffer = 0;
	this->buffer_in = buffer;
	this->buffer_out = buffer;
}
void SyncCircleBuffer::store(__in BYTE_P tsBuf, __in U32 inSize)
{
	if (0 == inSize || 0 == buffer || 0 == this->buffer)
	{
		return;
	}

	this->locker->lock();

	this->sizeInBuffer += inSize;

	if(this->buffer + inSize <= this->buffer + this->bufferSize)
	{
		memcpy(this->buffer_in, tsBuf, inSize);
		this->buffer_in += inSize;
	}
	else
	{
		U32 middleBytes = this->buffer + this->bufferSize - this->buffer_in;
		U32 leftBytes = inSize - middleBytes;
		memcpy(this->buffer_in, tsBuf, middleBytes);
		this->buffer_in = this->buffer;
		memcpy(this->buffer_in, &tsBuf[middleBytes], bufferSize);
		this->buffer_in += leftBytes;
	}


	/* overflow here */
	if(this->sizeInBuffer >= this->bufferSize)
	{
		//WriteLog("存储ts数据超出buffer大小\n");
		/*DMB_LOCK;*/
		this->sizeInBuffer = this->bufferSize;
		this->buffer_out = this->buffer_in;
		/*DMB_UNLOCK;*/
	}

	this->eventSignal->setEvent();

	this->locker->unlock();

	//WriteLog("存入：%d\t当前余量：%d\n",inSize, sizeInBuffer);

}
int SyncCircleBuffer::read(__inout BYTE_P pOutBuf, __in U32 nOutBufSize, __out U32* pBytesReturned)
{
	DWORD bytesRet = 0;

	if (nOutBufSize <= 0 || 0 == pOutBuf || 0 == this->buffer)
	{
		* pBytesReturned = 0;
		return -1;
	}

	/* 1. waiting for data */
	if(sizeInBuffer <= 0) 
	{
		this->eventSignal->reSetEvent();
		if (this->eventSignal->wait(this->timeout) != 0)
		{
			//WriteLog("没有数据，等待timeout！\n");
			* pBytesReturned = 0;
			return -1;
		}
	}

	/* 2. if have, transfer it */
	this->locker->lock();

	if(sizeInBuffer <= nOutBufSize) 
	{
		bytesRet = sizeInBuffer;
	}
	else
	{
		bytesRet = nOutBufSize;
	}

	//Copy the data
	if((this->buffer_out + bytesRet) <= (this->buffer + this->bufferSize))
	{
		memcpy(pOutBuf, this->buffer_out, bytesRet);
		this->buffer_out += bytesRet;
	} 
	else 
	{
		DWORD middleBytes = this->buffer + this->bufferSize - this->buffer_out;
		DWORD leftBytes = bytesRet - middleBytes;
		memcpy(pOutBuf, this->buffer_out, middleBytes);
		this->buffer_out = this->buffer;
		memcpy(&pOutBuf[middleBytes], this->buffer_out, leftBytes);
		this->buffer_out += leftBytes;
	}

	sizeInBuffer -= bytesRet;
	*pBytesReturned = bytesRet;

	this->locker->unlock();

	//WriteLog("读取：%d\t当前余量：%d\n",bytesRet, sizeInBuffer);

	return 0;
}
int SyncCircleBuffer::peek(__inout BYTE_P pOutBuf, __in U32 nOutBufSize, __out U32* pBytesReturned)
{
	DWORD bytesRet = 0;

	if (nOutBufSize <= 0 || 0 == pOutBuf || 0 == this->buffer)
	{
		* pBytesReturned = 0;
		return -1;
	}

	/* 1. waiting for data */
	if(sizeInBuffer <= 0) 
	{
		this->eventSignal->reSetEvent();
		if (this->eventSignal->wait(this->timeout) != 0)
		{
			//WriteLog("没有数据，等待timeout！\n");
			* pBytesReturned = 0;
			return -1;
		}
	}

	/* 2. if have, transfer it */
	this->locker->lock();

	BYTE_P buffer_out_org = this->buffer_out;
	if(sizeInBuffer <= nOutBufSize) 
	{
		bytesRet = sizeInBuffer;
	}
	else
	{
		bytesRet = nOutBufSize;
	}

	//Copy the data
	if((this->buffer_out + bytesRet) <= (this->buffer + this->bufferSize))
	{
		memcpy(pOutBuf, this->buffer_out, bytesRet);
		this->buffer_out += bytesRet;
	} 
	else 
	{
		DWORD middleBytes = this->buffer + this->bufferSize - this->buffer_out;
		DWORD leftBytes = bytesRet - middleBytes;
		memcpy(pOutBuf, this->buffer_out, middleBytes);
		this->buffer_out = this->buffer;
		memcpy(&pOutBuf[middleBytes], this->buffer_out, leftBytes);
		this->buffer_out += leftBytes;
	}

	/*sizeInBuffer -= bytesRet;*/

	this->buffer_out = buffer_out_org;
	*pBytesReturned = bytesRet;

	this->locker->unlock();

	return 0;
}
U32 SyncCircleBuffer::seek(U32 nOutBufSize)
{

	if (0 == sizeInBuffer)
	{
		return 0;
	}


	this->locker->lock();

	DWORD bytesRet = 0;

	if(sizeInBuffer <= nOutBufSize) 
	{
		bytesRet = sizeInBuffer;
	}
	else
	{
		bytesRet = nOutBufSize;
	}

	//Copy the data
	if((this->buffer_out + bytesRet) <= (this->buffer + this->bufferSize))
	{
		this->buffer_out += bytesRet;
	} 
	else 
	{
		DWORD middleBytes = this->buffer + this->bufferSize - this->buffer_out;
		DWORD leftBytes = bytesRet - middleBytes;
		this->buffer_out = this->buffer;
		this->buffer_out += leftBytes;
	}

	sizeInBuffer -= bytesRet;
	
	this->locker->unlock();
	return bytesRet;
}
U32 SyncCircleBuffer::getSize()
{
	return this->bufferSize;
}
U32 SyncCircleBuffer::getDataSize()
{
	return this->sizeInBuffer;
}
int SyncCircleBuffer::reSize(U32 newBufferSize)
{
	return -1;
}