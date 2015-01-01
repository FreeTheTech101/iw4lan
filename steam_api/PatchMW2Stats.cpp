// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Structured data handling.
//
// Initial author: NTAuthority
// Started: 2011-06-16
// ==========================================================

#include "StdInc.h"

// hook functions
void LiveStorage_DownloadStats(const char* profileName);

// hooks
CallHook downloadStatsHook;
DWORD downloadStatsHookLoc = 0x4B229D;

StompHook writeStatsHook;
DWORD writeStatsHookLoc = 0x682E70;

void WriteStatsHookStub();

void PatchMW2_Stats()
{
	downloadStatsHook.initialize(downloadStatsHookLoc, LiveStorage_DownloadStats);
	downloadStatsHook.installHook();

	writeStatsHook.initialize(writeStatsHookLoc, WriteStatsHookStub);
	writeStatsHook.installHook();

	// don't download stats every frame when not downloaded yet
	memset((void*)0x40F8AD, 0x90, 5);

	*(BYTE*)0x60A830 = 0xB0;
	*(BYTE*)0x60A831 = 0x00;
	*(BYTE*)0x60A832 = 0xC3;
}

char* statBuffer = (char*)0x1AD3690;

typedef void (__cdecl * LiveStorage_CompleteStatDownload_t)(int, int, int);
LiveStorage_CompleteStatDownload_t LiveStorage_CompleteStatDownload = (LiveStorage_CompleteStatDownload_t)0x4C04C0;

extern int* clientState;

void LiveStorage_DownloadStats(const char* profileName)
{
	char* buffer;
	int size = FS_ReadFile("iw4lan.stat", &buffer);

	if (size > 0)
	{
		Com_Printf(0, "[LiveStorage_DownloadStats] Successfully loaded stats.\n");
		
		memcpy(statBuffer, buffer, size);
		FS_FreeFile(buffer);
		
		if (*clientState < 3)
		{
			LiveStorage_CompleteStatDownload(0, 0, 0);
		}
	}
	else
	{
		Com_Printf(0, "[LiveStorage_DownloadStats] Failed loading stats, resetting...\n");

		// reset stats
		__asm
		{
			push esi
			mov eax, 60A5A0h
			mov esi, 0
			call eax
			pop esi
		}
	}
}

static const char* fileID;
static bool useSteamCloud;
static unsigned int bufferSize;

unsigned int GetPlayerSteamID();

bool WriteStatsFunc(int remote, void* buffer)
{
	// filename buffer
	char filename[256];

	// check buffer size to be valid, though it doesn't matter if we don't do TEA
	if (bufferSize > 8192 || bufferSize & 3) return false; // & 3 is for 4-byte alignment

	// don't do writing for steam cloud
	if (remote) return true;

	// sprintf a filename
	_snprintf(filename, sizeof(filename), "iw4lan.stat");

	// and write the file
	return FS_WriteFile(filename, "players", buffer, bufferSize);
}

void __declspec(naked) WriteStatsHookStub()
{
	__asm
	{
		// prepare weird optimized parameters
		mov fileID, ecx
		mov useSteamCloud, dl
		mov bufferSize, eax

		// and jump to our normal cdecl function
		jmp WriteStatsFunc
	}
}