#include "stdafx.h"
#include <afx.h>

#include <Windows.h>
#include "Debug.h"
#include "AntiAsm.h"
#include "DealEvent.h"
#include <vector>
#include "..//Interface.h"
#include "ThreadRelated.h"

using std::vector;

extern _DEBUG_EVENT	DbgEvt;	
HANDLE g_hProc;//�������ߵľ��
extern bool Breaking;
extern DWORD dwNowThread;


//////////////////////////////////////////////////////////////////////////
//һ�����ڱ���int3״̬������
vector<BYTE>Int3Back;	//ԭֵ
vector<void*>Int3Addr;	//��ַ
vector<bool>Int3Value;  //�öϵ��Ƿ���Ч?
//////////////////////////////////////////////////////////////////////////


//���̴�������.
bool Deal_CPEV()
{
	OutputDebug(L"���̴����¼�");

	//OEP��ַ
	LPVOID	lpIntAddress	= DbgEvt.u.CreateProcessInfo.lpStartAddress;

	g_hProc   = OpenProcess(PROCESS_ALL_ACCESS,0,DbgEvt.dwProcessId);
	if (NULL==g_hProc)
	{
		OutputDebug(L"�򿪽���ʧ��,����Ϊ:%p,FILE: %s, LINE: %d��",DbgEvt.dwProcessId,__FILE__,__LINE__);
		return true;
	}

	//��OEP��һ������ϵ�.
	//WriteInt3(lpIntAddress);

	return true;
}

//ʹInt3�ϵ���Ч
bool InvalidInt3(DWORD dwIndex)
{
	if ((dwIndex<Int3Addr.size())&&(!Int3Addr.empty()))
	{
		Int3Value[dwIndex]=0;
		if(false==ClearInt3(dwIndex))
		{
			return false;
		}
		return true;
	}
	return false;
}

//ʹInt3�ϵ���Ч
bool ValidInt3(DWORD dwIndex)
{
	if ((dwIndex<Int3Addr.size())&&(!Int3Addr.empty()))
	{
		Int3Value[dwIndex]=1;
		if(false==WriteInt3(Int3Addr[dwIndex]))
		{
			return false;
		}
		return true;
	}
	return false;
}

//ɾ��Int3�ϵ�
bool DeleteInt3(DWORD dwIndex)
{
	if ((dwIndex<Int3Addr.size())&&(!Int3Addr.empty()))
	{
		if (false==ClearInt3(dwIndex))
		{
			return false;
		}

		Int3Addr.erase(Int3Addr.begin()+dwIndex);
		Int3Back.erase(Int3Back.begin()+dwIndex);
		Int3Value.erase(Int3Value.begin()+dwIndex);

		return true;
	}

	return false;
}

