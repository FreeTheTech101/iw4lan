// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Increase maxclient limit.
//
// Initial author: zxz0O0
// Started: 2013-03-18
//
// Thanks to: NTAuthority, Nukem, master131
// ==========================================================
 
#include "StdInc.h"
 
void PatchMW2_MaxclientsCommon()
{
	//ui_maxclients & sv_maxclients
	*(BYTE*)0x426189 = MAX_USABLE_CLIENTS;
	*(BYTE*)0x42618D = MAX_USABLE_CLIENTS;
	*(BYTE*)0x4D3750 = MAX_USABLE_CLIENTS;
	*(BYTE*)0x4D3754 = MAX_USABLE_CLIENTS;
	*(BYTE*)0x5E376C = MAX_USABLE_CLIENTS;
	*(BYTE*)0x5E3770 = MAX_USABLE_CLIENTS;
	//sv_privateClientsForClients
	*(BYTE*)0x4D3712 = MAX_CLIENTS;
	//sv_privateClients
	*(BYTE*)0x4D3731 = MAX_CLIENTS;
	//party_maxplayers
	*(BYTE*)0x4D5D5E = MAX_CLIENTS;
	//party_minplayers
	*(BYTE*)0x4D5D47 = MAX_CLIENTS;
 
	//iw4m.exe+FC5C6 - 8D 70 12              - lea esi,[eax+12]
	//starting entityNum for nonplayers
	*(BYTE*)0x4FC5C8 = MAX_CLIENTS;
	//iw4m.exe+E8564 - 8D 70 EE              - lea esi,[eax-12]
	*(BYTE*)0x4E8566 = -MAX_CLIENTS;
 
	//showToClient check
	*(BYTE*)0x5F6E52 = MAX_CLIENTS;
 
	//some entity num check
	*(BYTE*)0x628562 = MAX_CLIENTS;
 
	//iw4m.exe+195295 - 83 EE 12              - sub esi,12
	//esi is entityNum of the corpse entity, so subtract MAX_CLIENTS to get the index of clientInfo_t corpseinfo[]
	*(BYTE*)0x595297 = MAX_CLIENTS;
	//iw4m_srv.exe+D76F4 - 83 E8 12              - sub eax,12
	//same as above
	*(BYTE*)0x4D76F6 = MAX_CLIENTS;
 
	//CG_Obituary: target out of range --> checking vicitim's clientNum
	*(BYTE*)0x586D9B = MAX_CLIENTS;
	//this one checking attacker's clientNum
	*(BYTE*)0x586E07 = MAX_CLIENTS - 1;
 
	//check if sv_maxclients (?) is > limit
	*(BYTE*)0x591F61 = MAX_CLIENTS;
	//if yes, replace by this
	*(int*)0x591F6F = MAX_CLIENTS;
 
	//parameter int numGEntities for SV_LocateGameData
	*(BYTE*)0x48EF4D = MAX_CLIENTS + MAX_CORPSES;
	//iw4m_server.exe+8EF53 - C7 05 B031A801 1A000000 - mov [iw4m_server2.exe+16831B0],0000001A
	*(int*)0x48EF59 = MAX_CLIENTS + MAX_CORPSES;
 
	//some check in G_FreeEntity
	*(BYTE*)0x44CB09 = MAX_CLIENTS + MAX_CORPSES;
 
	//Limit for loop in G_FreeEntityRefs
	*(int*)0x4163A6 = gentity_s_start + 0x158 + MAX_CLIENTS * sizeof(gentity_t);
	*(int*)0x41635D = gentity_s_start + 0x158 + MAX_CLIENTS * sizeof(gentity_t);
 
	//+0x94 may be some entity limit for usuable items
	//iw4m_server.exe+3837 - B8 12000000           - mov eax,00000012
	//iw4m_server.exe+383F - 89 86 94000000        - mov [esi+00000094],eax
	*(int*)0x403838 = MAX_CLIENTS;
	//iw4m_server.exe+1BC6C - C7 86 94000000 12000000 - mov [esi+00000094],00000012
	*(int*)0x41BC72 = MAX_CLIENTS;
	//iw4m_server.exe+74E13 - C7 86 94000000 12000000 - mov [esi+00000094],00000012
	*(int*)0x474E19 = MAX_CLIENTS;
	//iw4m.exe+DD026 - 8B 85 94000000        - mov eax,[ebp+00000094]
	//iw4m.exe+DD034 - 83 F8 12              - cmp eax,12
	*(BYTE*)0x4DD036 = MAX_CLIENTS;
 
 
	//Limit for loop, probably some gsc function
	*(int*)0x5F9923 = MAX_CLIENTS * sizeof(gclient_t);
 
	//Various checks which I did not document
	//iw4m_srv.exe+188041 - 83 F8 12              - cmp eax,12
	*(BYTE*)0x588043 = MAX_CLIENTS;
	//iw4m_srv.exe+422D5 - 83 FF 12              - cmp edi,12
	*(BYTE*)0x4422D7 = MAX_CLIENTS;
	//iw4m_srv.exe+42249 - 83 FF 12              - cmp edi,12
	*(BYTE*)0x44224B = MAX_CLIENTS;
	//iw4m_srv.exe+1713FE - 83 F9 12              - cmp ecx,12
	*(BYTE*)0x571400 = MAX_CLIENTS;
	//iw4m.exe+17FC31 - 83 F9 12              - cmp ecx,12
	*(BYTE*)0x57FC33 = MAX_CLIENTS;
	//iw4m.exe+4232B - 83 FF 12              - cmp edi,12
	*(BYTE*)0x44232D = MAX_CLIENTS;
	//iw4m_srv.exe+E1CE6 - 83 FD 11              - cmp ebp,11
	//CG_ProcessClientNoteTracks
	*(BYTE*)0x4E1CE8 = MAX_CLIENTS - 1;
 
	//"client not pointing to the level.clients array"
	*(BYTE*)0x5D847E = MAX_CLIENTS;
	//"forceSpectatorClient can only be set to -1 or a valid client number"
	*(BYTE*)0x5D8744 = MAX_CLIENTS;
 
	//unknown checks
	//iw4m.exe+EFC40 - 83 FF 12              - cmp edi,12
	*(BYTE*)0x4EFC42 = MAX_CLIENTS;
	//iw4m_srv.exe+25238 - BE 12000000           - mov esi,00000012
	*(int*)0x425239 = MAX_CLIENTS;
	//iw4m_srv.exe+25295 - 83 FF 12              - cmp edi,12
	*(BYTE*)0x425297 = MAX_CLIENTS;
	//iw4m.exe+1EB631 - 83 F8 12              - cmp eax,12
	*(BYTE*)0x5EB633 = MAX_CLIENTS;
}
 
