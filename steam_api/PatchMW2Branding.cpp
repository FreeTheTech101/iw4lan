// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: branding for IW4M
//
// Initial author: NTAuthority
// Started: 2011-06-09
// ==========================================================

#include "StdInc.h"

typedef int (__cdecl * R_TextWidth_t)(const char* text, int maxlength, Font* font);
R_TextWidth_t R_TextWidth = (R_TextWidth_t)0x5056C0;

typedef void* (__cdecl * R_RegisterFont_t)(const char* asset);
R_RegisterFont_t R_RegisterFont = (R_RegisterFont_t)0x505670;

typedef void (__cdecl * R_AddCmdDrawText_t)(const char* text, int, void* font, float screenX, float screenY, float, float, float rotation, float* color, int);
R_AddCmdDrawText_t R_AddCmdDrawText = (R_AddCmdDrawText_t)0x509D80;

dvar_t* cg_hideVersion;

CallHook drawDevStuffHook;
DWORD drawDevStuffHookLoc = 0x5ACB99;

int R_GetScaledWidth(const char* text, float sizeX, void* font)
{
	if (!R_TextWidth)
	{
		return 0;
	}

	int normalWidth = R_TextWidth(text, 0x7FFFFFFF, (Font*)font);
	double scaledWidth = normalWidth * sizeX;

	return (int)scaledWidth;
}

void DrawBranding()
{
	if (!cg_hideVersion->current.boolean)
	{
		float font_size = 0.7f;
		void* font = R_RegisterFont("fonts/normalFont");

		int textWidth = R_GetScaledWidth("IW4LAN", font_size, font);

		auto width = [] (int offset, int width)
		{
			return *(int*)0x66E1C68 - ( offset + width );
		};

		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (CL_IsCgameInitialized())
		{
			color[3] = 0.3f;
		}

		R_AddCmdDrawText("IW4LAN", 0x7FFFFFFF, font, width(10, textWidth), 30, font_size, font_size, 0.0f, color, 0);
	}

}

#pragma optimize("", off)
void __declspec(naked) DrawDevStuffHookStub()
{
	__asm
	{
		call DrawBranding
		jmp drawDevStuffHook.pOriginal
	}
}
#pragma optimize("", on)

HWND WINAPI CreateWindowExAWrap_WC(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (strcmp(lpClassName, "IW4 WinConsole"))
	{
		return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	}

	return CreateWindowExW(dwExStyle, L"IW4 WinConsole", L"IW4LAN Console", dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

HWND WINAPI CreateWindowExAWrap_G(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return CreateWindowExW(dwExStyle, L"IW4", L"IW4LAN", dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

void PatchMW2_Branding()
{
	cg_hideVersion = Dvar_RegisterBool("cg_hideVersion", false, DVAR_FLAG_SAVED, "Hide the version release branding text.");

	drawDevStuffHook.initialize(drawDevStuffHookLoc, DrawDevStuffHookStub);
	drawDevStuffHook.installHook();

	// createwindowexa on winconsole
	static DWORD wcCWEx = (DWORD)CreateWindowExAWrap_WC;
	static DWORD wcGEx = (DWORD)CreateWindowExAWrap_G;
	*(DWORD**)0x4289CA = &wcCWEx;
	*(DWORD**)0x5076AC = &wcGEx;

	// displayed build tag in UI code
	*(DWORD*)0x43F73B = (DWORD)VERSIONSTRING;

	// console '%s: %s> ' string
	*(DWORD*)0x5A44B4 = (DWORD)(VERSIONSTRING "> ");

	// console version string
	*(DWORD*)0x4B12BB = (DWORD)(VERSIONSTRING " (built " __DATE__ " " __TIME__ ")");

	// version string
	*(DWORD*)0x60BD56 = (DWORD)(VERSIONSTRING " (built " __DATE__ " " __TIME__ ")");

	// set fs_basegame to 'iw4lan' (will apply before fs_game, unlike the above line)
	*(DWORD*)0x6431D1 = (DWORD)"iw4lan";

	// increase font sizes for chat on higher resolutions
	static float float13 = 13.0f;
	static float float10 = 10.0f;
	*(float**)0x5814AE = &float13;
	*(float**)0x5814C8 = &float10;

#ifndef MATCHMAKING
	// use M2 playlists
	strcpy((char*)0x6EE7AC, "mp_playlists_m2");
	*(DWORD*)0x4D47FB = (DWORD)"mp_playlists_m2.ff";
#endif
}