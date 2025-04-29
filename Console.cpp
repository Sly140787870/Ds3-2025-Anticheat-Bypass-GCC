#include "Console.h"
#include "Funcs.h"
void RWConsole()
{
	AllocConsole();
    _wfreopen(L"CONIN$",L"r", stdin);
    _wfreopen(L"CONOUT$",L"w", stdout);
    _wfreopen(L"CONOUT$",L"w", stderr);
	__imp_wprintf(L"[LOG] Created console.\n[INFO] BaseImage:0x%llX\n", GetImageBase());
}
void ConsoleToLog(const wchar_t* File, const wchar_t* Text)
{
	FILE* file = _wfopen(File, L"a");
    if (file)
    {
        _fwprintf_l(file, Text, 0);
        fclose(file);
    }
}