//这是逻辑层与UI的接口.
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
bool bErrorCommandParsing = false;//命令行解析是否发生错误
extern DWORD dwAttachPID;//欲图附加目标进程的PID
WCHAR *wcInputDebuggedPath;
extern bool	InputDebuggedPath;

//UI to Logic

/*
命令行解析
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
		//一旦设置dwAttachPID,就会被等待状态的程序循环所捕获.但是前提是本进程没有在调试状态才会在此循环.
		dwAttachPID= _wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));


	}
	else if(L"load"==csCallFunctionName)
	{

		if(""==csCallArgv)
		{
			BECP();
			return;
		}
		InputDebuggedPath	=	true;//监视线程自己会复位该标志.
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
		else if(L"kill"==csCallFunctionName)//此函数所调用ntsd调试杀死方式可能有问题
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

			LoveIt(true);//这会导致什么你懂得


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

//命令解析发生错误,置位
void BECP()
{
	bErrorCommandParsing=true;
	//清理函数和参数
	csCallFunctionName	= "";
	csCallArgv			= "";
}

//检测需要执行的函数
void AutoAnalysisCommandParsing()
{

	bErrorCommandParsing=false;
	//比对函数表,如果执行错误,设置错误标记.用于UI端检测.


	if(true==Breaking)
		//已经中断

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
		//下面是线程相关
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
			//这是一条无效的指令,直到你操作硬件断点寄存器


		}
		else if(L"p"==csCallFunctionName)//单步
		{

			if(""!=csCallArgv)
			{
				BECP();
				return;
			}
			SetExecStepFlag();


		}
		//线程相关截止

		//内存断点需要双参数(范围)
		else if(L"bm"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			//无效,直到你打算实现这个函数


		}
		//下面是int3断点相关
		else if(L"bp"==csCallFunctionName)
		{

			if(""==csCallArgv)
			{
				BECP();
				return;
			}
			DWORD dwTempCT=0;
			dwTempCT=_wtoi(csCallArgv.GetBuffer(csCallArgv.GetLength()));

			WriteInt3((LPVOID)dwTempCT);//天知道强制转换会发生什么


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

		else if(L"kill"==csCallFunctionName)//此函数所调用ntsd调试杀死方式可能有问题
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

			LoveIt(true);//这会导致什么你懂得


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
对传入的字符串进行格式化处理,转输出到UI窗口
DWORD dwCode:标识打印格式及需要附加的来源等
*/
bool Printf2UI(CString stPrint,DWORD dwCode)
{
	switch (dwCode)
	{
	case MINIF_DEBUG_STRING:
		{
			stPrint=L"调试字符串:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_ANTIASM:
		{
			stPrint+=L"\r\n";
			break;
		}
	case MINIF_HARDWARE_BREAKPOINT:
		{
			stPrint=L"硬件断点:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_INT3:
		{
			stPrint=L"INT3断点:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_MODULE:
		{
			stPrint=L"模块:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_REGISTER:
		{
			stPrint=L"寄存器:\r\n"+stPrint+L"\r\n";
			break;
		}
	case MINIF_MEMORY_BREAKPOINT:
		{
			stPrint=L"内存断点:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_ERROR:
		{
			stPrint=L"错误:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_WARMING:
		{
			stPrint=L"警告:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_TIPS:
		{
			stPrint=L"提示:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_LIST_THREADS:
		{
			stPrint=L"线程:"+stPrint+L"\r\n";
			break;
		}
	case MINIF_STEP_BY_STEP:
		{
			stPrint=L"单步:"+stPrint+L"\r\n";
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









