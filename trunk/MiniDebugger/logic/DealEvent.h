#pragma once
#include <afx.h>

bool Deal_CPEV();//进程创建事件
bool Deal_EPDE();//被调试这退出
bool Deal_ODSE();//调试信息输出
bool Deal_CTDE();//线程创建
bool Deal_ETDE();//线程退出
bool Deal_LDDE();//映射DLL模块事件
bool Deal_UDDE();//模块卸载

bool ListInt3();
bool InvalidInt3(DWORD dwIndex);//使int3断点失效
bool ValidInt3(DWORD dwIndex);//使有效
bool DeleteInt3(DWORD dwIndex);
bool WriteInt3(LPVOID  lpAddress);

bool ReadDebuggedMemory(LPVOID lpAddress,DWORD dwGetNumber,_Out_ BYTE* wcGetValue);//通用的读内存的函数
bool WriteDebuggedMemory(LPVOID lpAddress,DWORD dwSetNumber,_In_ BYTE* wcSetValue);//通用的些内存的函数

bool kStackTrace();//栈回溯
void kRecur(DWORD* dwEbp);//栈回溯递归搜寻

void ebMemory(CString Arg);//修改目标内存的一个字节

bool ClearInt3(DWORD dwIndex);//!!外部勿使用,它没有清理Vector中的东东.会导致不可预料的结果.
int SerarchInt3(LPVOID lpAddress);


DWORD OnExceptionDebugEvent(LPEXCEPTION_DEBUG_INFO pDbgInfo);
