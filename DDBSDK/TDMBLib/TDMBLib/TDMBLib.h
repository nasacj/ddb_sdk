/***************************************************************************
 *                    
 * Project       TMBLib Dynamic Library based on DDBSDK
 *
 * Copyright (C) 2010, AVTECSH, Ltd.
 *
 ***************************************************************************/

#ifndef _DDBSDK_TDMBLIB_H
#define _DDBSDK_TDMBLIB_H

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TDMBLib_API	__declspec(dllexport)



/****************************************************************************************	
*	回调函数定义: typedef void (CALLBACK *pNaviCallbackFunc)
*								(BYTE *pData, DWORD dwLength, PVOID pParam);
*	描述:		该回调函数是对TS明流数据业务进行解析、接收并还原成相应文件的功能函数入口
*				以供动态链接库TDMBLib.dll进行回调，传递 TS明流数据进行进一步处理
*	参数说明:	参数1：BYTE *pData CA解密后的TS明流包含所需数据业务(PID)的TS流数据
*				参数2： DWORD dwLength 数据流长度(字节数)
*				参数3： PVOID pParam 其他传递的参数，目前设置为NULL
*/
typedef void (CALLBACK *SETGDCALLBACKFUNC)(BYTE *pData, DWORD dwLength, PVOID pParam);



/***************************************************************************************
*	接口名:		TDMB_Init(HWND hWnd,unsigned int MODE)
*	描述:		初始化接收TS流必要的Filter
*	参数说明:	参数1 : HWND hWnd 窗体句柄
*				参数2 : unsigned int MODE 模式参数 参数不变传固定值 3
*	返回值:		bool型 
*				返回true表示初始化成功， false表示初始化失败
*	备注:		在调用其它接口前必须先调用该接口
*/
TDMBLib_API bool TDMB_Init(HWND hWnd,unsigned int MODE);



/***************************************************************************************
*	接口名:		TDMB_SetGDCallbackFunc(pNaviCallbackFunc NaviCallbackFunc)
*	描述:		设置TS流接收或其他TS流数据处理的回调函数
*	参数说明:	参数：pNaviCallbackFunc NaviCallbackFunc 回调函数指针
*	返回值:		void型
*	备注:		设置完回调函数后，该函数会不断地获取到CA解密后的数据
*/
TDMBLib_API void TDMB_SetGDCallbackFunc(SETGDCALLBACKFUNC);



/***************************************************************************************
*	接口名:		TDMB_InitDataDown()
*	描述:		连接TDMB_Init方法中初始化的Filter开始数据下载
*	参数说明:	无参数
*	返回值:		bool型 
*				返回true表示成功， false表示失败
*	备注:		调用此接口前必须先调用TDMB_Init()并且返回成功
*/
TDMBLib_API bool TDMB_InitDataDown();



/***************************************************************************************
*	接口名:		TDMB_UnInit()
*	描述:		断开TDMB_InitDataDown()所连接的和删除TDMB_Init()所创建的Filter
*	参数说明:	无参数
*	返回值:		bool型 
*				返回true表示反初始化成功， false表示反初始化失败
*	备注:		调用该接口反初始化后不可以再调用TDMB_InitDataDown()
*				如需再次调用则必须先调用TDMB_Init()进行初始化
*/
TDMBLib_API bool TDMB_UnInit();



/***************************************************************************************
*	接口名:		TDMB_GetSignalSNR()
*	描述:		连接TDMB_Init方法中初始化的Filter开始数据下载
*	参数说明:	无参数
*	返回值:		unsigned int 型 
*				范围0-10，0表示信号最差，10表示信号最好
*				在没有搜索到频道列表或其它某些情况下该方法的返回值会是一个很大的值，
*				超出0-10的范围均可认为无信号
*	备注:		调用此接口前必须先调用TDMB_Init()并且返回成功
*/
TDMBLib_API unsigned int TDMB_GetSignalSNR();



/***************************************************************************************
*	接口名:		DVTCASTB_GetCardID(DWORD *pdwCardID)
*	描述:		连接TDMB_Init方法中初始化的Filter开始数据下载
*	参数说明:	参数: DWORD *pdwCardID 用于存储获取的CA软卡卡号
*	返回值:		bool型 
*				返回true表示获取成功， false表示获取失败
*	备注:		目前TDMBLib.dll没有将该方法封装成接口，文广上海通新增功能需显示CA卡号，
*				为了满足功能需求对TDMBLib.dll进行了一些改动
*				后续的CA SDK提供商最好将获取CA软卡卡号的方法封装成接口出来，以便调用
*/
TDMBLib_API bool DVTCASTB_GetCardID(DWORD *pdwCardID);




#ifdef __cplusplus
}
#endif
#endif
