
// DlgProxy.cpp : 实现文件
//

#include "stdafx.h"
#include "MiniDebugger.h"
#include "DlgProxy.h"
#include "MiniDebuggerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMiniDebuggerDlgAutoProxy

IMPLEMENT_DYNCREATE(CMiniDebuggerDlgAutoProxy, CCmdTarget)

CMiniDebuggerDlgAutoProxy::CMiniDebuggerDlgAutoProxy()
{
	EnableAutomation();
	
	// 为使应用程序在自动化对象处于活动状态时一直保持 
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CMiniDebuggerDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMiniDebuggerDlg)))
		{
			m_pDialog = reinterpret_cast<CMiniDebuggerDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CMiniDebuggerDlgAutoProxy::~CMiniDebuggerDlgAutoProxy()
{
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	// 	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CMiniDebuggerDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CMiniDebuggerDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMiniDebuggerDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IMiniDebugger 的支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {C52201EB-DF98-488E-A102-3DBDD26CCE56}
static const IID IID_IMiniDebugger =
{ 0xC52201EB, 0xDF98, 0x488E, { 0xA1, 0x2, 0x3D, 0xBD, 0xD2, 0x6C, 0xCE, 0x56 } };

BEGIN_INTERFACE_MAP(CMiniDebuggerDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CMiniDebuggerDlgAutoProxy, IID_IMiniDebugger, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏在此项目的 StdAfx.h 中定义
// {5B6EF2E1-EAE5-41CE-9BC7-8F2AD2E42F5D}
IMPLEMENT_OLECREATE2(CMiniDebuggerDlgAutoProxy, "MiniDebugger.Application", 0x5b6ef2e1, 0xeae5, 0x41ce, 0x9b, 0xc7, 0x8f, 0x2a, 0xd2, 0xe4, 0x2f, 0x5d)


// CMiniDebuggerDlgAutoProxy 消息处理程序
