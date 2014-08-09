/**
* Author: Orfeas Zafeiris (https://github.com/OrfeasZ)
* Project: LinkFilterDisabler
* Original Filename: Util.cpp
* Creation Date: 09/08/2014 17:45
*/

#include "Util.h"

DWORD FindProcessID(const char* p_ProcessName)
{
	PROCESSENTRY32 s_ProcessEntry;
	s_ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE s_Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	DWORD s_ProcessID = NULL;

	if (Process32First(s_Snapshot, &s_ProcessEntry))
	{
		while (Process32Next(s_Snapshot, &s_ProcessEntry))
		{
			if (_stricmp(s_ProcessEntry.szExeFile, p_ProcessName) == 0)
			{
				s_ProcessID = s_ProcessEntry.th32ProcessID;
				break;
			}
		}
	}

	CloseHandle(s_Snapshot);

	return s_ProcessID;
}

HMODULE FindModule(HANDLE p_Process, const char* p_ModuleName)
{
	HMODULE s_Modules[1024];
	DWORD s_CbNeeded;

	// Get a list of all the modules in this process.
	if (!EnumProcessModules(p_Process, s_Modules, sizeof(s_Modules), &s_CbNeeded))
		return NULL;

	for (size_t i = 0; i < (s_CbNeeded / sizeof(HMODULE)); i++)
	{
		char s_ModuleName[MAX_PATH];

		// These are not the modules you're looking for!
		if (!GetModuleBaseNameA(p_Process, s_Modules[i], s_ModuleName, sizeof(s_ModuleName) / sizeof(char)))
			continue;

		if (_stricmp(s_ModuleName, p_ModuleName) == 0)
			return s_Modules[i];
	}

	return NULL;
}

DWORD FindPattern(HANDLE p_Process, DWORD p_StartAddress, DWORD p_SearchLength, PBYTE p_Pattern, const char* p_Mask)
{
	size_t s_MaskLength = strlen(p_Mask);

	for (DWORD i = 0; i < p_SearchLength - s_MaskLength; ++i)
	{
		int s_FoundBytes = 0;

		for (DWORD j = 0; j < s_MaskLength; ++j)
		{
			BYTE s_ByteRead;

			if (!ReadProcessMemory(p_Process, (void*)(p_StartAddress + i + j), &s_ByteRead, 1, NULL))
				return NULL;

			if (s_ByteRead != p_Pattern[j] && p_Mask[j] != '?')
				break;

			++s_FoundBytes;

			if (s_FoundBytes == s_MaskLength)
				return p_StartAddress + i;
		}
	}

	return NULL;
}

void WaitForKeypress()
{
	while (!_kbhit())
		Sleep(1);

	_getch();
}

void ConfirmExit(bool p_Silent)
{
	if (p_Silent)
		return;

	printf("\nPress any key to exit...\n");

	// Wait for keypress
	WaitForKeypress();
}

bool FlagSet(const char* p_Flag)
{
	for (int i = 1; i < __argc; ++i)
		if (_stricmp(__argv[i], p_Flag) == 0)
			return true;

	return false;
}
