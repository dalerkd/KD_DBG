
// MiniDebugger.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMiniDebuggerApp:
// �йش����ʵ�֣������ MiniDebugger.cpp
//

class CMiniDebuggerApp : public CWinApp
{
public:
	CMiniDebuggerApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMiniDebuggerApp theApp;