/**
* Author: Orfeas Zafeiris (https://github.com/OrfeasZ)
* Project: LinkFilterDisabler
* Original Filename: Util.h
* Creation Date: 09/08/2014 17:45
*/

#include "stdafx.h"

#include <psapi.h>
#include <tlhelp32.h>
#include <conio.h>
#include <stdlib.h>

#pragma comment(lib, "Psapi.lib")

DWORD FindProcessID(const char* p_ProcessName);

HMODULE FindModule(HANDLE p_Process, const char* p_ModuleName);

DWORD FindPattern(HANDLE p_Process, DWORD p_StartAddress, DWORD p_SearchLength, PBYTE p_Pattern, const char* p_Mask);

void WaitForKeypress();

void ConfirmExit(bool p_Silent);

bool FlagSet(const char* p_Flag);
