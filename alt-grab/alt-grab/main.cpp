#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include "resource.h"
#include <stdio.h>
#include "alt-grab-app.h"

#define	WM_USER_SHELLICON WM_USER + 1
#define ID_ALTGRAB_EXIT 40001

NOTIFYICONDATA iconData = { 0 };
HMENU hPopMenu;
HINSTANCE hInst;
HWND wndHdl;
bool wantsExit = false;

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	POINT lpClickPoint;
	int wmId, wmEvent;
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0); break;
	case WM_CLOSE:
		Shell_NotifyIcon(NIM_DELETE, &iconData); wantsExit = true;  break;
	case WM_USER_SHELLICON:
		switch (LOWORD(lParam))
		{
		case WM_RBUTTONDOWN: //right click on tray icon
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			GetCursorPos(&lpClickPoint);
			hPopMenu = CreatePopupMenu();
			InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, ID_ALTGRAB_EXIT, "Quit");

			SetForegroundWindow(window);
			TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, window, NULL);
			return TRUE;

			break;
		}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case ID_ALTGRAB_EXIT:
		{
			wantsExit = true;
			Shell_NotifyIcon(NIM_DELETE, &iconData); wantsExit = true;  break;
			DestroyWindow(wndHdl);

		}break;
		default: return DefWindowProc(wndHdl, message, wParam, lParam);

		}

	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
	return result;
}

bool InitWin32TrayApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hInst = hInstance;

	WNDCLASSEX winClass; //create a class for our window
	winClass.hInstance = hInstance;
	winClass.lpszClassName = "Window Control Groups";
	winClass.lpfnWndProc = WndProc;
	winClass.style = CS_DBLCLKS;  // Catch double-clicks
	winClass.cbSize = sizeof(WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	winClass.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	winClass.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	winClass.lpszMenuName = NULL;                 /* No menu */
	winClass.cbClsExtra = 0;                      /* No extra bytes after the window class */
	winClass.cbWndExtra = 0;                      /* structure or the window instance */
	winClass.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));

	if (!RegisterClassEx(&winClass))
		return false;

	//we still need a window even if we just make a tray app
	wndHdl = CreateWindow("Window Control Groups", "Window Control Groups", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.hWnd = wndHdl;
	iconData.uID = IDI_ICON1;
	iconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	iconData.uCallbackMessage = WM_USER_SHELLICON; //Set up our invented Windows Message
	iconData.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	const char toolTip[128] = "Window Control Groups";
	strncpy_s(iconData.szTip, toolTip, sizeof(toolTip));

	Shell_NotifyIcon(NIM_ADD, &iconData);
	return true;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (!InitWin32TrayApp(hInstance, hPrevInstance, lpCmdLine, nShowCmd))
	{
		return -1;
	}

	initControlGroups();

	MSG messages;
	while (GetMessage(&messages, NULL, 0, 0))									  
	{
		if (wantsExit)
		{
			break;
		}

		/* Translate virtual-key messages into character messages */
		TranslateMessage(&messages);
		/* Send message to WindowProcedure */
		DispatchMessage(&messages);
	}

	shutdownControlGroups();

	return 0;

}