//�ָ�Int3�¶�ǰ��ֵ,���ǲ�����vector
bool ClearInt3(DWORD dwIndex)
{ 
	if ((dwIndex<Int3Addr.size())&&(!Int3Addr.empty()))
	{
		DWORD	dwRet	= 0;	
		//д��int3�ϵ�
		if(!WriteProcessMemory(g_hProc,Int3Addr[dwIndex],&Int3Back,1,&dwRet))
		{	
			OutputDebug(L"д�ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",Int3Addr[dwIndex],__FILE__,__LINE__);

			DWORD OldProtect	= 0;//��Ҫ��ֵ,������ű�������
			PDWORD lpOldProtect =&OldProtect;

			DWORD OldProtectB	= 0;//һ������ʹ�õ�ֵ,���ڴ�ű�������
			PDWORD  lpOldProtectB=&OldProtectB;

			if(0!=VirtualProtectEx(g_hProc,Int3Addr[dwIndex],4*1024,PAGE_READWRITE,lpOldProtect))
			{
				if(!WriteProcessMemory(g_hProc,Int3Addr[dwIndex],&Int3Back,1,&dwRet))
				{
					OutputDebug(L"д�ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",Int3Addr[dwIndex],__FILE__,__LINE__);
					if(0==VirtualProtectEx(g_hProc,Int3Addr[dwIndex],4*1024,*lpOldProtect,lpOldProtectB))
					{
						OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",Int3Addr[dwIndex],__FILE__,__LINE__,OldProtect);
						return false;
					}
					return false;
				}

				if(0==VirtualProtectEx(g_hProc,Int3Addr[dwIndex],4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",Int3Addr[dwIndex],__FILE__,__LINE__,OldProtect);
					return false;
				}

				return true;
			}
			else
			{
				OutputDebug(L"�����ڴ��ַ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,������Ϊ:%d,������Ϊ:PAGE_READWRITE",Int3Addr[dwIndex],__FILE__,__LINE__,OldProtect);
				return false;
			}
		}
		return true;
	}

	return false;
}


//����ָ����ַ�Ƿ�������Ѽ�¼Int3�б�,�����򷵻��±�.-1û�ҵ�.
int SerarchInt3(LPVOID lpAddress)
{
	vector<void*>::iterator it;

	int iIndex=0;//���
	for (it=Int3Addr.begin();it<Int3Addr.end();++it,++iIndex)
	{
		if (*it==lpAddress)
		{
			return iIndex;
		}
	}

	return -1;//��,�����Ҳ���

}


//д��int3�ϵ�,���������ʹ����Ч.������Ҫд�ϵ�ĵ�ַ
//����ֵ:false��������ȫ��ʧ��.���ÿ��������,�������������Ѿ����.ֻ��ԭ�����ڴ�ҳ����û�취д��.
bool WriteInt3(LPVOID	lpAddress)
{
	BYTE	byNowValue	= 0;//ʢ��Ŀ���ڴ��̽��ֵ
	DWORD	dwRetN		= 0;
	BYTE	byInt3		= 0xCC;

	int iIndex=SerarchInt3(lpAddress);


	if (-1!=iIndex)
	{
		if(false==Int3Value[iIndex])
		{
			//�ж�Ŀ��λ���Ƿ���0xCC.�������޸�.

			if(!ReadProcessMemory(g_hProc,lpAddress,&byNowValue,1,&dwRetN))
			{
				DWORD OldProtect	= 0;//��Ҫ��ֵ,������ű�������
				PDWORD lpOldProtect =&OldProtect;

				DWORD OldProtectB	= 0;//һ������ʹ�õ�ֵ,���ڴ�ű�������
				PDWORD  lpOldProtectB=&OldProtectB;

				OutputDebug(L"���ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
				if(0!=VirtualProtectEx(g_hProc,lpAddress,4*1024,PAGE_READWRITE,lpOldProtect))
				{

					if(!ReadProcessMemory(g_hProc,lpAddress,&byNowValue,1,&dwRetN))
					{
						OutputDebug(L"���ڴ��ַ���Գɹ���,���ڴ��ַ��Ȼ!!ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
						//�ָ�ԭ����.
						if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
						{
							OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
							return false;
						}
						return false;
					}
					else
					{
						if (byInt3!=byNowValue)
						{
							//!!���ｫ��⵽�ķ�0xCC��ֵд�뵽��vector��,��û�п��Ǵ�ֵ��vectorԭ����ֵ�Ĺ�ϵ.
							if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,(SIZE_T*)&Int3Back[iIndex]))
							{	
								OutputDebug(L"д�ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
								if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
								{
									OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
									return false;
								}
								return false;
							}
							if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
							{
								OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
								return false;
							}
							return true;
						}				
						Int3Value[iIndex]=true;
						if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
						{
							OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
							return false;
						}
						return true;
					}

				}
				else
				{
					OutputDebug(L"�����ڴ��ַ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,������Ϊ:%d,������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}

			}
			else
			{
				if(byInt3!=byNowValue)
				{
					if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,(SIZE_T*)&Int3Back[iIndex]))
					{	
						OutputDebug(L"д�ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
						return false;
					}

				}
				//���ڴ�ɹ����޸�Int3Value,����false
				Int3Value[iIndex]=true;
				return  true;
			}
		}
		else
		{
			return true;//�öϵ������Ѿ���Ч.
		}
		return true;
	}
	//����ϵ�δ��¼��.�����¼����:


	DWORD	dwRet	= 0;
	BYTE	byBack	= 0;

	//����ԭָ��
	if(!ReadProcessMemory(g_hProc,lpAddress,&byBack,1,&dwRet))
	{
		DWORD OldProtect	= 0;//��Ҫ��ֵ,������ű�������
		PDWORD lpOldProtect =&OldProtect;

		DWORD OldProtectB	= 0;//һ������ʹ�õ�ֵ,���ڴ�ű�������
		PDWORD  lpOldProtectB=&OldProtectB;

		OutputDebug(L"���ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
		if(0!=VirtualProtectEx(g_hProc,lpAddress,4*1024,PAGE_READWRITE,lpOldProtect))
		{

			if(!ReadProcessMemory(g_hProc,lpAddress,&byBack,1,&dwRet))
			{
				OutputDebug(L"���ڴ��ַ���Գɹ���,���ڴ��ַ��Ȼ!!ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
				//�ָ�ԭ����.
				if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}

				return false;
			}
			else
			{
				if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,&dwRet))
				{	
					OutputDebug(L"д�ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
					if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
					{
						OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
						return false;
					}
					return false;
				}

				Int3Back.push_back(byBack);
				Int3Addr.push_back(lpAddress);
				Int3Value.push_back(true);

				if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}

				return true;
			}
			OutputDebug(L"�����ڴ��ַ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,������Ϊ:%d,������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
			return false;
		}
	}
	//д��int3�ϵ�
	if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,&dwRet))
	{	
		OutputDebug(L"д�ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
		DWORD OldProtect	= 0;//��Ҫ��ֵ,������ű�������
		PDWORD lpOldProtect =&OldProtect;

		DWORD OldProtectB	= 0;//һ������ʹ�õ�ֵ,���ڴ�ű�������
		PDWORD  lpOldProtectB=&OldProtectB;

		if(0!=VirtualProtectEx(g_hProc,lpAddress,4*1024,PAGE_READWRITE,lpOldProtect))
		{
			if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,&dwRet))
			{
				OutputDebug(L"д�ڴ��ַʧ��,��ַΪ:%p,FILE: %s, LINE: %d��",lpAddress,__FILE__,__LINE__);
				if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}
				return false;
			}
			Int3Back.push_back(byBack);
			Int3Addr.push_back(lpAddress);
			Int3Value.push_back(true);

			if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
			{
				OutputDebug(L"�����ڴ��ַ�����Ա��ԭ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,ԭ����Ϊ:%d,Ŀ������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
				return false;
			}

			return true;
		}
		else
		{
			OutputDebug(L"�����ڴ��ַ����ʧ��,��ַΪ:%p,FILE: %s, LINE: %d��,������Ϊ:%d,������Ϊ:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
			return false;
		}

	}
	Int3Back.push_back(byBack);
	Int3Addr.push_back(lpAddress);
	Int3Value.push_back(true);

	return true;
}



DWORD OnExceptionDebugEvent(LPEXCEPTION_DEBUG_INFO pDbgInfo)
{
	Breaking=true;//�����жϱ�־

	EXCEPTION_RECORD* pstcExp = &pDbgInfo->ExceptionRecord;

	DWORD	dwExpCode	= pstcExp->ExceptionCode;	//�쳣����
	PVOID	lpExpAddr	= pstcExp->ExceptionAddress;//�쳣��ַ
	
	switch (dwExpCode)
	{
		//�Ƿ������쳣
	case EXCEPTION_ACCESS_VIOLATION:
		{
			break;
		}
		//�ϵ��쳣
	case EXCEPTION_BREAKPOINT:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
			//��ʾ�ж���Ϣ
			//�Ĵ�����Ϣ
			//�������Ϣ
			CString stPrint=L"������Int3,ʹ��g�������\n";
			Printf2UI(stPrint,MINIF_INT3);
			
			ListRegister();

			DisplayAntiASM(lpExpAddr,10);


			//�жϸöϵ��Ƿ���Ԥ���ϵ��б�,
			//�����,����Breaking=false��ʱ��,��Ҫʹ����ϵ���Ч.
			//�رնϵ�Breaking=false;
			//Ȼ�󷵻�DBG_CONTINUE
			//���������ȴ�Breaking=falseȻ��break;

			//���ȵȴ�
			while (true)
			{
				Sleep(500);
				if (false==Breaking)
				{
					break;
				}
				AutoAnalysisCommandParsing();
			}

			int iTempIndex=SerarchInt3(lpExpAddr);
			if (-1!=iTempIndex)
			{
				if(false==InvalidInt3(iTempIndex))
				{
					break;
				}
				Breaking = false;
				return DBG_CONTINUE;
			}
			else
			{
				break;
			}

			break;
		}
		//�ڴ�����쳣
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		{
			break;
		}
		//��Чָ��
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		{
			break;
		}
		//�������
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		{
			break;
		}
		//ָ�֧�ֵ�ǰģʽ
	case EXCEPTION_PRIV_INSTRUCTION:
		{
			break;
		}
		//������Ӳ���ϵ��쳣
	case EXCEPTION_SINGLE_STEP:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
			//��ʾ�ж���Ϣ
			//�Ĵ�����Ϣ
			//�������Ϣ
			CString stPrint=L"������Int3,ʹ��g�������\n";
			Printf2UI(stPrint,MINIF_STEP_BY_STEP);

			ListRegister();

			DisplayAntiASM(lpExpAddr,10);

			while (true)
			{
				Sleep(500);
				if (false==Breaking)
				{
					break;
				}
				AutoAnalysisCommandParsing();
			}

			return DBG_CONTINUE;



			break;
		}

	default:
		break;
	}

	Breaking = false;//!!!���汾������ÿһ��return ��Ҫ���������.���ر��жϱ�־.

	return DBG_EXCEPTION_NOT_HANDLED;////DBG_CONTINUE//�쳣�Ѿ�����

}