//For raising max corpse count
/*clientInfo_t corpseInfo [MAX_CORPSES];
void Patch_clientInfo_t_corpse()
{
	int clientInfo_t_corpse_patches [2] = { 0x7EE5F4, 0x7EEAF0 };
	DWORD allocated = (DWORD)corpseInfo;
 
	SearchAndPatch(clientInfo_t_corpse_patches, 2, clientInfo_t_corpse_start, allocated);
	//iw4m_server.exe+189E4D - 81 FE 50147F00        - cmp esi,iw4m_server.exe+3F1450
	*(int*)0x589E4F = sizeof(clientInfo_t) * MAX_CORPSES + allocated + 0x4FC;	
	//iw4m_server.exe+189EA9 - 81 FE 50147F00        - cmp esi,iw4m_server.exe+3F1450
	*(int*)0x589EAB = sizeof(clientInfo_t) * MAX_CORPSES + allocated + 0x4FC;	
}*/
 
clientState_s cS_clients[MAX_CLIENTS * 0x20];//0x20 *should* be correct
//just (2*(MAXCLIENTS * sizeof(clientState_s)) + 0x33134) is actually needed for the following but this looks cleaner
snapshot_s activeSnapshots[2];
void Patch_clientState_s()
{
	int patches[2] = { clientState_s_start, 0x51B15C };
	SearchAndPatch(patches, 2, clientState_s_start, (DWORD)cS_clients);
	//iw4m.exe+195082 - 81 3D E8427F00 F0427F00 - cmp [iw4m.exe+3F42E8],iw4m.exe+3F42F0
	*(int*)0x595088 = (int)activeSnapshots;
	//iw4m.exe+195097 - 69 C9 EC390300        - imul ecx,ecx,000339EC
	*(int*)0x595099 = sizeof(snapshot_s);
	//iw4m.exe+19509D - 81 C1 F0427F00        - add ecx,iw4m.exe+3F42F0
	*(int*)0x59509F = (int)activeSnapshots;
	//iw4m.exe+502C2 - 8B 8F E8390300        - mov ecx,[edi+000339E8]
	*(int*)0x4502C4 = (int)&activeSnapshots[0].serverCommandSequence - (int)activeSnapshots; //get size until serverCommandSequence
	//iw4m.exe+6B855 - 89 86 E8390300        - mov [esi+000339E8],eax
	*(int*)0x46B857 = (int)&activeSnapshots[0].serverCommandSequence - (int)activeSnapshots;
 
 
	//iw4m.exe+6B980 - 83 F8 12              - cmp eax,12
	*(BYTE*)0x46B982 = MAX_CLIENTS;
	//iw4m.exe+6B989 - C7 44 24 1C 12000000  - mov [esp+1C],00000012
	*(int*)0x46B98D = MAX_CLIENTS;
}
 
