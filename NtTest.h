#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#define INRANGE(x, a, b)   (x >= a && x <= b)
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
// exports

// TEB through masm
extern "C" void* _NtCurrentTeb(void);

// PEB STRUCTS
struct CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
};

// Required structures without headers
struct _UNICODE_STRING {
    unsigned short Length;
    unsigned short MaximumLength;
    wchar_t* Buffer;
};

struct __LIST_ENTRY {
    __LIST_ENTRY* Flink;
    __LIST_ENTRY* Blink;
};

struct _PEB_LDR_DATA {  ////___PPEB_LDR_DATA
    DWORD Length;
    unsigned char Initialized;
    void* SsHandle;
    __LIST_ENTRY InLoadOrderModuleList;
    __LIST_ENTRY InMemoryOrderModuleList;
    __LIST_ENTRY InInitializationOrderModuleList;
};

struct _LDR_DATA_TABLE_ENTRY {
    __LIST_ENTRY InLoadOrderLinks;
    __LIST_ENTRY InMemoryOrderLinks;
    __LIST_ENTRY InInitializationOrderLinks;
    void* DllBase;
    void* EntryPoint;
    DWORD SizeOfImage;
    _UNICODE_STRING FullDllName;
    _UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;;

typedef struct _PEB {
    unsigned char InheritedAddressSpace;
    unsigned char ReadImageFileExecOptions;
    unsigned char BeingDebugged;
    union {
        unsigned char BitField;
        struct {
            unsigned char ImageUsesLargePages : 1;
            unsigned char IsProtectedProcess : 1;
            unsigned char IsImageDynamicallyRelocated : 1;
            unsigned char SkipPatchingUser32Forwarders : 1;
            unsigned char IsPackagedProcess : 1;
            unsigned char IsAppContainer : 1;
            unsigned char IsProtectedProcessLight : 1;
            unsigned char IsLongPathAwareProcess : 1;
        };
    };
    void* Mutant;
    void* ImageBaseAddress;
    _PEB_LDR_DATA* Ldr; 
} ___PEB, * ___PPEB;

// TEB STRUCTS
struct TEB
{
    NT_TIB NtTib;
    PVOID EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    struct PEB* ProcessEnvironmentBlock;
};

#ifdef _WIN64
asm("_NtCurrentTeb:movq %gs:0x30,%rax;ret");
#else
asm("_NtCurrentTeb:movl %fs:0x18,%eax;ret");
#endif

// Get Peb from Teb
inline TEB* __NtCurrentTeb()
{
    return (struct TEB*)_NtCurrentTeb();
}

// Cross-platform image base getter
#ifdef _WIN64
inline uint64_t get_main_image_base() 
{
    _PEB* ProcessEnvironmentBlock = (_PEB*)((TEB*)__NtCurrentTeb())->ProcessEnvironmentBlock;
    return (uint64_t)ProcessEnvironmentBlock->ImageBaseAddress;
}

#else
inline uint32_t get_main_image_base() 
{
    _PEB* ProcessEnvironmentBlock = (_PEB*)((TEB*)__NtCurrentTeb())->ProcessEnvironmentBlock;
    return (uint32_t)ProcessEnvironmentBlock->ImageBaseAddress;
}
#endif

inline ___PPEB get_peb() {
    return reinterpret_cast<___PPEB>(((TEB*)__NtCurrentTeb())->ProcessEnvironmentBlock);
}

inline void* NtGetModuleHandle(const char* module_name) 
{
    if (!module_name)return nullptr;
#ifdef _WIN64
    ___PPEB peb = reinterpret_cast<___PPEB>(get_peb());
#else
    ___PPEB peb = reinterpret_cast<___PPEB>(get_peb());
#endif
	_PEB_LDR_DATA* ldr = peb->Ldr;
	__LIST_ENTRY* head = &ldr->InLoadOrderModuleList;
	__LIST_ENTRY* current = head->Flink;
	
	while (current != head) 
	{
		_LDR_DATA_TABLE_ENTRY* entry = CONTAINING_RECORD(current, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		char current_name[256] = { 0 };
		PWCHAR wide_name = entry->BaseDllName.Buffer;
        int name_len = entry->BaseDllName.Length / 2;

		for (int i = 0; i < name_len && i < 255; i++)current_name[i] = static_cast<char>(wide_name[i] & 0xFF);
		
		const char* a = module_name;
        const char* b = current_name;
        bool match = true;
		while (*a && *b) 
		{
            char ca = *a >= 'A' && *a <= 'Z' ? *a + 32 : *a;
            char cb = *b >= 'A' && *b <= 'Z' ? *b + 32 : *b;

            if (ca != cb) 
			{
                match = false;
                break;
            }
			
            ++a;
            ++b;
        }

        if (match && !*a && !*b) 
		{
            return entry->DllBase;
        }
		
        current = current->Flink;
	}
	
	// Write something here if we failed.
	//__imp_wprintf(L"[!] WARNING: Module '%s' not found! Prepare to crash.\n", module_name);
	return nullptr;
}

inline void* NtExportNoCRT(const char* module_name, const char* export_name) 
{
    // Get module base
    auto module_base = NtGetModuleHandle(module_name);
    if (!module_base)return nullptr;
	
    // Get DOS header
    auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
    if (dos_header->e_magic != 0x5A4D) return nullptr;  // "MZ"
	
	// Get NT headers
    auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<BYTE*>(module_base) + dos_header->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE)return nullptr;
	
    // Get export directory
    auto export_dir_rva = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (!export_dir_rva)return nullptr;
	
	auto export_dir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(reinterpret_cast<BYTE*>(module_base) + export_dir_rva);
	
	auto names = reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(module_base) + export_dir->AddressOfNames);
    auto ordinals = reinterpret_cast<WORD*>(reinterpret_cast<BYTE*>(module_base) + export_dir->AddressOfNameOrdinals);
    auto functions = reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(module_base) + export_dir->AddressOfFunctions);
	
	
	for (DWORD i = 0; i < export_dir->NumberOfNames; i++) 
	{
        const char* current_name = reinterpret_cast<const char*>(reinterpret_cast<BYTE*>(module_base) + names[i]);

        // Compare strings manually to avoid CRT
        const char* a = export_name;
        const char* b = current_name;
		
        while (*a && *b && *a == *b) 
		{
            ++a;
            ++b;
        }
		
        if (*a == 0 && *b == 0) 
		{
            // Found matching export
            WORD ordinal = ordinals[i];
            DWORD function_rva = functions[ordinal];
            return reinterpret_cast<void*>(reinterpret_cast<BYTE*>(module_base) + function_rva);
        }
    }
	
    return nullptr;
}

