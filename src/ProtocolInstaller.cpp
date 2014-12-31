#include "StdInc.h"
#include <shellapi.h>
#include <Shlobj.h>

void ProtocolInstaller_Register()
{
	LSTATUS result;

#define CHECK_STATUS(x) \
	result = (x); \
	if (result != ERROR_SUCCESS) { \
	Trace("IW4LAN.Connect", #x " failed: %x", result); \
	return; \
	}

	HKEY iw4lan;
	wchar_t path[MAX_PATH];
	wchar_t iconPath[MAX_PATH];
	wchar_t command[1024];

	GetModuleFileNameW(NULL, path, sizeof(path));
	swprintf_s(command, L"\"%s\" -connect \"%%1\"", path);
	swprintf_s(iconPath, L"\"%s\",1", path);

	CHECK_STATUS(RegCreateKey(HKEY_CURRENT_USER, "SOFTWARE\\Classes\\iw4lan", &iw4lan));
	CHECK_STATUS(RegSetValueEx(iw4lan, NULL, 0, REG_SZ, (BYTE*)"IW4LAN Connect Protocol", 24));
	CHECK_STATUS(RegCloseKey(iw4lan));

	CHECK_STATUS(RegCreateKey(HKEY_CURRENT_USER, "SOFTWARE\\Classes\\iw4lan\\DefaultIcon", &iw4lan));
	CHECK_STATUS(RegSetValueExW(iw4lan, NULL, 0, REG_SZ, (BYTE*)iconPath, (wcslen(iconPath) * sizeof(wchar_t)) + 2));
	CHECK_STATUS(RegCloseKey(iw4lan));

	CHECK_STATUS(RegCreateKey(HKEY_CURRENT_USER, "SOFTWARE\\Classes\\iw4lan\\shell\\open\\command", &iw4lan));
	CHECK_STATUS(RegSetValueExW(iw4lan, NULL, 0, REG_SZ, (BYTE*)command, (wcslen(command) * sizeof(wchar_t)) + 2));
	CHECK_STATUS(RegCloseKey(iw4lan));
}

void ConnectToAddress(netadr_t address);

void ProtocolInstaller_Connect(const wchar_t* address)
{
	// ask the user whether they want to connect
	int mbResult = MessageBoxA(NULL, va("Do you want to connect to '%s'?", address), "IW4LAN", MB_YESNO | MB_ICONQUESTION);

	if (mbResult == IDNO)
	{
		return;
	}
	else
	{
		MessageBoxA(NULL, "Connecting...", "IW4LAN", NULL);

		/*
		const char* str = Cmd_Argv(1);
		netadr_t adr;

		if (NET_StringToAdr(str, &adr))
		{
			ConnectToAddress(adr);
		}
		*/
	}
}

void ProtocolInstaller_Run()
{
	ProtocolInstaller_Register();

	int argc;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc == 3)
	{
		if (wcscmp(argv[1], L"-connect") == NULL)
		{
			FreeConsole();

			ProtocolInstaller_Connect(argv[2]);

			LocalFree(argv);
			ExitProcess(0);
		}
	}

	LocalFree(argv);
}