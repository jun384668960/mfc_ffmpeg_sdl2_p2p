
// mfc_clientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfc_client.h"
#include "mfc_clientDlg.h"
#include "afxdialogex.h"
#include "ConnHandle.h"  
#include <SDL.h>
#include "Log.h"
#include "GenericDecode.h"
#pragma comment(lib ,"sdl2.lib")  

/////////////////////////////////////////////////////
//SDL_Window* gWindow = NULL;
//SDL_Surface* gScreenSurface = NULL;
SDL_Renderer* gSdlRenderer = NULL;
SDL_Texture* gSdlTexture = NULL;
ConnHandle* gConnHandle = NULL;
ConnBase* gGssHandle = NULL;
GenericDecode* gGenericDecode = NULL;
bool gLogPause = false;
int gP2pClientId = 0;
/////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cmfc_clientDlg �Ի���



Cmfc_clientDlg::Cmfc_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmfc_clientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmfc_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cmfc_clientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &Cmfc_clientDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_LOGCLEAN, &Cmfc_clientDlg::OnBnClickedButtonLogclean)
	ON_BN_CLICKED(IDC_BUTTON_LOGPAUSE, &Cmfc_clientDlg::OnBnClickedButtonLogpause)
	ON_CBN_SELCHANGE(IDC_COMBO_CONNMODE, &Cmfc_clientDlg::OnSelchangeComboConnmode)
	ON_BN_CLICKED(IDC_BUTTON_STARSTREAM, &Cmfc_clientDlg::OnBnClickedButtonStarstream)
	ON_BN_CLICKED(IDC_BUTTON_CONNDEV, &Cmfc_clientDlg::OnBnClickedButtonConndev)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGLEVEL, &Cmfc_clientDlg::OnSelchangeComboLoglevel)
	ON_BN_CLICKED(IDC_BUTTON1, &Cmfc_clientDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// Cmfc_clientDlg ��Ϣ�������

BOOL Cmfc_clientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	ThirdPartInit();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cmfc_clientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cmfc_clientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cmfc_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT LogPrintfProc(LPVOID param)
{
	Cmfc_clientDlg* clientDlg = (Cmfc_clientDlg*)param;
	std::string logLine;
	while (true)
	{
		if (Log::getInstance()->PopLog(logLine))
		{
			if (gLogPause)
			{
				continue;
			}
			CString cstr(logLine.c_str());
			CEdit* pEdit = (CEdit*)clientDlg->GetDlgItem(IDC_EDIT_LOG);
			int nLength = pEdit->GetWindowTextLength();
			pEdit->SetSel(nLength, nLength);
			pEdit->ReplaceSel(cstr);
		}
		else
		{
			Sleep(10);
		}
	}

	return 0;
}

UINT MediaDecodeProc(LPVOID param)
{
	Cmfc_clientDlg* clientDlg = (Cmfc_clientDlg*)param;
	while (true)
	{
		P2pDataFrame frame;
		if (gGssHandle != NULL)
		{
			bool ret = gGssHandle->RecvMediaFrame(frame);
			if (ret)
			{
				bool isVideo = false;
				if (frame.type == gos_audio_AAC)
				{
					isVideo = false;
					gGenericDecode->DelayInitAudioDecode(AV_CODEC_ID_AAC);
				}
				else if (frame.type == gos_audio_G711A)
				{
					isVideo = false;
					gGenericDecode->DelayInitAudioDecode(AV_CODEC_ID_PCM_ALAW);
				}
				else
				{
					isVideo = true;
				}

				LOGT_print("GetP2pDataFrame ok frame.data%p frame.len%d", frame.data, frame.len);
				gGenericDecode->GenericMediaDecode(isVideo, frame.data, frame.len);
			}
			else
			{
				Sleep(10);
			}
		}
		else{
			Sleep(30);
		}
	}
	return 0;
}

void FuncDecodeCallback(AVFrame* pFrame, bool isVideo)
{
	if (isVideo)
	{//��Ƶ
		if (gSdlTexture != NULL && gSdlRenderer != NULL)
		{
			SDL_Rect src = { 0, 0, pFrame->width, pFrame->height };
			SDL_UpdateYUVTexture(gSdlTexture, &src, pFrame->data[0], pFrame->linesize[0],
				pFrame->data[1], pFrame->linesize[1],
				pFrame->data[2], pFrame->linesize[2]);
			SDL_RenderClear(gSdlRenderer);
		
			SDL_RenderCopy(gSdlRenderer, gSdlTexture, &src, NULL);
			SDL_RenderPresent(gSdlRenderer);
		}
	}
	else
	{//��Ƶ

	}
}

void Cmfc_clientDlg::ThirdPartInit()
{
	//Ĭ�ϲ���"", "119.23.128.209", 6000, "A99762101001002"
	SetDlgItemText(IDC_COMBO_DISPATCH, "119.23.128.209:6001");
	SetDlgItemText(IDC_COMBO_STATICSERVER, "119.23.128.209");
	SetDlgItemText(IDC_EDIT_STATICPORT, "6000");
	SetDlgItemText(IDC_EDIT_UID, "A99762000000013");
	SetDlgItemText(IDC_COMBO_CONNMODE, "P2P��͸");
	SetDlgItemText(IDC_COMBO_LOGLEVEL, "Debug");
	((CButton*)GetDlgItem(IDC_CHECK_DISPATCH))->SetCheck(TRUE);

	//P2P��ʼ��
	ConnHandle::GlobleInit();
	p2p_log_set_level(0);
	int maxRecvLen = 1024 * 1024;
	int maxClientCount = 10;
	p2p_set_global_opt(P2P_MAX_CLIENT_COUNT, &maxClientCount, sizeof(int));
	p2p_set_global_opt(P2P_MAX_RECV_PACKAGE_LEN, &maxRecvLen, sizeof(int));

	//����FFMPEG��ʼ��
	GenericDecode::GlobleInit();
	gGenericDecode = new GenericDecode();
	gGenericDecode->GenericInitDecode(AV_CODEC_ID_H264);
	gGenericDecode->GenericSetDisplayCallback(FuncDecodeCallback);
	CWinThread* m_tdDecode = AfxBeginThread(MediaDecodeProc, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

	//��־�߳�
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LOG);
	pEdit->SetLimitText(1000000);
	CWinThread* m_tdDispatch = AfxBeginThread(LogPrintfProc, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

	//���ȳ�ʼ��  
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOGE_print("Window could not be created! SDL_Error: %s", SDL_GetError());
		return;
	}

	//��������  
	SDL_Window* gWindow = SDL_CreateWindowFrom((void *)(GetDlgItem(IDC_PIC)->GetSafeHwnd()));
	if (gWindow == NULL)
	{
		LOGE_print("Window could not be created! SDL_Error: %s", SDL_GetError());
		return;
	}

	//gScreenSurface = SDL_GetWindowSurface(gWindow);
	//��ʾ
	//SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
	//SDL_UpdateWindowSurface(gWindow);
	gSdlRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	gSdlTexture = SDL_CreateTexture(gSdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING
		, 1920, 1080);
}

void Cmfc_clientDlg::OnBnClickedButtonPlay()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	bool ret;
	CString playStatu;
	CString playMode;
	GetDlgItemText(IDC_BUTTON_PLAY, playStatu);
	GetDlgItemText(IDC_COMBO_CONNMODE, playMode);

	if (playStatu == "���ӷ���")
	{
		//��ȡ����
		CString csDispatch;
		GetDlgItemText(IDC_COMBO_DISPATCH, csDispatch);
		CString csServer;
		GetDlgItemText(IDC_COMBO_STATICSERVER, csServer);
		CString csPort;
		GetDlgItemText(IDC_EDIT_STATICPORT, csPort);
		CString csUid;
		GetDlgItemText(IDC_EDIT_UID, csUid);

		int mode;
		if (playMode == "TCPһ�Զ�")
		{
			mode = CONN_TYPE_TCP_1VN_PULL;
		}
		else if (playMode == "P2P��͸")
		{
			mode = CONN_TYPE_P2P_1V1;
		}
		gGssHandle = ConnHandle::CreateNewConn(mode);
		if (gGssHandle == NULL)
		{
			LOGE_print("CreateNewConn type:%d ", mode);
			exit(-1);
		}

		//����Զ�̷�����
		ret = gGssHandle->Init((char*)csDispatch.GetBuffer(0), (char*)csServer.GetBuffer(0)
			, atoi((char*)csPort.GetBuffer(0)), (char*)csUid.GetBuffer(0), "", 0);
		if (!ret)
		{
			LOGE_print("ConnGssPull Init error");
			delete gGssHandle;
			gGssHandle = NULL;
			return;
		}

		bool useDispatch = false;
		if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_DISPATCH))
			useDispatch = true;

		if (mode == CONN_TYPE_TCP_1VN_PULL)
		{
			ConnGssPull* gssPull = (ConnGssPull*)gGssHandle;
			gssPull->ServerConnect(true, useDispatch);
		} 
		else
		{
			ConnGssP2p* gssP2p = (ConnGssP2p*)gGssHandle;
			gssP2p->ServerConnect(true, useDispatch);
		}

		SetDlgItemText(IDC_BUTTON_PLAY, "�Ͽ�����");
	}
	else if(playStatu == "�Ͽ�����")
	{
		if (gGssHandle != NULL)
		{
			gGssHandle->Stop();
			gGssHandle->UnInit();
			delete gGssHandle;
			gGssHandle = NULL;
		}
		SetDlgItemText(IDC_BUTTON_STARSTREAM, "ȡ��");
		SetDlgItemText(IDC_BUTTON_CONNDEV, "�����豸");
		SetDlgItemText(IDC_BUTTON_PLAY, "���ӷ���");
	}
}


