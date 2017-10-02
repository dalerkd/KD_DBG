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
DWORD	dwRet			= DBG_EXCEPTION_NOT_HANDLED;//δ����!!!!!!!!!!!dwRet�д���ʹ�õĴ����ƺ�
STARTUPINFO			stcStartupInfo	= {sizeof(STARTUPINFO)};
PROCESS_INFORMATION stcProcInfo		= {0};//������Ϣ
extern HANDLE g_hProc;//�������ߵľ��
vector<ThreadInformation> vTI;//�������ߵ��߳���Ϣ

int StartDebug(bool bNewProcess,void *Infor);
bool GetEvent();
//����Ƿ����ڵ���״̬
bool Debugging = false;
extern DWORD dwNowThread;


//////////////////////////////////////////////////////////////////////////
//bool bNewProcess	:�Ե��Եķ�ʽ����һ������(1),���ӵ�һ������(0)
//void *Infor		:ָ��һ��wchar_t�ַ�������DWORDֵ,��ȡ����bNewProcess.
int StartDebug(bool bNewProcess,void *Infor)
{
	//�жϵ�ǰ����״̬
	if (true == Debugging)
	{
		return -1;
	}

	if (bNewProcess)
	{
		if(0==CreateProcess((LPCWSTR)Infor,
			NULL,				//������
			NULL,				//��ȫ������
			NULL,				//�߳������Ƿ�ɼ̳�
			FALSE,				//�Ƿ�ӵ��ý��̳��̳��˾��
			DEBUG_PROCESS,		//�����DEBUG_ONLY_THIS_PROCESS,ֻ�������һ�����̶����������ӽ���һ��
			NULL,				//�½��̵Ļ�����
			NULL,				//�½��̵ĵ�ǰ����·��(��ǰĿ¼)
			&stcStartupInfo,	//ָ���½��̵�����������
			&stcProcInfo		//�����½��̵�ʶ����Ϣ
			))
		{
			OutputDebug(L"�������Խ���ʧ��");
			return 1;
		}



		;
		//DEBUG_PROCESS
		//DEBUG_ONLY_THIS_PROCESS
	}
	else
	{
		DebugActiveProcess(*((DWORD*)Infor));//����DWORDֵ,�����Ǵ���DWORDָ��.
	}

	Debugging=true;

	GetEvent();

	Debugging=false;

	return 1;
}

bool GetEvent()
{
	while(1==WaitForDebugEvent(&DbgEvt,INFINITE))//�ɵ����Խ���,����¼��ͻ����?
	{
		//����Ƿ������Ҫ���õĺ���,�������������ṩ
		AutoAnalysisCommandParsing();

		//��ÿ�ν����쳣�������ʱ���ܹ���ȡ����ǰ���߳�ID,�����߳���غ���.
		dwNowThread=DbgEvt.dwThreadId;

		switch (DbgEvt.dwDebugEventCode)
		{		
			//�����쳣
		case EXCEPTION_DEBUG_EVENT:
			{
				dwRet= OnExceptionDebugEvent(&DbgEvt.u.Exception);
				break;
			}	
			//���̴���
		case CREATE_PROCESS_DEBUG_EVENT:
			{
				Deal_CPEV();
				break;
			}
			//�����˳�
		case EXIT_PROCESS_DEBUG_EVENT:
			{
				Deal_EPDE();
				break;
			}
			//�̴߳���
		case CREATE_THREAD_DEBUG_EVENT:
			{
				Deal_CTDE();
				break;
			}
			//�߳��˳�
		case EXIT_THREAD_DEBUG_EVENT:
			{
				Deal_ETDE();
				break;
			}
			//ӳ��DLL
		case LOAD_DLL_DEBUG_EVENT:
			{
				Deal_LDDE();
				break;
			}
			//ж��DLL
		case UNLOAD_DLL_DEBUG_EVENT:
			{
				Deal_UDDE();
				break;
			}
			//�����ַ������
		case OUTPUT_DEBUG_STRING_EVENT:
			{
				Deal_ODSE();
				break;
			}
			//RIP(�ڲ�����)
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
		//dwRet��ֵ:
		//DBG_CONTINUE:				�쳣�Ѿ�����
		//DBG_EXCEPTION_NOT_HANDLED:�쳣��Ҫϵͳ��������(���ǵ�Ĭ��)
	}
	return false;
}

//�����ڽ������ԶԻ����߳��е���.
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


//bLove==true   �����߱���,�������߲����˳�
//bLove==false  �෴.Ĭ�����.
//����:false��ʾʧ��.
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

//��ʼ���߳��б�
//���߳�ID�洢��һ���ط�.
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

				//�������Ǳ������̵߳���Ϣ������ǵĽṹ.
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


//~*����,�г������߳�.
bool ListThreads()
{
	//��ȡ���������߳���Ϣ�����vTI����.
	if(false==GetDebuggedThreadList())
	{
		return false;
	}

	CString csTempA;
	bool bA=false;
	//��ӡ
	for (DWORD i=0;i<vTI.size();++i)
	{
		bA=true;

		csTempA.Format(L"%u",vTI[i].ThreadID);
		if(false==Printf2UI(csTempA,MINIF_LIST_THREADS))
		{
			return false;
		}
	}

	//�б�Ϊ��
	if (false==bA)
	{
		csTempA = L"����:��ǰû��δ��ȡ�κ��̼߳�¼.";
		if(false==Printf2UI(csTempA,MINIF_WARMING))
		{
			return false;
		}
	}

	return true;


}

//�л��߳�,�����ı��߳���غ���
//~ iNumber
bool ChangeThread(DWORD dwIndex)
{
	//��ȡ���������߳���Ϣ�����vTI����.
	if(false==GetDebuggedThreadList())
	{
		return false;
	}

	if (dwIndex<vTI.size())
	{
		dwNowThread = vTI[dwIndex].ThreadID;//Ӱ��ȫ�ֵ�ǰ�߳�ID������.
	

	}
	return  false;

}


