// DAB_PLAYERDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../CI_Player/CI_Player.h"
#include "DAB_PLAYER.h"
#include "DAB_PLAYERDlg.h"
#include "../dabsdk/ddbsdk_if.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool deviceOpen;
CI_Player *ci_player = NULL;
BOOL volatile isGetSignalValueThreadRunning = TRUE;
BOOL volatile isStartDeviceThreadRunning = TRUE;
/*CFile m_inputfile;*/

long GetStreamData(void * wParam, void * lParam, void * bHandled, void* pvContext)
{
	DWORD bytesReturn;
	unsigned char*  pECMBuff = (unsigned char*)lParam;
	ddbif_threadGetTsData(pECMBuff, *((PDWORD)bHandled), &bytesReturn);
	//TRACE(TEXT("...........CallBack...........\n"));
	TRACE(TEXT("...........CallBack...........%ld\n"),bytesReturn);
	Sleep(100);//�����ݲ�Ҫ̫����
	return bytesReturn;
}

unsigned long _threadGetSignalValue(void * Pram)
{
	TRACE(L"�����ź�ǿ��Thread.....START\n");
	CDialog *dialog_H = (CDialog *)Pram;
	int signalValue = 0;

	HBITMAP m_hBitmap;
	CStatic* staticId;
	staticId = (CStatic *)dialog_H->GetDlgItem(IDC_STATIC_SIGNAL);
	while (isGetSignalValueThreadRunning)
	{
		signalValue = ddbif_getSignalValue();
		switch(signalValue)
		{
		case 0:

			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SIGNAL0));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 1:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SIGNAL1));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 2:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SIGNAL2));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 3:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SIGNAL3));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 4:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SIGNAL4));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 5:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SIGNAL5));
			staticId->SetBitmap(m_hBitmap);
			break;
		}
		Sleep(2000);
	}
	TRACE(L"�����ź�ǿ��Thread.....EXIT\n");
	return 0;
}

// CDAB_PLAYERDlg �Ի���

CDAB_PLAYERDlg::CDAB_PLAYERDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDAB_PLAYERDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDAB_PLAYERDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDAB_PLAYERDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(PIC_EXIT, &CDAB_PLAYERDlg::OnBnClickExit)
	ON_STN_CLICKED(PIC_EXIT, &CDAB_PLAYERDlg::OnStnClickedExit)
	ON_STN_CLICKED(IDC_CCTV2, &CDAB_PLAYERDlg::OnStnClickedCctv2)
	ON_STN_CLICKED(IDC_DYCJ2, &CDAB_PLAYERDlg::OnStnClickedDycj2)
	ON_STN_CLICKED(VOL_UP, &CDAB_PLAYERDlg::OnStnClickedUp)
	ON_STN_CLICKED(VOL_DOWN, &CDAB_PLAYERDlg::OnStnClickedDown)
	ON_STN_CLICKED(PIC_VIDEO, &CDAB_PLAYERDlg::OnStnClickedVideo)
	ON_STN_CLICKED(PIC_AUDIO, &CDAB_PLAYERDlg::OnStnClickedAudio)
	ON_STN_CLICKED(IDC_STATIC_VIDEO, &CDAB_PLAYERDlg::OnStnClickedStaticVideo)
	ON_STN_CLICKED(IDC_STATIC_SCREEN, &CDAB_PLAYERDlg::OnStnClickedStaticScreen)
END_MESSAGE_MAP()


// CDAB_PLAYERDlg ��Ϣ�������

BOOL CDAB_PLAYERDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	GetDlgItem(IDC_STATIC_SCREEN)->GetWindowRect(&video_rect);
	video_rect.left = 4;
	video_rect.top = 40;
	video_rect.right = 271;
	video_rect.bottom = 224;
	currentUnit = VIDEO;
	isFullScreen = FALSE;
	currentProm = NONE;

	HBITMAP m_hBitmap;
	CStatic* MenuAudio = (CStatic *)GetDlgItem(PIC_AUDIO);
	CStatic* MenuVideo = (CStatic *)GetDlgItem(PIC_VIDEO);
	CStatic* VideoScreen = (CStatic *)GetDlgItem(IDC_STATIC_SCREEN);


	//��Ƶ��ť��
	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_MENU_VIDEO2));
	MenuVideo->SetBitmap(m_hBitmap);

	//��Ƶ��ť��
	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_MENU_AUDIO));
	MenuAudio->SetBitmap(m_hBitmap);

	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN0));
	VideoScreen->SetBitmap(m_hBitmap);
	
