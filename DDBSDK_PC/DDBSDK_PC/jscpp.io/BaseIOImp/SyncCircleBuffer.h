#ifndef SYNC_CRICLE_BUFFER_H_
#define SYNC_CRICLE_BUFFER_H_

#include "../BaseIO.h"

class SyncCircleBuffer : implements Buffer
{
public:
	SyncCircleBuffer();
	SyncCircleBuffer(U32 bufferSize, U32 timeout);
	void store(__in BYTE_P buffer, __in U32 size);
	int read(__inout BYTE_P buffer, __in U32 size, __out U32* bytesReturned);
	int peek(__inout BYTE_P buffer, __in U32 size, __out U32* bytesReturned);
	U32 seek(U32 bytes);
	U32 getSize();
	U32 getDataSize();
	int reSize(U32 bufferSize);
	~SyncCircleBuffer();

private:
	BYTE_P buffer;
	BYTE_P buffer_in;
	BYTE_P buffer_out;
	U32 sizeInBuffer;
	U32 bufferSize;
	U32 timeout;
	Locker* locker;
	EventSignal* eventSignal;
};

#endif