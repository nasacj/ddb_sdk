// PidFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PidFilter.h"
#include "PidFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPidFilterDlg dialog

CPidFilterDlg::CPidFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPidFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPidFilterDlg)
	m_nPid = 0;
	m_strFilePath = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPidFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPidFilterDlg)
	DDX_Text(pDX, IDC_EDIT_PID, m_nPid);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_strFilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPidFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CPidFilterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPidFilterDlg message handlers

BOOL CPidFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPidFilterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPidFilterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPidFilterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPidFilterDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	CFile file;
	if(!file.Open(m_strFilePath, CFile::modeRead))
	{
		AfxMessageBox("文件无法打开");
	}
	CFile fileDes;
	if(!fileDes.Open(m_strFilePath+"Enc.Ts", CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("文件无法打开");
	}
	BYTE byszBuf[18800];
	while(int nRead = file.Read(byszBuf, 18800))
	{
		for(int i=0; i<nRead; i+=188)
		{
			WORD wPid = ((byszBuf[i+1]&0x1f) << 8) | byszBuf[i+2];
			if(wPid != (WORD)m_nPid)
				continue;
			fileDes.Write(&byszBuf[i+4], 184);
		}
	}
	file.Close();
	fileDes.Close();
	AfxMessageBox("处理结束");
	//CDialog::OnOK();
}

void CPidFilterDlg::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CFileDialog file(true);
	file.DoModal();
	m_strFilePath = file.GetPathName();
	UpdateData(false);
}