//�����ַ�������������
bool Deal_ODSE()
{
	LPVOID lpDebugString = NULL;
	DWORD dwSizeofDebugString=0;
	DWORD dwRet =0;


	lpDebugString = DbgEvt.u.DebugString.lpDebugStringData;
	dwSizeofDebugString = DbgEvt.u.DebugString.nDebugStringLength;
	
	BYTE byDebugStringBuffer[MAX_PATH];
	char cDebugStringBuffer[MAX_PATH+1]={0};
	



	if (dwSizeofDebugString<=MAX_PATH)
	{
		if(0==ReadProcessMemory(g_hProc,lpDebugString,&byDebugStringBuffer,MAX_PATH,&dwRet))
		{
			return false;
		}
		else
		{
			strcpy_s(cDebugStringBuffer,(char*)byDebugStringBuffer);

			DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, cDebugStringBuffer, -1, NULL, 0);
			wchar_t *pwText=new wchar_t[dwNum];
			if(!pwText)
			{
				delete []pwText;
			}
			else
			{
				MultiByteToWideChar (CP_ACP, 0, cDebugStringBuffer, -1, pwText, dwNum);

				CString csStr;
				for(unsigned int i=0; i<wcslen(pwText); i++)  
				{
					csStr.AppendChar(pwText[i]);  
				}

				Printf2UI(csStr,MINIF_DEBUG_STRING);//���ýӿ�

				delete []pwText;
			}

			return true;
		}
	}
	else
	{
		if(0==ReadProcessMemory(g_hProc,lpDebugString,&byDebugStringBuffer,dwSizeofDebugString,&dwRet))
		{
			return false;
		}
		else
		{
			strcpy_s(cDebugStringBuffer,(char*)byDebugStringBuffer);
			DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, cDebugStringBuffer, -1, NULL, 0);
			wchar_t *pwText=new wchar_t[dwNum];
			if(!pwText)
			{
				delete []pwText;
			}
			else
			{
				MultiByteToWideChar (CP_ACP, 0, cDebugStringBuffer, -1, pwText, dwNum);

				CString csStr;
				for(unsigned int i=0; i<wcslen(pwText); i++)  
				{
					csStr.AppendChar(pwText[i]);  
				}

				Printf2UI(csStr,MINIF_DEBUG_STRING);//���ýӿ�

				delete []pwText;
			}

			return true;
		}
	}


}

