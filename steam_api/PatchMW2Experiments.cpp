// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Modern Warfare 2 patches: experimental dev code
//
// Initial author: NTAuthority
// Started: 2011-05-09
// ==========================================================

#include "StdInc.h"
#include <shlobj.h>
#include <direct.h>
#include <unordered_map>

typedef void* (__cdecl * LoadRawFile_t)(const char* filename, void* buffer, size_t buflen);
LoadRawFile_t LoadRawFile = (LoadRawFile_t)0x4DA0D0;

CallHook addEntryNameHook;
DWORD addEntryNameHookLoc = 0x5BB697;

StompHook endLoadingHook;
DWORD endLoadingHookLoc = 0x4E5E5C;

#define MAX_RAWFILE_NAMES 32768
static const char* rawFileNames[MAX_RAWFILE_NAMES];
static int currentRawFile = 0;

#define MAX_STABLE_NAMES 32768
static const char* sTableNames[MAX_STABLE_NAMES];
static int currentSTable = 0;

#define RAWFILE_BUFSIZE 2097152
static char rawFileBuffer[RAWFILE_BUFSIZE];

typedef struct stringTable_s {
	char* fileName;
	int columns;
	int rows;
	char** data;
} stringTable_t;

stringTable_t* stringTable;

bool useEntryNames = false;

void AddEntryNameHookFunc(int type, const char* name)
{
	if (GAME_FLAG(GAME_FLAG_ENTRIES) || useEntryNames)
	{
		OutputDebugString(va("%d: %s\n", type, name));
	}

	/*if (type == ASSET_TYPE_STRINGTABLE)
	{
		if (currentSTable < MAX_STABLE_NAMES)
		{
			sTableNames[currentSTable] = name;
			currentSTable++;
		}
	}
	return;

	if (type == ASSET_TYPE_RAWFILE)
	{
		if (currentRawFile < MAX_RAWFILE_NAMES)
		{
			rawFileNames[currentRawFile] = name;
			currentRawFile++;
		}
	}*/
}

void DumpRawFiles()
{
	if (Cmd_Argc() != 2)
	{
		return;
	}

	rawFileNames[0] = Cmd_Argv(1);
	currentRawFile = 1;

	for (int i = 0; i < currentRawFile; i++)
	{
		const char* name = rawFileNames[i];
		memset(rawFileBuffer, 0, RAWFILE_BUFSIZE);

		if (LoadRawFile(name, rawFileBuffer, RAWFILE_BUFSIZE))
		{
			char filename[512];
			char dir[512];
			size_t length = strlen(rawFileBuffer);
			sprintf(filename, "%s/%s", "maw", name);

			GetCurrentDirectoryA(sizeof(dir), dir);
			strcat(dir, "/");
			strcat(dir, filename);
			*(strrchr(dir, '/')) = '\0';

			size_t strl = strlen(dir);

			for (size_t i = 0; i < strl; i++)
			{
				if (dir[i] == '/') dir[i] = '\\';
			}

			SHCreateDirectoryExA(NULL, dir, NULL);

			FILE* file = fopen(filename, "wb");

			if (file)
			{
				fwrite(rawFileBuffer, 1, length, file);
				fclose(file);
			}
		}
	}

	currentRawFile = 0;

	/*for (int i = 0; i < currentSTable; i++)
	{
		const char* name = sTableNames[i];
		stringTable_t* stringTable;

		if (stringTable = (stringTable_t*)DB_FindXAssetHeader(37, name))
		{
			char filename[512];
			char dir[512];
			sprintf(filename, "%s/%s", "raw", name);

			GetCurrentDirectoryA(sizeof(dir), dir);
			strcat(dir, "/");
			strcat(dir, filename);
			*(strrchr(dir, '/')) = '\0';

			size_t strl = strlen(dir);

			for (size_t i = 0; i < strl; i++)
			{
				if (dir[i] == '/') dir[i] = '\\';
			}

			SHCreateDirectoryExA(NULL, dir, NULL);

			FILE* file = fopen(filename, "w");

			if (file)
			{
				int currentColumn = 0;
				int currentRow = 0;
				int total = stringTable->columns * stringTable->rows;

				for (int i = 0; i < total; i++) {
					char* current = stringTable->data[i * 2];

					fprintf(file, "%s", current);

					bool isNext = ((i + 1) % stringTable->columns) == 0;

					if (isNext) {
						fprintf(file, "\n");
					} else {
						fprintf(file, ",");
					}

					fflush(file);
				}

				fclose(file);
			}
		}
	}

	currentSTable = 0;*/
}

void __declspec(naked) EndLoadingHookStub()
{
	__asm
	{
		mov eax, 05091E0h
		call eax
		call DumpRawFiles
		retn
	}
}

void __declspec(naked) AddEntryNameHookStub()
{
	__asm
	{
		push ecx
		push eax
		call AddEntryNameHookFunc
		pop eax
		pop ecx
		jmp addEntryNameHook.pOriginal
	}
}

typedef struct weaponEntry_s
{
	const char* name;
	int offset;
	int type;
} weaponEntry_t;

#define NUM_ENTRIES 672
#define NUM_VEHICLE_ENTRIES 143

#define WEAPON_DO_ARRAY(ar, c) \
{ \
	for (int _l_1 = 0; _l_1 < c; _l_1++) \
	{ \
		if (*(int*)data == _l_1) \
		{ \
			fprintf(file, "%s", ((char**)ar)[_l_1]); /* why do I have to explicitly define ar as being a char**? */ \
		} \
	} \
}