#if 1
	//����DAB
	if(ddbif_register() == DDBIF_ERROR) {
		MessageBox(TEXT("ע��DDBSDKʧ�ܣ�"));
		printf("register error!\n");
		return TRUE;
	}
	ddbif_openDdbsdk();
	
	//������DMB�豸���߳�
	statrDeviceThread = CreateThread(NULL, 0, ddif_ThreadStartDMBDevice, NULL, 0, &statrDeviceThread_ID);

	//���ò�����
	ci_player = new CI_Player;
	if (ci_player->InitialPlayer(GetStreamData))
	{
		MessageBox(TEXT("��������ʼ��ʧ�ܣ�"));
	}
	/*m_nVolume = ci_player->GetVolumeValue();*/
	ChangVolumeView(48);
	m_nVolume = 48;

	//�����ź����õ��߳�
	getSignalValueThread = CreateThread(NULL, 0, _threadGetSignalValue, (LPVOID)this, 0, &getSignalValueThread_ID);

// 	if (!deviceOpen)
// 	{
// 		MessageBox(TEXT("DAB�豸����ʧ�ܣ�"));
// 		return TRUE;
// 	}
#endif

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CDAB_PLAYERDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_DAB_PLAYER_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_DAB_PLAYER_DIALOG));
	}
}
#endif


void CDAB_PLAYERDlg::OnStnClickedExit()
{
	// TODO: Add your control notification handler code here
	isGetSignalValueThreadRunning = FALSE;
	isStartDeviceThreadRunning = FALSE;
// 	HANDLE threads[2] = {0};
// 	threads[0] = getSignalValueThread;
// 	threads[1] = statrDeviceThread;
// 	WaitForMultipleObjects(2, threads, TRUE, INFINITE);
	WaitForSingleObject(getSignalValueThread, INFINITE);
	WaitForSingleObject(statrDeviceThread, INFINITE);

	if (ci_player)
	{
		ci_player->DeInitialPlayer();
		delete ci_player;
		ci_player = NULL;
	}

	Sleep(100);

	ddbif_StopVideoService();
	ddbif_closeDmbDevice();
	ddbif_closeDdbsdk();

	TRACE(L"�˳���\n");
	CDialog::DestroyWindow();
	//CDialog::OnClose();
}


void CDAB_PLAYERDlg::OnStnClickedCctv2()
{
	// TODO: Add your control notification handler code here

	HBITMAP m_hBitmap;
	CStatic* staticId = (CStatic *)GetDlgItem(IDC_CCTV2);
	CStatic* staticIdFormer = (CStatic *)GetDlgItem(IDC_DYCJ2);
	CStatic* videoStatic = (CStatic *)GetDlgItem(IDC_STATIC_VIDEO);

	if (VIDEO == currentUnit)
	{

		if (CCTV5 == currentProm)
		{
			return;
		}

		ci_player->ResetPlayer();

		//������Ϊ�������С�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
		videoStatic->SetBitmap(m_hBitmap);

		//����CCTV-5��ͼ��
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_CCTV1));
		staticId->SetBitmap(m_hBitmap);

		//����һ�ƾ�ͼ��Ļ�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_DYCJ2));
		staticIdFormer->SetBitmap(m_hBitmap);

		if (ddbif_startVideoService(3))
		{
			//MessageBox(TEXT("������Ŀʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_CCTV2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}

		//m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
		//videoStatic->SetBitmap(m_hBitmap);

		Sleep(1000);

		RECT rcDisplay = {0};
		GetDlgItem(IDC_STATIC_VIDEO)->GetWindowRect(&rcDisplay);
		/*if (ci_player->PlayDMBFlow(rcDisplay.left, rcDisplay.top, rcDisplay.right, rcDisplay.bottom, TEXT("dmb")))*/
		if (ci_player->PlayDMBFlow(4, 40, 271, 224, TEXT("dmb")))
		{
			ddbif_StopVideoService();
			//MessageBox(TEXT("����ʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_CCTV2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}
		currentProm = CCTV5;

	}
	else
	{

		if (ENEWS == currentProm)
		{
			return;
		}

		ci_player->ResetPlayer();

		//������Ϊ�������С�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
		videoStatic->SetBitmap(m_hBitmap);

		//ENEWS��ɫ
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_ENEWS1));
		staticId->SetBitmap(m_hBitmap);

		//���Ϲ㽻ͨͼ��Ļ�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_SGJT2));
		staticIdFormer->SetBitmap(m_hBitmap);

		if (ddbif_startVideoService(0))
		{
			//MessageBox(TEXT("������Ŀʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_ENEWS2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}

// 		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
// 		videoStatic->SetBitmap(m_hBitmap);

		Sleep(1000);

		RECT rcDisplay = {0};
		GetDlgItem(IDC_STATIC_VIDEO)->GetWindowRect(&rcDisplay);
		/*if (ci_player->PlayDMBFlow(rcDisplay.left, rcDisplay.top, rcDisplay.right, rcDisplay.bottom, TEXT("dab")))*/
		if (ci_player->PlayDMBFlow(4, 40, 271, 224, TEXT("dab")))
		{
			ddbif_StopVideoService();
			//MessageBox(TEXT("����ʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_ENEWS2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}
		currentProm = ENEWS;
	}
	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN0));
	videoStatic->SetBitmap(m_hBitmap);
}

