#include "VEH.h"
#include "P2P.h"

LONG Beh(_EXCEPTION_POINTERS*Breakpoint) 
{
	if (Breakpoint->ExceptionRecord->ExceptionAddress == (char*)GetModuleHandleW(NULL) + 0x78E700) 
	{
		Breakpoint->ContextRecord->ContextFlags |= CONTEXT_DEBUG_REGISTERS;
		Breakpoint->ContextRecord->Dr0 = Breakpoint->ContextRecord->Dr1 = Breakpoint->ContextRecord->Dr2 = Breakpoint->ContextRecord->Dr3 = Breakpoint->ContextRecord->Dr6 = Breakpoint->ContextRecord->Dr7 = 0;
		void* rcx = (void*)Breakpoint->ContextRecord->Rcx;
		void* from = (void*)Breakpoint->ContextRecord->Rdx;
		int id = Breakpoint->ContextRecord->R8;
		char* buf = (char*)Breakpoint->ContextRecord->R9;
		unsigned maxlen = *(uintptr_t*)(Breakpoint->ContextRecord->Rsp + 40);
		Breakpoint->ContextRecord->Rax = OnReceivePacket(rcx, from, id, buf, maxlen);
		Breakpoint->ContextRecord->Rip += 1;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	if(Breakpoint -> ExceptionRecord -> ExceptionAddress == (char*)GetModuleHandleW(NULL) + 0x1995695)
	{ 
        long long steamid = *(long long*)((char*)Breakpoint -> ContextRecord -> Rbp - 0x40);
        unsigned* buffer = *(unsigned**)((char*)Breakpoint -> ContextRecord -> Rdi + 0x4d0);
        int recvlen = Breakpoint -> ContextRecord -> Rsi & 0xFFFFFFFF;
        bool isPacketValid = ValidateP2PPacket(steamid, buffer, recvlen);
        if (isPacketValid) 
		{
            Breakpoint -> ContextRecord -> R11 = (DWORD64)buffer;
            Breakpoint -> ContextRecord -> Rip += 7;
        } else {
			memset(buffer, 0, recvlen);
            Breakpoint -> ContextRecord -> Rip = (DWORD64)GetModuleHandleW(NULL) + 0x19958a9;
        }
        return EXCEPTION_CONTINUE_EXECUTION;
    }
  return EXCEPTION_CONTINUE_SEARCH;
}