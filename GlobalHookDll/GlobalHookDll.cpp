// GlobalHookDll.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"
#include "GlobalHookDll.h"

#pragma data_seg(".shareddata")
HHOOK CwpHook::cHook(NULL);
HHOOK MouseHook::mHook(NULL);
HHOOK KeyboardHook::kHook(NULL);
BOOL WindowController::bMoving(FALSE);
UINT WindowController::nResizing(0);
UINT16 WindowController::nMoving(-1);
POINT WindowController::ptCurFromLT{ 0, 0 }; // �E�B���h�E���ォ��̈ʒu
POINT WindowController::ptCurFromRB{ 0, 0 }; // �E�B���h�E�E������̈ʒu
POINT WindowController::ptDispFromRB{ 0, 0 };
int WindowController::nGroupKey(VK_SHIFT);
int WindowController::nMoveKey(VK_CONTROL);
bool WindowController::bAddGroupFlag(false);
std::vector<HWND> WindowController::windows;
Window WindowController::noFitWindows[255];
int WindowController::nFitRange(10);
int WindowController::nFitRangeMax(100);
int WindowController::nFitRangeMin(0);
bool WindowController::isFitWindows(true);
bool WindowController::isFitDisplay(true);
bool WindowController::isFitTaskbar(true);
bool WindowController::KeyHook(false);
intptr_t WindowController::neighbors[255]{ 0 };
std::vector<Movement> WindowController::movement;
TCHAR WindowController::launcherWindowText[255]{ 0 };
TCHAR WindowController::mysetlistWindowText[255]{ 0 };
TCHAR WindowController::itemlistWindowText[255]{ 0 };
TCHAR WindowController::configWindowText[255]{ 0 };
#pragma data_seg()

//HINSTANCE hInst;
HWND parentHwnd;
HWND configHwnd;
COPYDATASTRUCT cdsNeighbor;
COPYDATASTRUCT cdsMovement;

TCHAR WinAppClassName[] = L"Notepad";
TCHAR ChildWinClassName[] = L"Edit";
HWND hWinAppHandle;

BOOL(__stdcall *GetWindowRect2)(HWND, LPRECT);

//static void GetWindowRect3(HWND, RECT&);

static void SetDwmapi();
static bool IsAeroEnabled();
static BOOL __stdcall DwmGetWindowAttribute_(HWND, LPRECT);

static bool IsRectNull(RECT& rect);
//static void NeighborWindowRegister(); // �אڃE�B���h�E��o�^
//static void BeforeWindowMoving(const CWPSTRUCT* p); // �E�B���h�E�ړ��O
//static void BeforeWindowSizing(const CWPSTRUCT* p); // �E�B���h�E�T�C�Y�ύX�O
//static bool MatchNeighborWindow(const RECT& rect1, const RECT& rect2);
void GetMonitorRect(HWND hwnd, RECT& rect);
void GetWorkRect(HWND hwnd, RECT& rect);
void GetScale(HWND hwnd, DSIZE& scale);
void ModifiedRect(HWND hwnd, RECT& rect);

_DLLEXPORT DSIZE GetScale2(intptr_t hwnd) {
	DSIZE scale;
	GetScale((HWND)hwnd, scale);
	return scale;
}

_DLLEXPORT void SetLauncherWindowText(TCHAR windowText[]) {
	memset(launcherWindowText, 0, sizeof(TCHAR) * 255);
	lstrcpyn(launcherWindowText, windowText, 255);
}

_DLLEXPORT void SetSubWindowText(TCHAR mysetlist[], TCHAR itemlist[]) {
	memset(mysetlistWindowText, 0, sizeof(TCHAR) * 255);
	memset(itemlistWindowText, 0, sizeof(TCHAR) * 255);
	lstrcpyn(mysetlistWindowText, mysetlist, 255);
	lstrcpyn(itemlistWindowText, itemlist, 255);
}

