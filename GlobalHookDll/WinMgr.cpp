#include "stdafx.h"

BOOL(__stdcall *WinMgr::GetWindowRect2)(HWND, LPRECT);

BOOL CALLBACK WinMgr::EnumWindowsProc(HWND hWnd, LPARAM lParam) {
	INT* lpCount = (INT *)lParam;
	LONG style = GetWindowLong(hWnd, GWL_STYLE);
	LONG exstyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	TCHAR windowText[255];

	GetWindowText(hWnd, windowText, 255);

	TCHAR* excludeWindowText[4] = {
		launcherWindowText,
		mysetlistWindowText,
		itemlistWindowText,
		configWindowText
	};

	for (int i = 0; i < 4; ++i) {
		if (_tcsncmp(excludeWindowText[i], windowText, 255) == 0) {
			return TRUE;
		}
	}

	// �����e�̃E�B���h�E��\�����i�[
	if ((style & WS_VISIBLE) != 0) {
		if ((exstyle & WS_EX_NOREDIRECTIONBITMAP) == 0) {
			if ((exstyle & WS_EX_TOOLWINDOW) == 0) {
				windows.push_back(hWnd);
				*lpCount += 1;
			}
		}
	}

	return TRUE;
}

int WinMgr::Magnet(HWND& hwnd, POINT dif, RECT& rect2, POINT srcPos, SIZE& minDiff, SIZE extSize, POINT& tempPos) {
	DSIZE scale;
	GetScale(hwnd, scale);

	POINT fit;
	fit.x = (LONG)round((double)nFitRange / scale.cx);
	fit.y = (LONG)round((double)nFitRange / scale.cy);
	//int dif = (int)((double)7 / scale.cx);

	int dir = 0;
	// �ړ����̃E�B���h�E�̍���
	// ���̃E�B���h�E�̍���
	if (rect2.left - dif.x - fit.x < srcPos.x - ptCurFromLT.x && srcPos.x - ptCurFromLT.x < rect2.left - dif.x + fit.x) {
		if (rect2.top - extSize.cy - dif.y - fit.y < srcPos.y - ptCurFromLT.y && srcPos.y - ptCurFromLT.y < rect2.bottom - dif.y + fit.y) {
			if (abs(rect2.left - dif.x - srcPos.x - ptCurFromLT.x) < minDiff.cx) {
				minDiff.cx = abs(rect2.left - dif.x - srcPos.x - ptCurFromLT.x);
				tempPos.x = rect2.left + ptCurFromLT.x - dif.x;
			}
			dir |= 0x01;
		}
	}
	// ���̃E�B���h�E�̉E��
	if (rect2.right - dif.x - fit.x < srcPos.x - ptCurFromLT.x && srcPos.x - ptCurFromLT.x < rect2.right - dif.x + fit.x) {
		if (rect2.top - extSize.cy - fit.y < srcPos.y - ptCurFromLT.y && srcPos.y - ptCurFromLT.y < rect2.bottom + fit.y) {
			if (abs(rect2.right - dif.x - srcPos.x - ptCurFromLT.x) < minDiff.cx) {
				minDiff.cx = abs(rect2.right - dif.x - srcPos.x - ptCurFromLT.x);
				tempPos.x = rect2.right + ptCurFromLT.x - dif.x;
			}
			dir |= 0x01;
		}
	}
	// �ړ����̃E�B���h�E�̉E��
	// ���̃E�B���h�E�̉E��
	if (rect2.right - dif.x + fit.x > srcPos.x - ptCurFromLT.x + extSize.cx && srcPos.x - ptCurFromLT.x + extSize.cx > rect2.right - dif.x - fit.x) {
		if (rect2.top - extSize.cy - fit.y < srcPos.y - ptCurFromLT.y && srcPos.y - ptCurFromLT.y < rect2.bottom + fit.y) {
			if (abs(rect2.right - dif.x - srcPos.x - ptCurFromLT.x) < minDiff.cx) {
				minDiff.cx = abs(rect2.right - dif.x - srcPos.x - ptCurFromLT.x);
				tempPos.x = rect2.right - extSize.cx + ptCurFromLT.x - dif.x;
			}
			dir |= 0x04;
		}
	}
	// ���̃E�B���h�E�̍���
	if (rect2.left - dif.x + fit.x > srcPos.x - ptCurFromLT.x + extSize.cx && srcPos.x - ptCurFromLT.x + extSize.cx > rect2.left - dif.x - fit.x) {
		if (rect2.top - extSize.cy - fit.y < srcPos.y - ptCurFromLT.y && srcPos.y - ptCurFromLT.y < rect2.bottom + fit.y) {
			if (abs(rect2.left - dif.x - srcPos.x - ptCurFromLT.x) < minDiff.cx) {
				minDiff.cx = abs(rect2.left - dif.x - srcPos.x - ptCurFromLT.x);
				tempPos.x = rect2.left - extSize.cx + ptCurFromLT.x - dif.x;
			}
			dir |= 0x04;
		}
	}
	// �ړ����̃E�B���h�E�̏�
	// ���̃E�B���h�E�̏�
	if (rect2.top - fit.y < srcPos.y - ptCurFromLT.y && srcPos.y - ptCurFromLT.y < rect2.top + fit.y) {
		if (rect2.left - extSize.cx - dif.x - fit.x < srcPos.x - ptCurFromLT.x && srcPos.x - ptCurFromLT.x < rect2.right - dif.x + fit.x) {
			if (abs(rect2.top - srcPos.y - ptCurFromLT.y) < minDiff.cy) {
				minDiff.cy = abs(rect2.top - srcPos.y - ptCurFromLT.y);
				tempPos.y = rect2.top + ptCurFromLT.y;
			}
			dir |= 0x02;
		}
	}
	// ���̃E�B���h�E�̉�
	if (rect2.bottom - fit.y < srcPos.y - ptCurFromLT.y && srcPos.y - ptCurFromLT.y < rect2.bottom + fit.y) {
		if (rect2.left - extSize.cx - dif.x - fit.x < srcPos.x - ptCurFromLT.x && srcPos.x - ptCurFromLT.x < rect2.right - dif.x + fit.x) {
			if (abs(rect2.bottom - srcPos.y - ptCurFromLT.y) < minDiff.cy) {
				minDiff.cy = abs(rect2.bottom - srcPos.y - ptCurFromLT.y);
				tempPos.y = rect2.bottom + ptCurFromLT.y;
			}
			dir |= 0x02;
		}
	}
	// �ړ����̃E�B���h�E�̉�
	// ���̃E�B���h�E�̉�
	if (rect2.bottom + fit.y > srcPos.y - ptCurFromLT.y + extSize.cy && srcPos.y - ptCurFromLT.y + extSize.cy > rect2.bottom - fit.y) {
		if (rect2.left - extSize.cx - dif.x - fit.x < srcPos.x - ptCurFromLT.x && srcPos.x - ptCurFromLT.x < rect2.right - dif.x + fit.x) {
			if (abs(rect2.bottom - srcPos.y - ptCurFromLT.y) < minDiff.cy) {
				minDiff.cy = abs(rect2.bottom - srcPos.y - ptCurFromLT.y);
				tempPos.y = rect2.bottom - extSize.cy + ptCurFromLT.y;
			}
			dir |= 0x08;
		}
	}
	// ���̃E�B���h�E�̏�
	if (rect2.top + fit.y > srcPos.y - ptCurFromLT.y + extSize.cy && srcPos.y - ptCurFromLT.y + extSize.cy > rect2.top - fit.y) {
		if (rect2.left - extSize.cx - dif.x - fit.x < srcPos.x - ptCurFromLT.x && srcPos.x - ptCurFromLT.x < rect2.right - dif.x + fit.x) {
			if (abs(rect2.top - srcPos.y - ptCurFromLT.y) < minDiff.cy) {
				minDiff.cy = abs(rect2.top - srcPos.y - ptCurFromLT.y);
				tempPos.y = rect2.top - extSize.cy + ptCurFromLT.y;
			}
			dir |= 0x08;
		}
	}

	return dir;
}

