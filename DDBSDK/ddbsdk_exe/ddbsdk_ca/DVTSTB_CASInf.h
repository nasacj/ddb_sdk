//���ļ�����Ҫ���ն�ʵ�ֲ��ṩ��CAģ��ʹ�õĽӿڡ�����HRESULT����ֵΪDVTCA_OK����ʾ�ɹ�������ֵ��ʾʧ�ܡ�

#ifndef _DVTSTB_CASINTERFACE_H_2007_11_10_
#define _DVTSTB_CASINTERFACE_H_2007_11_10_

#include "DVTCAS_STBDataPublic.h"

#ifdef  __cplusplus
extern "C" {
#endif 

/*----------------------------------���½ӿ���STB�ṩ��CAS����--------------------------------------------*/

/*++
���ܣ�����UUID���ն�Ψһ��ʶ��Ϣ���磺оƬ���к�,������16�ֽڣ���
������
	pbyUUid��UUID�Ŀ�ʼ��ַ
	pwLen��  ����Ϊ����UUID�ĳ��ȣ����Ϊʵ��UUID�ĳ��ȡ�
--*/

HRESULT DVTSTBCA_GetUUid(BYTE *pbyUUid, unsigned short *pwLen);

/*++
���ܣ���û����з����CAS�Ĵ洢�ռ������ַ�ʹ�С�����ֽ�Ϊ��λ����
������
	ppStartAddr:			�����з����CAS�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	lSize:					��������з����CAS�Ĵ���ռ�Ĵ�С��
--*/
HRESULT DVTSTBCA_GetDataBufferAddr(long* lSize, char ** ppStartAddr);

/*++
���ܣ���ȡ�����ڻ������е���Ϣ��
������
	pStartAddr:				Ҫ��ȡ�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪ��ȡ�������ֵ�����Ϊʵ�ʶ�ȡ�Ĵ�С��
	pData:					���������ݡ�
--*/
HRESULT DVTSTBCA_ReadDataBuffer(const char * pStartAddr, long * plDataLen, BYTE* pData);

/*++
���ܣ�������еĴ洢�ռ�д��Ϣ��
������
	lStartAddr:				Ҫд�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪд�����ݵĳ��ȣ����Ϊд���ʵ�ʳ��ȡ�
	pData:					Ҫд�����ݡ�
--*/
HRESULT DVTSTBCA_WriteDataBuffer(const char * pStartAddr, long * plDataLen, const BYTE* pData);

/*++
���ܣ�CA�����ô˺������ý�����������ǰ���ڼ���һ���ڵ�CW�͸���������
������
	wEcmPID:				CW������ECMPID��
	szOddKey:				��CW�����ݡ�
	szEvenKey:				żCW�����ݡ�
	byKeyLen:				CW�ĳ��ȡ�
--*/
HRESULT DVTSTBCA_SetDescrCW(WORD wEcmPID, BYTE byKeyLen, const BYTE* szOddKey, const BYTE* szEvenKey);

/*++
���ܣ��첽��ʾ�û����ܹۿ���Ŀ����ʾ��Ϣ����������������Ӧʹ�á�
������	
	byMesageNo:				Ҫ��ʾ����Ϣ��š�

  #define	DVTCA_NO_ENTITLE						5			//û�й���˽�Ŀ
  #define	DVTCA_PRODUCT_RESTRICT					6			//�ý�Ŀ�������տ�
  #define	DVTCA_DATA_INVALID						16			//������Ч
  #define	DVTCA_TVS_NOT_FOUND						19			//��Ӫ�̲�����
  #define	DVTCA_KEY_NOT_FOUND						20			//��Ӫ����Կ������
  #define	DVTCA_SC_IS_FORBIDDEN					21			//��������
  #define	DVTCA_DECRYPT_FAILURE					0xFF		//��Ŀ�����տ�
--*/
void   DVTSTBCA_ShowPromptMessage(BYTE byMesageNo);

//������ʾ�Ľ���
void   DVTSTBCA_HidePromptMessage(void);

/*
���ܣ���ӵ�����Ϣ��
����:	
	pszMsg:					������Ϣ��
--*/
void DVTSTBCA_AddDebugMsg(const char * pszMsg);
#ifdef  __cplusplus
}
#endif
#endif  
