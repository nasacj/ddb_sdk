// PidFilter.h : main header file for the PIDFILTER application
//

#if !defined(AFX_PIDFILTER_H__F683942E_8710_4649_BD96_BD0ECF3E8477__INCLUDED_)
#define AFX_PIDFILTER_H__F683942E_8710_4649_BD96_BD0ECF3E8477__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPidFilterApp:
// See PidFilter.cpp for the implementation of this class
//

class CPidFilterApp : public CWinApp
{
public:
	CPidFilterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPidFilterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPidFilterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIDFILTER_H__F683942E_8710_4649_BD96_BD0ECF3E8477__INCLUDED_)
