//�����߼�����UI�Ľӿ�.
#include "stdafx.h"

#include <afx.h>
#include <Windows.h>
#include "Interface.h"
#include "logic/DealEvent.h"
#include "logic/StartDebug.h"
#include "logic/ThreadRelated.h"

#include "MiniDebuggerDlg.h"

CString g_csCommandText=L"";

CString csCallFunctionName = L"";
CString csCallArgv	= L"";
bool Breaking=false;
extern bool  Debugging;
bool bErrorCommandParsing = false;//�����н����Ƿ�������
extern DWORD dwAttachPID;//��ͼ����Ŀ����̵�PID
WCHAR *wcInputDebuggedPath;
extern bool	InputDebuggedPath;

//UI to Logic

/*
�����н���
*/
void CommandParsing(CString CommandText)
{
	int iBlackIndex=0;
	CString m_functionName=L"";
	CString m_argv=L"";

	iBlackIndex=CommandText.Find(L" ");
	if (-1==iBlackIndex)
	{
		m_functionName=CommandText;
	}
	else
	{
		m_functionName=CommandText.Left(iBlackIndex);
		m_argv=CommandText.Mid(iBlackIndex+1);
	}

	csCallFunctionName=m_functionName;
	csCallArgv=m_argv;



	if (L"attach"==csCallFunctionName)
	{

		if(""==csCallArgv)
		{
			BECP();
			return;
		}
		//һ������dwAttachPID,�ͻᱻ�ȴ�״̬�ĳ���ѭ��������.����ǰ���Ǳ�����û���ڵ���״̬�Ż��ڴ�ѭ��.
		dwAttachPID= _wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));


	}
	else if(L"load"==csCallFunctionName)
	{

		if(""==csCallArgv)
		{
			BECP();
			return;
		}
		InputDebuggedPath	=	true;//�����߳��Լ��Ḵλ�ñ�־.
		wcInputDebuggedPath = (WCHAR*)csCallArgv.GetBuffer(csCallArgv.GetLength());  


	}
	if (false==Breaking)
	{
		if(L"break"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			BreakNow();


		}
		else if(L"kill"==csCallFunctionName)//�˺���������ntsd����ɱ����ʽ����������
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			KillDebuggedProcess();


		}
		else if(L"loveit"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}

			LoveIt(true);//��ᵼ��ʲô�㶮��


		}
		else if(L"exit"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			ExitProcess(-1);
		}
		else if(L"detach"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			Detach();
		}
		BECP();
		return;

	}
	else
	{


		csCallFunctionName=m_functionName;
		csCallArgv=m_argv;

		return;
	}

}

//���������������,��λ
void BECP()
{
	bErrorCommandParsing=true;
	//�������Ͳ���
	csCallFunctionName	= "";
	csCallArgv			= "";
}

//�����Ҫִ�еĺ���
void AutoAnalysisCommandParsing()
{

	bErrorCommandParsing=false;
	//�ȶԺ�����,���ִ�д���,���ô�����.����UI�˼��.


	if(true==Breaking)
		//�Ѿ��ж�

	{
		if(L"bp"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempBp=0;
			dwTempBp=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));

			WriteInt3((LPVOID)dwTempBp);

		}
		else if(L"bv"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempBv=0;
			dwTempBv=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));

			WriteInt3((LPVOID)dwTempBv);


		}
		else if(L"bnv"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}

			DWORD dwTempBnv=0;
			dwTempBnv=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));

			InvalidInt3(dwTempBnv);

		}
		else if(L"bc"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}

			DWORD dwTempBc=0;
			dwTempBc=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));

			DeleteInt3(dwTempBc);


		}
		else if(L"bl"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}

			ListInt3();

		}
		//�������߳����
		else if(L"~*"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}

			ListThreads();

		}
		else if(L"~"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempCT=0;
			dwTempCT=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));
			ChangeThread(dwTempCT);


		}
		else if(L"r"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			ListRegister();


		}
		else if(L"ba"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			//����һ����Ч��ָ��,ֱ�������Ӳ���ϵ�Ĵ���


		}
		else if(L"p"==csCallFunctionName)//����
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			SetExecStepFlag();


		}
		//�߳���ؽ�ֹ

		//�ڴ�ϵ���Ҫ˫����(��Χ)
		else if(L"bm"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			//��Ч,ֱ�������ʵ���������


		}
		//������int3�ϵ����
		else if(L"bp"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempCT=0;
			dwTempCT=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));

			WriteInt3((LPVOID)dwTempCT);//��֪��ǿ��ת���ᷢ��ʲô


		}
		else if(L"bv"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempCT=0;
			dwTempCT=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));
			ValidInt3(dwTempCT);


		}
		else if(L"bnv"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempCT=0;
			dwTempCT=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));
			InvalidInt3(dwTempCT);


		}
		else if(L"bc"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempCT=0;
			dwTempCT=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));
			DeleteInt3(dwTempCT);


		}
		else if(L"bl"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}

			ListInt3();


		}
		else if(L"g"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}

			Breaking = false;
		}
		
		else if(L"k"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}

			kStackTrace();
		}

		else if (L"eb"==csCallFunctionName)
		{

			if (""==csCallArgv)
			{
				BECP();
				return;
			}
			ebMemory(csCallArgv);

		}

		else if(L"kill"==csCallFunctionName)//�˺���������ntsd����ɱ����ʽ����������
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			KillDebuggedProcess();


		}
		else if(L"loveit"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}

			LoveIt(true);//��ᵼ��ʲô�㶮��


		}
		else if(L"exit"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			ExitProcess(-1);
		}
		else if(L"detach"==csCallFunctionName)
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			Detach();
		}

		BECP();
		return;


	}




}





//Logic to UI


/*
�Դ�����ַ������и�ʽ������,ת�����UI����
DWORD dwCode:��ʶ��ӡ��ʽ����Ҫ���ӵ���Դ��
*/
bool Printf2UI(CString stPrint,DWORD dwCode)
{
	switch (dwCode)
	{
	case MINIF_DEBUG_STRING:
		{
			stPrint=L"�����ַ���:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_ANTIASM:
		{
			stPrint+=L"\r\n";
			break;
		}
	case MINIF_HARDWARE_BREAKPOINT:
		{
			stPrint=L"Ӳ���ϵ�:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_INT3:
		{
			stPrint=L"INT3�ϵ�:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_MODULE:
		{
			stPrint=L"ģ��:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_REGISTER:
		{
			stPrint=L"�Ĵ���:\r\n"+stPrint+L"\r\n";
			break;
		}
	case MINIF_MEMORY_BREAKPOINT:
		{
			stPrint=L"�ڴ�ϵ�:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_ERROR:
		{
			stPrint=L"����:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_WARMING:
		{
			stPrint=L"����:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_TIPS:
		{
			stPrint=L"��ʾ:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_LIST_THREADS:
		{
			stPrint=L"�߳�:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_STEP_BY_STEP:
		{
			stPrint=L"����:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_STACK_TRACE:
		{
			stPrint=L"\t"+stPrint+L"\r\n";
		}

	default:
		break;
	};


	g_csCommandText = stPrint;

	Sleep(100);

	return true;
}