_DLLEXPORT void SetConfigWindowText(TCHAR windowText[]) {
	memset(configWindowText, 0, sizeof(TCHAR) * 255);
	lstrcpyn(configWindowText, windowText, 255);
}

_DLLEXPORT void SetFitRange(int range) {
	nFitRange = (nFitRangeMin > range ? nFitRangeMin : nFitRangeMax < range ? nFitRangeMax : range);
}

_DLLEXPORT void SetFitRangeLimit(int min, int max) {
	nFitRangeMin = min;
	nFitRangeMax = max;
}

_DLLEXPORT int GetFitRange() {
	return nFitRange;
}

_DLLEXPORT int GetFitRangeMax() {
	return nFitRangeMax;
}

_DLLEXPORT int GetFitRangeMin() {
	return nFitRangeMin;
}

_DLLEXPORT void SetFitWindows(bool flag) {
	isFitWindows = flag;
}

_DLLEXPORT void SetFitTaskbar(bool flag) {
	isFitTaskbar = flag;
}

_DLLEXPORT void SetFitDisplay(bool flag) {
	isFitDisplay = flag;
}

_DLLEXPORT bool GetFitWindows() {
	return isFitWindows;
}

_DLLEXPORT bool GetFitTaskbar() {
	return isFitTaskbar;
}

_DLLEXPORT bool GetFitDisplay() {
	return isFitDisplay;
}

_DLLEXPORT void SetGroupKey(UINT key) {
	nGroupKey = key;
}

_DLLEXPORT UINT GetGroupKey() {
	return nGroupKey;
}

_DLLEXPORT void SetMoveKey(UINT key) {
	nMoveKey = key;
}

_DLLEXPORT UINT GetMoveKey() {
	return nMoveKey;
}

_DLLEXPORT void GetWindows(intptr_t* arr, int length) {
	for (int i = 0; i < (int)windows.size() && i < length; ++i) {
		if (windows[i] == NULL) {
			arr[i] = 0;
		}
		else {
			arr[i] = (intptr_t)windows[i];
		}
	}
}

_DLLEXPORT void SetNoFitWindows(Window* pWindowArray, int size) {
	//for (int i = 0; i < size && i < 255; ++i)
	//{
	//	snprintf(noFitWindows[i].className, 254, "\s", pWindowArray[i].className);
	//	snprintf(noFitWindows[i].text, 254, "\s", pWindowArray[i].text);
	//}
}

_DLLEXPORT void GetNoFitWindows(Window* pWindowArray, int size) {
	//for (int i = 0; i < size && i < 255; ++i)
	//{
	//	if (noFitWindows != NULL)
	//	{
	//		snprintf(pWindowArray[i].className, 254, "\s", noFitWindows[i].className);
	//		snprintf(pWindowArray[i].text, 254, "\s", noFitWindows[i].text);
	//	}
	//}
}

_DLLEXPORT void SetKeyboardHook(bool flag) {
	KeyHook = flag;
}

// �G���g���|�C���g
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID lpReserved) {
	switch (reason) {
		case DLL_PROCESS_ATTACH:
		{
			// �A�^�b�`
			//hInst = hModule;
			DLL::hInst = hModule;

			SetDwmapi();

			break;
		}
		case DLL_PROCESS_DETACH:
			// �f�^�b�`
			break;
	}

	return TRUE;
}

int DLL::SetHook() {
	if (DLL::hInst == NULL) return -1;
	
	UINT nCount = 0;
	windows.clear();
	EnumWindows(EnumWindowsProc, (LPARAM)&nCount);
	
	if (CwpHook::Init() &&
		MouseHook::Init() &&
		KeyboardHook::Init()) {
		// �t�b�N����
		return 0;
	}
	else {
		// �t�b�N���s
		return -1;
	}

}

int DLL::ResetHook() {
	
	if (CwpHook::Fin() &&
		MouseHook::Fin() &&
		KeyboardHook::Fin()) {
		// �t�b�N��������
		return 0;
	}
	else {
		// �t�b�N�������s
		return -1;
	}

}


