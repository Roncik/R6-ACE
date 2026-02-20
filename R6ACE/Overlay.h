#ifndef WINDOW
#define WINDOW

#include <Windows.h>
#include <iostream>
#include <stdio.h>

#include "Main.h"
#include <dwmapi.h>
#include <io.h>
#include <Fcntl.h>
#include <TlHelp32.h>

#pragma comment(lib, "dwmapi.lib")

struct overlay_t
{
	WNDCLASSEX Class;
	char Name[256];
	int Width, Height;
	HWND Window;
	HWND menuWindow;
	MSG Message;
	MARGINS Margin;
	int OffsetLeft = 0, OffsetTop = 0;
	int OverlayLeft, OverlayTop;
};
extern overlay_t Overlay;

struct target_t
{
	char Name[256];
	HWND Window;
	RECT Size;
	DWORD Style;
	BOOL Checked;
	int width, height;
};
extern target_t Target;


namespace OverlayFunctions
{
	void CreateClass(WNDPROC winproc, char * windowname);
	void GetTargetWindow();
	void CreateWindowOverlay();
	void UpdateOverlayState(bool MenuOpen, HWND ownd);
}


extern HINSTANCE hInstance;
extern HANDLE pHandle;
extern DWORD pid;
extern DWORD Module;
#endif // !WINDOW
