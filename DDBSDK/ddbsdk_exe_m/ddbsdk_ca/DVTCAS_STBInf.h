//���ļ�����CAģ��ʵ���ṩ���ն˵��õĽӿ�
#ifndef _DVTCAS_STBINTERFACE_H_2007_11_10_
#define _DVTCAS_STBINTERFACE_H_2007_11_10_

#include "DVTCAS_STBDataPublic.h"

#ifdef  __cplusplus
extern "C" {
#endif 

/*-----------------------------------���½ӿ���CAS�ṩ��STB����----------------------------------*/
/*++
���ܣ�������������ʱ����ã���CASģ���ʼ����
����ֵ��
	true				��ʼ���ɹ���
	false				��ʼ��ʧ�ܡ�
--*/
bool DVTCASTB_Init(void);

/*++
���ܣ������л��CASID����øýӿ��ж�CASID�Ƿ���Ч�������Ч�򷵻�true�����򷵻�false��
������
	wCaSystemID:		�����з���PAT/PMT��Ϣ��õ�CASID��
����ֵ��
	true				CASID��Ч
	false				CASID��Ч
--*/
bool DVTCASTB_CASIDVerify(WORD wCaSystemID);

/*++
���ܣ������л��CAģ��İ汾��
������
	pdwVer:				���ذ汾�š�					
����ֵ��
	true				�ɹ���
	false				ʧ�ܡ�
˵����
	����CAģ��İ汾�š�����: �����0x01020304��˼����1.2.3.4��
--*/
bool DVTCASTB_GetVer(DWORD *pdwVer);

/*++
���ܣ�����CASģ�飬������ڲ�ID��
������
	pdwCardID:			�������ڲ�ID��						
����ֵ��
	true				�ɹ�
	false				ʧ��
--*/
bool DVTCASTB_GetCardID(DWORD *pdwCardID);

/*++
���ܣ����CA��������
������
	pbyCASManuName:			����CA������,'\0'�������ַ���						
����ֵ��
	true				�ɹ�
	false				ʧ��
--*/
bool DVTCASTB_GetCASManuName(char pbyCASManuName[DVTCA_MAXLEN_MANUFACTURERNAME]);

/*++
���ܣ������Ӫ����Ϣ��
������
	pbyCount			����Ϊ��Ӫ�̵������������Ϊʵ�ʵ���Ӫ�̸���, Ŀǰֻ֧�ָ���Ϊ1
	pTvsInfo:			������Ӫ����Ϣ�б�						
����ֵ��
	true				�ɹ�
	false				ʧ��
--*/
bool DVTCASTB_GetTvsInfo(BYTE* pbyCount, SDVTCATvsInfo* pTvsInfo);

/*++
���ܣ����õ�ǰ�����տ��Ľ�Ŀ����ϸ��Ϣ��
������
	wEcmpid
˵����
	EcmPid���ø�CAģ��,ÿ����̨ʱ��Ҫ���ã������Ŀδ���ţ�wEcmpid��Ϊ0��
--*/
void DVTCASTB_SetCurEcmPid(WORD wEcmPid);

/*++
���ܣ�����EMMPID��CASģ�齫�������õ�EMMPID��Ϣ����EMM���ݽ��մ���
������
		wEmmPid:emmPid
˵��:
	���������յ�CAT��󣬻�������Ҫ����CA�ṩ�Ĵ˺�������EMM PID������������CA����Ϳ��Ը��ݴ�PID��ȡ
	˽�б������û��������Ȩ��Ϊ�û��ṩ��ֵ����
--*/
void DVTCASTB_SetEmmPid(WORD wEmmPid);

/*++
���ܣ����������������յ�CAS˽�����ݣ���������ӿڽ������ṩ��CASģ�鴦��
������
	byType��				CAS˽�����ݵ����ͣ�ECM:1/EMM:2��
	wPID:					���յ����ݵ�����PID��
	byszReceiveData��		��ȡ��˽������
	wLen��					��ȡ����˽�����ݵĳ���
--*/
void DVTCASTB_StreamGuardDataGot(BYTE byType, WORD wPID, BYTE byLen, const BYTE* byszReceiveData);

/*++
���ܣ������ͨ��Ȩ��Ŀ���������������ڻ����н�������ʾ��
������
	wTVSID:					��Ӫ�̵��ڲ���š�
	pEntitleCount:			����ʱΪ����������������ʱΪʵ�ʻ�õĸ�����
	psEntitles:				���ص���Ȩ���顣						
����ֵ��
	DVTCA_OK					�ɹ�
	DVTCAERR_STB_TVS_NOT_FOUND	û���ҵ�����Ҫ�����Ӫ��
	����						ʧ��
--*/
HRESULT DVTCASTB_GetServiceEntitles(WORD wTvsId, BYTE *pEntitleCount,SDVTCAServiceEntitle* psEntitles);

/*++
���ܣ��򿪻�رյ�����Ϣ��
������
	bDebugMsgSign:			�򿪵�����Ϣ��־��1:�򿪵�����Ϣ;0:�رյ�����Ϣ��
--*/
void DVTCASTB_AddDebugMsgSign(bool bDebugMsgSign);

#ifdef  __cplusplus
}
#endif
#endif  

