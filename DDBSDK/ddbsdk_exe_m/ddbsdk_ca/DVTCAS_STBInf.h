//本文件定义CA模块实现提供给终端调用的接口
#ifndef _DVTCAS_STBINTERFACE_H_2007_11_10_
#define _DVTCAS_STBINTERFACE_H_2007_11_10_

#include "DVTCAS_STBDataPublic.h"

#ifdef  __cplusplus
extern "C" {
#endif 

/*-----------------------------------以下接口是CAS提供给STB调用----------------------------------*/
/*++
功能：机顶盒启动的时候调用，对CAS模块初始化。
返回值：
	true				初始化成功。
	false				初始化失败。
--*/
bool DVTCASTB_Init(void);

/*++
功能：机顶盒获得CASID后调用该接口判断CASID是否有效，如果有效则返回true，否则返回false。
参数：
	wCaSystemID:		机顶盒分析PAT/PMT信息获得的CASID。
返回值：
	true				CASID有效
	false				CASID无效
--*/
bool DVTCASTB_CASIDVerify(WORD wCaSystemID);

/*++
功能：机顶盒获得CA模块的版本。
参数：
	pdwVer:				返回版本号。					
返回值：
	true				成功。
	false				失败。
说明：
	返回CA模块的版本号。举例: 如果是0x01020304意思就是1.2.3.4。
--*/
bool DVTCASTB_GetVer(DWORD *pdwVer);

/*++
功能：调用CAS模块，获得软卡内部ID。
参数：
	pdwCardID:			返回软卡内部ID。						
返回值：
	true				成功
	false				失败
--*/
bool DVTCASTB_GetCardID(DWORD *pdwCardID);

/*++
功能：获得CA厂商名。
参数：
	pbyCASManuName:			返回CA厂商名,'\0'结束的字符串						
返回值：
	true				成功
	false				失败
--*/
bool DVTCASTB_GetCASManuName(char pbyCASManuName[DVTCA_MAXLEN_MANUFACTURERNAME]);

/*++
功能：获得运营商信息。
参数：
	pbyCount			输入为运营商的最大个数，输出为实际的运营商个数, 目前只支持个数为1
	pTvsInfo:			返回运营商信息列表						
返回值：
	true				成功
	false				失败
--*/
bool DVTCASTB_GetTvsInfo(BYTE* pbyCount, SDVTCATvsInfo* pTvsInfo);

/*++
功能：设置当前正在收看的节目的详细信息。
参数：
	wEcmpid
说明：
	EcmPid设置给CA模块,每次切台时需要调用，如果节目未加扰，wEcmpid设为0。
--*/
void DVTCASTB_SetCurEcmPid(WORD wEcmPid);

/*++
功能：设置EMMPID，CAS模块将根据设置的EMMPID信息进行EMM数据接收处理。
参数：
		wEmmPid:emmPid
说明:
	当机顶盒收到CAT表后，机顶盒需要调用CA提供的此函数，将EMM PID传过来，这样CA程序就可以根据此PID收取
	私有表，更新用户卡里的授权及为用户提供增值服务。
--*/
void DVTCASTB_SetEmmPid(WORD wEmmPid);

/*++
功能：机顶盒主程序在收到CAS私有数据，调用这个接口将数据提供给CAS模块处理。
参数：
	byType：				CAS私有数据的类型（ECM:1/EMM:2）
	wPID:					接收的数据的流的PID。
	byszReceiveData：		收取的私有数据
	wLen：					收取到的私有数据的长度
--*/
void DVTCASTB_StreamGuardDataGot(BYTE byType, WORD wPID, BYTE byLen, const BYTE* byszReceiveData);

/*++
功能：获得普通授权节目购买的情况，用于在机顶盒界面上显示。
参数：
	wTVSID:					运营商的内部编号。
	pEntitleCount:			输入时为数组的最大个数，输出时为实际获得的个数。
	psEntitles:				返回的授权数组。						
返回值：
	DVTCA_OK					成功
	DVTCAERR_STB_TVS_NOT_FOUND	没有找到符合要求的运营商
	其他						失败
--*/
HRESULT DVTCASTB_GetServiceEntitles(WORD wTvsId, BYTE *pEntitleCount,SDVTCAServiceEntitle* psEntitles);

/*++
功能：打开或关闭调试信息。
参数：
	bDebugMsgSign:			打开调试信息标志，1:打开调试信息;0:关闭调试信息。
--*/
void DVTCASTB_AddDebugMsgSign(bool bDebugMsgSign);

#ifdef  __cplusplus
}
#endif
#endif  

