#pragma once
#include <afx.h>
#include <Windows.h>


//bool bNewProcess	:�Ե��Եķ�ʽ����һ������(1),���ӵ�һ������(0)
//void *Infor		:ָ��һ���ַ�������DWORDֵ,��ȡ����bNewProcess.
int StartDebug(bool bNewProcess,void *Infor);

//return:
//false.����δ��ʼ��.
bool Detach();


//bLove==true   �����߱���,�������߲����˳�
//bLove==false  �෴.Ĭ�����.
//����:
//falseʧ��
bool LoveIt(bool bLove);

struct ThreadInformation
{
	DWORD ThreadID;
	DWORD OwnerPID;
	LONG  ThreadPLevel;

};

bool BreakNow();
void KillDebuggedProcess();

bool GetDebuggedThreadList();//��ʼ���߳��б�,���߳�ID�洢��һ���ط�.
bool ListThreads();
bool ChangeThread(DWORD dwIndex);