#include "Data_Pack.h"


DataPack_CAT::DataPack_CAT(U16 pid, U32 timeout):DataPack(pid, timeout)
{

}

U32 DataPack_CAT::getPkgLeng(BYTE_P plainTsBuf, int headerLen)
{
	return ((plainTsBuf[headerLen + 1] & 0xF) << 8) + plainTsBuf[headerLen + 2] + 3;
}

U32 DataPack_CAT::checkPkg(BYTE_P plainPkgBuf, U32* retBytes, U32 pkgLength)
{
	if(pkgLength == * retBytes) 
	{
		return 1;
	} 
	else 
	{
		return 0;
	}
}


DataPack_CAT::~DataPack_CAT()
{

}

