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

#define DVTCA_MAXLEN_TVSNAME					21		//���Ӫ�����Ƴ���+\0
#define DVTCA_MAXLEN_TVSDSC						21		//���Ӫ����������+\0
#define DVTCA_MAXLEN_MANUFACTURERNAME			21		//���CAS�������ֳ���+\0 
#define DVTCA_MAXLEN_PRODUCT_NAME				21		//��Ĳ�Ʒ����,���20���ַ�+\0

//Ŀǰֻ֧�ֵ���Ӫ��
#define	DVTCA_MAXNUMBER_TVSID					5		//֧�ֵ���Ӫ��������

//������ȡ˽�����ݵ�����
#define DVTCA_STB_ECM_TYPE						1		
#define DVTCA_STB_EMM_TYPE						2

/*----------------------------------------------------------------------*/
/*                       �����Ƿ���ֵ����                               */
/*----------------------------------------------------------------------*/

//�����з���ֵ��ֵ����
#define DVTCAERR_BASE_STB						0xF1000000

//����ֵ����
#define DVTCA_OK								0 

#define DVTCAERR_STB_POINTER_INVALID			(DVTCAERR_BASE_STB + 1)		//�����ָ����Ч
#define DVTCAERR_STB_DATA_LEN_ERROR				(DVTCAERR_BASE_STB + 2)		//��������ݳ��Ȳ��Ϸ�
#define DVTCAERR_STB_TVS_NOT_FOUND				(DVTCAERR_BASE_STB + 3)		//û���ҵ�����Ҫ�����Ӫ��
#define DVTCAERR_STB_EXCEED_MAX_COUNT			(DVTCAERR_BASE_STB + 4)		//��������������ĸ���
#define DVTCAERR_STB_KEY_NOT_FOUND				(DVTCAERR_BASE_STB + 5)		//û���ҵ�������Կ
#define DVTCAERR_STB_CMD_INVALID				(DVTCAERR_BASE_STB + 8)		//MACУ�����
#define DVTCAERR_STB_VER_EXPIRED				(DVTCAERR_BASE_STB + 9)		//�����Ѿ�����
#define DVTCAERR_STB_EXCEED_MAX_LEN				(DVTCAERR_BASE_STB + 10)	//��Ӫ�̵����Ƴ������ĳ���
#define DVTCAERR_STB_NO_SPACE					(DVTCAERR_BASE_STB + 11)	//�Ѿ�û�пռ�
#define DVTCAERR_STB_COUNT_ISZERO				(DVTCAERR_BASE_STB + 14)	//�����CountΪ0. 
#define	DVTCAERR_STB_OVERFLOW					(DVTCAERR_BASE_STB + 16)	//���
#define DVTCAERR_STB_UNKNOWN					(DVTCAERR_BASE_STB + 19)	//δ֪����

//����ȡ������ʾ��Ϣ�б�
#define	DVTCA_NO_ENTITLE						5			//û�й���˽�Ŀ
#define	DVTCA_PRODUCT_RESTRICT					6			//�ý�Ŀ�������տ�
#define DVTCA_DATA_INVALID						16			//������Ч
#define DVTCA_TVS_NOT_FOUND						19			//��Ӫ�̲�����
#define DVTCA_KEY_NOT_FOUND						20			//��Ӫ����Կ������
#define DVTCA_SC_IS_FORBIDDEN					21			//��������
#define	DVTCA_DECRYPT_FAILURE					0xFF		//��Ŀ�����տ�
	
/*----------------------------------------------------------------------*/
/*                       ������һЩ���ݽṹ����                         */
/*----------------------------------------------------------------------*/

//#pragma pack(push,1)
typedef struct _SDVTCAManuInfo
{
	DWORD	m_dwCardID;											//���ڲ�ID
	DWORD	m_dwSTBCASVer;										//������CASģ��İ汾��
	char	m_szSCCASManuName[DVTCA_MAXLEN_MANUFACTURERNAME];	//CAS��Ӧ�̵�����
}SDVTCAManuInfo;
	
typedef struct _SDVTCATvsInfo
{
	WORD	m_wTVSID; 											//��Ӫ�̵��ⲿ���
	char	m_szTVSName[DVTCA_MAXLEN_TVSNAME];					//��Ӫ������
	char	m_szTVSDsc[DVTCA_MAXLEN_TVSDSC];					//��Ӫ������
	BYTE	m_byServiceStatus;									//��Ӫ�̶Դ˿��ķ���״̬,������
}SDVTCATvsInfo;

typedef struct 
{
	WORD 	m_wProductID;										//��ƷID��
	DWORD	m_tEntitleTime;										//��Ȩʱ�䣬time_t��ʽ��
	DWORD	m_tStartTime;										//��ʼʱ�䣬time_t��ʽ��
	DWORD	m_tEndTime;											//����ʱ�䣬time_t��ʽ��
	char	m_szProductName[DVTCA_MAXLEN_PRODUCT_NAME];			//��Ʒ����
}SDVTCAServiceEntitle;											//��ͨ��Ʒ��Ȩ
//#pragma pack(pop)

//{{define for multithreading
typedef unsigned long DVTCA_Semaphore;
typedef void (*pThreadFunc)(void);
//}}define for multithreading

#ifdef  __cplusplus
}
#endif
#endif	