void CDAB_PLAYERDlg::OnStnClickedDycj2()
{
	// TODO: Add your control notification handler code here

	HBITMAP m_hBitmap;
	CStatic* staticId = (CStatic *)GetDlgItem(IDC_DYCJ2);
	CStatic* staticIdFormer = (CStatic *)GetDlgItem(IDC_CCTV2);
	CStatic* videoStatic = (CStatic *)GetDlgItem(IDC_STATIC_VIDEO);

	if (VIDEO == currentUnit)
	{
		if (DYCJ == currentProm)
		{
			return;
		}
		
		ci_player->ResetPlayer();

		//��������ĻΪ�������С�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
		videoStatic->SetBitmap(m_hBitmap);

		//���õ�һ�ƾ���ͼ��
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_DYCJ1));
		staticId->SetBitmap(m_hBitmap);

		//��cctv5ͼ��Ļ�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_CCTV2));
		staticIdFormer->SetBitmap(m_hBitmap);

		if (ddbif_startVideoService(2))
		{
			//MessageBox(TEXT("������Ŀʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_DYCJ2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}

		//m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
		//videoStatic->SetBitmap(m_hBitmap);

		Sleep(1000);

		RECT rcDisplay = {0};
		GetDlgItem(IDC_STATIC_VIDEO)->GetWindowRect(&rcDisplay);
		/*if (ci_player->PlayDMBFlow(rcDisplay.left, rcDisplay.top, rcDisplay.right, rcDisplay.bottom, TEXT("dmb")))*/
		if (ci_player->PlayDMBFlow(4, 40, 271, 224, TEXT("dmb")))
		{
			ddbif_StopVideoService();
			//MessageBox(TEXT("����ʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_DYCJ2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}

		currentProm = DYCJ;

	}

	//������Ƶ
	else
	{
		if (SGJT == currentProm)
		{
			return;
		}

		ci_player->ResetPlayer();

		//��������ĻΪ�������С�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
		videoStatic->SetBitmap(m_hBitmap);

		//�Ϲ㽻ͨ��ɫ
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_SGJT1));
		staticId->SetBitmap(m_hBitmap);

		//��ENEWSͼ��Ļ�
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_ENEWS2));
		staticIdFormer->SetBitmap(m_hBitmap);

		if (ddbif_startVideoService(1))
		{
			//MessageBox(TEXT("������Ŀʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_SGJT2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}

// 		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN1));
// 		videoStatic->SetBitmap(m_hBitmap);

		Sleep(1000);

		RECT rcDisplay = {0};
		GetDlgItem(IDC_STATIC_VIDEO)->GetWindowRect(&rcDisplay);
		/*if (ci_player->PlayDMBFlow(rcDisplay.left, rcDisplay.top, rcDisplay.right, rcDisplay.bottom, TEXT("dab")))*/
		if (ci_player->PlayDMBFlow(4, 40, 271, 224, TEXT("dab")))
		{
			ddbif_StopVideoService();
			//MessageBox(TEXT("����ʧ�ܣ�"));
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN2));
			videoStatic->SetBitmap(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_SGJT2));
			staticId->SetBitmap(m_hBitmap);
			currentProm = NONE;
			//��ǰ��ĿΪ��
			return;
		}
		currentProm = SGJT;
	}

	

	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_SCREEN0));
	videoStatic->SetBitmap(m_hBitmap);
}