int WinMgr::GetWindowIndex(HWND& hwnd) {
	if (hwnd == NULL) return -1;

	for (int i = 0; i < (int)windows.size(); i++) {
		if (windows[i] == hwnd) {
			return i;
		}
	}

	return -1;
}

bool WinMgr::IsRectNull(RECT& rect) {
	return ((rect.left | rect.top | rect.right | rect.bottom) == 0x00);
}

void WinMgr::SetDwmapi() {
	if (IsAeroEnabled()) {
		GetWindowRect2 = DwmGetWindowAttribute_;
	}
	else {
		GetWindowRect2 = GetWindowRect;
	}
}

// Aero���L�������� / �L��=TRUE
bool WinMgr::IsAeroEnabled() {
	BOOL bAero = FALSE;

	DwmIsCompositionEnabled(&bAero);

	return bAero != FALSE;
}

// GetWindowsRect �̈����Ɩ߂�l�̌^�����킹��
BOOL __stdcall WinMgr::DwmGetWindowAttribute_(HWND hWnd, LPRECT lpRect) {
	HRESULT h = DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, lpRect, sizeof(RECT));
	return SUCCEEDED(h);
}

// �X�P�[���̎擾
void WinMgr::GetScale(HWND hwnd, DSIZE& dSize) {
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
void WinMgr::GetMonitorRect(HWND hwnd, RECT& rect) {
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
void WinMgr::GetWorkRect(HWND hwnd, RECT& rect) {
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monInfo;
	monInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monInfo);

	// �������W�n�̃��[�N��`
	rect = monInfo.rcWork;
}

void WinMgr::ModifiedRect(HWND hwnd, RECT& rect) {
	DSIZE scale;
	GetScale(hwnd, scale);
	rect.left = (LONG)round((double)rect.left / scale.cx);
	rect.top = (LONG)round((double)rect.top / scale.cy);
	rect.right = (LONG)round((double)rect.right / scale.cx);
	rect.bottom = (LONG)round((double)rect.bottom / scale.cy);
}

void WinMgr::UpdateWindows() {
	UINT nCount = 0;
	windows.clear();
	EnumWindows(EnumWindowsProc, (LPARAM)&nCount);
}

void WinMgr::GetWindows(intptr_t* hwnds, int length) {
	for (int i = 0; i < (int)windows.size() && i < length; ++i) {
		if (windows[i] == NULL) {
			hwnds[i] = 0;
		}
		else {
			hwnds[i] = (intptr_t)windows[i];
		}
	}
}

bool WinMgr::MatchNeighborWindow(const RECT& rect1, const RECT& rect2) {

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