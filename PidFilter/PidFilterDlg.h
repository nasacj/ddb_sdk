// PidFilterDlg.h : header file
//

#if !defined(AFX_PIDFILTERDLG_H__26C318EC_E32E_488D_AA06_1DEB974CE4BF__INCLUDED_)
#define AFX_PIDFILTERDLG_H__26C318EC_E32E_488D_AA06_1DEB974CE4BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPidFilterDlg dialog

class CPidFilterDlg : public CDialog
{
// Construction
public:
	CPidFilterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPidFilterDlg)
	enum { IDD = IDD_PIDFILTER_DIALOG };
	int		m_nPid;
	CString	m_strFilePath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPidFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPidFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIDFILTERDLG_H__26C318EC_E32E_488D_AA06_1DEB974CE4BF__INCLUDED_)