void Cmfc_clientDlg::OnBnClickedButtonLogclean()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	SetDlgItemText(IDC_EDIT_LOG, "");
}


void Cmfc_clientDlg::OnBnClickedButtonLogpause()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	gLogPause = true;
	CString logStatu;
	GetDlgItemText(IDC_BUTTON_LOGPAUSE, logStatu);
	if (logStatu == "��ͣ")
	{
		gLogPause = true;
		SetDlgItemText(IDC_BUTTON_LOGPAUSE, "ץȡ");
	} 
	else if (logStatu == "ץȡ")
	{
		gLogPause = false;
		SetDlgItemText(IDC_BUTTON_LOGPAUSE, "��ͣ");
	}
}

void Cmfc_clientDlg::OnSelchangeComboConnmode()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CComboBox* cbx = (CComboBox*)GetDlgItem(IDC_COMBO_CONNMODE);
	int index = cbx->GetCurSel();
	if (index == 1)
	{
		SetDlgItemText(IDC_EDIT_STATICPORT, "6000");
	}
	else if (index == 0)
	{
		SetDlgItemText(IDC_EDIT_STATICPORT, "34780");
	}
}


void Cmfc_clientDlg::OnBnClickedButtonStarstream()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (gGssHandle == NULL)
	{
		LOGE_print("ȡ��ʧ�� gGssHandle:%p", gGssHandle);
		return;
	}

	bool ret;
	CString playMode;
	GetDlgItemText(IDC_COMBO_CONNMODE, playMode);
	if (playMode == "P2P��͸")
	{
		if (!gGssHandle->IsConnected())
		{
			LOGE_print("ȡ��ʧ�� IsConnected��%d", gGssHandle->IsConnected());
			return;
		}
	}

	CString streamStatu;
	GetDlgItemText(IDC_BUTTON_STARSTREAM, streamStatu);
	if (streamStatu == "ȡ��")
	{
		if (playMode == "TCPһ�Զ�")
		{
			ret = gGssHandle->Start();
			if (!ret)
			{
				LOGE_print("ConnGssPull Start error");
				return;
			}
			SetDlgItemText(IDC_BUTTON_STARSTREAM, "�Ͽ�");
		}
		else if (playMode == "P2P��͸")
		{
			//���Ϳ�ʼȡ��ָ��
			ret = gGssHandle->Send(gP2pClientId, CMD_START_VIDEO_START, "start", 0);
			ret = gGssHandle->Send(gP2pClientId, CMD_START_AUDIO_START, "start", 0);
			if (ret)
			{
			SetDlgItemText(IDC_BUTTON_STARSTREAM, "�Ͽ�");
			}
			
		}
	}
	else
	{
		if (playMode == "TCPһ�Զ�")
		{
			gGssHandle->Stop();
			SetDlgItemText(IDC_BUTTON_STARSTREAM, "ȡ��");
		}
		else
		{
			//���Ϳ�ʼ����ָ��
			ret = gGssHandle->Send(gP2pClientId, CMD_START_VIDEO_STOP, "stop", 0);
			ret = gGssHandle->Send(gP2pClientId, CMD_START_AUDIO_STOP, "stop", 0);
			if (ret)
			{
				SetDlgItemText(IDC_BUTTON_STARSTREAM, "ȡ��");
			}
			
		}
		
	}
}


