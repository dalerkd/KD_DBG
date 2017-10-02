
// MiniDebuggerDlg.cpp : 实现文件
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



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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


// CMiniDebuggerDlg 对话框


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
	// 如果该对话框有自动化代理，则
	//  将此代理指向该对话框的后向指针设置为 NULL，以便
	//  此代理知道该对话框已被删除。
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


// CMiniDebuggerDlg 消息处理程序

BOOL CMiniDebuggerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MAXIMIZE);

	// TODO: 在此添加额外的初始化代码

	m_MainEdit.SetString(L"欢迎使用MiniDebugger!\r\n它的代码地址:http://code.taobao.org/p/MiniDebuger/src/ \r\n\r\n\r\n");

	this->SetDlgItemTextW(IDC_EDIT1,m_MainEdit);//将m_MainEdit内容显示到ID为IDC_EDIT1的编辑框
	m_MainMsg.LineScroll(m_MainMsg.GetLineCount());//m_MainMsg为该编辑框的control变量

	//启动调试线程
	HANDLE handle = CreateThread(NULL, 0, StartDebugThread, NULL, 0, NULL); 

	SetTimer(TIMER_1,50,0);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMiniDebuggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMiniDebuggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 当用户关闭 UI 时，如果控制器仍保持着它的某个
//  对象，则自动化服务器不应退出。这些
//  消息处理程序确保如下情形: 如果代理仍在使用，
//  则将隐藏 UI；但是在关闭对话框时，
//  对话框仍然会保留在那里。

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
	// 如果代理对象仍保留在那里，则自动化
	//  控制器仍会保持此应用程序。
	//  使对话框保留在那里，但将其 UI 隐藏起来。
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
		if(GetFocus()== GetDlgItem(IDC_EDIT1))  //根据不同控件焦点判断是那个在执行  
		{  
			//UpdateData(TRUE);

			//dosomething...  


		}  

		if(GetFocus()== GetDlgItem(IDC_EDIT2))  //根据不同控件焦点判断是那个在执行  
		{  
			UpdateData(true);  

			CString CommandText = m_CommandText;

			CommandText.MakeLower();//改为小写
			CommandParsing(CommandText);

			m_CommandText="";
			UpdateData(false);

			CommandText =L">"+CommandText+L"\r\n";
			int  nLength  =  m_MainMsg.SendMessage(WM_GETTEXTLENGTH);
			m_MainMsg.SetSel(nLength, nLength);
			m_MainMsg.ReplaceSel(CommandText);


		}

	}  

	return 0; //这里必须返回0 否则运行报错  
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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
