
// MiniDebuggerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "resource.h"

class CMiniDebuggerDlgAutoProxy;


// CMiniDebuggerDlg �Ի���
class CMiniDebuggerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMiniDebuggerDlg);
	friend class CMiniDebuggerDlgAutoProxy;

// ����
public:
	CMiniDebuggerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CMiniDebuggerDlg();

// �Ի�������
	enum { IDD = IDD_MINIDEBUGGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMsg);  //���Լ���ӵ�.�������ջس�����

// ʵ��
protected:
	CMiniDebuggerDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_MainMsg;
	CString m_CommandText;
	CString m_MainEdit;


	bool InterfaceOfDisplay(CString CommandText);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