weaponEntry_t* weaponEntries = (weaponEntry_t*)0x795F00;
weaponEntry_t* vehicleEntries = (weaponEntry_t*)0x79F2E8;

const char* SL_ConvertToString(unsigned int sl)
{
	__asm
	{
		push sl
		mov eax, 4EC1D0h
		call eax
		add esp, 4h
		mov sl, eax
	}

	return (const char*)sl;
}

void DumpNoteTrackEntry(FILE* file, int type, char* data, char* data2)
{
	switch (type)
	{
		case 39: // notetrack-to-sound
		case 40: // notetrack-to-rumble
			{
				// as seen last time, SL is usable now.
				short* keys = *(short**)data;
				short* values = *(short**)data2;

				for (int i = 0; i < 16; i++)
				{
					short key = keys[i];
					short value = values[i];

					if (key)
					{
						fprintf(file, "%s %s%s", SL_ConvertToString(key), SL_ConvertToString(value), (i == 15 || keys[i + 1] == 0) ? "" : "\n"); // it seems like a newline is needed for all but the last tag
					}
				}
			}
			break;
	}
}

bool thisIsWeaponfile = true;

void DumpEntry(FILE* file, int type, char* data)
{
	switch (type)
	{
	case 0: // string
		{
			char* str = (char*)(*(DWORD_PTR*)data);

			if (str && (int)str != 0xFFFFFFFF && (int)str > 16)
			{
				fprintf(file, "%s", str);
			}
			break;
		}
	case 1: // char[1024]
	case 2: // char[64]
	case 3: // char[256]
		{
			fprintf(file, "%s", *(char**)data);
			break;
		}
	case 4:
	case 5: // bool-as-int
		{
			int number = *(int*)data;

			fprintf(file, "%d", number);
			break;
		}
	case 6:
		{
			bool boolean = *(bool*)data;

			fprintf(file, "%d", (boolean) ? 1 : 0);
			break;
		}
	case 7:
		{
			float number = *(float*)data;

			fprintf(file, "%g", number);
			break;
		}
	case 8:
		{
			float number = *(float*)data;

			fprintf(file, "%g", (number / 17.6f)); // miles/hour - inch/sec
			break;
		}
	case 9:
		{
			int number = *(int*)data;

			fprintf(file, "%g", (number / 1000.0f));
			break;
		}
	case 16:
		if (thisIsWeaponfile)
		{
			WEAPON_DO_ARRAY(0x795DA0, 4)
		}
		else
		{
			WEAPON_DO_ARRAY(0x79F2C0, 7)
		}
		break;
	case 17:
		if (thisIsWeaponfile)
		{
			WEAPON_DO_ARRAY(0x795DB0, 12)
		}
		else
		{
			WEAPON_DO_ARRAY(0x79F2DC, 3)
		}
		break;
	case 18:
		if (thisIsWeaponfile)
		{
			WEAPON_DO_ARRAY(0x795E68, 2)
		}
		else
		{
			WEAPON_DO_ARRAY(0x79F2DC, 3)
		}
		break;
	case 19:
		if (thisIsWeaponfile)
		{
			WEAPON_DO_ARRAY(0x795E10, 4)
		}
		else
		{
			WEAPON_DO_ARRAY(0x79F2DC, 3)
		}
		break;
	case 20:
		WEAPON_DO_ARRAY(0x795E20, 11)
		break;
	case 21:
		WEAPON_DO_ARRAY(0x795E70, 3)
		break;
	case 22:
		WEAPON_DO_ARRAY(0x795E4C, 7)
		break;
	case 23:
		WEAPON_DO_ARRAY(0x795E7C, 6)
		break;
	case 24:
		WEAPON_DO_ARRAY(0x7BE928, *(int*)0x7BDDDC)
		break;
	case 25:
		WEAPON_DO_ARRAY(0x795E94, 3)
		break;
	case 26:
		WEAPON_DO_ARRAY(0x795EA0, 4)
		break;
	case 28:
		WEAPON_DO_ARRAY(0x795EB0, 6)
		break;
	case 29:
		WEAPON_DO_ARRAY(0x795EC8, 3)
		break;
	case 30:
		WEAPON_DO_ARRAY(0x795DE0, 6)
		break;
	case 31:
		WEAPON_DO_ARRAY(0x795DF8, 6)
		break;
	case 32:
		WEAPON_DO_ARRAY(0x795ED4, 7)
		break;
	case 33:
		WEAPON_DO_ARRAY(0x795EF0, 3)
		break;
	case 34:
		WEAPON_DO_ARRAY(0x795EF0, 3) // same as 33
		break;
	case 35:
		WEAPON_DO_ARRAY(0x795EF0, 3) // same as 33
		break;
	case 36:
		WEAPON_DO_ARRAY(0x795EF0, 3) // same as 33
		break;
	case 37:
		WEAPON_DO_ARRAY(0x795EF0, 3) // same as 33
		break;
	case 13: // phys collmap
		break;
	case 10: // fx
	case 11: // xmodel
	case 12: // material
	case 14: // soundalias
	case 15: // tracer
		{
			BYTE* model = (BYTE*)(*(DWORD_PTR*)data);

			if (model)
			{
				char* str = (char*)(*(DWORD_PTR*)model);

				if (str)
				{
					fprintf(file, "%s", str);
				}
			}
			break;
		}
	case 38: // hideTags
		{
			// hopefully we'll have useful shit here, as if we don't, we're screwed
			// for some reason this code made me go deja vu - seems like I've done SL_ConvertToString before.
			short* tags = (short*)data;
			for (int i = 0; i < 32; i++)
			{
				short tag = tags[i];

				if (tag)
				{
					fprintf(file, "%s%s", SL_ConvertToString(tag), (i == 31 || tags[i + 1] == 0) ? "" : "\n"); // it seems like a newline is needed for all but the last tag
				}
			}
			break;
		}
	case 27: // bounceSound; surface sound
		{
			char*** dat = *(char****)data;
			if (dat != 0)
			{
				char bounceName[128];
				strcpy(bounceName, **dat);
				strrchr(bounceName, '_')[0] = '\0';

				fprintf(file, "%s", bounceName);
			}
			break;
		}
	default:
		fprintf(file, "lol no type %d", type);
		break;
	}
}

