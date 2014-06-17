/*define several macro and common data structure*/
#ifndef _DVTCAS_STBDataPublic_H_2004_12_31_
#define _DVTCAS_STBDataPublic_H_2004_12_31_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef BYTE	//8bit
#define BYTE unsigned char
#endif
#ifndef WORD	//16bit
#define WORD unsigned short
#endif
#ifndef HRESULT	//32bit
#define	HRESULT long
#endif
#ifndef DWORD	//32bit
#define DWORD unsigned long
#endif
#ifndef bool	//8bit
#define bool unsigned char
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#define DVTCA_MAXLEN_TVSNAME					21		//最长运营商名称长度+\0
#define DVTCA_MAXLEN_TVSDSC						21		//最长运营商描述长度+\0
#define DVTCA_MAXLEN_MANUFACTURERNAME			21		//最长的CAS厂商名字长度+\0 
#define DVTCA_MAXLEN_PRODUCT_NAME				21		//最长的产品名称,最多20个字符+\0

//目前只支持单运营商
#define	DVTCA_MAXNUMBER_TVSID					5		//支持的运营商最大个数

//请求收取私有数据的类型
#define DVTCA_STB_ECM_TYPE						1		
#define DVTCA_STB_EMM_TYPE						2

/*----------------------------------------------------------------------*/
/*                       下面是返回值定义                               */
/*----------------------------------------------------------------------*/

//机顶盒返回值基值定义
#define DVTCAERR_BASE_STB						0xF1000000

//返回值定义
#define DVTCA_OK								0 

#define DVTCAERR_STB_POINTER_INVALID			(DVTCAERR_BASE_STB + 1)		//输入的指针无效
#define DVTCAERR_STB_DATA_LEN_ERROR				(DVTCAERR_BASE_STB + 2)		//输入的数据长度不合法
#define DVTCAERR_STB_TVS_NOT_FOUND				(DVTCAERR_BASE_STB + 3)		//没有找到符合要求的运营商
#define DVTCAERR_STB_EXCEED_MAX_COUNT			(DVTCAERR_BASE_STB + 4)		//超过了允许的最大的个数
#define DVTCAERR_STB_KEY_NOT_FOUND				(DVTCAERR_BASE_STB + 5)		//没有找到解密密钥
#define DVTCAERR_STB_CMD_INVALID				(DVTCAERR_BASE_STB + 8)		//MAC校验错误
#define DVTCAERR_STB_VER_EXPIRED				(DVTCAERR_BASE_STB + 9)		//数据已经过期
#define DVTCAERR_STB_EXCEED_MAX_LEN				(DVTCAERR_BASE_STB + 10)	//运营商的名称超过最大的长度
#define DVTCAERR_STB_NO_SPACE					(DVTCAERR_BASE_STB + 11)	//已经没有空间
#define DVTCAERR_STB_COUNT_ISZERO				(DVTCAERR_BASE_STB + 14)	//输入的Count为0. 
#define	DVTCAERR_STB_OVERFLOW					(DVTCAERR_BASE_STB + 16)	//溢出
#define DVTCAERR_STB_UNKNOWN					(DVTCAERR_BASE_STB + 19)	//未知错误

//不能取消的显示信息列表
#define	DVTCA_NO_ENTITLE						5			//没有购买此节目
#define	DVTCA_PRODUCT_RESTRICT					6			//该节目被限制收看
#define DVTCA_DATA_INVALID						16			//数据无效
#define DVTCA_TVS_NOT_FOUND						19			//运营商不存在
#define DVTCA_KEY_NOT_FOUND						20			//运营商密钥不存在
#define DVTCA_SC_IS_FORBIDDEN					21			//卡被禁用
#define	DVTCA_DECRYPT_FAILURE					0xFF		//节目不能收看
	
/*----------------------------------------------------------------------*/
/*                       下面是一些数据结构定义                         */
/*----------------------------------------------------------------------*/

//#pragma pack(push,1)
typedef struct _SDVTCAManuInfo
{
	DWORD	m_dwCardID;											//卡内部ID
	DWORD	m_dwSTBCASVer;										//机顶盒CAS模块的版本号
	char	m_szSCCASManuName[DVTCA_MAXLEN_MANUFACTURERNAME];	//CAS供应商的名称
}SDVTCAManuInfo;
	
typedef struct _SDVTCATvsInfo
{
	WORD	m_wTVSID; 											//运营商的外部编号
	char	m_szTVSName[DVTCA_MAXLEN_TVSNAME];					//运营商名称
	char	m_szTVSDsc[DVTCA_MAXLEN_TVSDSC];					//运营商描述
	BYTE	m_byServiceStatus;									//运营商对此卡的服务状态,待定。
}SDVTCATvsInfo;

typedef struct 
{
	WORD 	m_wProductID;										//产品ID。
	DWORD	m_tEntitleTime;										//授权时间，time_t格式。
	DWORD	m_tStartTime;										//开始时间，time_t格式。
	DWORD	m_tEndTime;											//结束时间，time_t格式。
	char	m_szProductName[DVTCA_MAXLEN_PRODUCT_NAME];			//产品名称
}SDVTCAServiceEntitle;											//普通产品授权
//#pragma pack(pop)

//{{define for multithreading
typedef unsigned long DVTCA_Semaphore;
typedef void (*pThreadFunc)(void);
//}}define for multithreading

#ifdef  __cplusplus
}
#endif
#endif	
