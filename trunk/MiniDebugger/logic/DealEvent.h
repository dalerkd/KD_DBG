#pragma once
#include <afx.h>

bool Deal_CPEV();//���̴����¼�
bool Deal_EPDE();//���������˳�
bool Deal_ODSE();//������Ϣ���
bool Deal_CTDE();//�̴߳���
bool Deal_ETDE();//�߳��˳�
bool Deal_LDDE();//ӳ��DLLģ���¼�
bool Deal_UDDE();//ģ��ж��

bool ListInt3();
bool InvalidInt3(DWORD dwIndex);//ʹint3�ϵ�ʧЧ
bool ValidInt3(DWORD dwIndex);//ʹ��Ч
bool DeleteInt3(DWORD dwIndex);
bool WriteInt3(LPVOID  lpAddress);

bool ReadDebuggedMemory(LPVOID lpAddress,DWORD dwGetNumber,_Out_ BYTE* wcGetValue);//ͨ�õĶ��ڴ�ĺ���
bool WriteDebuggedMemory(LPVOID lpAddress,DWORD dwSetNumber,_In_ BYTE* wcSetValue);//ͨ�õ�Щ�ڴ�ĺ���

bool kStackTrace();//ջ����
void kRecur(DWORD* dwEbp);//ջ���ݵݹ���Ѱ

void ebMemory(CString Arg);//�޸�Ŀ���ڴ��һ���ֽ�

bool ClearInt3(DWORD dwIndex);//!!�ⲿ��ʹ��,��û������Vector�еĶ���.�ᵼ�²���Ԥ�ϵĽ��.
int SerarchInt3(LPVOID lpAddress);


DWORD OnExceptionDebugEvent(LPEXCEPTION_DEBUG_INFO pDbgInfo);
