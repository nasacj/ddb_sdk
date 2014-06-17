

#ifndef _DDBSDK_PATPMT_H_
#define _DDBSDK_PATPMT_H_

#include "ddbsdk_data.h"

class ddbsdk_pat : public ddbsdk_data
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

class ddbsdk_pmt : public ddbsdk_data
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
