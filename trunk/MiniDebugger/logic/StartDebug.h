#pragma once
#include <afx.h>
#include <Windows.h>


//bool bNewProcess	:以调试的方式启动一个进程(1),附加到一个进程(0)
//void *Infor		:指向一个字符串或者DWORD值,这取决于bNewProcess.
int StartDebug(bool bNewProcess,void *Infor);

//return:
//false.进程未初始化.
bool Detach();


//bLove==true   调试者崩溃,被调试者不会退出
//bLove==false  相反.默认情况.
//返回:
//false失败
bool LoveIt(bool bLove);

struct ThreadInformation
{
	DWORD ThreadID;
	DWORD OwnerPID;
	LONG  ThreadPLevel;

};

bool BreakNow();
void KillDebuggedProcess();

bool GetDebuggedThreadList();//初始化线程列表,将线程ID存储到一个地方.
bool ListThreads();
bool ChangeThread(DWORD dwIndex);