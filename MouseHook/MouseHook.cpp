// MouseHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MouseHook.h"

HWND g_hMainWnd = nullptr;
UINT g_mouseMsg = 0;
HHOOK g_hMouseHook = nullptr;

BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return(FALSE);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode < 0)  // do not process the message 
		return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

	if (wParam == WM_LBUTTONDOWN || wParam == WM_NCLBUTTONDOWN || wParam == WM_LBUTTONUP ||
		wParam == WM_NCLBUTTONUP || wParam == WM_MOUSEMOVE)
	{
		::PostMessage(g_hMainWnd, g_mouseMsg, wParam, lParam);
	}

	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

BOOL OpenMouseHook(HWND hMainWnd, UINT mouseMsg)
{
	g_hMainWnd = hMainWnd;
	g_mouseMsg = mouseMsg;

	EnableDebugPrivilege();

	g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, 0, 0);//GetCurrentThreadId()); 钩子过程的句柄
	if (g_hMouseHook == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

void CloseMouseHook()
{
	if (g_hMouseHook)
	{
		UnhookWindowsHookEx(g_hMouseHook);
		g_hMouseHook = nullptr;
	}
}
