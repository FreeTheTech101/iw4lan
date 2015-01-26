// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: DLL library entry point, SteamAPI exports
//
// Initial author: NTAuthority
// Started: 2010-08-29
// ==========================================================

#include "StdInc.h"
#include "AdminPlugin.h"

#undef Trace
#define Trace

void HideCode_FindCreateFile();
HANDLE WINAPI HideCode_DoCreateFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

void SteamProxy_Init();

void InitializeDediConfig();
void Com_SaveDediConfig();

// Steam API code
extern "C"
{
    __declspec(dllexport) HSteamPipe /*__cdecl*/ __declspec(naked) GetHSteamPipe()
    {
		__asm jmp HideCode_DoCreateFile
        //return NULL;
    }
    
    __declspec(dllexport) HSteamUser __cdecl GetHSteamUser()
    {
        return NULL;
    }

    __declspec(dllexport) HSteamPipe __cdecl SteamAPI_GetHSteamPipe()
    {
        return NULL;
    }

    __declspec(dllexport) HSteamUser __cdecl SteamAPI_GetHSteamUser()
    {
        return NULL;
    }

    __declspec(dllexport) const char *__cdecl SteamAPI_GetSteamInstallPath()
    {
        return NULL;
    }

    __declspec(dllexport) bool __cdecl SteamAPI_Init()
    {
#if USE_MANAGED_CODE
		IW4::AdminPluginCode::Initialize();
#endif
		//HideCode_FindCreateFile();

		if (GAME_FLAG(GAME_FLAG_DEDICATED))
		{
			InitializeDediConfig();
			Com_SaveDediConfig();
		}

		SteamProxy_Init();

		return true;
    }

    __declspec(dllexport) bool __cdecl SteamAPI_InitSafe()
    {
        return true;
    }

	__declspec(dllexport) char __cdecl SteamAPI_RestartApp()
    {
        return 1;
    }

	__declspec(dllexport) char __cdecl SteamAPI_RestartAppIfNecessary()
    {
        return 0;
    }
    
	__declspec(dllexport) void __cdecl SteamAPI_RegisterCallResult( CCallbackBase* pResult, SteamAPICall_t APICall )
    {
		CSteamBase::RegisterCallResult(APICall, pResult);
    }

    __declspec(dllexport) void __cdecl SteamAPI_RegisterCallback( CCallbackBase *pCallback, int iCallback )
    {
		CSteamBase::RegisterCallback(pCallback, iCallback);
    }

    __declspec(dllexport) void __cdecl SteamAPI_RunCallbacks()
    {
		CSteamBase::RunCallbacks();
    }

    __declspec(dllexport) void __cdecl SteamAPI_SetMiniDumpComment( const char *pchMsg )
    {

    }

    __declspec(dllexport) bool __cdecl SteamAPI_SetTryCatchCallbacks( bool bUnknown )
    {
        return bUnknown;
    }

    __declspec(dllexport) void __cdecl SteamAPI_Shutdown()
    {

    }

    __declspec(dllexport) void __cdecl SteamAPI_UnregisterCallResult( CCallbackBase* pResult, SteamAPICall_t APICall )
    {

    }

    __declspec(dllexport) void __cdecl SteamAPI_UnregisterCallback( CCallbackBase *pCallback, int iCallback )
    {

    }

    __declspec(dllexport) void __cdecl SteamAPI_WriteMiniDump( uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID )
    {

    }

    __declspec(dllexport) ISteamApps* __cdecl SteamApps()
    {
		Trace("S_API", "SteamApps");
		return NULL;
    }

    __declspec(dllexport) ISteamClient* __cdecl SteamClient()
    {
		Trace("S_API", "SteamClient");
		return NULL;
    }

    __declspec(dllexport) ISteamContentServer* __cdecl SteamContentServer()
    {
        return NULL;
    }

    __declspec(dllexport) ISteamUtils* __cdecl SteamContentServerUtils()
    {
        return NULL;
    }

    __declspec(dllexport) bool __cdecl SteamContentServer_Init( unsigned int uLocalIP, unsigned short usPort )
    {
        return NULL;
    }

    __declspec(dllexport) void __cdecl SteamContentServer_RunCallbacks()
    {

    }

    __declspec(dllexport) void __cdecl SteamContentServer_Shutdown()
    {

    }

    __declspec(dllexport) ISteamFriends* __cdecl SteamFriends()
    {
		Trace("S_API", "SteamFriends");
		return (ISteamFriends*)CSteamBase::GetInterface(INTERFACE_STEAMFRIENDS005);
    }

    __declspec(dllexport) ISteamGameServer* __cdecl SteamGameServer()
    {
		Trace("S_API", "SteamGameServer");
		return (ISteamGameServer*)CSteamBase::GetInterface(INTERFACE_STEAMGAMESERVER009);
    }

    __declspec(dllexport) ISteamUtils* __cdecl SteamGameServerUtils()
    {
        return NULL;
    }

    __declspec(dllexport) bool __cdecl SteamGameServer_BSecure()
    {
        return true;
    }

    __declspec(dllexport) HSteamPipe __cdecl SteamGameServer_GetHSteamPipe()
    {
        return NULL;
    }

    __declspec(dllexport) HSteamUser __cdecl SteamGameServer_GetHSteamUser()
    {
        return NULL;
    }

    __declspec(dllexport) int32 __cdecl SteamGameServer_GetIPCCallCount()
    {
        return NULL;
    }

    __declspec(dllexport) uint64 __cdecl SteamGameServer_GetSteamID()
    {
        return NULL;
    }

    __declspec(dllexport) bool __cdecl SteamGameServer_Init( uint32 unIP, uint16 usPort, uint16 usGamePort, EServerMode eServerMode, int nGameAppId, const char *pchGameDir, const char *pchVersionString )
    {
        return true;
    }

    __declspec(dllexport) bool __cdecl SteamGameServer_InitSafe( uint32 unIP, uint16 usPort, uint16 usGamePort, EServerMode eServerMode, int nGameAppId, const char *pchGameDir, const char *pchVersionString, unsigned long dongs )
    {
        return true;
    }

    __declspec(dllexport) void __cdecl SteamGameServer_RunCallbacks()
    {

    }

    __declspec(dllexport) void __cdecl SteamGameServer_Shutdown()
    {

    }

    __declspec(dllexport) ISteamMasterServerUpdater* __cdecl SteamMasterServerUpdater()
    {
		Trace("S_API", "SteamMasterServerUpdater");
		return (ISteamMasterServerUpdater*)CSteamBase::GetInterface(INTERFACE_STEAMMASTERSERVERUPDATER001);
    }

    __declspec(dllexport) ISteamMatchmaking* __cdecl SteamMatchmaking()
    {
		Trace("S_API", "SteamMatchmaking");
		return (ISteamMatchmaking*)CSteamBase::GetInterface(INTERFACE_STEAMMATCHMAKING007);
    }

    __declspec(dllexport) ISteamMatchmakingServers* __cdecl SteamMatchmakingServers()
    {
        return NULL;
    }

    __declspec(dllexport) ISteamNetworking* __cdecl SteamNetworking()
    {
		//Trace("S_API", "SteamNetworking");
		return (ISteamNetworking*)CSteamBase::GetInterface(INTERFACE_STEAMNETWORKING003);
    }

    __declspec(dllexport) void* __cdecl SteamRemoteStorage()
    {
		return CSteamBase::GetInterface(INTERFACE_STEAMREMOTESTORAGE002);
    }

    __declspec(dllexport) ISteamUser* __cdecl SteamUser()
    {
		//Trace("S_API", "SteamUser");
		return (ISteamUser*)CSteamBase::GetInterface(INTERFACE_STEAMUSER012);
    }

    __declspec(dllexport) ISteamUserStats* __cdecl SteamUserStats()
    {
		Trace("S_API", "SteamUserStats");
		return NULL;
    }

    __declspec(dllexport) ISteamUtils* __cdecl SteamUtils()
    {
		Trace("S_API", "SteamUtils");
		return (ISteamUtils*)CSteamBase::GetInterface(INTERFACE_STEAMUTILS005);
    }

    __declspec(dllexport) HSteamUser __cdecl Steam_GetHSteamUserCurrent()
    {
        return NULL;
    }

    __declspec(dllexport) void __cdecl Steam_RegisterInterfaceFuncs( void *hModule )
    {

	}

    __declspec(dllexport) void __cdecl Steam_RunCallbacks( HSteamPipe hSteamPipe, bool bGameServerCallbacks )
    {
 
    }

    __declspec(dllexport) void *g_pSteamClientGameServer = NULL;
}