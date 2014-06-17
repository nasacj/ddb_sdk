#ifndef JSCPP_IO_BASEIO_H
#define JSCPP_IO_BASEIO_H


#ifndef __in
#define __in
#endif
#ifndef __out
#define __out
#endif
#ifndef __inout
#define __inout
#endif


#define NULL 0

#define implements public
#define Interface class

#define DDBSDK_DEBUG
#ifdef DDBSDK_DEBUG
#include <assert.h>
#else
#define assert
#endif

typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned int	U32;

typedef unsigned char*		PDATA_1_BYTE;
typedef unsigned short*		PDATA_2_BYTE;
typedef unsigned int*		PDATA_4_BYTE;

typedef PDATA_1_BYTE BYTE_P;

Interface Clouseable
{
public:
	virtual void close() = 0;
	virtual ~Clouseable(){};

};

Interface Flushable
{
public:
	virtual void flush() = 0;
	virtual ~Flushable(){};
};


Interface InputStream : implements Clouseable
{
public:
	virtual int readBytes(__in BYTE_P buffer, __in U32 bufferSize, __out U32* bytesReturned) = 0;
	virtual ~InputStream(){};
};

Interface OutputStream : implements Clouseable, implements Flushable
{
public:
	virtual void writeBytes(__out BYTE_P buffer, __in U32 bufferSize) = 0;
	virtual ~OutputStream(){};
};

Interface Buffer
{
public:
	virtual void store(__in BYTE_P buffer, __in U32 size) = 0;
	virtual int read(__inout BYTE_P buffer, __in U32 size, __out U32* bytesReturned) = 0;
	virtual int peek(__inout BYTE_P buffer, __in U32 size, __out U32* bytesReturned) = 0;
	virtual U32 seek(U32 bytes) = 0;
	virtual U32 getSize() = 0;
	virtual U32 getDataSize() = 0;
	virtual int reSize(U32 bufferSize) = 0;
	virtual ~Buffer(){};
};

Interface Locker
{
public:
	virtual void lock(void) = 0;
	virtual void unlock(void) = 0;
	virtual ~Locker(){};
};

Interface EventSignal
{
public:
	virtual void setEvent() = 0;
	virtual void reSetEvent() = 0;
	virtual int wait(U32) = 0;
	virtual ~EventSignal(){};
};



// class TS_InputStream : implements InputStream
// {
// public:
// 	int readBytes(__in BYTE_P buffer, __in U32 bufferSize, __out U32 bytesReturned);
// 	~TS_InputStream();
// };




#endif


/****************************************************************************************************
	* Function declare:
	*	getData
	* Description: Get data from Stream and store them into the MemoryBuffer which user provided.
	* Parameters: 
	*	buffer 	[in/out]:
	*		The pointer to the user providing MemoryBuffer.
	*	bufferSize	[in]:
	*		Buffer size of which user provided.
	* Return value:
	*	Bytes which copied into the buffer.
	* Notes:
	*	N/A
	*************************************************************************************************/
