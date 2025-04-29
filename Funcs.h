#include "Framework.h"
#include "NtTest.h"
#include "Mem.h"
#include <math.h> 
void Ds3WindowFullScreen();
int isgoodaddr(void*p);
uintptr_t GetImageBase();
uintptr_t GetWorldChrMan();
char* Player(int index);
int GetPlayerIndex(char* pPlayer);
float* GetpPosition(void* pPlayer);
wchar_t* GetpName(void* pPlayer);
char*ForwardToChr(int fid);
float my_fmaxf(float a, float b);
int cmp(const void* k, const void* e);
char* GetParamAddress(int ParamOffset, int id);
int GetWeaponMaxUpgrade(void* weap);
#define GetAttackAddress(id) GetParamAddress(0x18,id)
#define GetBulletAddress(id) GetParamAddress(0x48,id)
#define GetCeremonyAddress(id) GetParamAddress(0x60,id)
#define GetRingAddress(id) GetParamAddress(0xb8,id)
#define GetGoodAddress(id) GetParamAddress(0xc0,id)
#define GetArmorAddress(id) GetParamAddress(0xc8,id)
#define GetWeaponAddress(id) GetParamAddress(0xd0,id)
#define GetMagicAddress(id) GetParamAddress(0x1a0,id)
#define GetLockCamAddress(id) GetParamAddress(0x180,id)
#define GetNetworkAddress(id)GetParamAddress(0x218,id)
#define GetEffectAddress(id) GetParamAddress(0x2a8,id)
#define ModifyBullet(id,offset,type,...)do{const type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetBulletAddress(id)+offset,data,sizeof data,0);}while(0)
#define ModifyEffect(id,offset,type,...)do{const static type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetEffectAddress(id)+offset,data,sizeof data,0);}while(0)
#define ModifyGood(id,offset,type,...)do{const type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetGoodAddress(id)+offset,data,sizeof data,0);}while(0)
#define ModifyRing(id,offset,type,...)do{const type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetRingAddress(id)+offset,data,sizeof data,0);}while(0)
#define ModifyCeremony(id,offset,type,...)do{const static type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetCeremonyAddress(id)+offset,data,sizeof data,0);}while(0)
#define ModifyWeapon(id,offset,type,...)do{const type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetWeaponAddress(id)+offset,data,sizeof data,0);}while(0)
#define ModifyFOV(id,offset,type,...)do{const type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetLockCamAddress(id)+offset,data,sizeof data,0);}while(0)
#define ModifyNet(id,offset,type,...)do{const type data[]={__VA_ARGS__};WriteProcessMemory((HANDLE)-1,GetLockCamAddress(id)+offset,data,sizeof data,0);}while(0)
void Souls(int Amount);
void ApplyEffect(int ID);
void ItemDrop(int Type, int ID, int Dur, int Quant);
void ItemGib(int Type, int ID, int Dur, int Quant);
void Throw(int ID);
void ThrowPlayer(unsigned Index, int ID);
void ForceThrow(unsigned Index_1, unsigned Index_2, int ID);
void LuaWarp(int ID);
void SpawnBullet(float Dist, float Elevation, float CoordAngle, float TBCoordX, float TBCoordY, float TBCoordZ, float TBAngX, float TBAngY, float TBAngZ, int BulletID, bool CustomAngle);
void InvokeAnimation(int ID);
void RemoveAvatar(void*charbase);
DWORD BreakConnection(void*charbase);
