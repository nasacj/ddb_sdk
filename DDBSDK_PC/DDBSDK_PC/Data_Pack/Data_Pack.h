#ifndef DATA_PACK_H
#define DATA_PACK_H

#include "../jscpp.io/BaseIO.h"


Interface I_DataPack
{
public:
// 	I_DataPack(){};
// 	I_DataPack(U32 timeout){};
	virtual U32 putPlainTsData(BYTE_P tsBuf, U32 inSize) = 0;
	virtual U32 getPlainPkgData(BYTE_P pOutBuf, U32 nOutBufSize, U32* pBytesReturned) = 0;

	virtual ~I_DataPack(){};
};

class Ts_DataPack : implements I_DataPack
{
public:
	Ts_DataPack(U32 timeout);

	U32 putPlainTsData(BYTE_P tsBuf, U32 inSize);
	U32 getPlainPkgData(BYTE_P pOutBuf, U32 nOutBufSize, U32* pBytesReturned);

	~Ts_DataPack();

private:
	Buffer* data_buffer;
// protected:
// 	U32 timeout;
};

class DataPack : implements I_DataPack
{
public:
	DataPack(U16 pid, U32 timeout);

	U32 putPlainTsData(BYTE_P tsBuf, U32 inSize);
	U32 getPlainPkgData(BYTE_P pOutBuf, U32 nOutBufSize, U32* pBytesReturned);
	virtual U32 getPkgLeng(BYTE_P plainTsBuf, int headerLen);
	virtual U32 checkPkg(BYTE_P plainPkgBuf, U32* retBytes, U32 pkgLength);

	~DataPack();

private:
	Buffer* data_buffer;
protected:
	//U32 timeout;
	U16 pid;
	U8	isHaveCW;//????????????????????????
	long Decrypt_data(BYTE_P tsBufOut, BYTE_P tsBufIn, void* g_OddCW, void* g_EvenCW);
};


class DataPack_PAT : public DataPack
{
public:
	DataPack_PAT(U16 pid, U32 timeout);

	U32 getPkgLeng(BYTE_P plainTsBuf, int headerLen);
	U32 checkPkg(BYTE_P plainPkgBuf, U32* retBytes, U32 pkgLength);

	~DataPack_PAT();
};


class DataPack_PMT : public DataPack
{
public:
	DataPack_PMT(U16 pid, U32 timeout);

	U32 getPkgLeng(BYTE_P plainTsBuf, int headerLen);
	U32 checkPkg(BYTE_P plainPkgBuf, U32* retBytes, U32 pkgLength);

	~DataPack_PMT();
};

class DataPack_CAT : public DataPack
{
public:
	DataPack_CAT(U16 pid, U32 timeout);

	U32 getPkgLeng(BYTE_P plainTsBuf, int headerLen);
	U32 checkPkg(BYTE_P plainPkgBuf, U32* retBytes, U32 pkgLength);

	~DataPack_CAT();
};






#endif