#pragma once


#include <Windows.h>

enum Printf2UICode{
	MINIF_DEBUG_STRING=1,
	MINIF_ANTIASM,
	MINIF_REGISTER,
	MINIF_MODULE,
	MINIF_INT3,
	MINIF_HARDWARE_BREAKPOINT,
	MINIF_MEMORY_BREAKPOINT,
	MINIF_ERROR,
	MINIF_WARMING,
	MINIF_TIPS,
	MINIF_LIST_THREADS,
	MINIF_STEP_BY_STEP,
	MINIF_STACK_TRACE
};



void BECP();//���������������,��λ.�������Ͳ���etc.
void CommandParsing(CString CommandText);
void AutoAnalysisCommandParsing();
bool Printf2UI(CString stPrint,DWORD dwCode);