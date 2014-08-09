/**
* Author: Orfeas Zafeiris (https://github.com/OrfeasZ)
* Project: LinkFilterDisabler
* Original Filename: LinkFilterDisabler.h
* Creation Date: 09/08/2014 17:59
*/

#include "stdafx.h"

extern bool g_Silent;
extern bool g_Retry;

int LocateSteamProcess();
int LocateFriendsModule(int p_ProcessID);
int PatchFriendsUI(HANDLE p_Process, HMODULE p_FriendsModule);
