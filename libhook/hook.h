#pragma once

#include <Windows.h>

extern "C" __declspec(dllimport)
BOOL WINAPI HookAllApps(BOOL bInstall, DWORD dwThreadId);
