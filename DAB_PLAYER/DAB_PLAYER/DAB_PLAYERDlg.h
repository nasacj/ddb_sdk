// DAB_PLAYERDlg.h : 头文件
//

#pragma once

// CDAB_PLAYERDlg 对话框
class CDAB_PLAYERDlg : public CDialog
{
// 构造
public:
	CDAB_PLAYERDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DAB_PLAYER_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	int					m_nVolume;
	enum
	{
		VIDEO,
		AUDIO
	}					currentUnit;
	enum
	{
		NONE,
		SGJT,
		ENEWS,
		CCTV5,
		DYCJ
	}					currentProm;
	HANDLE				getSignalValueThread;
	DWORD				getSignalValueThread_ID;
	HANDLE				statrDeviceThread;
	DWORD				statrDeviceThread_ID;
	RECT				video_rect;
	BOOL				isFullScreen;

	void ChangVolumeView(int nVolume);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickExit();
	afx_msg void OnStnClickedExit();
	afx_msg void OnStnDblclickAudio();
	afx_msg void OnStnClickedCctv2();
	afx_msg void OnStnClickedDycj2();
	afx_msg void OnStnClickedUp();
	afx_msg void OnStnClickedDown();
	afx_msg void OnStnClickedVideo();
	afx_msg void OnStnClickedAudio();
	afx_msg void OnStnClickedStaticVideo();
	afx_msg void OnEnChangeEditScreen();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnStnClickedStaticScreen();
	afx_msg BOOL isDeviceStarted();
};
