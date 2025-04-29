#include "BanBypass.h"
#include "Console.h"
#include "Framework.h"
#include "UEF.h"
#include "VEH.h"
#include "P2P.h"
#include "WPHook.h"
#include "Funcs.h"

EXTERN_C PVOID NTSYSAPI NTAPI RtlAddVectoredExceptionHandler(ULONG First,PVECTORED_EXCEPTION_HANDLER Handler);
HINSTANCE BaseAddress,BaseAddressGenuine;
wchar_t*BaseFileName,FullFilePath[512];
EXTERN_C BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			BaseAddress=hinstDLL;
			DisableThreadLibraryCalls(hinstDLL);
			BaseFileName=FullFilePath+GetModuleFileNameW(hinstDLL,FullFilePath,_countof(FullFilePath));
			while(BaseFileName-->FullFilePath+1&&*BaseFileName!=L'\\');
			RWConsole();
			SetUnhandledExceptionFilter(uef);
			RtlAddVectoredExceptionHandler(1, Beh);
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ACBypassThread, 0, 0, nullptr));
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)SlyAntiCheatThread, 0, 0, nullptr));
			break;
		case DLL_THREAD_ATTACH:
			WPHook();
			break;
		
	}
	return 1;
}

EXTERN_C void LoadGenuineDll()
{
	SetUnhandledExceptionFilter(uef);
	RtlAddVectoredExceptionHandler(1, Beh);
	WPHook();
	if(!BaseAddressGenuine)
	{
		static wchar_t filename[512];
		wcscpy(filename+GetSystemDirectoryW(filename,_countof(filename)),BaseFileName);
		BaseAddressGenuine=LoadLibraryW(filename);
		IMPORTBYNAME(DirectInput8Create,1);
		IMPORTBYNAME(DllCanUnloadNow,2);
		IMPORTBYNAME(DllGetClassObject,3);
		IMPORTBYNAME(DllRegisterServer,4);
		IMPORTBYNAME(DllUnregisterServer,5);
		IMPORTBYNAME(GetdfDIJoystick,6);
	}
}