clientInfo_t c_clientinfo[MAX_CLIENTS];
int c_clientInfo_t_patches [31] = { 0x8E77B0, 0x8E77BC, 0x8E77CC, 0x8E77D4, 0x8E77D8, 0x8E77DC, 0x8E7CAC, 0x8E77E4, 
								   0xF6838, 0xF6844, 0xF6854, 0xF6C54, 0xF6C58, 0xF685C, 0xF6860,
								   0x82988, 0x82DF4, 0x82DF8, 0x82E04, 0x82E0C, 0x82E10, 0x82E14, 0x82E1C, 0x82E24, 0x82E2C, 0x82E30, 0x82E34, 0x82E54, 0x82E5C, 0x82E64, 0x82E7C };
void Patch_c_clientInfo_t()
{
	SearchAndPatch(c_clientInfo_t_patches, 31, c_clientInfo_t_start, (DWORD)c_clientinfo);
 
	//Various limits for loops
	//iw4m_server.exe+FE65E - 81 FD C8D48E00        - cmp ebp,iw4m_server.exe+4ED4C8
	*(int*)0x4FE660 = sizeof(clientInfo_t) * MAX_CLIENTS + (DWORD)c_clientinfo;
	//iw4m_server.exe+17FDD1 - 81 FF C8D48E00        - cmp edi,iw4m_server.exe+4ED4C8
	*(int*)0x57FDD3 = sizeof(clientInfo_t) * MAX_CLIENTS + (DWORD)c_clientinfo;
	//iw4m_server.exe+F23B8 - 81 FE D4D48E00        - cmp esi,iw4m_server.exe+4ED4D4
	*(int*)0x4F23BA = sizeof(clientInfo_t) * MAX_CLIENTS + (DWORD)c_clientinfo + 0xC;
	//iw4m_server.exe+7464F - 81 FE ECD48E00        - cmp esi,iw4m_server.exe+4ED4EC
	*(int*)0x474651 = sizeof(clientInfo_t) * MAX_CLIENTS + (DWORD)c_clientinfo + 0x24;
	//iw4m.exe+189E23 - 81 FE C4D98E00        - cmp esi,iw4m.exe+4ED9C4
	*(int*)0x589E25 = sizeof(clientInfo_t) * MAX_CLIENTS + (DWORD)c_clientinfo + 0x4FC;	
	//iw4m.exe+189E86 - 81 FE C4D98E00        - cmp esi,iw4m.exe+4ED9C4
	*(int*)0x589E88 = sizeof(clientInfo_t) * MAX_CLIENTS + (DWORD)c_clientinfo + 0x4FC;
	//iw4m.exe+19E820 - 81 FE F4D48E00        - cmp esi,iw4m.exe+4ED4F4
	*(int*)0x59E822 = sizeof(clientInfo_t) * MAX_CLIENTS + (DWORD)c_clientinfo + 0x2C;
}
 