typedef struct 
{
	int offsetStart;
	int pointerOrigin;
} rtOffsetMap_t;

// nvm this is just impossible
/*
static rtOffsetMap_t offsetMap[] =
{
	{ 116, 4 },
	{ 1784, 12 },
	{ 1848, 16 },
	{ 1996, 120 },
	{ 2060, 128 },
	{ 2208, 132 },
	{ 2356, 140 },
	{ 2388, 144 },
	{ 2420, 148 },
	{ 2452, 152 },
	{ 2484, 588 },
	{ 2548, 1208 },
	{ 2672, 1212 },
	{ 2796, 1576 },
	{ 0, 0, 0 }
};

char* MapOffsetToPointer(char* origin, int offset)
{
	// let's say offset is 120, and origin is 0 (just to be funs)
	// pointer +4 redirects to 1000
	// we want to end up with 1004

	int i = 0;
	while (offsetMap[i].offsetStart != 0)
	{
		int max = (offsetMap[i + 1].offsetStart == 0) ? 0x7FFFFF : offsetMap[i + 1].offsetStart;

		// as 120 means it should go pointer 4, we should do so here.
		// after that we need to return a resolved pointer (wtf?) from whatever offset we came from
		if (offset >= offsetMap[i].offsetStart && offset < max)
		{
			return MapOffsetToPointer(origin, offset);
		}
	}

	return (origin + offset);
}
*/

/*
*(_DWORD *)(result + 16) = result + 1848;
*(_DWORD *)(result + 120) = result + 1996;
*(_DWORD *)(result + 128) = result + 2060;
*(_DWORD *)(result + 132) = result + 2208;
*(_DWORD *)(result + 140) = result + 2356;
*(_DWORD *)(result + 144) = result + 2388;
*(_DWORD *)(result + 148) = result + 2420;
*(_DWORD *)(result + 152) = result + 2452;
*(_DWORD *)(result + 588) = result + 2484;
*(_DWORD *)(result + 1208) = result + 2548;
*(_DWORD *)(result + 1212) = result + 2672;
*(_DWORD *)(result + 1576) = result + 2796;
*/

// lolnope again
/*
char* GetPointerForOffset(char* origin, int offset)
{
	char* retptr = origin;

	if (offset >= 1784 && offset < 1848)
	{
		retptr = *(BYTE**)(retptr + 12);
		retptr += (offset - 1784);
	}

	if (offset >= 1848 && offset < 1996)
	{
		retptr = *(BYTE**)(retptr + 16);
		retptr += (offset - 1848);
	}

	if (offset >= 116)
	{
		retptr = *(BYTE**)(retptr + 4);
		retptr += (offset - 116);
	}

	if (offset >= 1996 && offset < 2060)
	{
		retptr = *(BYTE**)(retptr + 4);
		retptr += (offset - 1996);
	}
}
*/

#define NUM_OFFSET_MAPS 14

static rtOffsetMap_t offsetMap[] =
{
	{ 116, 4 },
	{ 1784, 12 },
	{ 1848, 16 },
	{ 1996, 120 },
	{ 2060, 128 },
	{ 2208, 132 },
	{ 2356, 140 },
	{ 2388, 144 },
	{ 2420, 148 },
	{ 2452, 152 },
	{ 2484, 588 },
	{ 2548, 1208 },
	{ 2672, 1212 },
	{ 2796, 1576 },
	{ 0, 0 }
};

char* MapOffsetToPointer(char* origin, int offset)
{
	if (!thisIsWeaponfile)
	{
		return (origin + offset);
	}

	for (int i = (NUM_OFFSET_MAPS - 1); i >= 0; i--)
	{
		rtOffsetMap_t* current = &offsetMap[i];
		rtOffsetMap_t* next = &offsetMap[i + 1];

		int max = next->offsetStart;
		if (max == 0) max = 0xFFFFFF;

		if (offset >= current->offsetStart && offset < max)
		{
			char* pointer = *(char**)MapOffsetToPointer(origin, current->pointerOrigin);
			return (pointer + (offset - current->offsetStart));
		}
	}

	return (origin + offset);
}