void CDAB_PLAYERDlg::OnStnClickedUp()
{
	// TODO: Add your control notification handler code here
	m_nVolume += 6;
	if(m_nVolume > 100)
	{
		m_nVolume = 100;
	}
	if (ci_player)
	{
		ci_player->SetVolumeValue(m_nVolume);
	}
	ChangVolumeView(m_nVolume);
}

void CDAB_PLAYERDlg::OnStnClickedDown()
{
	// TODO: Add your control notification handler code here
	if (100 == m_nVolume)
	{
		m_nVolume = 96;
	}
	m_nVolume -= 6;
	if(m_nVolume < 0)
	{
		m_nVolume = 0;
	}
	if (ci_player)
	{
		ci_player->SetVolumeValue(m_nVolume);
	}
	ChangVolumeView(m_nVolume);
}

void CDAB_PLAYERDlg::ChangVolumeView(int nVolume)
{
	HBITMAP m_hBitmap;
	CStatic* staticId;
	staticId = (CStatic *)GetDlgItem(IDC_STATIC_VOLUME);
	switch(nVolume)
	{
		case 0:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR00));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 6:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR01));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 12:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR02));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 18:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR03));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 24:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR04));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 30:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR05));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 36:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR06));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 42:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR07));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 48:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR08));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 54:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR09));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 60:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR10));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 66:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR11));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 72:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR12));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 78:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR13));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 84:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR14));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 90:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR15));
			staticId->SetBitmap(m_hBitmap);
			break;
		case 100:
			m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_VOL_BAR16));
			staticId->SetBitmap(m_hBitmap);
			break;
	}
}

void CDAB_PLAYERDlg::OnStnClickedVideo()
{
	// TODO: Add your control notification handler code here

	HBITMAP m_hBitmap;
	CStatic* MenuAudio = (CStatic *)GetDlgItem(PIC_AUDIO);
	CStatic* MenuVideo = (CStatic *)GetDlgItem(PIC_VIDEO);

	//��Ƶ��ť��
	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_MENU_VIDEO2));
	MenuVideo->SetBitmap(m_hBitmap);

	//��Ƶ��ť��
	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_MENU_AUDIO));
	MenuAudio->SetBitmap(m_hBitmap);


	if (VIDEO == currentUnit)
	{
		return;
	}
	CStatic* staticId = (CStatic *)GetDlgItem(IDC_CCTV2);

	if (currentProm == CCTV5)
	{
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_CCTV1));
		staticId->SetBitmap(m_hBitmap);
		staticId = (CStatic *)GetDlgItem(IDC_DYCJ2);
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_DYCJ2));
		staticId->SetBitmap(m_hBitmap);
	}
	//��ǰ��ĿΪCCTV5������CCTV5ͼ��

	else if(currentProm == DYCJ)
	{
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_CCTV2));
		staticId->SetBitmap(m_hBitmap);
		staticId = (CStatic *)GetDlgItem(IDC_DYCJ2);
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_DYCJ1));
		staticId->SetBitmap(m_hBitmap);
	}
	//��ǰ��ĿΪDYCJ������DYCJͼ��

	else
	{
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_CCTV2));
		staticId->SetBitmap(m_hBitmap);
		staticId = (CStatic *)GetDlgItem(IDC_DYCJ2);
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_DYCJ2));
		staticId->SetBitmap(m_hBitmap);
	}
	//��ǰ��ĿΪ�գ���������Ƶ�κ�ͼ��
	currentUnit = VIDEO;
}

