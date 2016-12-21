#pragma once

#include ".\\BaseUI\\BaseWindow.h"
#include ".\\DebugMonitor\\DebugMonitor.h"

class MainWindow : public WINDOW::BaseWindow, GUI_WIN_Monitor
{
public:

    MainWindow(void);
    virtual ~MainWindow(void);

    virtual LRESULT OnCreated(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual LRESULT OnChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual	LRESULT OnDraw(HDC hDC);

    static DWORD WINAPI WinDebugMonitorThread(void *pData);

    HANDLE m_hReadingThread;

    BOOL m_bRunning;

protected:

    virtual void InputInit();
    virtual void MakeWindow();

    virtual BOOL ProcessKey(INT nKey);
    virtual BOOL ProcessPoints(POINT points[], INT times[], INT ncount);

    virtual DWORD   DefineWindowStyle() { return WS_OVERLAPPED | WS_SYSMENU; };
    virtual DWORD   DefineWindowExStyle() { return 0; };
    virtual LPCWSTR DefineWindowTitle() { return L"WinDebugger"; };
    virtual LPCWSTR DefineWindowClass() { return L"WinDebugger"; };

    virtual INT DefineKeyboardX() { return 50; };
    virtual INT DefineKeyboardY() { return 320; };

    virtual INT DefineListX() { return 20; };
    virtual INT DefineListY() { return 270; };
    virtual INT DefineListW() { return WIDTH - 40; };
    virtual INT DefineListH() { return 48; };

    virtual INT DefineEditorX() { return 20; };
    virtual INT DefineEditorY() { return 20; };
    virtual INT DefineEditorW() { return WIDTH - 20; };
    virtual INT DefineEditorH() { return 200; };

    virtual void GetVersionInfo(WCHAR* text, const int len);

    DWORD  _gTime;
    DWORD  _gCurTime;
    DWORD  _gInitTime;

    DIBPlane thePlane;

    static const int WIDTH = 800;
    static const int HEIGHT = 800;

    virtual	void DrawBackground(HDC hDC);
    virtual	void DrawKeyBackground(HDC hDC);
    virtual	void DrawProcessedTrace(HDC hDC);
    virtual	void DrawActiveTrace(HDC hDC);
    virtual	void DrawCandidates(HDC hDC);
    virtual	void DrawActiveKeyboard(HDC hDC);
    virtual void DrawTextEditor(HDC hDC);

    virtual	void DrawCurrentStatus(HDC hDC);
    virtual	void DrawVersionInfo(HDC hDC);

    int nWritePos, nReadPos;

    static const int LINESIZE = 512;
    static const int LINECOUNT = 1024;
    INT  _BufferLen[LINECOUNT];
    CHAR _Buffer[LINECOUNT][LINESIZE];

    INT _nWrLine, _nRdLine, _nRdLineOffset;
};
