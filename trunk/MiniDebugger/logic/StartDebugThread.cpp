#include "stdafx.h"
#include <afx.h>

#include "StartDebug.h"
#include <Windows.h>
DWORD	dwAttachPID			=	0;

bool	InputDebuggedPath	=	false;//�Ƿ���·�����뷽ʽ�������Գ���
extern WCHAR *wcInputDebuggedPath;



DWORD  WINAPI StartDebugThread(
	LPVOID LPM)
{
	Sleep(1000);//�ӳ�һ����UI��ʼ��

	wchar_t * pwText={0};

	if(L'\0'!=AfxGetApp()->m_lpCmdLine[0])
	{
		pwText=AfxGetApp()->m_lpCmdLine;

		StartDebug(true,pwText);
	}

	//AfxGetApp()->m_lpCmdLine;

	while(true)
	{


		if (0!=dwAttachPID)//���ȸ��ӷ�ʽ,
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