void CDAB_PLAYERDlg::OnStnClickedAudio()
{
	// TODO: Add your control notification handler code here

	HBITMAP m_hBitmap;
	CStatic* MenuAudio = (CStatic *)GetDlgItem(PIC_AUDIO);
	CStatic* MenuVideo = (CStatic *)GetDlgItem(PIC_VIDEO);

	//��Ƶ��ť��
	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_MENU_VIDEO));
	MenuVideo->SetBitmap(m_hBitmap);

	//��Ƶ��ť��
	m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_MENU_AUDIO2));
	MenuAudio->SetBitmap(m_hBitmap);

	if (AUDIO == currentUnit)
	{
		return;
	}
	CStatic* staticId = (CStatic *)GetDlgItem(IDC_CCTV2);

	if (currentProm == SGJT)
	{
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_ENEWS2));
		staticId->SetBitmap(m_hBitmap);
		staticId = (CStatic *)GetDlgItem(IDC_DYCJ2);
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_SGJT1));
		staticId->SetBitmap(m_hBitmap);
	}
	//��ǰ��ĿΪSGJT������SGJTͼ��

	else if(currentProm == ENEWS)
	{
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_ENEWS1));
		staticId->SetBitmap(m_hBitmap);
		staticId = (CStatic *)GetDlgItem(IDC_DYCJ2);
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_SGJT2));
		staticId->SetBitmap(m_hBitmap);
	}
	//��ǰ��ĿΪENEWS������ENEWSͼ��

	else
	{
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_ENEWS2));
		staticId->SetBitmap(m_hBitmap);
		staticId = (CStatic *)GetDlgItem(IDC_DYCJ2);
		m_hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE  (IDB_CHAN_SGJT2));
		staticId->SetBitmap(m_hBitmap);
	}
	//��ǰ��ĿΪ�գ���������Ƶ�κ�ͼ��

	currentUnit = AUDIO;
}

void CDAB_PLAYERDlg::OnStnClickedStaticVideo()
{
	// TODO: Add your control notification handler code here
	TRACE(L"PIC\n");
	if (currentProm != CCTV5 && currentProm != DYCJ)
	{
		return;
	}
	RECT temprect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_SCREEN);
	if (!isFullScreen)
	{
		SetRect(&temprect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		::MoveWindow(pWnd->GetSafeHwnd(), temprect.left, temprect.top, temprect.right, temprect.bottom, TRUE);
		::SetWindowPos(pWnd->GetSafeHwnd(), HWND_TOPMOST,temprect.left, temprect.top, temprect.right, temprect.bottom, SWP_SHOWWINDOW);
		if (ci_player)
		{
			ci_player->SetScreen(temprect.left, temprect.top, temprect.right, temprect.bottom);
		}
		
		isFullScreen = TRUE;
	}
	else
	{
		::MoveWindow(pWnd->GetSafeHwnd(), video_rect.left, video_rect.top, video_rect.right, video_rect.bottom, TRUE);
		::SetWindowPos(pWnd->GetSafeHwnd(), HWND_BOTTOM,temprect.left, temprect.top, temprect.right, temprect.bottom, SWP_SHOWWINDOW);
		if (ci_player)
		{
			ci_player->SetScreen(video_rect.left, video_rect.top, video_rect.right, video_rect.bottom);
		}
		
		isFullScreen = FALSE;
	}
}




void CDAB_PLAYERDlg::OnStnClickedStaticScreen()
{
	// TODO: Add your control notification handler code here
	TRACE(L"STATIC\n");
	if (currentProm != CCTV5 && currentProm != DYCJ)
	{
		return;
	}

	RECT temprect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_SCREEN);
	if (!isFullScreen)
	{
		SetRect(&temprect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		::MoveWindow(pWnd->GetSafeHwnd(), temprect.left, temprect.top, temprect.right, temprect.bottom, TRUE);
		::SetWindowPos(pWnd->GetSafeHwnd(), HWND_TOPMOST,temprect.left, temprect.top, temprect.right, temprect.bottom, SWP_SHOWWINDOW);
		if (ci_player)
		{
			ci_player->SetScreen(temprect.left, temprect.top, temprect.right, temprect.bottom);
		}

		isFullScreen = TRUE;
	}
	else
	{
		::MoveWindow(pWnd->GetSafeHwnd(), video_rect.left, video_rect.top, video_rect.right, video_rect.bottom, TRUE);
		::SetWindowPos(pWnd->GetSafeHwnd(), HWND_BOTTOM,temprect.left, temprect.top, temprect.right, temprect.bottom, SWP_SHOWWINDOW);
		if (ci_player)
		{
			ci_player->SetScreen(video_rect.left, video_rect.top, video_rect.right, video_rect.bottom);
		}

		isFullScreen = FALSE;
	}
}

BOOL CDAB_PLAYERDlg::isDeviceStarted()
{
	DWORD status = WaitForSingleObject(this->statrDeviceThread,0);
	if (deviceOpen == true && status == WAIT_OBJECT_0)
	{
		return TRUE;
	}
	return FALSE;
}