void DumpWeaponFile(FILE* file, char* data)
{
	for (int i = 0; i < ((thisIsWeaponfile) ? NUM_ENTRIES : NUM_VEHICLE_ENTRIES); i++)
	{
		weaponEntry_t* entry = (thisIsWeaponfile) ? &weaponEntries[i] : &vehicleEntries[i];

		char* ptr = MapOffsetToPointer(data, entry->offset);//(data + entry->offset);

		// quick patch to not export empty models (causes too large weaponfiles)
		if (entry->type == 11 && *(int*)ptr == 0)
		{
			continue;
		}

		// same for empty strings
		if (entry->type == 0)
		{
			char* str = (char*)(*(DWORD_PTR*)ptr);

			if (str && (int)str != 0xFFFFFFFF && (int)str > 16 && *str != 0)
			{
				// yeah I know, but it's 3:45 AM and I'm too lazy to invert this
			}
			else
			{
				continue;
			}
		}

		fprintf(file, "%s\\", entry->name);

		/*if (entry->offset > 116)
		{
			ptr = *(BYTE**)(data + 4);
			ptr += (entry->offset - 116);
		}*/

		if (entry->type == 39 || entry->type == 40)
		{
			DumpNoteTrackEntry(file, entry->type, MapOffsetToPointer(data, 140), MapOffsetToPointer(data, 144));
		}
		else
		{
			DumpEntry(file, entry->type, ptr);
		}

		if (i < (((thisIsWeaponfile) ? NUM_ENTRIES : NUM_VEHICLE_ENTRIES) - 1))
		{
			fprintf(file, "\\");
		}
	}
}

void DumpWeaponTypes(FILE* file)
{
	for (int i = 0; i < NUM_ENTRIES; i++)
	{
		weaponEntry_t* entry = &weaponEntries[i];

		//fprintf(file, "%s\\", entry->name);

		//DumpEntry(file, entry->type, (data + entry->offset));

		fprintf(file, "+0x%x:\t%s", entry->offset, entry->name);

		switch (entry->type)
		{
		case 0:
			fprintf(file, " (string)");
			break;
		case 7:
			fprintf(file, " (float)");
			break;
		case 8:
			fprintf(file, " (float / 17.6)");
			break;
		case 9:
			fprintf(file, " (float / 1000.0)");
			break;
		case 11:
			fprintf(file, " (xmodel)");
			break;
		case 0xD:
			fprintf(file, " (phys collmap)");
			break;
		default:
			fprintf(file, " (type %d)", entry->type);
			break;
		}

		fprintf(file, "\n");
	}
}

StompHook weaponFileHook;
DWORD weaponFileHookLoc = 0x57B650;

void* WeaponFileHookFunc(const char* filename)
{
	//if (!_stricmp(filename, "m40a3") || !_stricmp(filename, "winchester1200") || !_stricmp(filename, "mg4_heartbeat_mp") || !_stricmp(filename, "famas_mp") || !_stricmp(filename, "ak47_mp") || !_stricmp(filename, "cheytac_mp"))
	/*if (!Legacy_IsLegacyMode())
	{
		if (!_stricmp(filename, "destructible_car") || !_stricmp(filename, "none"))
		{
			return DB_FindXAssetHeader(ASSET_TYPE_WEAPON, "defaultweapon_mp");
		}
	}*/

	if (FS_ReadFile(va("weapons/mp/%s", filename), NULL) > 0)
	{
		return ((void* (*)(const char*))0x57B5F0)(filename); // BG_LoadWeaponDef_LoadObj
	}

	char* file = (char*)DB_FindXAssetHeader(0x1C, filename);

	if (GAME_FLAG(GAME_FLAG_DUMPDATA))
	{
		_mkdir("raw\\weapons");
		_mkdir("raw\\weapons\\mp");

		char dumpfile[512];
		strcpy(dumpfile, "raw\\weapons\\mp\\");
		strcat(dumpfile, filename);

		FILE* dump = fopen(dumpfile, "w");
		fprintf(dump, "WEAPONFILE\\");

		thisIsWeaponfile = true;
		DumpWeaponFile(dump, file);

		fclose(dump);

		//dump = fopen("raw\\weaponFields.txt", "w");
		//DumpWeaponTypes(dump);
		//fclose(dump);
	}

	//TerminateProcess(GetCurrentProcess(), 0);

	return file;
}

void __declspec(naked) WeaponFileHookStub()
{
	__asm jmp WeaponFileHookFunc
}

StompHook vehicleFileHook;
DWORD vehicleFileHookLoc = 0x67CD40;

void* G_LoadVehicleDef(const char* filename);

void* VehicleFileHookFunc(const char* filename)
{
	if (FS_ReadFile(va("vehicles/%s", filename), NULL) > 0)
	{
		//return G_LoadVehicleDef(filename);
	}

	char* file = (char*)DB_FindXAssetHeader(ASSET_TYPE_VEHICLE, filename);

	if (GAME_FLAG(GAME_FLAG_DUMPDATA))
	{
		_mkdir("raw\\vehicles");
		_mkdir("raw\\vehicles\\mp");

		char dumpfile[512];
		strcpy(dumpfile, "raw\\vehicles\\mp\\");
		strcat(dumpfile, filename);

		FILE* dump = fopen(dumpfile, "w");
		fprintf(dump, "VEHICLEFILE\\");

		thisIsWeaponfile = false;
		DumpWeaponFile(dump, file);

		fclose(dump);
	}

	return file;
}

StompHook scriptOddLogHook;
DWORD scriptOddLogHookLoc = 0x4938D0;

StompHook scriptOddLogHook2;
DWORD scriptOddLogHook2Loc = 0x49D3F0;

void ScriptOddLogHookFunc(int meh, char* mehh)
{
	Com_Printf(0, "sd: %s\n", mehh);
}

void __declspec(naked) ScriptOddLogHookStub()
{
	__asm jmp ScriptOddLogHookFunc
}

// HELL YEAH PART 2 OF ODD LOG HOOK
void ScriptOddLogHook2Func(char* mehh)
{
	Com_Printf(0, "sd: %s\n", mehh);
}

void __declspec(naked) ScriptOddLogHook2Stub()
{
	__asm jmp ScriptOddLogHook2Func // if it breaks spawning, firstly go 'wtf'.
}

