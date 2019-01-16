#pragma once

class WindowController {

private:

	static BOOL bMoving;
	static UINT nResizing;
	static UINT16 nMoving;
	static POINT ptCurFromLT; // �E�B���h�E���ォ��̈ʒu
	static POINT ptCurFromRB; // �E�B���h�E�E������̈ʒu
	static POINT ptDispFromRB;
	static int nGroupKey;
	static int nMoveKey;
	static bool bAddGroupFlag;
	static std::vector<HWND> windows;
	static Window noFitWindows[255];
	static int nFitRange;
	static int nFitRangeMax;
	static int nFitRangeMin;
	static bool isFitWindows;
	static bool isFitDisplay;
	static bool isFitTaskbar;
	static bool KeyHook;
	static intptr_t neighbors[255];
	static std::vector<Movement> movement;
	static TCHAR launcherWindowText[255];
	static TCHAR mysetlistWindowText[255];
	static TCHAR itemlistWindowText[255];
	static TCHAR configWindowText[255];


public:

	static int Magnet(HWND& hwnd, POINT dif, RECT& rect2, POINT srcPos, SIZE& minDiff, SIZE extSize, POINT& tempPos);
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	static int GetWindowIndex(HWND& hwnd);

};
