// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Code to replace localized strings in the game.
//
// Initial author: NTAuthority
// Started: 2013-01-17
// ==========================================================

#include "StdInc.h"
#include <google/dense_hash_map>

struct localizedEntry_s
{
	const char* value;
	const char* name;
};

StompHook loadLocalizeHook;
DWORD loadLocalizeHookLoc = 0x629B90;

static google::dense_hash_map<std::string, std::string> _localizedStrings;

const char* SEH_GetLocalizedString(const char* key)
{
	google::dense_hash_map<std::string, std::string>::const_iterator iter = _localizedStrings.find(key);

	if (iter != _localizedStrings.end())
	{
		return (*iter).second.c_str();
	}

	localizedEntry_s* entry = (localizedEntry_s*)DB_FindXAssetHeader(ASSET_TYPE_LOCALIZE, key);

	if (entry)
	{
		return entry->value;
	}

	return NULL;
}

void PatchMW2_LocalizedStrings()
{
	_localizedStrings.set_empty_key("");

	_localizedStrings["EXE_AWAITINGGAMESTATE"] = "Awaiting gamestate";

	loadLocalizeHook.initialize(loadLocalizeHookLoc, SEH_GetLocalizedString);
	loadLocalizeHook.installHook();
}