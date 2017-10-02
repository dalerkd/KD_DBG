#include "stdafx.h"
#include <afx.h>

#include <Windows.h>
#include "Debug.h"
#include "DealEvent.h"
#include "..\Interface.h"
#include <TlHelp32.h>
#include <vector>
#include "StartDebug.h"

using std::vector;


_DEBUG_EVENT	DbgEvt	= {0};		
DWORD	dwRet			= DBG_EXCEPTION_NOT_HANDLED;//未处理!!!!!!!!!!!dwRet有错误使用的代码似乎
STARTUPINFO			stcStartupInfo	= {sizeof(STARTUPINFO)};
PROCESS_INFORMATION stcProcInfo		= {0};//进程信息
extern HANDLE g_hProc;//被调试者的句柄
vector<ThreadInformation> vTI;//被调试者的线程信息

int StartDebug(bool bNewProcess,void *Infor);
bool GetEvent();
//标记是否正在调试状态
bool Debugging = false;
extern DWORD dwNowThread;


//////////////////////////////////////////////////////////////////////////
//bool bNewProcess	:以调试的方式启动一个进程(1),附加到一个进程(0)
//void *Infor		:指向一个wchar_t字符串或者DWORD值,这取决于bNewProcess.
int StartDebug(bool bNewProcess,void *Infor)
{
	//判断当前调试状态
	if (true == Debugging)
	{
		return -1;
	}

	if (bNewProcess)
	{
		if(0==CreateProcess((LPCWSTR)Infor,
			NULL,				//命令行
			NULL,				//安全描述符
			NULL,				//线程属性是否可继承
			FALSE,				//是否从调用进程出继承了句柄
			DEBUG_PROCESS,		//如果以DEBUG_ONLY_THIS_PROCESS,只会调试这一个进程而不是它的子进程一起
			NULL,				//新进程的环境块
			NULL,				//新进程的当前工作路径(当前目录)
			&stcStartupInfo,	//指定新进程的主窗口特性
			&stcProcInfo		//接收新进程的识别信息
			))
		{
			OutputDebug(L"创建调试进程失败");
			return 1;
		}



		;
		//DEBUG_PROCESS
		//DEBUG_ONLY_THIS_PROCESS
	}
	else
	{
		DebugActiveProcess(*((DWORD*)Infor));//传入DWORD值,而不是传入DWORD指针.
	}

	Debugging=true;

	GetEvent();

	Debugging=false;

	return 1;
}

bool GetEvent()
{
	while(1==WaitForDebugEvent(&DbgEvt,INFINITE))//干掉调试进程,这个事件就会结束?
	{
		//检测是否存在需要调用的函数,它们由命令行提供
		AutoAnalysisCommandParsing();

		//在每次进入异常处理步骤的时候都能够获取到当前的线程ID,用于线程相关函数.
		dwNowThread=DbgEvt.dwThreadId;

		switch (DbgEvt.dwDebugEventCode)
		{		
			//调试异常
		case EXCEPTION_DEBUG_EVENT:
			{
				dwRet= OnExceptionDebugEvent(&DbgEvt.u.Exception);
				break;
			}	
			//进程创建
		case CREATE_PROCESS_DEBUG_EVENT:
			{
				Deal_CPEV();
				break;
			}
			//进程退出
		case EXIT_PROCESS_DEBUG_EVENT:
			{
				Deal_EPDE();
				break;
			}
			//线程创建
		case CREATE_THREAD_DEBUG_EVENT:
			{
				Deal_CTDE();
				break;
			}
			//线程退出
		case EXIT_THREAD_DEBUG_EVENT:
			{
				Deal_ETDE();
				break;
			}
			//映射DLL
		case LOAD_DLL_DEBUG_EVENT:
			{
				Deal_LDDE();
				break;
			}
			//卸载DLL
		case UNLOAD_DLL_DEBUG_EVENT:
			{
				Deal_UDDE();
				break;
			}
			//调试字符串输出
		case OUTPUT_DEBUG_STRING_EVENT:
			{
				Deal_ODSE();
				break;
			}
			//RIP(内部错误)
		case RIP_EVENT:
			{
				break;
			}

		default:
			{
				break;
			}
		}
		ContinueDebugEvent(
			DbgEvt.dwProcessId,
			DbgEvt.dwThreadId,
			dwRet);
		//dwRet的值:
		//DBG_CONTINUE:				异常已经处理
		//DBG_EXCEPTION_NOT_HANDLED:异常需要系统继续处理(我们的默认)
	}
	return false;
}