void Cmfc_clientDlg::OnBnClickedButtonConndev()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (gGssHandle == NULL)
	{
		LOGE_print("�����豸ʧ�� gGssHandle:%p", gGssHandle);
		return;
	}

	CString playMode;
	GetDlgItemText(IDC_COMBO_CONNMODE, playMode);
	if (playMode == "TCPһ�Զ�")
	{
		LOGW_print("TCPһ�Զ�ģʽ����Ҫ�����豸��");
	}
	else
	{
		if (!gGssHandle->IsConnected())
		{
			LOGE_print("�����豸ʧ�� IsConnected��%d", gGssHandle->IsConnected());
			return;
		}
		CString csUid;
		ConnGssP2p* gssP2p = (ConnGssP2p*)gGssHandle;

		CString connDev;
		GetDlgItemText(IDC_BUTTON_CONNDEV, connDev);
		if (connDev == "�����豸")
		{
			GetDlgItemText(IDC_EDIT_UID, csUid);
			gP2pClientId = 0;
			int id = gssP2p->CreateNewConnAv((char*)csUid.GetBuffer(0));
			if (id == 0)
			{
				LOGE_print("CreateNewConnAv Start error");
				SetDlgItemText(IDC_BUTTON_CONNDEV, "�����豸");
				return;
			}
			else
			{
				gP2pClientId = id;
				SetDlgItemText(IDC_BUTTON_CONNDEV, "�Ͽ��豸");
				SetDlgItemText(IDC_BUTTON_STARSTREAM, "ȡ��");
			}
		}
		else
		{
			gssP2p->ClearAvClient();
			SetDlgItemText(IDC_BUTTON_CONNDEV, "�����豸");
			SetDlgItemText(IDC_BUTTON_STARSTREAM, "ȡ��");
		}
	}
}


void Cmfc_clientDlg::OnSelchangeComboLoglevel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CComboBox* cbx = (CComboBox*)GetDlgItem(IDC_COMBO_LOGLEVEL);
	int index = cbx->GetCurSel();
	Log::getInstance()->SetLevle(index);
}


void Cmfc_clientDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�����л�������ָ��
	gGssHandle->Send(gP2pClientId, IOTYPE_USER_IPCAM_SETSTREAMCTRL_REQ, "stream ctrl", 0);
}
