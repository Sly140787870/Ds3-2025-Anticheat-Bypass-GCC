#pragma once
#include <windows.h>
#ifdef __cplusplus
#define call(f,...) (((char*(*)(...))(void*)(f))(__VA_ARGS__))
#else
#define call(f,...) (((char*(*)())(void*)(f))(__VA_ARGS__))
#endif
#define EmitJmp(from,to) _emitjmp((void*)(from),(void*)(to))
#define Emit(addr,type,...) do{type data[]={__VA_ARGS__};DWORD OldProtect;if(memcmp(data,(void*)(addr),sizeof data)&&VirtualProtectEx((HANDLE)-1,(void*)(addr),sizeof data,PAGE_EXECUTE_READWRITE,&OldProtect)){memcpy((void*)(addr),data,sizeof data);VirtualProtectEx((HANDLE)-1,(void*)(addr),sizeof data,OldProtect,&OldProtect);}}while(0)
#define EmitWithSize(addr,type,size,...)do{type data[]={__VA_ARGS__};DWORD OldProtect;if(memcmp(data,(void*)(addr),size)&&VirtualProtectEx((HANDLE)-1,(void*)(addr),size,PAGE_EXECUTE_READWRITE,&OldProtect)){memcpy((void*)(addr),data,size);VirtualProtectEx((HANDLE)-1,(void*)(addr),size,OldProtect,&OldProtect);}}while(0)
#define Fill(addr,type,value,count)do{DWORD OldProtect;if(VirtualProtectEx((HANDLE)-1,(void*)(addr),sizeof(type)*(count),PAGE_EXECUTE_READWRITE,&OldProtect)){for(int iter=0;iter<(count);++iter)((type*)addr)[iter]=(value);VirtualProtectEx((HANDLE)-1,(void*)(addr),sizeof(type)*(count),OldProtect,&OldProtect);}}while(0)
#define Nop(addr,size) Fill(addr,unsigned char,0x90,size)
#define Ret(addr) Fill(addr,unsigned char,0xC3,1)
#define BackUp(addr, backup, size) memcpy(backup, (void*)addr, size)
#define Restore(addr, backup, size) memcpy((void*)addr, backup, size)
#define EmitBFJ(from,to,reg) _EmitBFJ((unsigned char*)(from),(unsigned char*)(to),(reg))
EXTERN_C int(*__imp_wprintf)(const wchar_t* fmt, ...);
int memcmp_wc(PBYTE a, PBYTE b, unsigned len, PBYTE wcbits);
PBYTE convpattern(const char* pattern, unsigned* scanlen);
PBYTE aobscan(HANDLE hProcess, const char* Pattern, PBYTE StartAddress, PBYTE EndAddress, DWORD Protect);
int _emitjmp(HANDLE pHandle, void* from, void* to);
void _EmitBFJ(unsigned char* from, unsigned char* to, unsigned char reg);
void* Tramp64(void* src, void* dst, int len);
template<class ResultType, class PointerType>
ResultType* GetAddrs(PointerType BaseAddress)
{
	return (intptr_t)BaseAddress < 65536 || IsBadReadPtr((void*)BaseAddress, sizeof(ResultType)) ? 0 : (ResultType*)BaseAddress;
}
template<class ResultType, class PointerType, class OffsetType, class...OffsetTypes>
ResultType* GetAddrs(PointerType BaseAddress, OffsetType Offset, OffsetTypes... Offsets)
{
	return (intptr_t)BaseAddress < 65536 || (intptr_t)BaseAddress & (sizeof(void*) - 1) || IsBadReadPtr((void*)BaseAddress, sizeof(void*)) ? 0 : GetAddrs<ResultType>(*(char**)BaseAddress + Offset, Offsets...);
}

//Class
uintptr_t WorldChrMan{};