#include "P2P.h"
#include "Funcs.h"
#include "Mem.h"
#include "Console.h"

DWORD WINAPI SlyAntiCheatThread(HMODULE hModule)
{
	__imp_wprintf(L"[INFO] Anticheat thread created\n");
	Emit(GetImageBase() + 0x78E700, unsigned char, 0xF4, 0xC3);
    __imp_wprintf(L"[HOOK] wrote exception hook at 0x%llX \n", GetImageBase() + 0x78E700);
	Emit(GetImageBase() + 0x1995695, unsigned char, 0xF4, 0xC3);
	__imp_wprintf(L"[HOOK] wrote exception hook at 0x%llX \n", GetImageBase() + 0x1995695);
	Nop(GetImageBase() + 0x197DF85, 5);
	__imp_wprintf(L"[HOOK] disabled black crystal at 0x%llX \n", GetImageBase() + 0x197DF85);
	__imp_wprintf(L"[INFO] exiting Anticheat thread\n");
}
int OnReceivePacket(void* rcx, void* from, int id, char* buf, unsigned maxlen)
{
	int recvlen = ReceivePacket_(rcx, from, id, buf, maxlen);
	int* bufint = (int*)buf;
	char* Client = Player(0);
	char* Sender = NetToChr(from);
	unsigned i = GetPlayerIndex(Sender);
	wchar_t* pName = GetpName(Sender);
	if (recvlen > 0)
	{
		if (id == 1)
		{
			int hpoffset = recvlen == 80 ? 52 : 40;
            TrueHP[i] = *(unsigned short*)(buf + hpoffset);
			if ((bufint[2]&~0x3010237))  
			{
				__imp_wprintf(L"Player [%s] sent bad walkstate\n", pName);
				swprintf(CheatLogBuff, 100, L"Player [%s] sent bad walkstate\n", pName);
				ConsoleToLog(L"CheaterLog.txt", CheatLogBuff);
				return 0;
			}
		}
		if (id == 8)
		{
			int SoulLevel = bufint[21], Vigor = bufint[11], Attunement = bufint[12], Endurance = bufint[13], Vitality = bufint[20], Strength = bufint[14], Dexterity = bufint[15], Intelligence = bufint[16], Faith = bufint[17], Luck = bufint[18];
			if (Vigor + Attunement + Endurance + Vitality + Strength + Dexterity + Intelligence + Faith + Luck - 89 != SoulLevel)
			{
				int sum = Vigor + Attunement + Endurance + Vitality + Strength + Dexterity + Intelligence + Faith + Luck - 89;
				__imp_wprintf(L"Player [%s] sent impossible stats : Stats claim to be [%d] but are [%d]\n", pName, SoulLevel, sum);
				swprintf(CheatLogBuff, 100, L"Player [%s] sent bad walkstate\n", pName);
				ConsoleToLog(L"CheaterLog.txt", CheatLogBuff);
			}
		}
		if (id == 14) //MsgMap
        {
			unsigned short SenderID = *(unsigned short*)(Sender + 0x78);
			if (bufint[0] == 12 && bufint[1] == 4044 && bufint[2] == 1)
			{
				__imp_wprintf(L"Player [%s] sent bad MsgMap : Demon souls archstone warp_1.\n", pName);
				return 0;
			}
			if (bufint[0] == 12 && bufint[1] == 4044 && bufint[2] == 2)
            {
				__imp_wprintf(L"Player [%s] sent bad MsgMap : Demon souls archstone warp_2.\n", pName);
				return 0;
			}
			if (bufint[0] == 0xE && bufint[1] == 0xFFA && bufint[2] == 0x0 || bufint[0] == 0xC && bufint[1] == 0xFA0 && bufint[2] == 0x1)
            {
				if (SenderID != 0xD001)
				{
					__imp_wprintf(L"Player [%s] sent bad MsgMap : Non-host send OnDeadEvent_HostDead.Expected 0xD001 was [%X]\n", pName, SenderID);
					return 0;
				}
				if (TrueHP[i] > 0)
				{
					__imp_wprintf(L"Player [%s] sent bad MsgMap : OnDeadEvent_HostDead while alive HP = %u.\n", pName, TrueHP[i]);
					return 0;
				}
			}
			//Sly's funny Lil' memes
			if (bufint[0] == 113 && bufint[1] == 1)
			{
				Throw(bufint[2]);
				ApplyEffect(9400);
			}
			//
		}
		if (id == 18) //ThrowPacket
		{
			int ThrowID = bufint[9];
			float* throwerpos = (float*)(buf + 0);
            float* throweepos = (float*)(buf + 16);
			unsigned short ThrowerID = *((unsigned short*)(buf + 32));
            unsigned short ThroweeID = *((unsigned short*)(buf + 34));
			unsigned short ClientID = *(unsigned short*)(Client + 0x78);
            unsigned short SenderID = *(unsigned short*)(Sender + 0x78);
			float* ClientXYZ = GetpPosition(Client);
			float* SenderXYZ = GetpPosition(Sender);
			//Chebyshev distance max(y2 - y1, x2 - x1) between client and sender
			float Delta[3]{}, Dist;
            Delta[0] = fabs(ClientXYZ[0] - SenderXYZ[0]);
            Delta[1] = fabs(ClientXYZ[1] - SenderXYZ[1]);
            Delta[2] = fabs(ClientXYZ[2] - SenderXYZ[2]);
            Dist = my_fmaxf(my_fmaxf(Delta[0], Delta[1]), Delta[2]); 
            float ThrowerDelta[3]{}, ThrowerDist;
            ThrowerDelta[0] = fabs(ClientXYZ[0] - throwerpos[0]);
            ThrowerDelta[1] = fabs(ClientXYZ[1] - throwerpos[1]);
            ThrowerDelta[2] = fabs(ClientXYZ[2] - throwerpos[2]);
            ThrowerDist = my_fmaxf(my_fmaxf(ThrowerDelta[0], ThrowerDelta[1]), ThrowerDelta[2]);
            float ThroweeDelta[3]{}, ThroweeDist;
            ThroweeDelta[0] = fabs(ClientXYZ[0] - throweepos[0]);
            ThroweeDelta[1] = fabs(ClientXYZ[1] - throweepos[1]);
            ThroweeDelta[2] = fabs(ClientXYZ[2] - throweepos[2]);
            ThroweeDist = my_fmaxf(my_fmaxf(ThroweeDelta[0], ThroweeDelta[1]), ThroweeDelta[2]); 
			int pThrowList[] = {100, 101, 110, 1000100, 1000101, 1000110, 2000100, 2000101, 3000100, 3000101};
			if (ThrowerID == ClientID && ThroweeID == SenderID || ThroweeID == ClientID && ThrowerID == SenderID)
			{
				for (auto pThrows : pThrowList)
				{
					if (ThrowID != pThrows)
					{
						__imp_wprintf(L"Player [%s] sent bad throw packet : Throw ID does not match player throw ID list [%d].\n", pName, ThrowID);
						return 0;
					}
				}
				if (Dist > 3.f || ThrowerDist > 3.f || ThroweeDist > 3.f)
				{
					__imp_wprintf(L"Player [%s] sent bad throw packet : Distance between clients too large.\n", pName);
					return 0;
				}
			}
			if (ThrowerID == ClientID && ThroweeID == ClientID)
			{
				__imp_wprintf(L"Player [%s] sent bad throw packet : ThrowerID and Throwee ID match.\n", pName);
				return 0;
			}
			if (ThroweeID == ClientID && ThrowerID != SenderID || ThrowerID == ClientID && ThroweeID != SenderID)
			{
				__imp_wprintf(L"Player [%s] sent bad throw packet : SenderID mismatch.\n", pName);
				return 0;
			}
			return recvlen;
		}
		if (id == 20) //Attack
		{
			int SenderChrType=*(int*)(Sender+0x70);
			int ClientChrType=*(int*)(Client+0x70);
			int FriendlyTypes[] = { 0, 1, 8, 13};
			int *atk, * bullet, * magic; float* bufl = (float*)buf;
			int behavior = bufint[16], atkbullet = bufint[17], weapon = bufint[28], Weapon = weapon / 1000 * 1000, * weap = (int*)GetParamAddress(0xD0, weapon / 100 * 100), weaplvl = weapon % 100, c = 0;
			if (behavior > 0 && atkbullet < 9000000 && (atk = (int*)GetParamAddress(0x18, atkbullet)))bullet = 0;
			else if (bullet = (int*)GetParamAddress(0x48, atkbullet)) 
			{
                atk = (int*)GetParamAddress(0x18, *(int*)bullet);
            } else atk = (int*)GetParamAddress(0x18, atkbullet);
			if (!atk || recvlen < 136)
			{
				__imp_wprintf(L"Player [%s] sent bad damage packet : Data mismatch.\n", pName);
				return 0;
			}
			static uint64_t LastHitTime[5], HitCount[5], ReflectAttackUntil[5]; uint64_t tc = GetTickCount(); unsigned MaxAllowedMeleeAttacks = 5, MaxAllowedRangedAttacks = 7;
			if (tc - LastHitTime[i] < 768)
            {
                if (++HitCount[i] > (bullet ? atkbullet == 100130700 || atkbullet == 11741010 || atkbullet == 11741000 || atkbullet == 11741020 || atkbullet == 12420000 || atkbullet == 12421000 || atkbullet == 12422000 ? MaxAllowedRangedAttacks * 3 : atkbullet == 855 || atkbullet == 856 || atkbullet == 857 ? MaxAllowedRangedAttacks * 2 : MaxAllowedRangedAttacks : MaxAllowedMeleeAttacks)) 
				{
					BreakConnection(NetToChr(from));
					__imp_wprintf(L"Player [%s] sent bad damage packet : Exceeds rate limit.\n", pName);
					return 0;
				}
			} else {
				HitCount[i] = 0;
                LastHitTime[i] = tc;
			}
			if (bufl[0] + bufl[1] + bufl[2] + bufl[3] + bufl[4] > 3000)
            {
				__imp_wprintf(L"Player [%s] sent bad damage packet : Exceeds damage limit.\n", pName);
				return 0;
			}
			if (weap) 
			{
				int eflvl = GetWeaponMaxUpgrade(weap) == 5 ? 2 * weaplvl : weaplvl;
				for (int g = 0; g < 3; ++g)if (bufint[24 + g] > 0 && bufint[24 + g] != weap[16 + g])
				{
					if (weap[16 + g] > 129000000 && bufint[24 + g] == weap[16 + g] + eflvl)continue;
					bufint[24 + g] = weap[16 + g];
					__imp_wprintf(L"Player [%s] sent bad damage packet : level mismatch.\n", pName);
					return 0;
				}
			} else {
				bufint[24] = bufint[25] = bufint[26] = 0;
                if (atkbullet > 500 || (bufl[0] > 0 || bufl[1] > 0 || bufl[2] > 0 || bufl[3] > 0 || bufl[4] > 0)) 
				{
					__imp_wprintf(L"Player [%s] sent bad damage packet :  Exceeds damage threshold.\n", pName);
					return 0;
				}
			}
			if((500<=atkbullet&&atkbullet<1000) || atkbullet>100000000) 
			{
				if((500<=atkbullet&&atkbullet<600&&Weapon!=14010000&&Weapon!=14020000&&Weapon!=14060000&&Weapon!=14070000&&Weapon!=14130000&&Weapon!=14150000&&Weapon!=14120000) ||
				(700<=atkbullet&&atkbullet<800&&Weapon!=13600000&&Weapon!=14140000&&Weapon!=14180000) ||
				(800<=atkbullet&&atkbullet<900&&Weapon!=14040000&&Weapon!=14050000&&Weapon!=14090000&&Weapon!=14100000&&Weapon!=14110000&&Weapon!=14160000&&Weapon!=14170000&&Weapon!=14190000))
				{
					__imp_wprintf(L"Player [%s] sent bad damage packet : atkbullet doesnt match Weapon data.\n", pName);
					return 0;
				}
				if (behavior <= 0)
				{
					__imp_wprintf(L"Player [%s] sent bad damage packet : Invalid behavior.\n", pName);
					return 0;
				}
			}
			for (auto Friendlies : FriendlyTypes)
			{
				if (SenderChrType == Friendlies && ClientChrType == Friendlies)
				{
					__imp_wprintf(L"Player [%s] sent bad damage packet : Same team.\n", pName);
					return 0;
				}
			}
			return recvlen;
		}
		if (id == 28) //VFX
		{
			float*fp;
			char*Victim=ForwardToChr(bufint[1]),*Attacker=ForwardToChr(bufint[2]);
			if (Victim == Attacker)
			{
				__imp_wprintf(L"Player [%s] sent bad VFX packet : matching forward ID's.\n", pName);
				return 0;
			}

			static uint64_t LastVFXTime[5], VFXCount[5];
            uint64_t tc = GetTickCount();
			if (tc - LastVFXTime[i] < 512) 
            {
                if (++VFXCount[i] > 10)
                {
					BreakConnection(NetToChr(from));
					__imp_wprintf(L"Player [%s] sent bad VFX packet : Rate limit exceeds threshold.\n", pName);
					return 0;
				}
			} else {
                LastVFXTime[i] = 0;
                VFXCount[i] = tc;
            }
			return recvlen;
		}
		if (id == 39)return 0;
		if (id == 49)
		{
			int BadBoys[] = { 13300412, 13305814, 13305810, 13005550, 13005550, 13805809 };
            if (bufint[0] == 65536 && bufint[1] == 8 && bufint[2] == 13705644)return 0;
            for (auto Baddies : BadBoys)if (bufint[2] == Baddies)return 0;
		}
		if (id == 63) //BulletCreation
		{
			int* BulletBuffer = (int*)(buf + 1);
			static uint64_t LastBulletTime[5], BulletCount[5]; 
            uint64_t tc = GetTickCount();
            if (tc - LastBulletTime[i] < 512) 
            {
                if (++BulletCount[i] > 5)
                {
                    __imp_wprintf(L"[%ls]%s sent bad bullet packet : Exceeds rate limit\n", pName);
                    return 0;
                }
            }
            else {
                BulletCount[i] = 0;
                LastBulletTime[i] = tc;
            }
			return recvlen;
		}
		if (id == 73) //GuestBlock reply
        {
           bufint[0] = 0;
           return recvlen;
        }
	}
	return recvlen;
}
bool ValidateP2PPacket(uint64_t steamid, unsigned* data, unsigned recvlen)
{
    unsigned checksum=data[0],id=data[1],packetlen=data[2],i=steam2i(steamid);
    LowerPID[i] = id;LowerPRecLeng[i] = packetlen + 12;LowerPTrueLeng[i] = recvlen;
    uint64_t tc = GetTickCount();
    if((LowerPID[i]>76 || LowerPRecLeng[i] != LowerPTrueLeng[i]) && LowerPID[i] != 0x5652584e)return false;
    if(tc - LastRecTime[i] < 700) {
        if(++RecCount[i] > MAX_ALLOWED_PACKET_COUNT)return false;
    } else {
        RecCount[i] = 0;
        LastRecTime[i] = tc;
    }
    return true;
}
char* NetToChr(void* pPlayer)
{
	auto _NetToChr = (char*)GetModuleHandleW(NULL) + 0x8D9E70;
	auto WorldChrManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x477FDB8;
	char* Ptr = *(char**)WorldChrManImp; 
	if (Ptr)return call(_NetToChr, Ptr, pPlayer);
	return 0;
}
unsigned steam2i(uint64_t steamid)
{
	auto SprjSessionManager = (char*)GetModuleHandleW(NULL) + 0x4796260;
    char*netbase=*(char**)SprjSessionManager,*p;
	uint64_t***conn;
    if(netbase&&steamid&&(conn=(uint64_t***)(*(char**)(netbase+0x18)+0x68)))
	{
        for(unsigned i=0;conn[0]&&conn[0]+i<conn[1];++i)if(conn[0][i][25]==steamid)return i;
    }
    return -1;
}
uint64_t net2steam(void* np)
{
    auto SprjSessionManImp = (uintptr_t)GetModuleHandleW(NULL) + 0x4796260;
    char* netbase = *(char**)SprjSessionManImp, * p, **** conn;
    if (netbase && np && (conn = (char****)(*(char**)(netbase + 0x18) + 0x68))) 
    {
        for (int i = 0; conn[0] && conn[0] + i < conn[1]; ++i)if (conn[0][i][13] == *(char**)np)return (uint64_t)conn[0][i][25];
    }
    return 0;
}
