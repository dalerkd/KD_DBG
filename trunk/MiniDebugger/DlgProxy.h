
// DlgProxy.h: ͷ�ļ�
//

#pragma once

class CMiniDebuggerDlg;


// CMiniDebuggerDlgAutoProxy ����Ŀ��

class CMiniDebuggerDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CMiniDebuggerDlgAutoProxy)

	CMiniDebuggerDlgAutoProxy();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��

// ����
public:
	CMiniDebuggerDlg* m_pDialog;

// ����
public:

// ��д
	public:
	virtual void OnFinalRelease();

// ʵ��
protected:
	virtual ~CMiniDebuggerDlgAutoProxy();

	// ���ɵ���Ϣӳ�亯��

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CMiniDebuggerDlgAutoProxy)

	// ���ɵ� OLE ����ӳ�亯��

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

