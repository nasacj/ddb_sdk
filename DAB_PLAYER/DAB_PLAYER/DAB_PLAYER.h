// DAB_PLAYER.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CDAB_PLAYERApp:
// �йش����ʵ�֣������ DAB_PLAYER.cpp
//

class CDAB_PLAYERApp : public CWinApp
{
public:
	CDAB_PLAYERApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDAB_PLAYERApp theApp;
