// CA_STB_DEMO.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CA_STB_DEMO.h"
#include "CA_STB_DEMODlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCA_STB_DEMOApp

BEGIN_MESSAGE_MAP(CCA_STB_DEMOApp, CWinApp)
	//{{AFX_MSG_MAP(CCA_STB_DEMOApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCA_STB_DEMOApp construction

CCA_STB_DEMOApp::CCA_STB_DEMOApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCA_STB_DEMOApp object

CCA_STB_DEMOApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCA_STB_DEMOApp initialization

BOOL CCA_STB_DEMOApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CCA_STB_DEMODlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