//���������˳�
bool Deal_EPDE()
{
	OutputDebug(L"���������˳�");

	CString stPrint=L"���������˳�";
	Printf2UI(stPrint,MINIF_TIPS);

	return true;
}

//�̴߳���
bool Deal_CTDE()
{
	wchar_t swPrintBuffer[200]={0};

	OutputDebug(L"�̴߳���: ��ַ:0x%p OEP:0x%p ���:0x%p",
		DbgEvt.u.CreateThread.lpThreadLocalBase,DbgEvt.u.CreateThread.lpStartAddress,DbgEvt.u.CreateThread.hThread);


	wsprintfW(swPrintBuffer,L"�̴߳���: ��ַ:0x%p OEP:0x%p ���:0x%p",
		DbgEvt.u.CreateThread.lpThreadLocalBase,DbgEvt.u.CreateThread.lpStartAddress,DbgEvt.u.CreateThread.hThread);

	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}

	Printf2UI(csStr,MINIF_TIPS);

	return true;

}

//�߳��˳�
bool Deal_ETDE()
{
	wchar_t swPrintBuffer[200]={0};

	OutputDebug(L"�߳��˳�:		�߳�ID:0x%p		�˳���:0x%p",
		DbgEvt.dwThreadId,DbgEvt.u.ExitThread.dwExitCode);

	wsprintfW(swPrintBuffer,L"�߳��˳�:		�߳�ID:0x%p		�˳���:0x%p",
		DbgEvt.dwThreadId,DbgEvt.u.ExitThread.dwExitCode);
	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}

	Printf2UI(csStr,MINIF_TIPS);

	return true;

}


