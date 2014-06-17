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
*	�ص���������: typedef void (CALLBACK *pNaviCallbackFunc)
*								(BYTE *pData, DWORD dwLength, PVOID pParam);
*	����:		�ûص������Ƕ�TS��������ҵ����н��������ղ���ԭ����Ӧ�ļ��Ĺ��ܺ������
*				�Թ���̬���ӿ�TDMBLib.dll���лص������� TS�������ݽ��н�һ������
*	����˵��:	����1��BYTE *pData CA���ܺ��TS����������������ҵ��(PID)��TS������
*				����2�� DWORD dwLength ����������(�ֽ���)
*				����3�� PVOID pParam �������ݵĲ�����Ŀǰ����ΪNULL
*/
typedef void (CALLBACK *SETGDCALLBACKFUNC)(BYTE *pData, DWORD dwLength, PVOID pParam);



/***************************************************************************************
*	�ӿ���:		TDMB_Init(HWND hWnd,unsigned int MODE)
*	����:		��ʼ������TS����Ҫ��Filter
*	����˵��:	����1 : HWND hWnd ������
*				����2 : unsigned int MODE ģʽ���� �������䴫�̶�ֵ 3
*	����ֵ:		bool�� 
*				����true��ʾ��ʼ���ɹ��� false��ʾ��ʼ��ʧ��
*	��ע:		�ڵ��������ӿ�ǰ�����ȵ��øýӿ�
*/
TDMBLib_API bool TDMB_Init(HWND hWnd,unsigned int MODE);



/***************************************************************************************
*	�ӿ���:		TDMB_SetGDCallbackFunc(pNaviCallbackFunc NaviCallbackFunc)
*	����:		����TS�����ջ�����TS�����ݴ���Ļص�����
*	����˵��:	������pNaviCallbackFunc NaviCallbackFunc �ص�����ָ��
*	����ֵ:		void��
*	��ע:		������ص������󣬸ú����᲻�ϵػ�ȡ��CA���ܺ������
*/
TDMBLib_API void TDMB_SetGDCallbackFunc(SETGDCALLBACKFUNC);



/***************************************************************************************
*	�ӿ���:		TDMB_InitDataDown()
*	����:		����TDMB_Init�����г�ʼ����Filter��ʼ��������
*	����˵��:	�޲���
*	����ֵ:		bool�� 
*				����true��ʾ�ɹ��� false��ʾʧ��
*	��ע:		���ô˽ӿ�ǰ�����ȵ���TDMB_Init()���ҷ��سɹ�
*/
TDMBLib_API bool TDMB_InitDataDown();



/***************************************************************************************
*	�ӿ���:		TDMB_UnInit()
*	����:		�Ͽ�TDMB_InitDataDown()�����ӵĺ�ɾ��TDMB_Init()��������Filter
*	����˵��:	�޲���
*	����ֵ:		bool�� 
*				����true��ʾ����ʼ���ɹ��� false��ʾ����ʼ��ʧ��
*	��ע:		���øýӿڷ���ʼ���󲻿����ٵ���TDMB_InitDataDown()
*				�����ٴε���������ȵ���TDMB_Init()���г�ʼ��
*/
TDMBLib_API bool TDMB_UnInit();



/***************************************************************************************
*	�ӿ���:		TDMB_GetSignalSNR()
*	����:		����TDMB_Init�����г�ʼ����Filter��ʼ��������
*	����˵��:	�޲���
*	����ֵ:		unsigned int �� 
*				��Χ0-10��0��ʾ�ź���10��ʾ�ź����
*				��û��������Ƶ���б������ĳЩ����¸÷����ķ���ֵ����һ���ܴ��ֵ��
*				����0-10�ķ�Χ������Ϊ���ź�
*	��ע:		���ô˽ӿ�ǰ�����ȵ���TDMB_Init()���ҷ��سɹ�
*/
TDMBLib_API unsigned int TDMB_GetSignalSNR();



/***************************************************************************************
*	�ӿ���:		DVTCASTB_GetCardID(DWORD *pdwCardID)
*	����:		����TDMB_Init�����г�ʼ����Filter��ʼ��������
*	����˵��:	����: DWORD *pdwCardID ���ڴ洢��ȡ��CA������
*	����ֵ:		bool�� 
*				����true��ʾ��ȡ�ɹ��� false��ʾ��ȡʧ��
*	��ע:		ĿǰTDMBLib.dllû�н��÷�����װ�ɽӿڣ��Ĺ��Ϻ�ͨ������������ʾCA���ţ�
*				Ϊ�����㹦�������TDMBLib.dll������һЩ�Ķ�
*				������CA SDK�ṩ����ý���ȡCA�����ŵķ�����װ�ɽӿڳ������Ա����
*/
TDMBLib_API bool DVTCASTB_GetCardID(DWORD *pdwCardID);




#ifdef __cplusplus
}
#endif
#endif
