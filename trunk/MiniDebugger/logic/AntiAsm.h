#pragma  once
#include <afx.h>
#include <Windows.h>


UINT DBG_Disasm(LPVOID lpAddress, PWCHAR pOPCode,__out PWCHAR pASM,PWCHAR pComment);

bool DisplayAntiASM(LPVOID lpAddress,DWORD dwLine,DWORD dwMode=0);

