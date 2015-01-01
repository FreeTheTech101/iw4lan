// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Steam proxying functionality
//
// Initial author: NTAuthority
// Started: 2011-11-05
// ==========================================================

#include "StdInc.h"
#include "Interface_OSW.h"

static CSteamAPILoader loader;

struct
{
	CreateInterfaceFn clientFactory;
	HSteamPipe pipe;
	HSteamUser user;

	ISteamClient008* steamClient;

	bool initialized;

	int eaCall;
} g_steamProxy;

bool SteamProxy_InitInternal()
{
	g_steamProxy.clientFactory = loader.Load();

	if (g_steamProxy.clientFactory == NULL)
	{
		return false;
	}

	g_steamProxy.steamClient = (ISteamClient008*)g_steamProxy.clientFactory(STEAMCLIENT_INTERFACE_VERSION_008, NULL);

	if (g_steamProxy.steamClient == NULL)
	{
		return false;
	}

	g_steamProxy.pipe = g_steamProxy.steamClient->CreateSteamPipe();

	if (g_steamProxy.pipe == NULL)
	{
		return false;
	}

	g_steamProxy.user = g_steamProxy.steamClient->ConnectToGlobalUser(g_steamProxy.pipe);

	if (g_steamProxy.user == NULL)
	{
		return false;
	}

	ISteamUtils005* steamUtils = (ISteamUtils005*)g_steamProxy.steamClient->GetISteamUtils(g_steamProxy.pipe, STEAMUTILS_INTERFACE_VERSION_005);
	int appID = steamUtils->GetAppID();

	if (appID == 0)
	{
		return false;
	}

	ISteamUser012* steamUser = (ISteamUser012*)g_steamProxy.steamClient->GetISteamUser(g_steamProxy.user, g_steamProxy.pipe, STEAMUSER_INTERFACE_VERSION_012);
	CSteamID steamID = steamUser->GetSteamID();

	return true;
}

void SteamProxy_LoadGameOverlayRenderer()
{
	if (GetModuleHandle("gameoverlayrenderer.dll") != NULL)
	{
		return;
	}

	std::string gameOverlayRendererPath = loader.GetSteamDir() + "\\gameoverlayrenderer.dll";
	LoadLibrary(gameOverlayRendererPath.c_str());
}

void SteamProxy_Init()
{
	//SetEnvironmentVariable("SteamAppId", "10190");

	FILE* file = fopen("steam_appid.txt", "w");
	if (file)
	{
		fprintf(file, "10190");
		fclose(file);
	}

	g_steamProxy.initialized = SteamProxy_InitInternal();

	if (g_steamProxy.initialized)
	{
		SteamProxy_LoadGameOverlayRenderer();
	}
}