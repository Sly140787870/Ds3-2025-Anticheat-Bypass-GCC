#include "Funcs.h"

void Ds3WindowFullScreen() 
{
    HANDLE handle = 0;
    if (handle == NULL) 
	{
        DWORD pID = 0;
        HWND hwnd = FindWindowA(NULL, "DARK SOULS III");
        GetWindowThreadProcessId(hwnd, &pID);
        handle = OpenProcess(PROCESS_ALL_ACCESS, 0, pID);
        LONG lStyle = GetWindowLong(hwnd, GWL_STYLE);
        lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
        SetWindowLong(hwnd, GWL_STYLE, lStyle);
        LONG lExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        SetWindowLong(hwnd, GWL_EXSTYLE, lExStyle);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }
}

int isgoodaddr(void* p)
{
    intptr_t x = (intptr_t)p;
    return 65536 < x && (x & 3) == 0 && !IsBadReadPtr(p, 8);
}


uintptr_t GetImageBase()
{
	return (uintptr_t)get_main_image_base();
}

uintptr_t GetWorldChrMan()
{
	return (uintptr_t)GetImageBase + 0x477FDB8;
}

char* Player(int index)
{
	void* pIndex = (char*)GetModuleHandleW(NULL) + 0x477FDB8;
	if (*(char**)pIndex && *(char**)(*(char**)pIndex + 0x40))return *(char**)(*(char**)(*(char**)pIndex + 0x40) + 0x38 * index);
	return nullptr;
}

int GetPlayerIndex(char* pPlayer)
{
	if (pPlayer == Player(0))return 0;
    if (pPlayer == Player(1))return 1;
    if (pPlayer == Player(2))return 2;
    if (pPlayer == Player(3))return 3;
    if (pPlayer == Player(4))return 4;
    if (pPlayer == Player(5))return 5;
    return -1;
}

float* GetpPosition(void* pPlayer)
{
	if (pPlayer && *(char**)((char*)(pPlayer)+0x1F90) && *(char**)(*(char**)((char*)(pPlayer)+0x1F90) + 0x68)) return (float*)(*(char**)(*(char**)((char*)pPlayer + 0x1F90) + 0x68) + 0x80);
	return 0;
}

wchar_t* GetpName(void* pPlayer)
{
	if (pPlayer && (*(wchar_t**)((char*)(pPlayer)+0x1fa0)))return 68 + *(wchar_t**)((char*)(pPlayer)+0x1fa0);
	return 0;
}

char*ForwardToChr(int fid)
{
	int*p;
	for(int i=0;i<=5;++i)if((p=(int*)Player(i))&&p[30]==fid)return (char*)p;
	return 0;
}

float my_fmaxf(float a, float b) 
{
    return (a > b) ? a : b;
}

int cmp(const void* k, const void* e)
{
	return((int)(uintptr_t)k - *(int*)e);
}

char* GetParamAddress(int ParamOffset, int id)
{
	char* p; unsigned short c;
    auto ParamPatch = (uintptr_t)GetModuleHandleW(NULL) + 0x479B8B0;
    p = *(char**)ParamPatch;
    p = p ? *(char**)(p + 0x10) : 0;
    p = p ? *(char**)(p + ParamOffset) : 0;
    p = p ? *(char**)(p + 0x68) : 0;
    p = p ? *(char**)(p + 0x68) : 0;
    if (p && (c = *(unsigned short*)(p + 10)))
    {
        int* e = (int*)bsearch((void*)(uintptr_t)id, p + 0x40, c, sizeof(int) * 6, cmp);
        return e ? p + e[2] : 0;
    }
    else return 0;
}

int GetWeaponMaxUpgrade(void* weap)
{
    int* p = (int*)weap, i;
    if (!p)return 0;
    for (i = 23; i < 38 && p[i]>0; ++i);
    return i - 24;
}

void Souls(int Amount)
{
    auto WorldChrManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x477FDB8;
    auto SoulFunc = (char*)GetModuleHandleW(NULL) + 0x5A5410;
    char* ptr = *(char**)WorldChrManImp;
    ptr = ptr ? *(char**)(ptr + 0x80) : 0;
    ptr = ptr ? *(char**)(ptr + 0x1FA0) : 0;
    if (ptr)call(SoulFunc, ptr, Amount);
}

void ApplyEffect(int ID)
{
    auto WorldChrManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x477FDB8;
    auto ApplyEffect = (char*)GetModuleHandleW(NULL) + 0x88F710;
    char* ptr = *(char**)WorldChrManImp;
    ptr = ptr ? *(char**)(ptr + 0x80) : 0;
    if (ptr)call(ApplyEffect, ptr, ID, ptr);
}

