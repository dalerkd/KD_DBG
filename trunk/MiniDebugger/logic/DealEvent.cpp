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
HANDLE g_hProc;//被调试者的句柄
extern bool Breaking;
extern DWORD dwNowThread;


//////////////////////////////////////////////////////////////////////////
//一组用于保存int3状态的向量
vector<BYTE>Int3Back;	//原值
vector<void*>Int3Addr;	//地址
vector<bool>Int3Value;  //该断点是否有效?
//////////////////////////////////////////////////////////////////////////


//进程创建处理.
bool Deal_CPEV()
{
	OutputDebug(L"进程创建事件");

	//OEP地址
	LPVOID	lpIntAddress	= DbgEvt.u.CreateProcessInfo.lpStartAddress;

	g_hProc   = OpenProcess(PROCESS_ALL_ACCESS,0,DbgEvt.dwProcessId);
	if (NULL==g_hProc)
	{
		OutputDebug(L"打开进程失败,进程为:%p,FILE: %s, LINE: %d…",DbgEvt.dwProcessId,__FILE__,__LINE__);
		return true;
	}

	//在OEP放一个软件断点.
	//WriteInt3(lpIntAddress);

	return true;
}

//使Int3断点无效
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

//使Int3断点有效
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

//删除Int3断点
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