bool IsRectNull(RECT& rect) {
	return ((rect.left | rect.top | rect.right | rect.bottom) == 0x00);
}

void SetDwmapi() {
	if (IsAeroEnabled()) {
		GetWindowRect2 = DwmGetWindowAttribute_;
	}
	else {
		GetWindowRect2 = GetWindowRect;
	}
}

// Aero���L�������� / �L��=TRUE
bool IsAeroEnabled() {
	BOOL bAero = FALSE;

	DwmIsCompositionEnabled(&bAero);

	return bAero != FALSE;
}

// GetWindowsRect �̈����Ɩ߂�l�̌^�����킹��
BOOL __stdcall DwmGetWindowAttribute_(HWND hWnd, LPRECT lpRect) {
	HRESULT h = DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, lpRect, sizeof(RECT));
	return SUCCEEDED(h);
}

bool MatchNeighborWindow(const RECT& rect1, const RECT& rect2) {
	//bool w = (rect1.left == rect2.left || rect1.left == rect2.right ||
	//	rect1.right == rect2.left || rect1.right == rect1.right);

	//bool h = (rect1.top == rect2.top || rect1.top == rect2.bottom ||
	//	rect1.bottom == rect2.top || rect1.bottom || rect2.bottom);

	if (rect1.left == rect2.left || rect1.left == rect2.right ||
		rect1.right == rect2.left || rect1.right == rect2.right) {
		if (rect2.bottom >= rect1.top && rect1.bottom >= rect2.top) {
			return true;
		}
	}
	if (rect1.top == rect2.top || rect1.top == rect2.bottom ||
		rect1.bottom == rect2.top || rect1.bottom == rect2.bottom) {
		if (rect2.right >= rect1.left && rect1.right >= rect2.left) {
			return true;
		}
	}

	return false;
}

// �X�P�[���̎擾
void GetScale(HWND hwnd, DSIZE& dSize) {
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monInfo;
	monInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monInfo);

	// ���j�^�[�������W
	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = sizeof(POINTL);
	devMode.dmFields = DM_POSITION;
	EnumDisplaySettings(monInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	// ���[�N�������W
	SIZE logicalDesktopSize;
	logicalDesktopSize.cx = monInfo.rcMonitor.right - monInfo.rcMonitor.left;
	logicalDesktopSize.cy = monInfo.rcMonitor.bottom - monInfo.rcMonitor.top;

	dSize.cx = (double)devMode.dmPelsWidth / (double)logicalDesktopSize.cx;
	dSize.cy = (double)devMode.dmPelsHeight / (double)logicalDesktopSize.cy;
}

// ���j�^�[�̕������W���擾
void GetMonitorRect(HWND hwnd, RECT& rect) {
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monInfo;
	monInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monInfo);

	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = sizeof(POINTL);
	devMode.dmFields = DM_POSITION;
	EnumDisplaySettings(monInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	rect.left = devMode.dmPosition.x;
	rect.top = devMode.dmPosition.y;
	rect.right = devMode.dmPosition.x + devMode.dmPelsWidth;
	rect.bottom = devMode.dmPosition.y + devMode.dmPelsHeight;

	ModifiedRect(hwnd, rect);
}

// ���[�N�̘_�����W���擾
void GetWorkRect(HWND hwnd, RECT& rect) {
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monInfo;
	monInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monInfo);

	// �������W�n�̃��[�N��`
	rect = monInfo.rcWork;
}

void ModifiedRect(HWND hwnd, RECT& rect) {
	DSIZE scale;
	GetScale(hwnd, scale);
	rect.left = (LONG)round((double)rect.left / scale.cx);
	rect.top = (LONG)round((double)rect.top / scale.cy);
	rect.right = (LONG)round((double)rect.right / scale.cx);
	rect.bottom = (LONG)round((double)rect.bottom / scale.cy);
}
