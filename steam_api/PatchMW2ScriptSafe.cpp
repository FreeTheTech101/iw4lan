// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Prevents script loading from non-fastfiles if
//          fs_game == ''.
//
// Initial author: NTAuthority
// Started: 2013-02-25
// ==========================================================

#include "StdInc.h"
#include <time.h>

void* LoadModdableScript(const char* baseName, const char* fileName)
{
	dvar_t* fs_game = Dvar_FindVar("fs_game");
	dvar_t* developer = Dvar_FindVar("developer_script");

	if (fs_game->current.string[0] || developer->current.boolean)
	{
		int fh;

		if (FS_FOpenFileRead(fileName, &fh, 0) >= 0)
		{
			FS_FCloseFile(fh);

			return ((void*(*)(const char*, const char*))0x61AB00)(baseName, fileName);
		}
	}

	return ((void*(*)(const char*, const char*))0x61AAB0)(baseName, fileName);
}

void PatchMW2_ScriptSafe()
{
	//call(0x427DA8, LoadModdableScript, PATCH_CALL);

	// re-add 'impure stats' checking
	//*(BYTE*)0x4BB250 = 0xA0;
	//*(BYTE*)0x4BB251 = 0x08;
	//*(DWORD*)0x4BB252 = 0xC301DC22;
}