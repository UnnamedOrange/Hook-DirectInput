#include "hook.h"

#include <cassert>

#include <Windows.h>

#include "APIHook.h"

typedef SHORT(WINAPI* Proc)(int);

extern CAPIHook g_GetAsyncKeyState;

SHORT WINAPI Hook_GetAsyncKeyState(int vKey) {
	return 0;
}

CAPIHook g_GetAsyncKeyState("User32.dll", "GetAsyncKeyState",
	(PROC)Hook_GetAsyncKeyState);
HHOOK g_hhook = NULL;

static LRESULT WINAPI GetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
	return(CallNextHookEx(g_hhook, code, wParam, lParam));
}
// Returns the HMODULE that contains the specified memory address
static HMODULE ModuleFromAddress(PVOID pv) {
	MEMORY_BASIC_INFORMATION mbi;
	return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
		? (HMODULE)mbi.AllocationBase : NULL);
}

extern "C"
BOOL WINAPI HookAllApps(BOOL bInstall, DWORD dwThreadId) {
	BOOL bOk;

	if (bInstall) {
		assert(g_hhook == NULL); // Illegal to install twice in a row

		// Install the Windows' hook
		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc,
			ModuleFromAddress(HookAllApps), dwThreadId);

		bOk = (g_hhook != NULL);
	}
	else {
		assert(g_hhook != NULL); // Can't uninstall if not installed
		bOk = UnhookWindowsHookEx(g_hhook);
		g_hhook = NULL;
	}

	return(bOk);
}