//恢复Int3下断前的值,但是不操作vector
bool ClearInt3(DWORD dwIndex)
{ 
	if ((dwIndex<Int3Addr.size())&&(!Int3Addr.empty()))
	{
		DWORD	dwRet	= 0;	
		//写入int3断点
		if(!WriteProcessMemory(g_hProc,Int3Addr[dwIndex],&Int3Back,1,&dwRet))
		{	
			OutputDebug(L"写内存地址失败,地址为:%p,FILE: %s, LINE: %d…",Int3Addr[dwIndex],__FILE__,__LINE__);

			DWORD OldProtect	= 0;//重要的值,用来存放保护属性
			PDWORD lpOldProtect =&OldProtect;

			DWORD OldProtectB	= 0;//一个不会使用的值,用于存放保护属性
			PDWORD  lpOldProtectB=&OldProtectB;

			if(0!=VirtualProtectEx(g_hProc,Int3Addr[dwIndex],4*1024,PAGE_READWRITE,lpOldProtect))
			{
				if(!WriteProcessMemory(g_hProc,Int3Addr[dwIndex],&Int3Back,1,&dwRet))
				{
					OutputDebug(L"写内存地址失败,地址为:%p,FILE: %s, LINE: %d…",Int3Addr[dwIndex],__FILE__,__LINE__);
					if(0==VirtualProtectEx(g_hProc,Int3Addr[dwIndex],4*1024,*lpOldProtect,lpOldProtectB))
					{
						OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",Int3Addr[dwIndex],__FILE__,__LINE__,OldProtect);
						return false;
					}
					return false;
				}

				if(0==VirtualProtectEx(g_hProc,Int3Addr[dwIndex],4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",Int3Addr[dwIndex],__FILE__,__LINE__,OldProtect);
					return false;
				}

				return true;
			}
			else
			{
				OutputDebug(L"更改内存地址属性失败,地址为:%p,FILE: %s, LINE: %d…,旧属性为:%d,新属性为:PAGE_READWRITE",Int3Addr[dwIndex],__FILE__,__LINE__,OldProtect);
				return false;
			}
		}
		return true;
	}

	return false;
}


//搜索指定地址是否存在于已记录Int3列表,存在则返回下标.-1没找到.
int SerarchInt3(LPVOID lpAddress)
{
	vector<void*>::iterator it;

	int iIndex=0;//序号
	for (it=Int3Addr.begin();it<Int3Addr.end();++it,++iIndex)
	{
		if (*it==lpAddress)
		{
			return iIndex;
		}
	}

	return -1;//空,或者找不到

}


//写入int3断点,如果备份则使其有效.传入需要写断点的地址
//返回值:false不代表完全的失败.还得看具体情况,例如其它工作已经完成.只是原来的内存页属性没办法写回.
bool WriteInt3(LPVOID	lpAddress)
{
	BYTE	byNowValue	= 0;//盛放目标内存的探测值
	DWORD	dwRetN		= 0;
	BYTE	byInt3		= 0xCC;

	int iIndex=SerarchInt3(lpAddress);


	if (-1!=iIndex)
	{
		if(false==Int3Value[iIndex])
		{
			//判断目标位置是否还是0xCC.不是则修改.

			if(!ReadProcessMemory(g_hProc,lpAddress,&byNowValue,1,&dwRetN))
			{
				DWORD OldProtect	= 0;//重要的值,用来存放保护属性
				PDWORD lpOldProtect =&OldProtect;

				DWORD OldProtectB	= 0;//一个不会使用的值,用于存放保护属性
				PDWORD  lpOldProtectB=&OldProtectB;

				OutputDebug(L"读内存地址失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
				if(0!=VirtualProtectEx(g_hProc,lpAddress,4*1024,PAGE_READWRITE,lpOldProtect))
				{

					if(!ReadProcessMemory(g_hProc,lpAddress,&byNowValue,1,&dwRetN))
					{
						OutputDebug(L"改内存地址属性成功后,读内存地址竟然!!失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
						//恢复原属性.
						if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
						{
							OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
							return false;
						}
						return false;
					}
					else
					{
						if (byInt3!=byNowValue)
						{
							//!!这里将检测到的非0xCC的值写入到了vector中,而没有考虑此值和vector原来的值的关系.
							if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,(SIZE_T*)&Int3Back[iIndex]))
							{	
								OutputDebug(L"写内存地址失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
								if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
								{
									OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
									return false;
								}
								return false;
							}
							if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
							{
								OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
								return false;
							}
							return true;
						}				
						Int3Value[iIndex]=true;
						if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
						{
							OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
							return false;
						}
						return true;
					}

				}
				else
				{
					OutputDebug(L"更改内存地址属性失败,地址为:%p,FILE: %s, LINE: %d…,旧属性为:%d,新属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}

			}
			else
			{
				if(byInt3!=byNowValue)
				{
					if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,(SIZE_T*)&Int3Back[iIndex]))
					{	
						OutputDebug(L"写内存地址失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
						return false;
					}

				}
				//读内存成功则修改Int3Value,否则false
				Int3Value[iIndex]=true;
				return  true;
			}
		}
		else
		{
			return true;//该断点现在已经有效.
		}
		return true;
	}
	//这个断点未记录过.进入记录环节:


	DWORD	dwRet	= 0;
	BYTE	byBack	= 0;

	//备份原指令
	if(!ReadProcessMemory(g_hProc,lpAddress,&byBack,1,&dwRet))
	{
		DWORD OldProtect	= 0;//重要的值,用来存放保护属性
		PDWORD lpOldProtect =&OldProtect;

		DWORD OldProtectB	= 0;//一个不会使用的值,用于存放保护属性
		PDWORD  lpOldProtectB=&OldProtectB;

		OutputDebug(L"读内存地址失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
		if(0!=VirtualProtectEx(g_hProc,lpAddress,4*1024,PAGE_READWRITE,lpOldProtect))
		{

			if(!ReadProcessMemory(g_hProc,lpAddress,&byBack,1,&dwRet))
			{
				OutputDebug(L"改内存地址属性成功后,读内存地址竟然!!失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
				//恢复原属性.
				if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}

				return false;
			}
			else
			{
				if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,&dwRet))
				{	
					OutputDebug(L"写内存地址失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
					if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
					{
						OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
						return false;
					}
					return false;
				}

				Int3Back.push_back(byBack);
				Int3Addr.push_back(lpAddress);
				Int3Value.push_back(true);

				if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}

				return true;
			}
			OutputDebug(L"更改内存地址属性失败,地址为:%p,FILE: %s, LINE: %d…,旧属性为:%d,新属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
			return false;
		}
	}
	//写入int3断点
	if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,&dwRet))
	{	
		OutputDebug(L"写内存地址失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
		DWORD OldProtect	= 0;//重要的值,用来存放保护属性
		PDWORD lpOldProtect =&OldProtect;

		DWORD OldProtectB	= 0;//一个不会使用的值,用于存放保护属性
		PDWORD  lpOldProtectB=&OldProtectB;

		if(0!=VirtualProtectEx(g_hProc,lpAddress,4*1024,PAGE_READWRITE,lpOldProtect))
		{
			if(!WriteProcessMemory(g_hProc,lpAddress,&byInt3,1,&dwRet))
			{
				OutputDebug(L"写内存地址失败,地址为:%p,FILE: %s, LINE: %d…",lpAddress,__FILE__,__LINE__);
				if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
				{
					OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
					return false;
				}
				return false;
			}
			Int3Back.push_back(byBack);
			Int3Addr.push_back(lpAddress);
			Int3Value.push_back(true);

			if(0==VirtualProtectEx(g_hProc,lpAddress,4*1024,*lpOldProtect,lpOldProtectB))
			{
				OutputDebug(L"更改内存地址的属性变回原属性失败,地址为:%p,FILE: %s, LINE: %d…,原属性为:%d,目的属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
				return false;
			}

			return true;
		}
		else
		{
			OutputDebug(L"更改内存地址属性失败,地址为:%p,FILE: %s, LINE: %d…,旧属性为:%d,新属性为:PAGE_READWRITE",lpAddress,__FILE__,__LINE__,OldProtect);
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
	Breaking=true;//启动中断标志

	EXCEPTION_RECORD* pstcExp = &pDbgInfo->ExceptionRecord;

	DWORD	dwExpCode	= pstcExp->ExceptionCode;	//异常代码
	PVOID	lpExpAddr	= pstcExp->ExceptionAddress;//异常地址
	
	switch (dwExpCode)
	{
		//非法访问异常
	case EXCEPTION_ACCESS_VIOLATION:
		{
			break;
		}
		//断点异常
	case EXCEPTION_BREAKPOINT:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
			//显示中断信息
			//寄存器信息
			//反汇编信息
			CString stPrint=L"发生了Int3,使用g命令继续\n";
			Printf2UI(stPrint,MINIF_INT3);
			
			ListRegister();

			DisplayAntiASM(lpExpAddr,10);


			//判断该断点是否在预定断点列表,
			//如果是,则在Breaking=false的时候,需要使这个断点无效.
			//关闭断点Breaking=false;
			//然后返回DBG_CONTINUE
			//如果不是则等待Breaking=false然后break;

			//首先等待
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
		//内存对齐异常
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		{
			break;
		}
		//无效指令
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		{
			break;
		}
		//除零错误
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		{
			break;
		}
		//指令不支持当前模式
	case EXCEPTION_PRIV_INSTRUCTION:
		{
			break;
		}
		//单步或硬件断点异常
	case EXCEPTION_SINGLE_STEP:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
			//显示中断信息
			//寄存器信息
			//反汇编信息
			CString stPrint=L"发生了Int3,使用g命令继续\n";
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

	Breaking = false;//!!!警告本函数中每一个return 需要包含此语句.来关闭中断标志.

	return DBG_EXCEPTION_NOT_HANDLED;////DBG_CONTINUE//异常已经处理

}


//调试字符串输出处理程序
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

				Printf2UI(csStr,MINIF_DEBUG_STRING);//调用接口

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

				Printf2UI(csStr,MINIF_DEBUG_STRING);//调用接口

				delete []pwText;
			}

			return true;
		}
	}


}

//被调试者退出
bool Deal_EPDE()
{
	OutputDebug(L"被调试者退出");

	CString stPrint=L"被调试者退出";
	Printf2UI(stPrint,MINIF_TIPS);

	return true;
}

//线程创建
bool Deal_CTDE()
{
	wchar_t swPrintBuffer[200]={0};

	OutputDebug(L"线程创建: 基址:0x%p OEP:0x%p 句柄:0x%p",
		DbgEvt.u.CreateThread.lpThreadLocalBase,DbgEvt.u.CreateThread.lpStartAddress,DbgEvt.u.CreateThread.hThread);


	wsprintfW(swPrintBuffer,L"线程创建: 基址:0x%p OEP:0x%p 句柄:0x%p",
		DbgEvt.u.CreateThread.lpThreadLocalBase,DbgEvt.u.CreateThread.lpStartAddress,DbgEvt.u.CreateThread.hThread);

	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}

	Printf2UI(csStr,MINIF_TIPS);

	return true;

}

//线程退出
bool Deal_ETDE()
{
	wchar_t swPrintBuffer[200]={0};

	OutputDebug(L"线程退出:		线程ID:0x%p		退出码:0x%p",
		DbgEvt.dwThreadId,DbgEvt.u.ExitThread.dwExitCode);

	wsprintfW(swPrintBuffer,L"线程退出:		线程ID:0x%p		退出码:0x%p",
		DbgEvt.dwThreadId,DbgEvt.u.ExitThread.dwExitCode);
	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}

	Printf2UI(csStr,MINIF_TIPS);

	return true;

}


