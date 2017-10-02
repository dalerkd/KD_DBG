#include "stdafx.h"
#include <afx.h>

#include <Windows.h>
#include "Debug.h"
#include <strsafe.h>
#include "AntiAsm.h"

#include "../Interface.h"


#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "BeaEngine.h"
#pragma comment(lib,"logic/BeaEngine.lib")

#pragma comment(linker,"/NODEFAULTLIB:\"crt.lib\"")


extern HANDLE g_hProc;

//////////////////////////////////////////////////////////////////////////
//Return:-1:��������淴������.
//
//
UINT DBG_Disasm(LPVOID lpAddress, PWCHAR pOPCode,__out PWCHAR pASM,PWCHAR pComment)
{
	//1. �����Գ�����ڴ渴�Ƶ�����o
	DWORD	dwRetSize		= 0;
	LPVOID	lpRemote_Buf	= new BYTE[32];
	ZeroMemory(lpRemote_Buf, 32);
	ReadProcessMemory(g_hProc,lpAddress,lpRemote_Buf,32,&dwRetSize);

	//2. ��ʼ�����������
	DISASM objDiasm;
	objDiasm.EIP		=(UIntPtr)lpRemote_Buf; //��ʼ��ַ
	objDiasm.VirtualAddr=(UINT64)lpAddress;		//�����ڴ��ַ(������������ڼ����ַ)
	objDiasm.Archi		=0;						//AI_X86
	objDiasm.Options	=0x000;					//MASM

	//3.��������
	UINT unLen =Disasm(&objDiasm);
	if (-1==unLen) 
	{
		return unLen;
	}

	//4.��������ת����ַ���
	LPWSTR lpOPCode = pOPCode;
	PBYTE  lpBuffer = (PBYTE)lpRemote_Buf;
	for (UINT i=0;i>unLen;i++)
	{
		StringCbPrintf(lpOPCode++, 50, L"%X", *lpBuffer&0xF0);
		StringCbPrintf(lpOPCode++, 50, L"%X", *lpBuffer&0x0F);
		lpBuffer++;
	}

	//5. ���淴������ָ��
	WCHAR szASM[50] = {0};
	MultiByteToWideChar(CP_ACP,0,objDiasm.CompleteInstr,-1,szASM,_countof(szASM));
	StringCchCopy(pASM, 50,szASM);

	return unLen;

}


/*
//LPVOID lpAddress:	��ʼ�ķ�����ַ
//DWORD  dwLine	  :	��Ҫ����������	
//DWORD	 dwMode(��ʱ��Ч)	  :	Ĭ��Ϊ0,��ʾ�ӷ�����ַ��ʼ��ַ����ʼ�����,
//					�������ֱ�ʾ�ӷ������ʼ��ַ��͵�ַƫ��N�ĵ�ַ��ʼ�����.
//
*/
bool DisplayAntiASM(LPVOID lpAddress,DWORD dwLine,DWORD dwMode)
{


	wchar_t swOPCode[64]	= {0};
	wchar_t swASM[64]		= {0};
	wchar_t swComment[64]	= {0};
	wchar_t swPrintBuffer[200]={0};
	DWORD	dwTempAddr		= (DWORD)lpAddress;//��ʼ������ַ
	DWORD   dwOPCodeLen		= 0;


	for (DWORD i=0;dwOPCodeLen!=-1&&i<dwLine;i++)
	{
		dwOPCodeLen = DBG_Disasm((LPVOID)dwTempAddr,swOPCode,swASM,swComment);
		dwTempAddr+=dwOPCodeLen;
		OutputDebug(L"Addr:0x%p %-12s %-16s %s\n",dwTempAddr,swOPCode,swASM,swComment);
		wsprintfW(swPrintBuffer,L"Addr:0x%p %-12s %-16s %s",dwTempAddr,swOPCode,swASM,swComment);
		
		CString csStr;
		for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
		{
			csStr.AppendChar(swPrintBuffer[i]);  
		}
		Printf2UI(csStr,MINIF_ANTIASM);
	}

	return true;
}
