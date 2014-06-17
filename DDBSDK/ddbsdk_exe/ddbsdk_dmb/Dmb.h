#ifndef _DMB_H_
#define _DMB_H_
#include "../platformHeader/platform.h"
#define SMSHOSTLIB_MULTI_DEVICE_API

#ifdef DDBSDK_PLATFORM_GUOWEI_H
#include <windows.h>
HANDLE   SianoPowerOn(void);
void  SianoPowerDown(void);
#endif
long  DmbStart(int frequency);
void  DmbStop(void);
void  checkSignal(int frequency);
unsigned long  startDmbService(int  index, int testInSMGLabFlag);
unsigned long  stopDmbService();
void dmbGetServiceNum(unsigned int * ServiceNum);

int getMusicamServNum(void);
int getMusicamServerIndex(int num);
void getMusicamDescr(char * descr, int num);
int getVideoServNum(void);
int getVideoServerIndex(int num);
void getVideoDescr(char * descr, int num);
int getDataServNum();
int getDataServerIndex(int num);
int getSmsDmbLibVersion(void);
#endif
