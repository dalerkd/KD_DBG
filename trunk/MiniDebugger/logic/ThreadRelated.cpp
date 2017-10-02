#include "stdafx.h"
#include <afx.h>

#include <Windows.h>
#include <vector>
#include "../Interface.h"
#include "StartDebug.h"

using std::vector;


extern bool Breaking;
extern vector<ThreadInformation> vTI;//�������ߵ��߳���Ϣ

//ȫ�ֵ�ǰ�߳�ID,�ɵ���ѭ�����ȸ���.
DWORD dwNowThread=0;

typedef struct _EFLAGS{
	unsigned CF			:1;//��λ��λ
	unsigned Reserve1	:1;
	unsigned PF			:1;//��������λ����ż����1ʱ=1
	unsigned Reserve2	:1;
	unsigned AF			:1;//������λ��־
	unsigned Reserve3	:1;
	unsigned ZF			:1;//������Ϊ0ʱ=1
	unsigned SF			:1;//���ű�־,������Ϊ��ʱ�ñ�־Ϊ1
	unsigned TF			:1;//*�����־,�˱�־Ϊ1ʱ,CPUÿ�ν���ִ��һ��ָ��
	unsigned IF			:1;//�жϱ�־,Ϊ0ʱ��ֹ��Ӧ(���ο������ж�)
	unsigned DF			:1;//�����־
	unsigned OF			:1;//�����־,����������������ﷶΧʱΪ1
	unsigned IOPL		:2;//���ڱ�����ǰ�����I/O��Ȩ��
	unsigned NT			:1;//����Ƕ�ױ�־
	unsigned Reserve4	:1;
	unsigned RF			:1;//�����쳣��Ӧ���Ʊ�־λ,Ϊ1��ֹ��Ӧָ��ϵ��쳣
	unsigned VM			:1;//Ϊ1ʱ��������8086ģʽ
	unsigned AC			:1;//�ڴ�������־
	unsigned VIF		:1;//�����жϱ�־
	unsigned VIP		:1;//�����жϱ�־
	unsigned ID			:1;//CPUID����־
	unsigned Reserve5	:10;

}EFLAGS,*PEFLAGS;






//�г��Ĵ���
bool ListRegister()
{
	//��ȡ����������
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
	//ͨ���޸�stcCxt���޸ļĴ���...
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

	////���ý���������
	//if (!SetThreadContext(hTargetThread,&stcCxt))
	//{
	//	return false;
	//}


	return true;

}

bool SetExecStepFlag()
{
	//��ȡ���������߳���Ϣ�����vTI����.
	if(false==GetDebuggedThreadList())
	{
		return false;
	}

	bool bSuccess=false;

	//�������߳��µ����ϵ�
	for (DWORD i=0;i<vTI.size();++i)
	{
		bSuccess = true;

		//��ȡ����������
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
		//ͨ���޸�stcCxt���޸ļĴ���...
		DWORD dwEflags	= 0;	
		stcCxt.EFlags	= 0x302;


		//���ý���������
		if (!SetThreadContext(hTargetThread,&stcCxt))
		{
			return false;
		}

		//���ǵ�ָ���߳���Ϳ�����Int3.	
		//go
		Breaking = false;//�ر��ж�״̬,�ж�ָ������.
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