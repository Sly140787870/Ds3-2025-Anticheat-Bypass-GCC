#include "Mem.h"

#include "Memory.h"

int memcmp_wc(PBYTE a, PBYTE b, unsigned len, PBYTE wcbits)
{
    for (unsigned i = 0; i < len; ++i)if (a[i] != b[i] && (wcbits[i / 8] & 1 << i % 8) == 0)return a[i] - b[i];return 0;
}

PBYTE convpattern(const char* pattern, unsigned* scanlen)
{
    unsigned  c = 0;
    for (unsigned i = 0; pattern[i]; ++i)if (isspace(pattern[i]))++c;
    PBYTE addr = (PBYTE)HeapAlloc(GetProcessHeap(), 0, (c + 1) + (c + 1) / 8 + 1);
    if (addr) 
    {
        PBYTE wcbits = addr + c + 1;
        *scanlen = c + 1;
        unsigned i = 0;
        const char* p, * np;
        for (p = pattern; i < c + 1 && *p; p = np, ++i) 
        {
            while (isspace(*p))++p;
            if (*p == '?' || *p == 'x' || *p == 'X') 
            {
                wcbits[i / 8] |= 1 << i % 8;
                np = p + 1;
                while (*np && !isspace(*np)) 
                {
                    ++np;
                }
                continue;
            }
            else 
            {
                wcbits[i / 8] &= ~(1 << i % 8);
            }
            addr[i] = strtoul(p, (char**)&np, 16);
            if (p == np)break;
        }
        return addr;
    }
    return 0;
}

PBYTE aobscan(HANDLE hProcess, const char* Pattern, PBYTE StartAddress, PBYTE EndAddress, DWORD Protect)
{
    PBYTE Result = 0; unsigned scanlen = 0;
    PBYTE sb = convpattern(Pattern, &scanlen);
    PBYTE wcbits = sb + scanlen;
    MEMORY_BASIC_INFORMATION mbi;
    if (hProcess == 0)hProcess = (HANDLE)-1;
    BOOLEAN scanself = hProcess == (HANDLE)-1;
    for (PBYTE i = StartAddress; i < EndAddress && VirtualQueryEx(hProcess, i, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) && !Result; i = (PBYTE)mbi.BaseAddress + mbi.RegionSize) if (mbi.State == MEM_COMMIT && mbi.Protect & Protect && mbi.RegionSize >= scanlen) 
    {
        PBYTE buf = scanself ? i : (PBYTE)VirtualAllocEx((HANDLE)-1, 0, mbi.RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (buf != 0) 
        {
            if (scanself || ReadProcessMemory(hProcess, i, buf, (PBYTE)mbi.BaseAddress + mbi.RegionSize - i, NULL)) 
            {
                for (int j = 0; j <= mbi.RegionSize - scanlen; ++j) 
                {
                    if (memcmp_wc(sb, buf + j, scanlen, wcbits) == 0) 
                    {
                        Result = i + j; break;
                    }
                }
            }
            if (!scanself)VirtualFreeEx((HANDLE)-1, buf, 0, MEM_RELEASE);
        }
    }
    HeapFree(GetProcessHeap(), 0, sb);
    return Result;
}

int _emitjmp(HANDLE pHandle, void* from, void* to)
{
    if (*PDWORD(from) == 0x25FF && *PWORD((char*)from + 4) == 0 && *(void**)((char*)from + 6) == to)return NULL;
    DWORD oldprot;
    VirtualProtectEx((HANDLE)-1, from, 5, PAGE_EXECUTE_READWRITE, &oldprot);
    *PDWORD(from) = 0x25FF;
    *PWORD((char*)from + 4) = 0;
    *(void**)((char*)from + 6) = to;
    return VirtualProtectEx((HANDLE)-1, from, 5, oldprot, &oldprot);
}

void _EmitBFJ(unsigned char* from, unsigned char* to, unsigned char reg)
{
    DWORD OldProtect;
    if (reg < 16 && VirtualProtectEx((HANDLE)-1, from, 16, PAGE_EXECUTE_READWRITE, &OldProtect))
    {
        memcpy(from + 2, &to, sizeof(to));
        if (reg < 8)
        {
            from[0] = 0x48;
            from[1] = 0xb8 + reg;
            from[10] = 0xff;
            from[11] = 0xe0 + reg;
        }
        else {
            from[0] = 0x49;
            from[1] = 0xb0 + reg;
            from[10] = 0x41;
            from[11] = 0xff;
            from[12] = 0xd8 + reg;
        }
        VirtualProtectEx((HANDLE)-1, from, 16, OldProtect, &OldProtect);
    }
}

void* Tramp64(void* src, void* dst, int len)
{
    int MinLen = 14;
    if (len < MinLen) return NULL;
		BYTE stub[] = {
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
    };
    void* pTrampoline = VirtualAlloc(0, len + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    DWORD dwOld = 0;
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwOld);
    uintptr_t retto = (uintptr_t)src + len;
    memcpy(stub + 6, &retto, 8);
    memcpy((void*)((uintptr_t)pTrampoline), src, len);
    memcpy((void*)((uintptr_t)pTrampoline + len), stub, sizeof(stub));
    memcpy(stub + 6, &dst, 8);
    memcpy(src, stub, sizeof(stub));
    for (int i = MinLen; i < len; i++)
    {
        *(BYTE*)((uintptr_t)src + i) = 0x90;
    }
    VirtualProtect(src, len, dwOld, &dwOld);
    return (void*)((uintptr_t)pTrampoline);
}