// debug hook to find whoever fucked with my party state
CallHook psClearHook;
DWORD psClearHookLoc = 0x48569C;

int curret;

void __declspec(naked) PsClearHookStub()
{
	__asm
	{
		mov eax, [esp + 14h]
		mov curret, eax
	}

	Com_Printf(0, "GUILTY AS CHARGED AT 0x%x\n", curret);

	__asm retn
}

CallHook modelOverflowHook;
DWORD modelOverflowHookLoc = 0x44F265;

void ModelOverflowHookFunc()
{
	for (int i = 1125; i < (1125 + 512); i++)
	{
		unsigned int string;
		__asm
		{
			push i
			mov eax, 468500h
			call eax
			add esp, 4h
			mov string, eax
		}

		if (string)
		{
			Com_Printf(0, "\t%i: %s\n", i, SL_ConvertToString(string));
		}
	}
}

void __declspec(naked) ModelOverflowHookStub()
{
	__asm
	{
		pushad
		call ModelOverflowHookFunc
		popad
		jmp modelOverflowHook.pOriginal
	}
}

void* MaterialLoadHookFunc(assetType_t type, const char* filename);

cmd_function_t mehCmd;
XModel* WeaponCamos_Init();
void MehFunc()
{
	/*Material* material = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "preview_mp_boneyard");
	Material* material2 = (Material*)MaterialLoadHookFunc(ASSET_TYPE_MATERIAL, "preview_oilrig");
	void* image = DB_FindXAssetHeader(ASSET_TYPE_IMAGE, "3_cursor3");*/

	//Material* material = (Material*)MaterialLoadHookFunc(ASSET_TYPE_MATERIAL, "$floatz");
	XModel* xmodel = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "weapon_usp");

	//Com_Printf(0, "HASH: 0x%x\n", R_HashString("colorMap"));

	for (int i = 0; i < xmodel->numMaterials; i++)
	{
		Com_Printf(0, "MAT NAME: %s\n", xmodel->materials[i]->name);

		for (int j = 0; j < xmodel->materials[i]->numMaps; j++)
		{
			Com_Printf(0, "IMAGE NAME #%d: %s\n", j, xmodel->materials[i]->maps[j].image->name);
		}
	}

	printf("Yep.");
}

void SndAliasListFunc()
{
	if (Cmd_Argc() < 2)
	{
		return;
	}

	const char* name = Cmd_Argv(1);
	snd_alias_list_t* list = (snd_alias_list_t*)DB_FindXAssetHeader(ASSET_TYPE_SOUND, name);

	Com_Printf(0, "%s\n", list->aliases[0].stream->file);
}

struct localizedEntry_s
{
	const char* value;
	const char* name;
};

//#include <d3dx9.h>

void AssetRestrict_PreLoadFromExperimental(assetType_t type, void* entry, const char* zone)
{
	/*if (type == ASSET_TYPE_VERTEXSHADER)
	{
		VertexShader* shader = (VertexShader*)entry;

		if (shader->bytecode)
		{
			ID3DXBuffer* buffer;
			D3DXDisassembleShader(shader->bytecode, TRUE, NULL, &buffer);

			FILE* shaderDisassembly = fopen(va("raw/%s.html", shader->name), "w");
			fwrite(buffer->GetBufferPointer(), 1, buffer->GetBufferSize(), shaderDisassembly);
			fclose(shaderDisassembly);

			buffer->Release();
		}
	}*/

	// fix for sort keys
	/*if (type == ASSET_TYPE_MATERIAL)
	{
		const char* name = *(char**)entry;

		if (strstr(name, "distortion"))
		{
			Material* mat = (Material*)entry;

			if ((mat->flags >> 8) == 44)
			{
				mat->flags = (mat->flags & 0xFF) | (43 << 8);
			}
		}
	}*/
	return;

	if (type == ASSET_TYPE_MATERIAL)
	{
		const char* name = *(char**)entry;

		if (strstr(name, "distortion") && name[0] != ',')
		{
			Material* mat = (Material*)entry;

			OutputDebugString(va("distortion %s: firstChar %d, secLast %d, unk1 %d, unk2 %d\n", name, mat->maps[0].firstCharacter, mat->maps[0].secondLastCharacter, mat->maps[0].unknown, mat->maps[0].unknown2));
		}
	}

	if (type == ASSET_TYPE_XANIM)
	{
		const char* name = *(char**)entry;

		if (strstr(name, "lobby"))
		{
			char* anim = (char*)entry;

			OutputDebugString(va("anim %s: %g rate, %g freq, %i frame\n", name, *(float*)(anim + 40), *(float*)(anim + 44), *(short*)(anim + 14)));
		}
	}
	return;

	if (type == ASSET_TYPE_MATERIAL)
	{
		Material* material = (Material*)entry;

		if (material->maps)
		{
			if (material->maps[0].image)
			{
				if (material->maps[0].image->name)
				{
					//OutputDebugStringA(va("'%s' => '%s',\n", material->name, material->maps[0].image->name));
				}
			}
		}

		if (material->animationX > 1 || material->animationY > 1)
		{
			OutputDebugStringA(va("'%s' => array(%d, %d),", material->name, material->animationX, material->animationY));
		}
	}

	if (type == ASSET_TYPE_LOCALIZE)
	{
		_mkdir("raw/english/");
		_mkdir("raw/english/localizedstrings/");

		char nameStuff[512];
		char valueStuff[1024];
		localizedEntry_s* ent = (localizedEntry_s*)entry;
		const char* module = "unknown";

		strcpy_s(nameStuff, sizeof(nameStuff), ent->name);

		char* entryName = strchr(nameStuff, '_');
		entryName[0] = '\0';
		entryName++;

		for (char* pt = nameStuff; *pt; pt++)
		{
			*pt = tolower(*pt);
		}

		char* p2 = &valueStuff[0];
		for (const char* pt = ent->value; *pt; pt++)
		{
			char c = *pt;
			if (c == '\n')
			{
				*p2 = '\\';
				p2++;
				*p2 = 'n';
			}
			else if (c == '"')
			{
				*p2 = '\\';
				p2++;
				*p2 = '"';
			}
			else
			{
				*p2 = c;
			}

			p2++;
		}

		*p2 = '\0';

		FILE* file = fopen(va("raw/english/localizedstrings/%s.str", nameStuff), "a");

		if (file)
		{
			fprintf(file, "REFERENCE           %s\n", entryName);
			fprintf(file, "LANG_ENGLISH        \"%s\"\n\n", valueStuff);
			fclose(file);
		}
	}
}