//ģ�����
bool Deal_LDDE()
{
	wchar_t swPrintBuffer[MAX_PATH+64]={0};

	OutputDebug(L"ģ�����:		��ַ:Ox%p	%s",
		DbgEvt.u.LoadDll.lpBaseOfDll,DbgEvt.u.LoadDll.lpImageName);//����û���ж��Ƿ���Unicode����.fUnicode���.

	wsprintfW(swPrintBuffer,L"ģ�����:		��ַ:Ox%p	%s",
		DbgEvt.u.LoadDll.lpBaseOfDll,(wchar_t*)DbgEvt.u.LoadDll.lpImageName);

	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}


	Printf2UI(csStr,MINIF_MODULE);

	return true;
}

//ģ��ж��
bool Deal_UDDE()
{
	wchar_t swPrintBuffer[MAX_PATH+64]={0};

	OutputDebug(L"ģ��ж��:		��ַ:Ox%p",
		DbgEvt.u.UnloadDll.lpBaseOfDll);//����û���ж��Ƿ���Unicode����.fUnicode���.

	wsprintfW(swPrintBuffer,L"ģ�����:		��ַ:Ox%p",
		DbgEvt.u.UnloadDll.lpBaseOfDll);
	
	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}


	Printf2UI(csStr,MINIF_MODULE);

	return true;
}




