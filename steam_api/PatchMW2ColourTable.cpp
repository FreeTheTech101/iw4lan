// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Apply W² Colour Tables
//
// Initial author: Unknown? 
// Started: 2015-05-04
// ==========================================================

#include "StdInc.h"

DWORD* color_table;

void PatchMW2_ColorTable()
{
	//Find IW4 ColourTable
	color_table = (DWORD*)FindPattern(0x401000, 0x800000, (PBYTE)"\x00\x00\x00\xFF\xFF\x5C\x5C\xFF\x00\xFF\x00\xFF\xFF\xFF\x00\xFF\x00\x00\xFF\xFF\x00\xFF\xFF\xFF\xFF\x5C\xFF\xFF\xFF\xFF\xFF\xFF", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	//Set W² ColourTable
	color_table[1] = RGB(255, 49, 49);
	color_table[2] = RGB(134, 192, 0);
	color_table[3] = RGB(255, 173, 34);
	color_table[4] = RGB(0, 135, 193);
	color_table[5] = RGB(32, 197, 255);
	color_table[6] = RGB(151, 80, 221);
}
