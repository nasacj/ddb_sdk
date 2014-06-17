//本文件定义要求终端实现并提供给CA模块使用的接口。所有HRESULT返回值为DVTCA_OK都表示成功，其他值表示失败。

#ifndef _DVTSTB_CASINTERFACE_H_2007_11_10_
#define _DVTSTB_CASINTERFACE_H_2007_11_10_

#include "DVTCAS_STBDataPublic.h"

#ifdef  __cplusplus
extern "C" {
#endif 

/*----------------------------------以下接口是STB提供给CAS调用--------------------------------------------*/

/*++
功能：返回UUID（终端唯一标识信息，如：芯片序列号,不超过16字节）。
参数：
	pbyUUid：UUID的开始地址
	pwLen：  输入为申请UUID的长度，输出为实际UUID的长度。
--*/

HRESULT DVTSTBCA_GetUUid(BYTE *pbyUUid, unsigned short *pwLen);

/*++
功能：获得机顶盒分配给CAS的存储空间的起点地址和大小（以字节为单位）。
参数：
	ppStartAddr:			机顶盒分配给CAS的存储空间的开始地址。
	lSize:					输出机顶盒分配给CAS的储存空间的大小。
--*/
HRESULT DVTSTBCA_GetDataBufferAddr(long* lSize, char ** ppStartAddr);

/*++
功能：读取保存在机顶盒中的信息。
参数：
	pStartAddr:				要读取的存储空间的开始地址。
	plDataLen:				输入为要读取的最长数据值；输出为实际读取的大小。
	pData:					存放输出数据。
--*/
HRESULT DVTSTBCA_ReadDataBuffer(const char * pStartAddr, long * plDataLen, BYTE* pData);

/*++
功能：向机顶盒的存储空间写信息。
参数：
	lStartAddr:				要写的存储空间的开始地址。
	plDataLen:				输入为要写的数据的长度；输出为写入的实际长度。
	pData:					要写的数据。
--*/
HRESULT DVTSTBCA_WriteDataBuffer(const char * pStartAddr, long * plDataLen, const BYTE* pData);

/*++
功能：CA程序用此函数设置解扰器。将当前周期及下一周期的CW送给解扰器。
参数：
	wEcmPID:				CW所属的ECMPID。
	szOddKey:				奇CW的数据。
	szEvenKey:				偶CW的数据。
	byKeyLen:				CW的长度。
--*/
HRESULT DVTSTBCA_SetDescrCW(WORD wEcmPID, BYTE byKeyLen, const BYTE* szOddKey, const BYTE* szEvenKey);

/*++
功能：异步显示用户不能观看节目等提示信息，下面两个函数对应使用。
参数：	
	byMesageNo:				要显示的消息编号。

  #define	DVTCA_NO_ENTITLE						5			//没有购买此节目
  #define	DVTCA_PRODUCT_RESTRICT					6			//该节目被限制收看
  #define	DVTCA_DATA_INVALID						16			//数据无效
  #define	DVTCA_TVS_NOT_FOUND						19			//运营商不存在
  #define	DVTCA_KEY_NOT_FOUND						20			//运营商密钥不存在
  #define	DVTCA_SC_IS_FORBIDDEN					21			//卡被禁用
  #define	DVTCA_DECRYPT_FAILURE					0xFF		//节目不能收看
--*/
void   DVTSTBCA_ShowPromptMessage(BYTE byMesageNo);

//隐藏显示的界面
void   DVTSTBCA_HidePromptMessage(void);

/*
功能：添加调试信息。
参数:	
	pszMsg:					调试信息。
--*/
void DVTSTBCA_AddDebugMsg(const char * pszMsg);
#ifdef  __cplusplus
}
#endif
#endif  