void ItemDrop(int Type, int ID, int Dur, int Quant)
{
    auto ItemClass = (uintptr_t)GetModuleHandleW(NULL) + 0x4769240;
    auto ItemDropFunc = (char*)GetModuleHandleW(NULL) + 0x7C05D0;
    auto MapItemMan = (uintptr_t)GetModuleHandleW(NULL) + 0x4763258;
    uintptr_t* iType = (uintptr_t*)(*(uintptr_t*)MapItemMan + 0x840);
    *iType = ID;
    uintptr_t* iID = (uintptr_t*)(*(uintptr_t*)MapItemMan + 0x844);
    *iID = ID;
    uintptr_t* iDur = (uintptr_t*)(*(uintptr_t*)MapItemMan + 0x848);
    *iDur = Dur;
    uintptr_t* iQuant = (uintptr_t*)(*(uintptr_t*)MapItemMan + 0x84C);
    *iQuant = Quant;
    call(ItemDropFunc, *(char**)ItemClass, 1, NULL);
}

void ItemGib(int Type, int ID, int Dur, int Quant)
{
    auto ItemGibFunc = (char*)GetModuleHandleW(NULL) + 0x7C4080;
    auto MapItemMan = (uintptr_t)GetModuleHandleW(NULL) + 0x4769240;
    char ItemData[52]{};
    memset(ItemData, 0, sizeof(ItemData));
    *(int*)(ItemData + 0x10) = 0x1;
    *(int*)(ItemData + 0x14) = ID;
    *(int*)(ItemData + 0x18) = Quant;
    *(int*)(ItemData + 0x1C) = Dur;
    call(ItemGibFunc, *(char**)MapItemMan, ItemData + 0x10, ItemData);
}

void Throw(int ID)
{
    auto ThrowClassPtr = (uintptr_t)GetModuleHandleW(NULL) + 0x475A7A8;
    auto ThrowFunc = (char*)GetModuleHandleW(NULL) + 0x5F5810;
    auto WorldChrManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x477FDB8;
    char* ptr = *(char**)WorldChrManImp;
    ptr = ptr ? *(char**)(ptr + 0x80) : 0;
    ptr = ptr ? *(char**)(ptr + 0x1F90) : 0;
    ptr = ptr ? *(char**)(ptr + 0x88) : 0;
    ptr = ptr ? *(char**)(ptr + 0x28) : 0;
    if (ptr)call(ThrowFunc, *(char**)ThrowClassPtr, (char*)ptr, (char*)ptr, ID);
}

void ThrowPlayer(unsigned Index, int ID)
{
    auto ThrowClassPtr = (uintptr_t)GetModuleHandleW(NULL) + 0x475A7A8;
    auto ThrowFunc = (char*)GetModuleHandleW(NULL) + 0x5F5810;
    auto WorldChrManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x477FDB8;
    char* Thrower = *(char**)WorldChrManImp;
    Thrower = Thrower ? *(char**)(Thrower + 0x80) : 0;
    Thrower = Thrower ? *(char**)(Thrower + 0x38 * Index) : 0;
    Thrower = Thrower ? *(char**)(Thrower + 0x88) : 0;
    Thrower = Thrower ? *(char**)(Thrower + 0x28) : 0;
    char* Throwee = *(char**)WorldChrManImp;
    Throwee = Throwee ? *(char**)(Throwee + 0x40) : 0;
    Throwee = Throwee ? *(char**)(Throwee + 0x38 * Index) : 0;
    Throwee = Throwee ? *(char**)(Throwee + 0x1F90) : 0;
    Throwee = Throwee ? *(char**)(Throwee + 0x88) : 0;
    Throwee = Throwee ? *(char**)(Throwee + 0x28) : 0;
    if (Index <= 0 or Index >= 5)if (Thrower && Throwee)call(ThrowFunc, *(char**)ThrowClassPtr, (char*)Thrower, (char*)Throwee, ID);
}

void LuaWarp(int ID)
{
    auto SprjLuaEventManager = (uintptr_t)GetModuleHandleW(NULL) + 0x4751B08;
    auto WarpFunc = (char*)GetModuleHandleW(NULL) + 0x475F00;
    uintptr_t r8Value = ID;
    uintptr_t edxValue = r8Value - 0x3E8;
    uint32_t r8dValue = static_cast<uint32_t>(r8Value - 0x3E8);
    call(WarpFunc, *(char**)SprjLuaEventManager, edxValue, r8dValue, NULL);
}

