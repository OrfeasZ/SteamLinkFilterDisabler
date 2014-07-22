/**
* Author: Orfeas Zafeiris (https://github.com/OrfeasZ)
* Project: LinkFilterDisabler
* Original Filename: LinkFilterDisabler.cpp
* Creation Date: 22/07/2014 23:30
*/

#include "stdafx.h"

#include <tlhelp32.h>
#include <conio.h>
#include <stdlib.h>
#include <psapi.h>

#pragma comment(lib, "Psapi.lib")

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

int main(int argc, char* argv[])
{
	bool s_Silent = FlagSet("--silent");
	bool s_Retry = s_Silent || FlagSet("--retry");

	SetConsoleTitleA("Steam Link Filter Disabler - v1.0");

	// Print disclaimer.
	printf("DISCLAIMER:\n");
	printf("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\n");
	printf("This software is in no way sponsored, endorsed, or supported by, or associated with, Valve, Steam, or any of their partners.\n\n");
	printf("USE THIS AT YOUR OWN RISK!\n\n");
	printf("You can find the source on GitHub at:\n");
	printf("https://github.com/OrfeasZ/SteamLinkFilterDisabler\n\n");

	if (!s_Silent)
	{
		printf("Press any key to confirm and continue...\n");

		// Wait for keypress
		WaitForKeypress();
	}

	// Clear the console
	system("cls");

	// 
	printf("Locating Steam process... ");
	DWORD s_ProcessID = FindProcessID("Steam.exe");

	if (s_ProcessID == NULL && !s_Retry)
	{
		printf("FAILED\n");
		printf("\nPlease make sure Steam is running and try again.\n");
		ConfirmExit(s_Silent);
		return 1;
	}

	// Keep retrying every 2 seconds of the --retry flag is set.
	while (s_ProcessID == NULL && s_Retry)
	{
		printf("FAILED\n");
		printf("\nRetrying in 2 seconds...\n");

		Sleep(2000);

		printf("Locating Steam process... ");
		s_ProcessID = FindProcessID("Steam.exe");
	}

	printf("DONE\n");

	// Find target module
	printf("Finding friends module... ");

	// Try opening the process.
	HANDLE s_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, s_ProcessID);

	if (s_Process == NULL)
	{
		printf("FAILED\n");
		printf("\nFailed to open process. Please make sure you have the appropriate permissions and try again.\n");
		ConfirmExit(s_Silent);
		return 1;
	}

	HMODULE s_FriendsModule = FindModule(s_Process, "friendsui.dll");

	if (s_FriendsModule == NULL && !s_Retry)
	{
		printf("FAILED\n");
		printf("\nFailed to find friends module. Please make sure you are fully logged in and try again.\n");
		ConfirmExit(s_Silent);
		return 1;
	}

	// Keep retrying every 2 seconds of the --retry flag is set.
	while (s_FriendsModule == NULL && s_Retry)
	{
		printf("FAILED\n");
		printf("\nRetrying in 2 seconds...\n");

		Sleep(2000);

		printf("Finding friends module... ");
		s_FriendsModule = FindModule(s_Process, "friendsui.dll");
	}

	printf("DONE\n");

	// Find the byte we need to patch.
	printf("Finding address to patch... ");
	
	// NOTE: This is slow. Could've been done better.
	DWORD s_Address = FindPattern(s_Process, (DWORD)s_FriendsModule, 0x100000,
		(PBYTE)"\x75\x10\x83\xC6\x00\x81\xFE\x00\x00\x00\x00\x72\xE5\x5F\x5E\x5B", 
		"xxxx?xx?xxxxxxxx");

	if (s_Address == NULL)
	{
		printf("FAILED\n");
		printf("\nFailed to find patch address. Are you running an updated or already patched version?\n");
		ConfirmExit(s_Silent);
		return 1;
	}

	printf("DONE\n");

	// Patch the link filter to consider all links as official links.
	printf("Patching... ");
	
	BYTE s_PatchByte = 0x71;
	if (!WriteProcessMemory(s_Process, (void*)s_Address, &s_PatchByte, 1, NULL))
	{
		printf("FAILED\n");
		printf("\nFailed to patch. Please make sure you have the appropriate permissions and try again.\n");
		ConfirmExit(s_Silent);
		return 1;
	}

	printf("DONE\n\n");

	// Release the handle to the process.
	CloseHandle(s_Process);

	// We're done here!
	printf("Steam has been successfully patched!\n");
	
	if (!s_Silent)
	{
		printf("Exiting in 5 seconds...\n");
		Sleep(5000);
	}

	return 0;
}

