// MyOGL.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "..\OSLib\oslib.h"

/*
	----------------------------------
	Known issues
	  - Sound is not supported
	  - Color keying is not supported (oslSetTransparentColor), except when loading images
	  - Transparency: only OSL_FX_ALPHA (|OSL_FX_COLOR) & OSL_FX_ADD are supported with old graphic cards
	  - Runs very slow, and some things aren't be emulated correctly
	  - Accessing draw buffer data (oslSetDrawBuffer) needs use of oslLockImage & oslUnlockImage
	----------------------------------
	System requirements
	Recommended (will run at a speed comparable to the PSP):
	  - CPU: Pentium 4 with HyperThreading, Pentium D, AMD X2 or Pentium 4 @ 2.4 GHz or Centrino / Core [[2] Duo] @ 1.5 GHz
	  - RAM: 768 MB recommended for Visual Studio 2005
	  - ATI Radeon X600 (Radeon 9xxx insufficient) or more
	  - nVidia GeForce FX 5200 or more
	  - Intel Extreme Graphics (any model, the first generation might be a bit slow)
	Minimal (will run but extremely slow):
	  - x86-compatible CPU
	  - OpenGL 1.0 compatible graphic card with 2 MB of memory
	  - 32 MB of RAM
	----------------------------------
*/

#include "resource.h"

//CMyGL *mgl;
HINSTANCE emuHInst;
HWND emuHWnd;
HANDLE emuHThread;
LRESULT CALLBACK emuWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
ATOM emuRegisterClass(HINSTANCE hInstance);
BOOL emuInitInstance(HINSTANCE hInstance, int nCmdShow);
int main();
void emuProgThreadStart();
BOOL emuKeyHeld[256];

void Debug(const char *format, ...)		{
	char      vbuf[1024];
	va_list   arg_ptr;
	va_start(arg_ptr, format);
	_vsnprintf(vbuf, sizeof(vbuf), format, arg_ptr);
	MessageBox(emuHWnd, vbuf, "Débogage", 0);
}

int emuThreadInit()		{
	const HINSTANCE hInstance = NULL;

	emuRegisterClass(hInstance);

	// Perform application initialization:
	if (!emuInitInstance(hInstance, SW_SHOW)) 
	{
		return FALSE;
	}
	return TRUE;
}