//模块加载
bool Deal_LDDE()
{
	wchar_t swPrintBuffer[MAX_PATH+64]={0};

	OutputDebug(L"模块加载:		基址:Ox%p	%s",
		DbgEvt.u.LoadDll.lpBaseOfDll,DbgEvt.u.LoadDll.lpImageName);//这里没有判断是否是Unicode类型.fUnicode标记.

	wsprintfW(swPrintBuffer,L"模块加载:		基址:Ox%p	%s",
		DbgEvt.u.LoadDll.lpBaseOfDll,(wchar_t*)DbgEvt.u.LoadDll.lpImageName);

	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}


	Printf2UI(csStr,MINIF_MODULE);

	return true;
}

//模块卸载
bool Deal_UDDE()
{
	wchar_t swPrintBuffer[MAX_PATH+64]={0};

	OutputDebug(L"模块卸载:		基址:Ox%p",
		DbgEvt.u.UnloadDll.lpBaseOfDll);//这里没有判断是否是Unicode类型.fUnicode标记.

	wsprintfW(swPrintBuffer,L"模块加载:		基址:Ox%p",
		DbgEvt.u.UnloadDll.lpBaseOfDll);
	
	CString csStr;

	for(DWORD i=0; i<wcslen(swPrintBuffer); i++)  
	{
		csStr.AppendChar(swPrintBuffer[i]);  
	}


	Printf2UI(csStr,MINIF_MODULE);

	return true;
}




