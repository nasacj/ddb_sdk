// DAB_PLAYER.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DAB_PLAYER.h"
#include "DAB_PLAYERDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDAB_PLAYERApp

BEGIN_MESSAGE_MAP(CDAB_PLAYERApp, CWinApp)

END_MESSAGE_MAP()


// CDAB_PLAYERApp ����
CDAB_PLAYERApp::CDAB_PLAYERApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDAB_PLAYERApp ����
CDAB_PLAYERApp theApp;

// CDAB_PLAYERApp ��ʼ��

BOOL CDAB_PLAYERApp::InitInstance()
{

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CDAB_PLAYERDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
