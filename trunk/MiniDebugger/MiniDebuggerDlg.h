
// MiniDebuggerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"

class CMiniDebuggerDlgAutoProxy;


// CMiniDebuggerDlg 对话框
class CMiniDebuggerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMiniDebuggerDlg);
	friend class CMiniDebuggerDlgAutoProxy;

// 构造
public:
	CMiniDebuggerDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CMiniDebuggerDlg();

// 对话框数据
	enum { IDD = IDD_MINIDEBUGGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);  //我自己添加的.用来接收回车命令

// 实现
protected:
	CMiniDebuggerDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 生成的消息映射函数
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