int emuThreadLoop()		{
	MSG msg;
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

void emuInit()		{
	DWORD ThreadID;
	HANDLE hThread;
	emuThreadInit();
	hThread = CreateThread(	(LPSECURITY_ATTRIBUTES)NULL,
							(DWORD)0,
							(LPTHREAD_START_ROUTINE)emuProgThreadStart,
							(LPVOID)NULL,
							(DWORD)0,
							(LPDWORD)&ThreadID
						  );
//	SetThreadPriority(hThread,THREAD_PRIORITY_ABOVE_NORMAL);
	emuHThread = hThread;
	emuThreadLoop();
}

ATOM emuRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	wcex.lpfnWndProc	= (WNDPROC)emuWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "OSLib";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

enum {EMU_OM_FILE_PAUSE, EMU_OM_FILE_STEP, EMU_OM_FILE_QUIT, EMU_OM_VIDEO_PERF_TEXCACHE, EMU_OM_VIDEO_PERF_SHADERS,
		EMU_OM_VIDEO_FSKIP_NORMAL, EMU_OM_VIDEO_FSKIP_NONE, EMU_OM_VIDEO_PERF_NOUSE_5650, EMU_OM_VIDEO_PERF_NOUSE_5551, EMU_OM_VIDEO_PERF_NOUSE_4444};

int emuGetKey(char *name, char *buffer, DWORD type, DWORD len)		{
	HKEY key;
	int result;
	DWORD dispo;
	result = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OSLib\\Configuration", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &key, &dispo);
	if (result != ERROR_SUCCESS)
		return 0;
	RegQueryValueEx(
		key,
		name,
		0,
		NULL,
		(unsigned char*)buffer,
		&len
	);
	RegCloseKey(key);
	return len;
}

int emuSetKey(char *name, char *buffer, DWORD type, DWORD len)		{
	HKEY key;
	int result;
	DWORD dispo;
	result = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OSLib\\Configuration", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &key, &dispo);
	if (result != ERROR_SUCCESS)
		return 0;
	RegSetValueEx(
		key,
		name,
		0,
		type,
		(unsigned char*)buffer,
		len
	);
	RegCloseKey(key);
	return 1;
}

void emuLoadMenuItems()		{
	emuGetKey("emuReuseTextures", (char*)&emuReuseTextures, REG_DWORD, sizeof(emuReuseTextures));
	emuGetKey("emuUsePixelShaders", (char*)&emuUsePixelShaders, REG_DWORD, sizeof(emuUsePixelShaders));
	if (emuDebugMode)
		emuGetKey("emuFrameSkipMode", (char*)&emuFrameSkipMode, REG_DWORD, sizeof(emuFrameSkipMode));
//	emuGetKey("emuConvertPixelTypes", (char*)&emuConvertPixelTypes, REG_DWORD, sizeof(emuConvertPixelTypes));
}

void emuSaveMenuItems()		{
	emuSetKey("emuReuseTextures", (char*)&emuReuseTextures, REG_DWORD, sizeof(emuReuseTextures));
	emuSetKey("emuUsePixelShaders", (char*)&emuUsePixelShaders, REG_DWORD, sizeof(emuUsePixelShaders));
	if (emuDebugMode)
		emuSetKey("emuFrameSkipMode", (char*)&emuFrameSkipMode, REG_DWORD, sizeof(emuFrameSkipMode));
//	emuSetKey("emuConvertPixelTypes", (char*)&emuConvertPixelTypes, REG_DWORD, sizeof(emuConvertPixelTypes));
}

void emuCheckMenus(HWND hWnd)		{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(GetSubMenu(hMenu, 0), EMU_OM_FILE_PAUSE, MF_BYCOMMAND|(emuPauseGame?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(GetSubMenu(GetSubMenu(hMenu, 1), 0), EMU_OM_VIDEO_PERF_TEXCACHE, MF_BYCOMMAND|(emuReuseTextures?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(GetSubMenu(GetSubMenu(hMenu, 1), 0), EMU_OM_VIDEO_PERF_SHADERS, MF_BYCOMMAND|(emuUsePixelShaders?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(GetSubMenu(GetSubMenu(hMenu, 1), 1), EMU_OM_VIDEO_FSKIP_NORMAL, MF_BYCOMMAND|((emuFrameSkipMode==0)?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(GetSubMenu(GetSubMenu(hMenu, 1), 1), EMU_OM_VIDEO_FSKIP_NONE, MF_BYCOMMAND|((emuFrameSkipMode==1)?MF_CHECKED:MF_UNCHECKED));

/*	CheckMenuItem(GetSubMenu(GetSubMenu(GetSubMenu(hMenu, 1), 0), 2), EMU_OM_VIDEO_PERF_NOUSE_5650, MF_BYCOMMAND | ((emuConvertPixelTypes & 1) ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(GetSubMenu(GetSubMenu(GetSubMenu(hMenu, 1), 0), 2), EMU_OM_VIDEO_PERF_NOUSE_5551, MF_BYCOMMAND | ((emuConvertPixelTypes & 2) ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(GetSubMenu(GetSubMenu(GetSubMenu(hMenu, 1), 0), 2), EMU_OM_VIDEO_PERF_NOUSE_4444, MF_BYCOMMAND | ((emuConvertPixelTypes & 4) ? MF_CHECKED : MF_UNCHECKED));*/
}

void emuCreateWindowMenu(HWND hWnd)		{
	HMENU hMenu, hPopupFile, hPopupVideo, hPopupVideoPerformance, hPopupVideoFrameskip;
	//File
	hPopupFile = CreateMenu();
	if (emuDebugMode)		{
		AppendMenu(hPopupFile,MF_STRING, EMU_OM_FILE_STEP, "Step frame");
		AppendMenu(hPopupFile,MF_STRING, EMU_OM_FILE_PAUSE, "Pause");
	}
	AppendMenu(hPopupFile,MF_STRING, EMU_OM_FILE_QUIT, "Quit");

	hPopupVideoPerformance = CreateMenu();
	AppendMenu(hPopupVideoPerformance, MF_STRING, EMU_OM_VIDEO_PERF_TEXCACHE, "Reuse textures (faster)");
	AppendMenu(hPopupVideoPerformance, MF_STRING|MF_GRAYED, EMU_OM_VIDEO_PERF_SHADERS, "Use shaders");


	hPopupVideoFrameskip = CreateMenu();
	AppendMenu(hPopupVideoFrameskip, MF_STRING, EMU_OM_VIDEO_FSKIP_NONE, "None");
	AppendMenu(hPopupVideoFrameskip, MF_STRING, EMU_OM_VIDEO_FSKIP_NORMAL, "Normal (PC-timed)");
	AppendMenu(hPopupVideoFrameskip, MF_STRING|MF_GRAYED, EMU_OM_VIDEO_FSKIP_NORMAL, "Frameskip (PC-sided)");

/*	hPopupVideoPerformanceNoTexUse = CreateMenu();
	AppendMenu(hPopupVideoPerformanceNoTexUse, MF_STRING, EMU_OM_VIDEO_PERF_NOUSE_5650, "GL_UNSIGNED_SHORT_5_6_5_REV");
	AppendMenu(hPopupVideoPerformanceNoTexUse, MF_STRING, EMU_OM_VIDEO_PERF_NOUSE_5551, "GL_UNSIGNED_SHORT_1_5_5_5_REV");
	AppendMenu(hPopupVideoPerformanceNoTexUse, MF_STRING, EMU_OM_VIDEO_PERF_NOUSE_4444, "GL_UNSIGNED_SHORT_4_4_4_4_REV");*/

	//Vidéo
	hPopupVideo = CreateMenu();
	AppendMenu(hPopupVideo,MF_STRING|MF_POPUP, (UINT_PTR)hPopupVideoPerformance, "Performance");
	if (emuDebugMode)
		AppendMenu(hPopupVideo,MF_STRING|MF_POPUP, (UINT_PTR)hPopupVideoFrameskip, "Frameskip");
//	AppendMenu(hPopupVideoPerformance, MF_STRING | MF_POPUP, (UINT_PTR)hPopupVideoPerformanceNoTexUse, "Don't use");

	hMenu = CreateMenu();
	AppendMenu(hMenu,MF_STRING|MF_POPUP, (UINT_PTR)hPopupFile, "Application");
	AppendMenu(hMenu,MF_STRING|MF_POPUP, (UINT_PTR)hPopupVideo, "Video");
	SetMenu(hWnd, hMenu);
	emuLoadMenuItems();
	emuCheckMenus(hWnd);
}

BOOL emuInitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   RECT rect;
   HDC hdc;

	const int STYLE_FENETRE=WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
	rect.left = 0;
	rect.top = 0;
	rect.right = 480;
	rect.bottom = 272;
	AdjustWindowRect(&rect, STYLE_FENETRE, 1);
	hWnd = CreateWindow("OSLib", emuWindowTitle, STYLE_FENETRE,
		CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
	emuCreateWindowMenu(hWnd);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   hdc = GetDC(hWnd);
   PatBlt(hdc, 0, 0, 480, 272, BLACKNESS);
//   SetTextColor(hdc, RGB(255, 255, 255));
//   TextOut(hdc, 0, 0, "Please wait...", 14);
   ReleaseDC(hWnd, hdc);
   UpdateWindow(hWnd);
   emuHWnd = hWnd;

   return TRUE;
}

void emuDrawSonyScreen(HWND hWnd)	{
/*	HBRUSH hbr;
	HDC hdc, hdcScreen, myDC;
	RECT rt;
	int i, j;
	HBITMAP hbm;
	u32 bits[480][272];*/
	HWND hStatic;

/*	hStatic = CreateWindow ("Static", "Please wait...", WS_CHILD,
		0,0,480,272, hWnd, NULL, NULL, NULL);*/
	hStatic = CreateWindow ("Static", "Please wait...", WS_CHILD,
		0,0,480,272, hWnd, NULL, NULL, NULL);
	ShowWindow(hStatic, SW_SHOW);
/*	hdcScreen = GetDC(NULL);
	myDC = CreateCompatibleDC(hdcScreen);
	memset(bits, 128, sizeof(bits));
	hbm = CreateBitmap(480, 272, 1, 32, bits);
	SelectObject(myDC, hbm);
	//Pour couvrir
	hStatic = CreateWindow ("Static", "", WS_CHILD,
		0,0,480,272, hWnd, NULL, NULL, NULL);
	hdc = GetDC(hStatic);
	SetBkMode(myDC, TRANSPARENT);
	SetTextColor(myDC, RGB(255, 255, 255));
	TextOut(myDC, 0, 0, "Please wait...", 14);
	ShowWindow(hStatic, SW_SHOW);
	BitBlt(hdc, 0, 0, 480, 272, myDC, 0, 0, SRCCOPY);
	ReleaseDC(hStatic, hdc);
	ReleaseDC(NULL, hdcScreen);*/
}

void emuExit()		{
	emuSaveMenuItems();
	PostQuitMessage(0);
	TerminateProcess(GetCurrentProcess(), 0);
}

LRESULT CALLBACK emuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hbrSonyScreen = NULL;
	switch (message) 
	{
		case WM_KEYDOWN:
			emuKeyHeld[(int) wParam] = 1;
			if ((int) wParam == VK_ESCAPE)
				goto close;
			break;

		case WM_KEYUP:
			emuKeyHeld[(int) wParam] = 0;
			break;

		case WM_COMMAND:
			//Analyse les sélections du menu
			switch (LOWORD(wParam))
			{
				case EMU_OM_FILE_PAUSE:
					emuPauseGame = !emuPauseGame;
					emuCheckMenus(hWnd);
					break;
				case EMU_OM_FILE_STEP:
					emuPauseGame = 2;
					emuCheckMenus(hWnd);
					break;
				case EMU_OM_FILE_QUIT:
					goto close;
				case EMU_OM_VIDEO_PERF_TEXCACHE:
					emuReuseTextures = !emuReuseTextures;
					emuCheckMenus(hWnd);
					break;
				case EMU_OM_VIDEO_PERF_SHADERS:
					emuUsePixelShaders = !emuUsePixelShaders;
					emuCheckMenus(hWnd);
					break;
				case EMU_OM_VIDEO_FSKIP_NORMAL:
					emuFrameSkipMode = 0;
					emuCheckMenus(hWnd);
					break;
				case EMU_OM_VIDEO_FSKIP_NONE:
					emuFrameSkipMode = 1;
					emuCheckMenus(hWnd);
					break;
			}
			break;

		case WM_ERASEBKGND:
			return TRUE;

/*		case WM_CTLCOLORSTATIC:
			HDC hdcStatic;
			HWND hwndStatic;
			hdcStatic = (HDC) wParam;   // handle to display context
			hwndStatic = (HWND) lParam; // handle to static control
			if (!hbrSonyScreen)
				hbrSonyScreen = CreateSolidBrush(RGB(255, 255, 255));
			return (LRESULT)hbrSonyScreen;*/

		case WM_DESTROY:
		case WM_CLOSE:
close:
			DWORD exitCode;
			if (!osl_quit)			{
				osl_quit = 1;
				emuDrawSonyScreen(hWnd);
//				WaitForSingleObject(emuHThread, 3000);
			}
			else	{
				GetExitCodeThread(emuHThread, &exitCode);
				TerminateThread(emuHThread, exitCode);
				emuExit();
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void emuProgThreadStart()		{
	emuInitEmulation();
	main();
}

