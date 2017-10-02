#include "stdafx.h"
#include <afx.h>

#include "StartDebug.h"
#include <Windows.h>
DWORD	dwAttachPID			=	0;

bool	InputDebuggedPath	=	false;//是否以路径传入方式启动调试程序
extern WCHAR *wcInputDebuggedPath;



DWORD  WINAPI StartDebugThread(
	LPVOID LPM)
{
	Sleep(1000);//延迟一下让UI初始化

	wchar_t * pwText={0};

	if(L'\0'!=AfxGetApp()->m_lpCmdLine[0])
	{
		pwText=AfxGetApp()->m_lpCmdLine;

		StartDebug(true,pwText);
	}

	//AfxGetApp()->m_lpCmdLine;

	while(true)
	{


		if (0!=dwAttachPID)//优先附加方式,
		{
			DWORD dwPIDBuffer=dwAttachPID;
			StartDebug(false,&dwPIDBuffer);

		}
		else if(true==InputDebuggedPath)

		{

			InputDebuggedPath	=	false;
			StartDebug(true,wcInputDebuggedPath);



		}

		Sleep(500);

	}

	return 0;

}