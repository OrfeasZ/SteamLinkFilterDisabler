/**
* Author: Orfeas Zafeiris (https://github.com/OrfeasZ)
* Project: LinkFilterDisabler
* Original Filename: LinkFilterDisabler.cpp
* Creation Date: 22/07/2014 23:30
*/

#include "LinkFilterDisabler.h"
#include "Util.h"

bool g_Ghost = false;
bool g_Retry = false;
bool g_Silent = false;

int main(int argc, char* argv[])
{
	g_Ghost = FlagSet("--ghost");
	g_Silent = g_Ghost || FlagSet("--silent");
	g_Retry = g_Silent || FlagSet("--retry");

	// We don't want a console in ghost mode.
	if (g_Ghost)
		FreeConsole();

	SetConsoleTitleA("Steam Link Filter Disabler - v1.2");

	// Print disclaimer.
	printf("DISCLAIMER:\n");
	printf("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\n");
	printf("This software is in no way sponsored, endorsed, or supported by, or associated with, Valve, Steam, or any of their partners.\n\n");
	printf("USE THIS AT YOUR OWN RISK!\n\n");
	printf("You can find the source on GitHub at:\n");
	printf("https://github.com/OrfeasZ/SteamLinkFilterDisabler\n\n");

	if (!g_Silent)
	{
		printf("Press any key to confirm and continue...\n");

		// Wait for keypress
		WaitForKeypress();
	}

	// Clear the console
	system("cls");

	return LocateSteamProcess();
}

//////////////////////////////////////////////////////////////////////////

int LocateSteamProcess()
{
	printf("Locating Steam process... ");
	DWORD s_ProcessID = FindProcessID("Steam.exe");

	if (s_ProcessID == NULL && !g_Retry)
	{
		printf("FAILED\n");
		printf("\nPlease make sure Steam is running and try again.\n");
		ConfirmExit(g_Silent);
		return 1;
	}

	// Keep retrying every 2 seconds of the --retry flag is set.
	while (s_ProcessID == NULL && g_Retry)
	{
		printf("FAILED\n");
		printf("\nRetrying in 2 seconds...\n");

		Sleep(2000);

		printf("Locating Steam process... ");
		s_ProcessID = FindProcessID("Steam.exe");
	}

	printf("DONE\n");

	return LocateFriendsModule(s_ProcessID);
}

int LocateFriendsModule(int p_ProcessID)
{
	printf("Finding friends module... ");

	// Try opening the process.
	HANDLE s_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, p_ProcessID);

	if (s_Process == NULL)
	{
		printf("FAILED\n");
		printf("\nFailed to open process. Please make sure you have the appropriate permissions and try again.\n");
		ConfirmExit(g_Silent);
		return 1;
	}

	HMODULE s_FriendsModule = FindModule(s_Process, "friendsui.dll");

	if (s_FriendsModule == NULL && !g_Retry)
	{
		printf("FAILED\n");
		printf("\nFailed to find friends module. Please make sure you are fully logged in and try again.\n");
		ConfirmExit(g_Silent);
		return 1;
	}

	// Keep retrying every 2 seconds of the --retry flag is set.
	while (s_FriendsModule == NULL && g_Retry)
	{
		printf("FAILED\n");
		printf("\nRetrying in 2 seconds...\n");

		Sleep(2000);

		// Steam process has exited; find it again.
		DWORD s_ExitCode = 0;
		if (!GetExitCodeProcess(s_Process, &s_ExitCode) || s_ExitCode != STILL_ACTIVE)
			return LocateSteamProcess();

		printf("Finding friends module... ");
		s_FriendsModule = FindModule(s_Process, "friendsui.dll");
	}

	printf("DONE\n");

	return PatchFriendsUI(s_Process, s_FriendsModule);
}

int PatchFriendsUI(HANDLE p_Process, HMODULE p_FriendsModule)
{
	// Find the byte we need to patch.
	printf("Finding address to patch... ");

	// NOTE: This is slow. Could've been done better.
	DWORD s_Address = FindPattern(p_Process, (DWORD)p_FriendsModule, 0x100000,
		(PBYTE)"\x75\x10\x83\xC6\x00\x81\xFE\x00\x00\x00\x00\x72\xE5\x5F\x5E\x5B",
		"xxxx?xx?xxxxxxxx");

	if (s_Address == NULL)
	{
		printf("FAILED\n");
		printf("\nFailed to find patch address. Are you running an updated or already patched version?\n");
		ConfirmExit(g_Silent);
		return 1;
	}

	printf("DONE\n");

	// Patch the link filter to consider all links as official links.
	printf("Patching... ");

	BYTE s_PatchByte = 0x71;
	if (!WriteProcessMemory(p_Process, (void*)s_Address, &s_PatchByte, 1, NULL))
	{
		printf("FAILED\n");
		printf("\nFailed to patch. Please make sure you have the appropriate permissions and try again.\n");
		ConfirmExit(g_Silent);
		return 1;
	}

	printf("DONE\n\n");

	// Release the handle to the process.
	CloseHandle(p_Process);

	// We're done here!
	printf("Steam has been successfully patched!\n");

	if (!g_Silent)
	{
		printf("Exiting in 5 seconds...\n");
		Sleep(5000);
	}

	return 0;
}
