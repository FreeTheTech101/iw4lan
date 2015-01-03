// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Notify server owners of an update.
//
// Initial author: redacted
// Started: 2013-12-08
// ==========================================================

#include "StdInc.h"

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

static int lastUpdateCheck;
static int timeTillShutdown;
static bool warningGiven;

#define UPDATE_LOCATION "http://twiz.re/"

size_t UDataReceived(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t rsize = (size * nmemb);
	char* text = (char*)ptr;
	int version = atoi(text);

	if (version > BUILDNUMBER)
	{
		if (GAME_FLAG(GAME_FLAG_DEDICATED))
		{
			if ((Com_Milliseconds() - timeTillShutdown) > 21600000)
			{
				// kick all players
				for (int i = 0; i < *svs_numclients; i++)
				{
					client_t* client = &svs_clients[i];

					if (client->state < 3)
					{
						continue;
					}

					Cmd_ExecuteSingleCommand(0, 0, va("clientkick %i", client));
				}

				Com_Error(0, "This version (%d) of IW4LAN is outdated.\nPlease visit " UPDATE_LOCATION " to obtain a new version (%d).", BUILDNUMBER, version);
			}

			Com_Printf(0, "[UpdateCheck] This version (%d) of IW4LAN, please visit " UPDATE_LOCATION " to obtain a new version (%d).", BUILDNUMBER, version);

			if (!warningGiven)
			{
				MessageBoxA(NULL, va("This version (%d) of IW4LAN is outdated. Please shut down the server and visit " UPDATE_LOCATION " to obtain a new version (%d).\nThe server will automatically shutdown in 6 hours should you not update.", BUILDNUMBER, version), "Update available", MB_OK | MB_ICONINFORMATION);

				warningGiven = true;
				timeTillShutdown = Com_Milliseconds();
			}

			for (int i = 0; i < *svs_numclients; i++)
			{
				client_t* client = &svs_clients[i];

				if (client->state < 3)
				{
					continue;
				}

				SV_GameSendServerCommand(i, 0, va("%c \"This version (%d) of ^2IW4LAN^7 is outdated.\"", 104, BUILDNUMBER));
				SV_GameSendServerCommand(i, 0, va("%c \"Please visit " UPDATE_LOCATION " to obtain a new version (%d).\"", 104, version));
			}
		}
		else
		{
			Com_Error(1, "This version (%d) of IW4LAN is outdated. Please visit " UPDATE_LOCATION " to obtain a new version (%d).", BUILDNUMBER, version);
		}
	}

	return rsize;
}

void UpdateCheck()
{
	if (IsDebuggerPresent()) return;

	char url[255];
	_snprintf(url, sizeof(url), UPDATE_LOCATION "/version.txt");

	if (GAME_FLAG(GAME_FLAG_DEDICATED))
	{
		if ((Com_Milliseconds() - lastUpdateCheck) > 120000)
		{
			curl_global_init(CURL_GLOBAL_ALL);

			CURL* curl = curl_easy_init();

			if (curl)
			{
				curl_easy_setopt(curl, CURLOPT_URL, url);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, UDataReceived);
				curl_easy_setopt(curl, CURLOPT_USERAGENT, VERSIONSTRING);
				curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

				CURLcode code = curl_easy_perform(curl);
				curl_easy_cleanup(curl);

				curl_global_cleanup();

				if (code == CURLE_OK)
				{
					lastUpdateCheck = Com_Milliseconds();

					return;
				}
				else
				{
					lastUpdateCheck = Com_Milliseconds();

					Com_Printf(0, "[UpdateCheck] Could not reach the update server.\n");
				}
			}

			curl_global_cleanup();

			lastUpdateCheck = Com_Milliseconds();
		}
	}
	else
	{
		curl_global_init(CURL_GLOBAL_ALL);

		CURL* curl = curl_easy_init();

		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, UDataReceived);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, VERSIONSTRING);
			curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

			CURLcode code = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			curl_global_cleanup();

			if (code == CURLE_OK)
			{
				return;
			}
			else
			{
				Com_Printf(0, "[UpdateCheck] Could not reach the update server.\n");
			}
		}

		curl_global_cleanup();
	}
}