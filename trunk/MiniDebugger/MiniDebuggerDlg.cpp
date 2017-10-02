
// MiniDebuggerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MiniDebugger.h"
#include "MiniDebuggerDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#include "Interface.h"
#include "logic/StartDebugThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern CString g_csCommandText;



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


// CMiniDebuggerDlg �Ի���


IMPLEMENT_DYNAMIC(CMiniDebuggerDlg, CDialogEx);

CMiniDebuggerDlg::CMiniDebuggerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMiniDebuggerDlg::IDD, pParent)
	, m_CommandText(_T(""))
	, m_MainEdit(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CMiniDebuggerDlg::~CMiniDebuggerDlg()
{
	// ����öԻ������Զ���������
	//  ���˴���ָ��öԻ���ĺ���ָ������Ϊ NULL���Ա�
	//  �˴���֪���öԻ����ѱ�ɾ����
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CMiniDebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_MainMsg);
	DDX_Text(pDX, IDC_EDIT2, m_CommandText);
	DDX_Text(pDX, IDC_EDIT1, m_MainEdit);
}

BEGIN_MESSAGE_MAP(CMiniDebuggerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMiniDebuggerDlg ��Ϣ�������

BOOL CMiniDebuggerDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//ShowWindow(SW_MAXIMIZE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	m_MainEdit.SetString(L"��ӭʹ��MiniDebugger!\r\n���Ĵ����ַ:http://code.taobao.org/p/MiniDebuger/src/ \r\n\r\n\r\n");

	this->SetDlgItemTextW(IDC_EDIT1,m_MainEdit);//��m_MainEdit������ʾ��IDΪIDC_EDIT1�ı༭��
	m_MainMsg.LineScroll(m_MainMsg.GetLineCount());//m_MainMsgΪ�ñ༭���control����

	//���������߳�
	HANDLE handle = CreateThread(NULL, 0, StartDebugThread, NULL, 0, NULL); 

	SetTimer(TIMER_1,50,0);



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMiniDebuggerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMiniDebuggerDlg::OnPaint()
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
HCURSOR CMiniDebuggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ���û��ر� UI ʱ������������Ա���������ĳ��
//  �������Զ�����������Ӧ�˳�����Щ
//  ��Ϣ�������ȷ����������: �����������ʹ�ã�
//  ������ UI�������ڹرնԻ���ʱ��
//  �Ի�����Ȼ�ᱣ�������

void CMiniDebuggerDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CMiniDebuggerDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CMiniDebuggerDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CMiniDebuggerDlg::CanExit()
{
	// �����������Ա�����������Զ���
	//  �������Իᱣ�ִ�Ӧ�ó���
	//  ʹ�Ի���������������� UI ����������
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

BOOL CMiniDebuggerDlg::PreTranslateMessage(MSG* pMsg)  
{  
	if(WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)     
	{       
		if(GetFocus()== GetDlgItem(IDC_EDIT1))  //���ݲ�ͬ�ؼ������ж����Ǹ���ִ��  
		{  
			//UpdateData(TRUE);

			//dosomething...  


		}  

		if(GetFocus()== GetDlgItem(IDC_EDIT2))  //���ݲ�ͬ�ؼ������ж����Ǹ���ִ��  
		{  
			UpdateData(true);  

			CString CommandText = m_CommandText;

			CommandText.MakeLower();//��ΪСд
			CommandParsing(CommandText);

			m_CommandText="";
			UpdateData(false);

			CommandText =L">"+CommandText+L"\r\n";
			int  nLength  =  m_MainMsg.SendMessage(WM_GETTEXTLENGTH);
			m_MainMsg.SetSel(nLength, nLength);
			m_MainMsg.ReplaceSel(CommandText);


		}

	}  

	return 0; //������뷵��0 �������б���  
} 



bool CMiniDebuggerDlg::InterfaceOfDisplay(CString CommandText)
{
	int  nLength  =  m_MainMsg.SendMessage(WM_GETTEXTLENGTH);
	m_MainMsg.SetSel(nLength, nLength);
	m_MainMsg.ReplaceSel(CommandText);

	return true;
}

void CMiniDebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	switch(nIDEvent)
	{
	case TIMER_1:
		{ 

			if(L""!=g_csCommandText)
			{
				InterfaceOfDisplay(g_csCommandText);

				g_csCommandText	= L"";

			}
			break;
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}
