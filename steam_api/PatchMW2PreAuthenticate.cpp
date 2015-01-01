// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Patches to perform NP authentication at challenge
//          time.
//
// Initial author: NTAuthority
// Started: 2012-12-26
// ==========================================================

#include "StdInc.h"
#include "Hooking.h"

void PatchMW2_PreAuthenticate()
{
	// disable Steam auth
	*(BYTE*)0x4663A8 = 0xEB;
}