//列出所有记录的Int3记录.
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

	//列表为空
	if (false==bA)
	{
		csTempA = L"警告:当前没有任何断点被记录.";
		if(false==Printf2UI(csTempA,MINIF_WARMING))
		{
			return false;
		}
	}

	return true;


}



//栈回溯递归搜寻
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
	//printf("函数返回地址:%p\r\n",(DWORD)dwFunReturnPath);


	wchar_t swPrintBuffer[200]={0};

	OutputDebug(L"返回地址:		0x%p\r\n",
		dwFunReturnPath);

	wsprintfW(swPrintBuffer,L"返回地址:		0x%p",
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
//用途:
通用读被调试者内存函数.读失败不做改变权限尝试.
//参数
LPVOID	lpAddress	//起始读地址
DWORD	dwGetNumber	//希望获得的内存内容字节数量.
_Out_	BYTE* wcGetValue	//用于存放内容的地方.例如:BYTE byA[20]中的byA;
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
	//获取进程上下文
	CONTEXT stcCxt = {CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS};
	HANDLE hTargetThread;

	hTargetThread= OpenThread(THREAD_ALL_ACCESS , FALSE, dwNowThread);        //线程句柄

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
修改被调试进程中指定地址的数据为指定数据。


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
		Printf2UI(L"写目标内存成功",MINIF_TIPS);
	}
	else
	{
		Printf2UI(L"写目标内存失败",MINIF_TIPS);
	}

}