extern const char* allowedImagery[];

char lastZoneName[256];

CallHook loadTextureHook;
DWORD loadTextureHookLoc = 0x4D32BC;

static int lastImageTime;
static __int64 totalImageTime;

void WriteEndImageProfile()
{
	int time = Com_Milliseconds() - lastImageTime;
	GfxImage* image = *(GfxImage**)0x112AD40;

	//WriteProfile(Com_Milliseconds(), "image_end", va("%s - %ims", image->name, time));

	totalImageTime += time;
}

bool CanWeLoadThisImage()
{
	GfxImage* image = *(GfxImage**)0x112AD40;

	//WriteProfile(Com_Milliseconds(), "image", image->name);
	lastImageTime = Com_Milliseconds();

	if (strcmp(CURRENT_ZONE_NAME, "ui_viewer_mp") != NULL)
	{
		return true;
	}

	for (const char** imageName = allowedImagery; *imageName != NULL; imageName++)
	{
		if (!strcmp(*imageName, image->name))
		{
			return true;
		}
	}

	return false;
}

void __declspec(naked) Load_TextureHookStub()
{
	__asm
	{
		call CanWeLoadThisImage
		test al, al
		jz justNope

		mov eax, [esp + 4]
		mov ecx, [esp + 8]
		push ecx
		push eax
		call loadTextureHook.pOriginal
		add esp, 8h

		call WriteEndImageProfile

justNope:
		retn
	}
}

/*
bool FilterImageLoad(GfxImage* image)
{
	for (const char** imageName = allowedImagery; *imageName != NULL; imageName++)
	{
		if (!strcmp(*imageName, image->name))
		{
			return true;
		}
	}

	image->texture = (IDirect3DTexture9*)0xCA3E1337; // will cause oddities if referenced...
	OutputDebugString(va("%s\n", image->name));

	return false;
}

static char lastZoneName[256];

CallHook loadTextureHook;
DWORD loadTextureHookLoc = 0x51F595;

bool IsImageAllowedToLoad1(GfxImage* image)
{
	strcpy(lastZoneName, CURRENT_ZONE_NAME);
	WriteProfile(Com_Milliseconds(), "image", image->name);

	return true;
}

void __declspec(naked) Load_TextureHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		push eax
		call IsImageAllowedToLoad1
		add esp, 4h

		test al, al
		jz returnOne
		jmp loadTextureHook.pOriginal

returnOne:
		mov eax, 1
		retn
	}
}*/

CallHook rDelayLoadImageHook;
DWORD rDelayLoadImageHookLoc = 0x51F486;

bool IsImageAllowedToLoad2(GfxImage* image)
{
	WriteProfile(Com_Milliseconds(), "image_delay", image->name);

	return true;
}

void __declspec(naked) R_DelayLoadImageHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		push eax
		call IsImageAllowedToLoad2
		add esp, 4h

		test al, al
		jz returnOne
		jmp rDelayLoadImageHook.pOriginal

returnOne:
		mov eax, 1
		retn
	}
}

StompHook endLoadZoneHook;
DWORD endLoadZoneHookLoc = 0x415AC2;

void EndLoadZoneHookFunc()
{
	//WriteProfile(Com_Milliseconds(), "zone_end", lastZoneName);
	totalImageTime = 0;
}

CallHook loadDynEntityDefHook;
DWORD loadDynEntityDefHookLoc = 0x47CE5F;

void* LoadDefaultAsset(assetType_t atype);

void LoadDynEntityDefHookFunc(int arg)
{
	if (**(DWORD**)0x112A934 == 0x3F800000)
	{
		void** defaultAsset = (void**)LoadDefaultAsset(ASSET_TYPE_XMODEL);

		**(DWORD**)0x112A934 = (DWORD)(defaultAsset[1]);
	}
}

void __declspec(naked) LoadDynEntityDefHookStub()
{
	__asm
	{
		push 0
		call loadDynEntityDefHook.pOriginal
		add esp, 4h

		call LoadDynEntityDefHookFunc

		retn
	}
}

// prevents an infinite loop if the string ID does not exist in some secondary table
StompHook slFreeStringHook;
DWORD slFreeStringHookLoc = 0x61BFCC;
DWORD slFreeStringHookRet = 0x61BFD3;

DWORD slFreeStringHookZF;