void SpawnBullet(float Dist, float Elevation, float CoordAngle, float TBCoordX, float TBCoordY, float TBCoordZ, float TBAngX, float TBAngY, float TBAngZ, int BulletID, bool CustomAngle)
{
    auto SprjBulletMan = (uintptr_t)GetModuleHandleW(NULL) + 0x4789CA8;
    auto SpawnBullet = (char*)GetModuleHandleW(NULL) + 0x980F60;
    float BAngleX, BAngleY, BAngleZ, BCoordX, BCoordY, BCoordZ;
    float dir = CoordAngle + 3.14;
    float Elv = Elevation + 3.14;
    BCoordX = TBCoordX - Dist * sin(CoordAngle);
    BCoordZ = TBCoordZ - Dist * cos(CoordAngle);
    BCoordY = TBCoordY + Elevation;
    BAngleZ = Dist * cos(CoordAngle);
    BAngleX = Dist * sin(CoordAngle);
    BAngleY = -tan(Elv);
    char BulletData[256];
    memset(BulletData, 0, sizeof(BulletData));
    *(int*)(BulletData + 0x30) = 0x10068000;
    *(int*)(BulletData + 0x34) = 0xFFFFFFFF;
    *(int*)(BulletData + 0x38) = 0xFFFFFFFF;
    *(int*)(BulletData + 0x3C) = 0xFFFFFFFF;
    *(int*)(BulletData + 0x40) = BulletID;
    *(int*)(BulletData + 0x4C) = 0xFFFFFFFF;
    *(int*)(BulletData + 0x50) = 0xFFFFFFFF;
    *(int*)(BulletData + 0x54) = 0xFFFFFFFF;
    *(int*)(BulletData + 0x6C) = 0x9;
    *(float*)(BulletData + 0x7C) = BCoordX;
    *(float*)(BulletData + 0x8C) = BCoordY;
    *(float*)(BulletData + 0x9C) = BCoordZ;
    if (CustomAngle == true)
    {
        *(float*)(BulletData + 0x78) = TBAngX;
        *(float*)(BulletData + 0x88) = TBAngY;
        *(float*)(BulletData + 0x98) = TBAngZ;
    }
    else {
        *(float*)(BulletData + 0x78) = BAngleX;
        *(float*)(BulletData + 0x88) = BAngleY;
        *(float*)(BulletData + 0x98) = BAngleZ;
    }
    call(SpawnBullet, *(char**)SprjBulletMan, BulletData, BulletData + 0x30, 0x80000080);
}

void InvokeAnimation(int ID)
{
    int Animation[6] = { ID, 0, 0, 0, 0, 0 };
    auto WorldChrManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x477FDB8;
    auto AnimFunc = (char*)GetModuleHandleW(NULL) + 0xD913E0;
    auto Ptr = *(char**)WorldChrManImp;
    Ptr = Ptr ? *(char**)(Ptr + 0x80) : 0;
    Ptr = Ptr ? *(char**)(Ptr + 0x1F90) : 0;
    Ptr = Ptr ? *(char**)(Ptr + 0x58) : 0;
    Ptr = Ptr ? *(char**)(Ptr + 0x8) : 0;
    Ptr = Ptr ? *(char**)(Ptr + 0x1F90) : 0;
    Ptr = Ptr ? *(char**)(Ptr + 0x28) : 0;
    Ptr = Ptr ? *(char**)(Ptr + 0x10) : 0;
    Ptr = Ptr ? *(char**)(Ptr + 0x28) : 0;
    if (Ptr)call(AnimFunc, *(char**)(Ptr + 0xB8), Animation);
}

void RemoveAvatar(void*charbase)
{
	if(charbase)
	{
		auto WorldChrManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x477FDB8;
		auto GameManager = (uintptr_t)GetModuleHandleW(NULL) + 0x475AC00;
		auto RemoveChr_1 = (char*)GetModuleHandleW(NULL) + 0x6859F0;
		auto RemoveChr_2 = (char*)GetModuleHandleW(NULL) + 0x8DBC30;
		call(RemoveChr_1, *(char**)(*(char**)GameManager+0xC60), *(int*)((char*)charbase+8));
		call(RemoveChr_2, *(char**)WorldChrManImp, charbase);
	}
}

DWORD BreakConnection(void*charbase)
{
	auto SprjSessionManager = (char*)GetModuleHandleW(NULL) + 0x4796260;
	auto GameManager = (uintptr_t)GetModuleHandleW(NULL) + 0x475AC00;
    char*netbase=*(char**)SprjSessionManager,*p, ****conn,***connb,**np;
	if(netbase&&charbase&&isgoodaddr(conn=(char****)(*(char**)(netbase+0x18)+0x68))&&isgoodaddr(np=(char**)(8+*(char**)((char*)charbase+0x1fd0))))
	{
		connb=(char***)conn;
		for(int i=0;conn[0]+i<conn[1];++i)if(conn[0][i][13]==*np){*(int*)(connb[0][i]+0x58)=0;break;}
		RemoveAvatar(charbase);
	}
	if(p=*(char**)GameManager)
	{
		unsigned pc=0;
		for(int i=0;i<=5;++i)if(Player(i))++pc;
		if(*(int*)(p+0xd38)>pc)*(int*)(p+0xd38)=pc;
	}
}