#include "Framework.h"
#include <cmath>
#define MAX_ALLOWED_PACKET_COUNT 300
DWORD WINAPI SlyAntiCheatThread(HMODULE hModule);
EXTERN_C int ReceivePacket_(void* rcx, void* from, int id, void* buf, unsigned maxlen);
int OnReceivePacket(void* rcx, void* from, int id, char* buf, unsigned maxlen);
asm("ReceivePacket_:"
	"pushq %rcx;"
	"pushq %rbx;"
	"pushq %rbp;"
	"pushq %rsi;"
	"pushq %rdi;"
	"subq $0x38, %rsp;"
	"xorl %ecx, %ecx;"
	"call GetModuleHandleW;"
	"addq $0x38, %rsp;"
	"popq %rdi;"
	"popq %rsi;"
	"popq %rbp;"
	"popq %rbx;"
	"popq %rcx;"
	"addq $0x78E70F, %rax;"
	"movq %rbx, 8(%rsp);"
	"movq %rbp, 16(%rsp);"
	"movq %rsi, 24(%rsp);"
	"movq %rdi, 32(%rsp);"
	"jmp *%rax;");
bool ValidateP2PPacket(uint64_t steamid, unsigned* data, unsigned recvlen);
char* NetToChr(void* pPlayer);
unsigned steam2i(uint64_t steamid);
uint64_t net2steam(void* np);
static unsigned LowerPID[5];
static unsigned LowerPRecLeng[5];
static unsigned LowerPTrueLeng[5];
static uint64_t LastRecTime[5];
static uint64_t RecCount[5];
static int TrueHP[5];