void __declspec(naked) SL_FreeStringHookStub()
{
	__asm
	{
		jz setZFZero

		mov slFreeStringHookZF, 0
		jmp continueHook

setZFZero:
		mov slFreeStringHookZF, 1

continueHook:

		lea eax, ds:1DC2280h[ecx * 8]
		cmp cx, word ptr [eax]

		je returnToSender

		/*push eax
		push ebx
		mov eax, [eax]
		mov ebx, eax
		lea eax, ds:1DC2280h[eax * 8]
		cmp bx, word ptr [eax]
		pop ebx
		pop eax

		jge returnToSender*/

		push eax
		mov eax, slFreeStringHookZF
		dec eax // will set ZF to 0 if it was 1
		pop eax

		jmp slFreeStringHookRet

returnToSender:
		push 11h
		mov eax, 41B8C0h // Sys_LeaveCriticalSection
		call eax
		add esp, 4
		pop esi
		pop ebp
		pop ebx
		retn
	}
}

struct refdef_t
{
	int x;
	int y;
	int width;
	int height;
	float fovX;
	float fovY;
	float origin[3]; 
	float viewmatrix[3][3]; 
	char pad[16200];
	float viewangles[3];
	char pad2[1232];
};

#include <d3d9.h>

CallHook renderSceneHook;
DWORD renderSceneHookLoc = 0x4EB3DB;

StompHook clearViewportHook1;
DWORD clearViewportHookLoc1 = 0x555A71;
DWORD clearViewportHookRet1 = 0x555A76;

static struct  
{
	LONG rect[4];
	bool active;
} g_clearViewport;

LONG* ViewportRectToD3DRect(LONG* rect, IDirect3DDevice9* device)
{
	static LONG rects[4][4];
	static int callNum;

	int i = (callNum++ % 4);
	callNum = i;

	rects[i][0] = rect[0];
	rects[i][1] = rect[1];
	rects[i][2] = rect[0] + rect[2];
	rects[i][3] = rect[1] + rect[3];

	D3DVIEWPORT9 vp;
	vp.X = rect[0];
	vp.Y = rect[1];
	vp.Width = rect[2];
	vp.Height = rect[3];

	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	device->SetViewport(&vp);

	return rects[i];
}

void __declspec(naked) ClearViewportHook1Stub()
{
	__asm
	{
		push ecx

		lea eax, [ebp + 336]

		//push eax
		push ecx
		push edx
		push esi
		push eax
		call ViewportRectToD3DRect
		add esp, 8
		pop edx
		pop ecx
		//add esp, 4h

		push eax

		//push offset g_clearViewport.rect
		push 1

		jmp clearViewportHookRet1
	}
}

StompHook clearViewportHook2;
DWORD clearViewportHookLoc2 = 0x555994;
DWORD clearViewportHookRet2 = 0x55599A;

void __declspec(naked) ClearViewportHook2Stub()
{
	__asm
	{
		push 6

		mov al, g_clearViewport.active
		test al, al

		jz returnAsPlanned

		lea eax, [ebp + 336]

		push ecx
		push edx
		push esi
		push eax
		call ViewportRectToD3DRect
		add esp, 8
		pop edx
		pop ecx

		push eax
		//push offset g_clearViewport.rect
		push 1

		jmp clearViewportHookRet2

returnAsPlanned:
		push 0
		push 0
		jmp clearViewportHookRet2
	}
}

#define COPY_VIEWPORT(refdef) \
	g_clearViewport.rect[0] = (refdef)->x; \
	g_clearViewport.rect[1] = (refdef)->y; \
	g_clearViewport.rect[2] = (refdef)->x + (refdef)->width; \
	g_clearViewport.rect[3] = (refdef)->y + (refdef)->height

void CL_RenderSceneHookFunc(refdef_t* refdef)
{
	DWORD rendSync = 0x509500;
	DWORD issueRenderCommands = 0x509350;

	g_clearViewport.active = true;

	refdef->height /= 2;
	refdef->fovY /= 2;

	COPY_VIEWPORT(refdef);

	// and re-enable clearing
	//*(WORD*)0x555A3F = 0x0575;
	//*(BYTE*)0x555A44 = 0x74;

	__asm
	{
		push refdef
		call renderSceneHook.pOriginal
		add esp, 4h

		push 1
		call issueRenderCommands
		add esp, 4h

		call rendSync
	}

	static refdef_t replacementDef;
	memcpy(&replacementDef, refdef, sizeof(refdef_t));

	replacementDef.y += replacementDef.height;
	replacementDef.origin[2] += 50.0f;

	replacementDef.viewangles[1] += 30.0f;

	COPY_VIEWPORT(&replacementDef);

	// disable clearing the color buffer at this stage (by default; that is - but r_clear generally is unset anyway)
	//*(WORD*)0x555A3F = 0x9090;
	//*(BYTE*)0x555A44 = 0xEB;

	__asm
	{
		push offset replacementDef
		call renderSceneHook.pOriginal
		add esp, 4h

		//push 2
		//call issueRenderCommands
		//add esp, 4h

		//call rendSync
	}

	*(BYTE*)0x5ACBA9 &= ~1;

	/*static refdef_t replacementDef2;
	memcpy(&replacementDef2, refdef, sizeof(refdef_t));

	replacementDef2.x = 150;
	replacementDef2.y = 150;

	replacementDef2.width = 150;
	replacementDef2.height = 150;

	replacementDef2.fovX = replacementDef2.fovY;

	replacementDef2.viewangles[1] += 30.0f;

	__asm
	{
		push offset replacementDef2
		call renderSceneHook.pOriginal
		add esp, 4h
	}*/
}