//�г����м�¼��Int3��¼.
bool ListInt3()
{
	CString csTempA;
	bool bA=false;
	for (DWORD i=0;i<Int3Addr.size();++i)
	{	
		bA=true;
		
		csTempA.Format(L"%u",(DWORD)Int3Addr[i]);
		if(false==Printf2UI(csTempA,MINIF_INT3))
		{
			return false;
		}
	}

	//�б�Ϊ��
	if (false==bA)
	{
		csTempA = L"����:��ǰû���κζϵ㱻��¼.";
		if(false==Printf2UI(csTempA,MINIF_WARMING))
		{
			return false;
		}
	}

	return true;


}



//ջ���ݵݹ���Ѱ
void kRecur(DWORD* dwEbp)
{
	if(nullptr==dwEbp)
	{return;}

	DWORD dwNewEbp=0;

	if(	false==ReadDebuggedMemory((PVOID)dwEbp,4,_Out_ (BYTE*)(&dwNewEbp)))
	{
		return ;
	}

	DWORD dwFunReturnPath=0;

	if (false==ReadDebuggedMemory((PVOID)(dwEbp+1),4,_Out_ (BYTE*)(&dwFunReturnPath)))
	{

		return;
	}
	//printf("�������ص�ַ:%p\r\n",(DWORD)dwFunReturnPath);


	wchar_t swPrintBuffer[200]={0};

	OutputDebug(L"���ص�ַ:		0x%p\r\n",
		dwFunReturnPath);

	wsprintfW(swPrintBuffer,L"���ص�ַ:		0x%p",
		dwFunReturnPath);
	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}

	Printf2UI(csStr,MINIF_STACK_TRACE);



	if (0!=(DWORD)dwNewEbp)
	{
		kRecur((DWORD*)dwNewEbp);
	}


	return ;

}


/*
//��;:
ͨ�ö����������ڴ溯��.��ʧ�ܲ����ı�Ȩ�޳���.
//����
LPVOID	lpAddress	//��ʼ����ַ
DWORD	dwGetNumber	//ϣ����õ��ڴ������ֽ�����.
_Out_	BYTE* wcGetValue	//���ڴ�����ݵĵط�.����:BYTE byA[20]�е�byA;
*/

bool ReadDebuggedMemory(LPVOID lpAddress,DWORD dwGetNumber,_Out_ BYTE* wcGetValue)
{

	DWORD	dwRetN		= 0;

	if(!ReadProcessMemory(g_hProc,lpAddress,wcGetValue,dwGetNumber,&dwRetN))
	{
		return false;
	}
	return true;
}

bool WriteDebuggedMemory(LPVOID lpAddress,DWORD dwSetNumber,_In_ BYTE* wcSetValue)
{

	DWORD	dwRetN		= 0;

	if(!WriteProcessMemory(g_hProc,lpAddress,wcSetValue,dwSetNumber,&dwRetN))
	{
		return false;
	}
	return true;
}

bool kStackTrace()
{
	//��ȡ����������
	CONTEXT stcCxt = {CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS};
	HANDLE hTargetThread;

	hTargetThread= OpenThread(THREAD_ALL_ACCESS , FALSE, dwNowThread);        //�߳̾��

	if (NULL==hTargetThread)
	{
		return false;
	}


	if (NULL==GetThreadContext(hTargetThread, &stcCxt))
	{
		return false;
	}

	
	kRecur((DWORD*)stcCxt.Ebp);

	return true;
};

/*
�޸ı����Խ�����ָ����ַ������Ϊָ�����ݡ�


0x12345678 0x40
*/
void ebMemory(CString Arg)
{
	int index = Arg.Find(_T(" "));
	CString address = Arg.Left(index);
	CString data	= Arg.Right(Arg.GetLength()-index-1);

	unsigned int m_add = wcstol(address.GetBuffer(0),NULL,16);
	unsigned int m_data = wcstol(data.GetBuffer(0),NULL,16);


	if (WriteDebuggedMemory((LPVOID)m_add,1,(BYTE*)&m_data))
	{
		Printf2UI(L"дĿ���ڴ�ɹ�",MINIF_TIPS);
	}
	else
	{
		Printf2UI(L"дĿ���ڴ�ʧ��",MINIF_TIPS);
	}

}