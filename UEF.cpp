#include "UEF.h"

LONG uef(_EXCEPTION_POINTERS* except)
{
	MEMORY_BASIC_INFORMATION mbi = {};
	uintptr_t ModuleBase = 0;
    if (VirtualQuery(except->ExceptionRecord->ExceptionAddress, &mbi, sizeof(mbi)))ModuleBase = (uintptr_t)mbi.AllocationBase;
	wchar_t moduleName[MAX_PATH] = L"Unknown Module";
    if (ModuleBase && GetModuleFileNameW((HMODULE)ModuleBase, moduleName, MAX_PATH));
	FILE*file=_wfopen(L"CrashLog.txt", L"a");
    if(file)
	{
		if(except->ExceptionRecord->ExceptionAddress==(char*)GetModuleHandleW(nullptr) + 0x1217B3F) //RandomCrash [Rdx+10] Dereferences from [r13+0] ->From Stack ([rsp+70])
		{
			except->ContextRecord->Rip=(DWORD64)GetModuleHandleW(nullptr) + 0x1217EAB;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		if(except->ExceptionRecord->ExceptionAddress==(char*)GetModuleHandleW(nullptr) + 0x1777ED2) //RandomCrash mov [00000000],00DEADBA)
		{
			except->ContextRecord->Rip=(DWORD64)GetModuleHandleW(nullptr) + 0x1777EDD;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		if (except->ExceptionRecord->ExceptionAddress) 
		{
			_fwprintf_l(file,
			L"|SlySpy-ErrorReport|\n"
			L"Fault type:0x%p\n"
			L"ImageBase:0x%016llX\n"
			L"Fault address:0x%p\n"
			L"Calculated offset:0x%X\n"
			L"Referenced memory 0x:%p\n"
			L"Faulting module:%s\n"
			L"Register dump\n"
			L"RAX:%p\n"
			L"RBX:%p\n"
			L"RCX:%p\n"
			L"RDX:%p\n"
			L"RSI:%p\n"
			L"RDI:%p\n"
			L"RBP:%p\n"
			L"R8:%p\n"
			L"R9:%p\n"
			L"R10:%p\n"
			L"R11:%p\n"
			L"R12:%p\n"
			L"R14:%p\n"
			L"R15:%p\n"
			L"Stack dump\n",
			0,
			except->ExceptionRecord->ExceptionCode,
			ModuleBase,
			except->ExceptionRecord->ExceptionAddress,
			(uintptr_t)except->ExceptionRecord->ExceptionAddress - (uintptr_t)ModuleBase,
			except->ExceptionRecord->ExceptionInformation[1],
			moduleName,
			except->ContextRecord->Rax,
			except->ContextRecord->Rbx,
			except->ContextRecord->Rcx,
			except->ContextRecord->Rdx,
			except->ContextRecord->Rsi,
			except->ContextRecord->Rdi,
			except->ContextRecord->Rbp,
			except->ContextRecord->Rsp,
			except->ContextRecord->R8,
			except->ContextRecord->R9,
			except->ContextRecord->R10,
			except->ContextRecord->R11,
			except->ContextRecord->R12,
			except->ContextRecord->R13,
			except->ContextRecord->R14,
			except->ContextRecord->R15);
			uintptr_t*rsp=(uintptr_t*)except->ContextRecord->Rsp;
			for(int i=0;i<64;++i)
			{
				if (!VirtualQuery(rsp, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT)break;
				uintptr_t address = rsp[i];
				if (address == 0) continue;
				wchar_t stackModuleName[MAX_PATH] = L"Unknown Module";
				if (VirtualQuery((void*)address, &mbi, sizeof(mbi)) && GetModuleFileNameW((HMODULE)mbi.AllocationBase, stackModuleName, MAX_PATH)) 
				{
					_fwprintf_l(file, L"[S%d] 0x%p (0x%X) (%s)\n", 0, i, (void*)address, (uintptr_t)address - (uintptr_t)ModuleBase, stackModuleName);
				} else {
					_fwprintf_l(file, L"[S%d] 0x%p (0x%X) (Unknown Module)\n", 0, i, (void*)address, (uintptr_t)address - (uintptr_t)ModuleBase);
				}
			}
			//for(int i=0;i<64&&!IsBadReadPtr(rsp,sizeof*rsp);++i)_fwprintf_l(file, L"S[%i]:%p ", 0, i, rsp[i]);
			_fwprintf_l(file, L"\n", 0);	
			fclose(file);
		}
    }
	return EXCEPTION_CONTINUE_SEARCH;
}