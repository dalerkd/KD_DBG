#include "stdafx.h"
#include <afx.h>

#include <Windows.h>
#include <vector>
#include "../Interface.h"
#include "StartDebug.h"

using std::vector;


extern bool Breaking;
extern vector<ThreadInformation> vTI;//被调试者的线程信息

//全局当前线程ID,由调试循环优先更新.
DWORD dwNowThread=0;

typedef struct _EFLAGS{
	unsigned CF			:1;//进位借位
	unsigned Reserve1	:1;
	unsigned PF			:1;//计算结果低位包含偶数个1时=1
	unsigned Reserve2	:1;
	unsigned AF			:1;//辅助进位标志
	unsigned Reserve3	:1;
	unsigned ZF			:1;//计算结果为0时=1
	unsigned SF			:1;//符号标志,计算结果为负时该标志为1
	unsigned TF			:1;//*陷阱标志,此标志为1时,CPU每次仅会执行一条指令
	unsigned IF			:1;//中断标志,为0时禁止响应(屏蔽可屏蔽中断)
	unsigned DF			:1;//方向标志
	unsigned OF			:1;//溢出标志,计算结果超出机器表达范围时为1
	unsigned IOPL		:2;//用于表明当前任务的I/O特权级
	unsigned NT			:1;//任务嵌套标志
	unsigned Reserve4	:1;
	unsigned RF			:1;//调试异常响应控制标志位,为1禁止响应指令断点异常
	unsigned VM			:1;//为1时启用虚拟8086模式
	unsigned AC			:1;//内存对齐检查标志
	unsigned VIF		:1;//虚拟中断标志
	unsigned VIP		:1;//虚拟中断标志
	unsigned ID			:1;//CPUID检查标志
	unsigned Reserve5	:10;

}EFLAGS,*PEFLAGS;






//列出寄存器
bool ListRegister()
{
	//获取进程上下文
	CONTEXT stcCxt = {CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS};
	HANDLE hTargetThread;
	hTargetThread= OpenThread(THREAD_ALL_ACCESS , FALSE, dwNowThread);
	if(NULL==hTargetThread)
	{
		return false;
	}

	if (!GetThreadContext(hTargetThread, &stcCxt))
	{
		return false;
	}
	//通过修改stcCxt来修改寄存器...
	DWORD dwEflags	= 0;	
	dwEflags	= stcCxt.EFlags;

	PEFLAGS pEftempA= (PEFLAGS)&dwEflags;

	wchar_t swPrintBuffer[1024]={0};
	wsprintfW(swPrintBuffer,
		L"EAX=%p EBX=%p ECX=%p EDX=%p \r\nESI=%p EDI=%p ESP=%p EBP=%p EIP=%p \r\nEFLAGS=%p \r\nCF=%d,PF=%d,AF=%d,ZF=%d,SF=%d,TF=%d,IF=%d,DF=%d,OF=%d",
		stcCxt.Eax,stcCxt.Ebx,stcCxt.Ecx,stcCxt.Edx,
		stcCxt.Esi,stcCxt.Edi,stcCxt.Esp,stcCxt.Ebp,stcCxt.Eip,stcCxt.EFlags,pEftempA->CF,pEftempA->PF,pEftempA->AF,pEftempA->ZF,pEftempA->SF,pEftempA->TF,pEftempA->IF,pEftempA->DF,pEftempA->OF);

	CString csStr;
	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}
	Printf2UI(csStr,MINIF_REGISTER);

	////设置进程上下文
	//if (!SetThreadContext(hTargetThread,&stcCxt))
	//{
	//	return false;
	//}


	return true;

}

bool SetExecStepFlag()
{
	//获取被调试者线程信息并填充vTI向量.
	if(false==GetDebuggedThreadList())
	{
		return false;
	}

	bool bSuccess=false;

	//对所有线程下单步断点
	for (DWORD i=0;i<vTI.size();++i)
	{
		bSuccess = true;

		//获取进程上下文
		CONTEXT stcCxt = {CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS};
		HANDLE hTargetThread;
		hTargetThread= OpenThread(THREAD_ALL_ACCESS , FALSE, vTI[i].ThreadID);
		if(NULL==hTargetThread)
		{
			return false;
		}

		if (!GetThreadContext(hTargetThread, &stcCxt))
		{
			return false;
		}
		//通过修改stcCxt来修改寄存器...
		DWORD dwEflags	= 0;	
		stcCxt.EFlags	= 0x302;


		//设置进程上下文
		if (!SetThreadContext(hTargetThread,&stcCxt))
		{
			return false;
		}

		//我们的指令线程早就卡在了Int3.	
		//go
		Breaking = false;//关闭中断状态,中断指令会继续.
	}
	

	if (true==bSuccess)
	{
		return true;
	}
	else
	{
		return false;
	}

}