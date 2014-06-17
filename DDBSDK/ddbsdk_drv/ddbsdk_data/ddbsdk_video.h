

#ifndef _DDBSDK_VIDEO_H_
#define _DDBSDK_VIDEO_H_

#include "ddbsdk_data.h"

class ddbsdk_dmbVideo : public ddbsdk_data
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
