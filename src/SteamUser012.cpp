// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: ISteamUser012 implementation
//
// Initial author: NTAuthority
// Started: 2010-09-10
// ==========================================================

#include "StdInc.h"
#include "SteamUser012.h"
#include "base64.h"
#include <Iphlpapi.h>
#include <shellapi.h>

unsigned int steamID = 0;

unsigned int jenkins_one_at_a_time_hash(char *key, size_t len)
{
	unsigned int hash, i;
	for(hash = i = 0; i < len; ++i)
	{
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

char macAddress[15] = "";
BYTE macid[6];

int getMAC(char *macadr)
{
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	char *mac_addr = (char*)malloc(17);
	
	strcpy(macadr, "");
	AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));

	if (AdapterInfo == NULL) 
	{
		return 0;
	}
	
	// make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);

		if (AdapterInfo == NULL)
		{
			return 0;
		}
	}
	
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) 
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // contains pointer to current adapter info
		
		sprintf(macadr, "%02X%02X%02X%02X%02X%02X",
			pAdapterInfo->Address[0],
			pAdapterInfo->Address[1],
			pAdapterInfo->Address[2],
			pAdapterInfo->Address[3],
			pAdapterInfo->Address[4],
			pAdapterInfo->Address[5]);

		memcpy(&macid[0], &pAdapterInfo->Address[0], 6);
		free(AdapterInfo);

		return 1;
	}

	return 0;
}

unsigned int GetHardwareID()
{
	std::string hwid;
	HW_PROFILE_INFO hwProfileInfo;

	// get MAC address
	getMAC(macAddress);	

	if (GetCurrentHwProfile(&hwProfileInfo) != NULL && getMAC(macAddress) == 1)
	{ // if both are fine
		hwid = va("%s%s", hwProfileInfo.szHwProfileGuid, macAddress);
	}
	else if (GetCurrentHwProfile(&hwProfileInfo) != NULL && getMAC(macAddress) != 1)
	{ // if GetCurrentHwProfile isfine but getMAC isbad
		hwid = va("%s", hwProfileInfo.szHwProfileGuid);
	}
	else if (GetCurrentHwProfile(&hwProfileInfo) == NULL && getMAC(macAddress) == 1)
	{ // if GetCurrentHwProfile is bad but getMAC is fine
		hwid = va("%s", macAddress);
	}
	else
	{
		int mbResult = MessageBoxA(NULL, "Unable to generate hardware ID. Please report this error on GitHub.\nClick OK to be redirected to GitHub.", "Error", MB_OKCANCEL | MB_ICONERROR);
		
		if (mbResult == IDOK)
		{
			ShellExecute(NULL, "open", "https://github.com/twizre/iw4lan/issues", 0, 0, SW_SHOWNORMAL);
		}

		TerminateProcess(GetCurrentProcess(), 0);
	}

	// encode with base64
	size_t idLen;
	char* idEncoded = base64_encode((unsigned char*)hwid.c_str(), hwid.size(), &idLen);

	return jenkins_one_at_a_time_hash(idEncoded, strlen(idEncoded));
}

unsigned int GetPlayerSteamID()
{
	if (steamID) return steamID;

	/* OSVERSIONINFO versionInfo;
	GetVersionEx(&versionInfo);

	if (versionInfo.dwMajorVersion < 6)
	{
		return 2;
	} */

	steamID = GetHardwareID();
	return steamID;
}

HSteamUser CSteamUser012::GetHSteamUser()
{
	return NULL;
}

bool CSteamUser012::LoggedOn()
{
	return true;
}

CSteamID CSteamUser012::GetSteamID()
{
	int id = 0;
	
	id = GetPlayerSteamID();

	return CSteamID( id, 1, k_EUniversePublic, k_EAccountTypeIndividual );
}

int CSteamUser012::InitiateGameConnection( void *pAuthBlob, int cbMaxAuthBlob, CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer, bool bSecure )
{
	return 0;
}

void CSteamUser012::TerminateGameConnection( uint32 unIPServer, uint16 usPortServer )
{
}

void CSteamUser012::TrackAppUsageEvent( CGameID gameID, EAppUsageEvent eAppUsageEvent, const char *pchExtraInfo )
{
}

bool CSteamUser012::GetUserDataFolder( char *pchBuffer, int cubBuffer )
{
	return true;
}

void CSteamUser012::StartVoiceRecording( )
{
}

void CSteamUser012::StopVoiceRecording( )
{
}

EVoiceResult CSteamUser012::GetCompressedVoice( void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten )
{
	return k_EVoiceResultOK;
}

EVoiceResult CSteamUser012::DecompressVoice( void *pCompressed, uint32 cbCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten )
{
	return k_EVoiceResultOK;
}

HAuthTicket CSteamUser012::GetAuthSessionTicket( void *pTicket, int cbMaxTicket, uint32 *pcbTicket )
{
	return 0;
}

EBeginAuthSessionResult CSteamUser012::BeginAuthSession( const void *pAuthTicket, int cbAuthTicket, CSteamID steamID )
{
	return k_EBeginAuthSessionResultOK;
}

void CSteamUser012::EndAuthSession( CSteamID steamID )
{

}

void CSteamUser012::CancelAuthTicket( HAuthTicket hAuthTicket )
{

}

uint32 CSteamUser012::UserHasLicenseForApp( CSteamID steamID, AppId_t appID )
{
	return 1;
}