//必须在建立调试对话的线程中调用.
bool Detach()
{
	if (0!=DbgEvt.dwProcessId)
	{
		DebugActiveProcessStop(DbgEvt.dwProcessId);
		Debugging=false;
		return true;
	}

	return false;

}


//bLove==true   调试者崩溃,被调试者不会退出
//bLove==false  相反.默认情况.
//返回:false表示失败.
bool LoveIt(bool bLove)
{
	if (0==DebugSetProcessKillOnExit(!bLove))
	{
		return false;
	}

	return true;
}

bool BreakNow()
{
	if(0==DebugBreakProcess(g_hProc))
	{
		return false;
	}
	else
	{
		return true;
	}

}

void KillDebuggedProcess()
{


	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pi;


	TCHAR cmdline[MAX_PATH] =TEXT("ntsd.exe -c q -p ");
	TCHAR BuffA[20]={0};

	wsprintf(BuffA,L"%d",DbgEvt.dwProcessId);

	wcscat_s(cmdline,BuffA);

	CreateProcess(NULL,cmdline,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);

}

//初始化线程列表
//将线程ID存储到一个地方.
bool GetDebuggedThreadList()
{

	vTI.clear();

	DWORD th32ProcessID = DbgEvt.dwProcessId;

	HANDLE hSnapThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, th32ProcessID);

	if (INVALID_HANDLE_VALUE == hSnapThread)
	{
		return false;
	}
	
		THREADENTRY32 te32 ={0};
		te32.dwSize=sizeof(THREADENTRY32);
	

	if (Thread32First(hSnapThread, &te32))

	{


		do {

			if (th32ProcessID == te32.th32OwnerProcessID)

			{

				//属于我们被调试线程的信息填充我们的结构.
				ThreadInformation TITempA;

				TITempA.ThreadID=te32.th32ThreadID;
				TITempA.OwnerPID=te32.th32OwnerProcessID;
				TITempA.ThreadPLevel=te32.tpBasePri;

				vTI.push_back(TITempA);


			}

		} while (Thread32Next(hSnapThread, &te32));

	}



	CloseHandle(hSnapThread);
	return true;

}


//~*命令,列出所有线程.
bool ListThreads()
{
	//获取被调试者线程信息并填充vTI向量.
	if(false==GetDebuggedThreadList())
	{
		return false;
	}

	CString csTempA;
	bool bA=false;
	//打印
	for (DWORD i=0;i<vTI.size();++i)
	{
		bA=true;

		csTempA.Format(L"%u",vTI[i].ThreadID);
		if(false==Printf2UI(csTempA,MINIF_LIST_THREADS))
		{
			return false;
		}
	}

	//列表为空
	if (false==bA)
	{
		csTempA = L"警告:当前没有未获取任何线程记录.";
		if(false==Printf2UI(csTempA,MINIF_WARMING))
		{
			return false;
		}
	}

	return true;


}

//切换线程,仅仅改变线程相关函数
//~ iNumber
bool ChangeThread(DWORD dwIndex)
{
	//获取被调试者线程信息并填充vTI向量.
	if(false==GetDebuggedThreadList())
	{
		return false;
	}

	if (dwIndex<vTI.size())
	{
		dwNowThread = vTI[dwIndex].ThreadID;//影响全局当前线程ID的内容.
	

	}
	return  false;

}


