
// DlgProxy.cpp : ʵ���ļ�
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
	
	// ΪʹӦ�ó������Զ��������ڻ״̬ʱһֱ���� 
	//	���У����캯������ AfxOleLockApp��
	AfxOleLockApp();

	// ͨ��Ӧ�ó����������ָ��
	//  �����ʶԻ������ô�����ڲ�ָ��
	//  ָ��Ի��򣬲����öԻ���ĺ���ָ��ָ��
	//  �ô���
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
	// Ϊ������ OLE �Զ����������ж������ֹӦ�ó���
	// 	������������ AfxOleUnlockApp��
	//  ���������������⣬�⻹���������Ի���
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CMiniDebuggerDlgAutoProxy::OnFinalRelease()
{
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease�����ཫ�Զ�
	// ɾ���ö����ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CMiniDebuggerDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMiniDebuggerDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// ע��: ��������˶� IID_IMiniDebugger ��֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡��� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {C52201EB-DF98-488E-A102-3DBDD26CCE56}
static const IID IID_IMiniDebugger =
{ 0xC52201EB, 0xDF98, 0x488E, { 0xA1, 0x2, 0x3D, 0xBD, 0xD2, 0x6C, 0xCE, 0x56 } };

BEGIN_INTERFACE_MAP(CMiniDebuggerDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CMiniDebuggerDlgAutoProxy, IID_IMiniDebugger, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 ���ڴ���Ŀ�� StdAfx.h �ж���
// {5B6EF2E1-EAE5-41CE-9BC7-8F2AD2E42F5D}
IMPLEMENT_OLECREATE2(CMiniDebuggerDlgAutoProxy, "MiniDebugger.Application", 0x5b6ef2e1, 0xeae5, 0x41ce, 0x9b, 0xc7, 0x8f, 0x2a, 0xd2, 0xe4, 0x2f, 0x5d)


// CMiniDebuggerDlgAutoProxy ��Ϣ�������
