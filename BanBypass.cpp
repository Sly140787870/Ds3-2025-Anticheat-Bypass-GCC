#include "BanBypass.h"
#include "Mem.h"
#include "Funcs.h"
void BanBypass()	
{
	
}

DWORD WINAPI ACBypassThread(HMODULE hModule)
{
	__imp_wprintf(L"[INFO] Anticheat-bypass thread created\n");
	uintptr_t Ds3 =  (uintptr_t)GetModuleHandleW(NULL);
	auto RequestUpdatePlayerStatus_REF1 = (char*)Ds3 + 0x1B30A90;
	auto RequestUpdatePlayerStatus_REF2 = (char*)Ds3 + 0x1B2FAC0;
	auto StatusDelayedSendTask_REF1 = (char*)Ds3 + 0x1B2EAA0;
	auto StatusDelayedSendTask_REF2 = (char*)Ds3 + 0x1B2E510;
	auto GOTYOpcode = (char*)Ds3 + 0xA55124;
	DWORD oldProtect = 0;
	if (VirtualProtectEx((HANDLE)-1, RequestUpdatePlayerStatus_REF1, sizeof RequestUpdatePlayerStatus_REF1, PAGE_READWRITE, &oldProtect))
	{
		Emit(RequestUpdatePlayerStatus_REF1, unsigned char, 0xC3, 0x90);
		__imp_wprintf(L"[ANTI-BAN] wrote memory to RequestUpdatePlayerStatus packet (1/2)\n");
		VirtualProtectEx((HANDLE)-1, RequestUpdatePlayerStatus_REF1, sizeof RequestUpdatePlayerStatus_REF1, oldProtect, &oldProtect);
	}
	if (VirtualProtectEx((HANDLE)-1, RequestUpdatePlayerStatus_REF2, sizeof RequestUpdatePlayerStatus_REF2, PAGE_READWRITE, &oldProtect))
	{
		Emit(RequestUpdatePlayerStatus_REF2, unsigned char, 0xC3, 0x90);
		__imp_wprintf(L"[ANTI-BAN] wrote memory to RequestUpdatePlayerStatus packet (2/2)\n");
		VirtualProtectEx((HANDLE)-1, RequestUpdatePlayerStatus_REF2, sizeof RequestUpdatePlayerStatus_REF2, oldProtect, &oldProtect);
	}
	if (VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF1, sizeof StatusDelayedSendTask_REF1, PAGE_READWRITE, &oldProtect))
	{
		Emit(StatusDelayedSendTask_REF1, unsigned char, 0xC3, 0x90);
		__imp_wprintf(L"[ANTI-BAN] wrote memory to StatusDelayedSendTask packet (1/2)\n");
		VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF1, sizeof StatusDelayedSendTask_REF1, oldProtect, &oldProtect);
	}
	if (VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF2, sizeof StatusDelayedSendTask_REF2, PAGE_READWRITE, &oldProtect))
	{
		Emit(StatusDelayedSendTask_REF2, unsigned char, 0xC3, 0x90);
		__imp_wprintf(L"[ANTI-BAN] wrote memory to StatusDelayedSendTask packet (2/2)\n");
		VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF2, sizeof StatusDelayedSendTask_REF2, oldProtect, &oldProtect);
	}
	if (VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF2, sizeof StatusDelayedSendTask_REF2, PAGE_READWRITE, &oldProtect))
	{
		Emit(GetImageBase() + 0x22B7ADF, unsigned char, 0x30, 0xC0, 0x90);
		__imp_wprintf(L"[ANTI-BAN] stopped ban packet (1/1)\n");
		VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF2, sizeof StatusDelayedSendTask_REF2, oldProtect, &oldProtect);
	}
	if (VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF2, sizeof StatusDelayedSendTask_REF2, PAGE_READWRITE, &oldProtect))
	{
		Emit(GOTYOpcode, unsigned char, 0x83, 0xF8, 0x00);
		__imp_wprintf(L"[MISC] wrote memory to GOTY\n");
		VirtualProtectEx((HANDLE)-1, StatusDelayedSendTask_REF2, sizeof StatusDelayedSendTask_REF2, oldProtect, &oldProtect);
	}
	__imp_wprintf(L"[INFO] exiting Anticheat-bypass thread\n");
}



	/*
	for (auto AddrHashOffset_1: game_hash_compare_checks)
	{
		uintptr_t address = ((uintptr_t)Ds3 + AddrHashOffset_1 + 3);
		Emit(address, unsigned char, 0x39, 0xC0); //Checksums	
		__imp_wprintf(L"[CHECKSUM-BYPASS (1/3)] wrote memory to 0x%llX\n", address);
	}
	for (auto AddrHashOffset_Alt : game_hash_compare_checks_alt)
	{
		uintptr_t address = (uintptr_t)Ds3 + AddrHashOffset_Alt + 3;
		Emit((void*)address, unsigned char, 0xB9, 0x00, 0x00, 0x00, 0x00); //Checksums for checksums 
		__imp_wprintf(L"[CHECKSUM-BYPASS (2/3)] wrote memory to 0x%llX\n", address);
	}
	for (auto HashRTAddr : game_runtime_hash_checks)
	{
		uintptr_t address = (uintptr_t)Ds3 + HashRTAddr;
		//Emit((void*)address, unsigned char, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90); //Runtime checksums for checksums 
		__imp_wprintf(L"[CHECKSUM-BYPASS (3/3)] wrote memory to 0x%llX\n", address);
	}
	*/