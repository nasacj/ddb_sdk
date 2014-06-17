// CA_STB_DEMO.h : main header file for the CA_STB_DEMO application
//

#if !defined(AFX_CA_STB_DEMO_H__147D5D3F_252D_43A4_B06D_A567632798D1__INCLUDED_)
#define AFX_CA_STB_DEMO_H__147D5D3F_252D_43A4_B06D_A567632798D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCA_STB_DEMOApp:
// See CA_STB_DEMO.cpp for the implementation of this class
//

class CCA_STB_DEMOApp : public CWinApp
{
public:
	CCA_STB_DEMOApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCA_STB_DEMOApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCA_STB_DEMOApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CA_STB_DEMO_H__147D5D3F_252D_43A4_B06D_A567632798D1__INCLUDED_)