//
// REMEMBER
// Please keep in mind, when you call these you will still need to hide your strings :P - FightingSquirrel69 <3
//
template<typename T = void*, typename... Args>
inline T fnCaller(const char* szModule, const char* szExport, Args... args) 
{	
    auto fn = reinterpret_cast<T(*)(Args...)>(NtExportNoCRT(szModule, szExport));
    if (!fn) return T();
    return fn(args...);
}

inline void DbgBeep() {
    fnCaller<BOOL>("kernel32.dll", "Beep", 750, 300);
}

inline void NtAllocConsole()
{
	//BOOL __stdcall AllocConsole()
	fnCaller<BOOL>("kernel32.dll", "AllocConsole");
	freopen_s((FILE**)stdout, ("CONOUT$"), ("w"), stdout);
    freopen_s((FILE**)stdout, ("CONIN$"), ("r"), stdin);  
}

inline void DbgPrint(const char* message) 
{
    fnCaller<NTSTATUS>("ntdll.dll", "DbgPrint", message);
}

// Example: int   __stdcall MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
//       TYPE | MODULE IMPORT | FUNCNAME | ARGS
void NtMsgBox(const char* Msg, const char* Title)
{
	fnCaller<int>("User32.dll", "MessageBoxA", nullptr, Msg, Title, 0);	
}

inline void NtThreadPoolCallBack(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_CALLBACK_ENVIRON pcbe)
{
	fnCaller<BOOL>("Kernel32.dll", "TrySubmitThreadpoolCallback", Instance, 0, 0);
}

inline NTSTATUS NtRead(HANDLE process, PVOID base_address, PVOID buffer, SIZE_T size, SIZE_T* bytes_read = nullptr) 
{
    return fnCaller<NTSTATUS>("ntdll.dll", "NtReadVirtualMemory", process, base_address, buffer, size, bytes_read);
}

inline NTSTATUS NtWrite(HANDLE process, PVOID base_address, PVOID buffer, SIZE_T size, SIZE_T* bytes_written = nullptr) 
{
    return fnCaller<NTSTATUS>("ntdll.dll", "NtWriteVirtualMemory", process, base_address, buffer, size, bytes_written);
}

inline bool NtIsBadReadPtr(const void* ptr, UINT_PTR size)
{
	return fnCaller<BOOL>("Kernel32.dll", "IsBadReadPtr", ptr, size);
}

inline uintptr_t find_pattern(const char* module_name, const char* pattern)
{
	const auto get_module_size = [=](uintptr_t module_base) { return reinterpret_cast<PIMAGE_NT_HEADERS>(module_base + reinterpret_cast<PIMAGE_DOS_HEADER>(module_base)->e_lfanew)->OptionalHeader.SizeOfImage; };
	const auto module_start = (uintptr_t)NtGetModuleHandle(module_name);
    if (!module_start)return 0ULL;
	if (module_start != 0ULL)
    {
		const auto module_end = module_start + get_module_size(module_start);
		const char* pattern_current = pattern;
        uintptr_t current_match = 0;
#ifdef _WIN64
        MEMORY_BASIC_INFORMATION64 page_information = {};
#else
        MEMORY_BASIC_INFORMATION32 page_information = {};
#endif
		for (auto current_page = reinterpret_cast<unsigned char*>(module_start); current_page < reinterpret_cast<unsigned char*>(module_end); current_page = reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize))
		{
			fnCaller<SIZE_T>("kernel32.dll", "VirtualQuery", reinterpret_cast<LPCVOID>(current_page), reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page_information), sizeof(MEMORY_BASIC_INFORMATION));
			
			if (page_information.Protect == PAGE_NOACCESS)continue;
            if (page_information.State != MEM_COMMIT)continue;
            if (page_information.Protect & PAGE_GUARD)continue;
			for (auto current_address = reinterpret_cast<unsigned char*>(page_information.BaseAddress); current_address < reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize - 0x8); current_address++)
			{
				if (*current_address != GET_BYTE(pattern_current) && *pattern_current != '\?')
				{
					current_match = 0ULL;
                    pattern_current = pattern;
                    continue;
                }
				if (!current_match)current_match = reinterpret_cast<uintptr_t>(current_address);
				pattern_current += 3;
                if (pattern_current[-1] == 0)return current_match;
			}
		}
	}
	return 0ULL;
}


