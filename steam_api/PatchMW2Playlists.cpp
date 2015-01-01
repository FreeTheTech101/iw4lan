// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Obtain playlists from XNP/dedicated server.
//
// Initial author: NTAuthority
// Started: 2011-10-31
// ==========================================================

#include "StdInc.h"

bool* gotPlaylists = (bool*)0x1AD3680;
dvar_t* playlist_enabled;

void PatchMW2_Playlists()
{
#ifndef MATCHMAKING
	// disable playlist download function
	*(BYTE*)0x4D4790 = 0xC3;

	// disable playlist.ff loading function
	*(BYTE*)0x4D6E60 = 0xC3;

	// playlist dvar 'validity check'
	*(BYTE*)0x4B1170 = 0xC3;
	
	// not sure about this, was enabled and dedi-only
	// disable playlist checking
	*(BYTE*)0x5B69E9 = 0xEB; // too new
	*(BYTE*)0x5B696E = 0xEB; // too old

	*gotPlaylists = true;
#endif

	if (GAME_FLAG(GAME_FLAG_DEDICATED))
	{
		playlist_enabled = Dvar_RegisterBool("playlist_enabled", false, DVAR_FLAG_NONE, "Enable playlists");
	}
}