/*CallHook renderSceneHook;
DWORD renderSceneHookLoc = 0x5ACB79;

int CL_RenderSceneHookFunc(int clientNum)
{
	int retval;
	DWORD issueRenderCommands = 0x509350;

	__asm
	{
		push 0
		call renderSceneHook.pOriginal
		add esp, 4h

		mov retval, eax
	}

	if (retval)
	{
		__asm
		{
			push 3
			call issueRenderCommands
			add esp, 4h
		}

		*(BYTE*)0x5ACBA9 &= ~1;

		int coords[4];
		memcpy(coords, (int*)0x7ED3B8, 16);

		int* rCoords = (int*)0x7ED3B8;

		rCoords[0] = 150;
		rCoords[1] = 150;
		rCoords[2] = 300;
		rCoords[3] = 300;

		__asm
		{
			push 0
			call renderSceneHook.pOriginal
			add esp, 4h
		}

		memcpy((int*)0x7ED3B8, coords, 16);
	}

	return retval;
}*/

CallHook logPixelShaderHook;
DWORD logPixelShaderHookLoc = 0x5B9D48;

void LogPixelShader()
{
	PixelShader* shader = *(PixelShader**)0x112AEF4;
	MaterialTechniqueSet* techset = *(MaterialTechniqueSet**)0x112AE8C;

	//WriteProfile(Com_Milliseconds(), "pixelshader", va("%08x - %s (%s)", (DWORD)shader->shader, shader->name, techset->name));
}

void __declspec(naked) LogPixelShaderHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		mov ecx, [esp + 8]
		push ecx
		push eax
		call logPixelShaderHook.pOriginal
		add esp, 8

		jmp LogPixelShader
	}
}

CallHook logVertexShaderHook;
DWORD logVertexShaderHookLoc = 0x420BC8;

void LogVertexShader()
{
	VertexShader* shader = *(VertexShader**)0x112B338;
	MaterialTechniqueSet* techset = *(MaterialTechniqueSet**)0x112AE8C;

	//WriteProfile(Com_Milliseconds(), "vertexshader", va("%08x - %s (%s)", (DWORD)shader->shader, shader->name, techset->name));
}

void __declspec(naked) LogVertexShaderHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		mov ecx, [esp + 8]
		push ecx
		push eax
		call logVertexShaderHook.pOriginal
		add esp, 8

		jmp LogVertexShader
	}
}

void PatchMW2_DumpModel();

void PatchMW2_Experimental()
{
	static cmd_function_s sndAliasListCmd;
	static cmd_function_s dumptyCmd;

	//psClearHook.initialize(PsClearHookStub);
	//psClearHook.installHook();

	weaponFileHook.initialize(weaponFileHookLoc, WeaponFileHookStub);
	weaponFileHook.installHook();

	vehicleFileHook.initialize(vehicleFileHookLoc, VehicleFileHookFunc);
	vehicleFileHook.installHook();

	addEntryNameHook.initialize(addEntryNameHookLoc, AddEntryNameHookStub);
	addEntryNameHook.installHook();

	modelOverflowHook.initialize(modelOverflowHookLoc, ModelOverflowHookStub);
	modelOverflowHook.installHook();

	loadTextureHook.initialize(loadTextureHookLoc, Load_TextureHookStub);
	loadTextureHook.installHook();

	rDelayLoadImageHook.initialize(rDelayLoadImageHookLoc, R_DelayLoadImageHookStub);
	rDelayLoadImageHook.installHook();

	endLoadZoneHook.initialize(endLoadZoneHookLoc, EndLoadZoneHookFunc);
	endLoadZoneHook.installHook();

	Cmd_AddCommand("meh", MehFunc, &mehCmd, 0);
	Cmd_AddCommand("snd_alias_list_t", SndAliasListFunc, &sndAliasListCmd, 0);
	Cmd_AddCommand("dumptydump", DumpRawFiles, &dumptyCmd, 0);

	loadDynEntityDefHook.initialize(loadDynEntityDefHookLoc, LoadDynEntityDefHookStub);
	loadDynEntityDefHook.installHook();

	//slFreeStringHook.initialize(slFreeStringHookLoc, SL_FreeStringHookStub);
	//slFreeStringHook.installHook();

	logPixelShaderHook.initialize(logPixelShaderHookLoc, LogPixelShaderHookStub);
	logPixelShaderHook.installHook();

	logVertexShaderHook.initialize(logVertexShaderHookLoc, LogVertexShaderHookStub);
	logVertexShaderHook.installHook();

	/*renderSceneHook.initialize(renderSceneHookLoc, CL_RenderSceneHookFunc);
	renderSceneHook.installHook();

	clearViewportHook1.initialize(clearViewportHookLoc1, ClearViewportHook1Stub);
	clearViewportHook1.installHook();

	clearViewportHook2.initialize(clearViewportHookLoc2, ClearViewportHook2Stub);
	clearViewportHook2.installHook();*/

	//endLoadingHook.initialize(endLoadingHookLoc, EndLoadingHookStub);
	//endLoadingHook.installHook();

	//scriptOddLogHook.initialize(scriptOddLogHookLoc, ScriptOddLogHookStub);
	//scriptOddLogHook.installHook();

	//scriptOddLogHook2.initialize(scriptOddLogHook2Loc, ScriptOddLogHook2Stub);
	//scriptOddLogHook2.installHook();

	// BAD HACK: load any weapon without cache in G_GetWeaponByName
	// wow, this was needed in T5 as well. wonder why.
	// (nope, didn't fix it)
	//*(WORD*)0x43DF17 = 0x9090;

	// allow fs_debug open prints from other threads
	*(WORD*)0x643787 = 0x9090;
	*(WORD*)0x64357F = 0x9090;

	PatchMW2_DumpModel();
}