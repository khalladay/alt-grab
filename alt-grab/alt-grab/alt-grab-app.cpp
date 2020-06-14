#include "alt-grab-app.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unordered_map>

//extra keycodes for readability
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define NO_INDEX -1

HHOOK hook;
bool shift[2];
bool alt[2];

const int NUM_WINDOW_SLOTS = 4;
HWND winSlots[NUM_WINDOW_SLOTS];
HWND fallbackWin;

int vkCodeToIndex(DWORD vkey)
{
	switch (vkey)
	{
	case VK_OEM_1: return 0;
	case VK_J: return 1;
	case VK_K: return 2;
	case VK_L: return 3;
	}
	return NO_INDEX;
}

bool isSlotInForeground(int slotIdx)
{
	return GetForegroundWindow() == winSlots[slotIdx];
}

//this only works if the system thinks the alt key is down, so that 
//it bypasses the foreground lock. It seems like this only applies to VK_MENU, not VK_LMENU
void forceWindowToFront(HWND win)
{
	DWORD foregroundThreadId = GetWindowThreadProcessId(GetForegroundWindow(), 0);
	fallbackWin = GetForegroundWindow();

	DWORD targetWinThreadId = GetWindowThreadProcessId(win, 0);

	if (foregroundThreadId == targetWinThreadId) return;

	SwitchToThisWindow(win, true);
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	bool keyDown = false;
	bool swallowKeyPress = false;
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
	DWORD vkCode = p->vkCode;

	switch (wParam)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		keyDown = true;
		int index = vkCodeToIndex(vkCode);

		if (index != NO_INDEX && keyDown)
		{
			bool shiftPressed = shift[0] | shift[1];
			bool altPressed = alt[0] | alt[1];
			
			if (!altPressed) break;

			HWND curWin = GetForegroundWindow(); //note that this won't get task manager
			HWND& slot = winSlots[index];
			swallowKeyPress = true;

			if (shiftPressed && curWin)
			{
				slot = curWin;
			}
			else
			{
				if (isSlotInForeground(index))
				{
					//yes, you do actually need the multiple PostMessage calls if you want to properly restore keyboard focus

					//this isn't 100% bulletproof. If you summon a window, and then use alt tab to switch to new stuff, then
					//use alt-tab to go BACK to the summoned window, and then try to dismiss that window by using the alt-grab
					//shortcut again, keyboard focus isn't returned properly. I'll have to fix that in the future. 
					ShowWindow(slot, SW_MINIMIZE);
					PostMessage(slot, WM_ACTIVATE, WA_INACTIVE, (LPARAM)fallbackWin);
					PostMessage(fallbackWin, WM_ACTIVATE, WA_ACTIVE, (LPARAM)slot);
					SetActiveWindow(fallbackWin);
				}
				else
				{
					forceWindowToFront(slot);
				}
			}
		}
	} //intentional fall through
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		if (vkCode == VK_LSHIFT) shift[0] = keyDown;
		if (vkCode == VK_LMENU) alt[0] = keyDown;

		//also the VK_MENU key event actually disables the keyboard lock, 
		//so LMENU and RMNEU need to trigger VK_MENU
		if (vkCode == VK_LMENU)
		{
			keybd_event(VK_MENU, 0xb8, keyDown ? 0 : KEYEVENTF_KEYUP, 0);
		}

	}break;
	}

	return swallowKeyPress ? 1 : CallNextHookEx(0, code, wParam, lParam);
}

void initControlGroups()
{
	hook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), NULL); //install a global hook, located in this app
}

void shutdownControlGroups()
{
	UnhookWindowsHookEx((HHOOK)hook);
}