score_t scores [MAX_CLIENTS+1]; //why +1 ? it also uses the offset 0x863C10 to calculate which is -0x28 before the real start
int scores_t_patches [4] = { 0x863C38, 0x863C48, 0x863C4C, 0x863C10 };
void Patch_scores_t()
{
	SearchAndPatch(scores_t_patches, 4, scores_t_start, (DWORD)&scores[1]); //the actual start is at scores[1]
}
 
void __declspec(naked) fastzeroHk()
{
	_asm
	{
		pushad
		mov ebp, esp
		sub esp, __LOCAL_SIZE
	}
	int StructSize;
	int ReturnAd;
	__asm mov eax,[ebp+0x70]
	__asm mov ReturnAd,eax
	//we don't care about it setting old structs to 0, we just also want to set our new structs to 0
	switch(ReturnAd)
	{
	case 0x4AD343:
		__asm
		{
			push 0xBEEF
			sub esp,0x4C
			call fastzeroHk
			add esp,0x50
		}
		StructSize = sizeof(scores);
		__asm lea edi,[scores]
		break;
	case 0x4D54B4:
		StructSize = sizeof(cS_clients);
		__asm lea edi,[cS_clients]//clientState_s in clientActive_t
		break;
	case 0x4E32C4:
		StructSize = sizeof(c_clientinfo);
		__asm lea edi,[c_clientinfo]
		break;
	case 0xBEEF:
		StructSize = sizeof(activeSnapshots);
		__asm lea edi,[activeSnapshots];
		break;
	default:
		goto End;
	}
 
	__asm
	{
		mov eax,[StructSize]
		push eax
		push 00
		push edi
		call memset
		add esp,0xC
	}
End:
	__asm
	{
		mov esp, ebp
		popad
		ret
	}
}
 
void PatchMW2_MaxclientsClient()
{
	//map structs
	Patch_c_clientInfo_t();
	Patch_clientState_s();
	Patch_scores_t();
 
	//fix -> show scavenger bags and stuff
	*(BYTE*)0x5E2AB7 = MAX_CLIENTS;
 
	//show clients on radar
	//iw4m_srv.exe+6CD97 - C7 44 24 28 12000000  - mov [esp+28],00000012
	*(int*)0x46CD9B = MAX_CLIENTS;
 
	//red crosshair
	*(BYTE*)0x581E36 = MAX_CLIENTS;
	//draw enemy names over head check
	*(BYTE*)0x582376 = MAX_CLIENTS;
 
	//Fix for laggy movement of players > 17
	*(BYTE*)0x4D7685 = MAX_CLIENTS;
 
	//CL_ParseGamestate: bad clientNum %i
	*(BYTE*)0x5AC630 = MAX_CLIENTS;
 
	//Hook fastzero func which resets struct's mem to 0
	*(int*)0x6BDA56 = 0x909003EB;
	*(short*)0x6BDA5A = (short)0xE890;
	*(int*)0x6BDA5C = (int)&fastzeroHk - 0x6BDA60;
}
 
void PatchMW2_Maxclients()
{
	PatchMW2_MaxclientsCommon();
	PatchMW2_MaxclientsClient();
 
	DWORD clientStart = *(DWORD*)0x412AE4; // address of one user patched by a function above
	svs_clients = (client_t*)clientStart;
}
