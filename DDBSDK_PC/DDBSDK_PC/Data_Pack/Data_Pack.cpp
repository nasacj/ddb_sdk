#include <string.h>

#include "SyncCircleBuffer.h"
#include "Data_Pack.h"
#include "../ca/Rijndael.h"


#define IMPOSSIBLE_PKGLONG 0x1000;
#define CHECK_IF_START_PACK(TSBUF)   (TSBUF[1]) & 0x40
#define PLAINTSBUFSIZE  1000 * 188

typedef struct _TSHEADER_INFO_
{
	unsigned sync_byte:         8;

	unsigned pid_5:             5;
	unsigned transport_piority: 1;
	unsigned start_indicator:   1;
	unsigned error_indicator:   1;

	unsigned pid_8:              8;

	unsigned continue_counter:   4;
	unsigned adaptation_control: 2;
	unsigned scrambling_control: 2;

} TsHeader_t;

__inline U32 getAdaptionLen(BYTE_P ts_buf)
{
	//载荷前面有调整字段 0x30, 仅有调整字段 0x20
	U32 wAdaptionLen = 0;
	if(0x30 == (ts_buf[3] & 0x30) || 0x20 == (ts_buf[3] & 0x30))
	{
		wAdaptionLen = ts_buf[4] + 1;
	}
	return wAdaptionLen;
}

Ts_DataPack::Ts_DataPack(U32 timeout)
{
	this->data_buffer = new SyncCircleBuffer(PLAINTSBUFSIZE, timeout);
	assert(this->data_buffer);
}

Ts_DataPack::~Ts_DataPack()
{
	delete this->data_buffer;
}

U32 Ts_DataPack::putPlainTsData(BYTE_P tsBuf, U32 inSize)
{
	if (0 == tsBuf || 0 == inSize)
	{
		return -1;
	}

	data_buffer->store(tsBuf, inSize);

	return 0;
}

U32 Ts_DataPack::getPlainPkgData(BYTE_P pOutBuf, U32 nOutBufSize, U32* pBytesReturned)
{
	if (0 == pOutBuf || 0 == nOutBufSize)
	{
		* pBytesReturned = 0;
		return -1;
	}
	return this->data_buffer->read(pOutBuf, nOutBufSize, pBytesReturned);
}

DataPack::DataPack(U16 pid, U32 timeout):pid(pid)
{
	this->data_buffer = new SyncCircleBuffer(PLAINTSBUFSIZE, timeout);
	assert(this->data_buffer);
}

DataPack::~DataPack()
{
	delete this->data_buffer;
}

long DataPack::Decrypt_data(BYTE_P tsBufOut, BYTE_P tsBufIn, void* g_OddCW, void* g_EvenCW)
{
	CRijndael aes;

	//仅有调整字段
	if(0x20 == (tsBufIn[3] & 0x30))
		return -1;

	//载荷前面有调整字段
	unsigned short wAdaptionLen = 0;
	if(0x30 == (tsBufIn[3] & 0x30))
	{
		wAdaptionLen = tsBufIn[4];
	}

	int DataLen = (188 - 5 - wAdaptionLen) / 16 * 16;

	switch(tsBufIn[3] & 0xc0)
	{
		//加扰的包才需要解扰
	case 0xc0:	//c0用odd key

		memcpy(tsBufOut, tsBufIn, 188);
		aes.MakeKey((char*)g_OddCW, (char*)g_OddCW, 16, 16);
		aes.Decrypt((char*)(&tsBufIn[0]+4+wAdaptionLen), (char*)&tsBufOut[0]+4+wAdaptionLen, DataLen);
		break;
	case 0x80:	//80用even key

		memcpy(tsBufOut, tsBufIn, 188);
		aes.MakeKey((char*)g_EvenCW, (char*)g_EvenCW, 16, 16);
		aes.Decrypt((char*)(&tsBufIn[0]+4+wAdaptionLen), (char*)&tsBufOut[0]+4+wAdaptionLen, DataLen);
		break;
	default:
		break;
	}
	return 0;
}

U32 DataPack::putPlainTsData(BYTE_P tsBuf, U32 inSize)
{
	if (0 == tsBuf || inSize != 188 || tsBuf[0] != 0x47)
	{
		return -1;
	}
	TsHeader_t * tsHeader = (TsHeader_t *)tsBuf;
	U16 wPID = (tsHeader->pid_5 << 8) + tsHeader->pid_8;
	if (this->pid == wPID)
	{
		data_buffer->store(tsBuf, 188);
	}
	
	return 0;
}

U32 DataPack::getPlainPkgData(BYTE_P pOutBuf, U32 nOutBufSize, U32* pBytesReturned)
{
	U8 ts_packet[188] = {0};
	U32 bytesRt = 0;
	do 
	{
		this->data_buffer->read(ts_packet, 188, &bytesRt);
		if (bytesRt <= 0)
		{
			*pBytesReturned = 0;
			return -1;
		}
	} while (!CHECK_IF_START_PACK(ts_packet));
	
	U32 wAdaptionLen = getAdaptionLen(ts_packet);
	U32 headerLen = 5 + wAdaptionLen;

	/* continuous counter check */
	U8 cc = ts_packet[3] & 0xf;
	U8 prev_cc = 0;

	BYTE_P ptr = pOutBuf;
	U32 bytesRet = 0;
	U32 length = 0;
	U32 pkgLength = 0;
	U32 leftLength = 0;

	/* total package length check */
	leftLength = pkgLength = getPkgLeng(ts_packet, headerLen);

	do 
	{
		length = 188 - headerLen;
		length = length <= leftLength ? length : leftLength;
		leftLength -= length;

		/* check left package length */
		if(bytesRet + length > nOutBufSize) 
		{
// 			printf("too long pkg, overflowed!\n");
// 			WriteLog("too long pkg, overflowed!\n");
			* pBytesReturned = 0;
			return -1;
		}

		memcpy(ptr, &ts_packet[headerLen], length);
		bytesRet += length;
		ptr += length;

		if(pkgLength <= 188 - headerLen) 
			break;
		if(leftLength <= 0) 
			break;


		this->data_buffer->peek(ts_packet, 188, &bytesRt);
		if (bytesRt == 0)
		{
			* pBytesReturned = 0;
			return -1;
		}

		/* check continuous counter */
		prev_cc = cc;
		cc = ts_packet[3] & 0xf;
		if(cc != ((prev_cc + 1) & 0xF)) {
// 			printf("continue counter error!\n");
// 			WriteLog("continue counter error!\n");
			* pBytesReturned = 0;
			return -1;
		}

		/* check if the start package */
		if(CHECK_IF_START_PACK(ts_packet)) 
			break;

		this->data_buffer->seek(188);
		
		
		wAdaptionLen = getAdaptionLen(ts_packet);
		headerLen = 4 + wAdaptionLen;

	} while (1);

	/* check data integrity */
	if(!checkPkg(pOutBuf, &bytesRet, pkgLength)) {
// 		printf("error, pkgLen: %d, getBytes: %d\n", pkgLength, bytesRet);
// 		WriteLog("error, pkgLen: %d, getBytes: %d\n", pkgLength, bytesRet);
		* pBytesReturned = 0;
	} else {
		* pBytesReturned = bytesRet;
	}

	return 0;
}

U32 DataPack::getPkgLeng(BYTE_P plainTsBuf, int headerLen)
{
	return IMPOSSIBLE_PKGLONG;
}

U32 DataPack::checkPkg(BYTE_P plainPkgBuf, U32* retBytes, U32 pkgLength)
{
	return 1;
}