#include "BaseWindow.h"

#pragma comment (lib,"Comctl32.lib")

ATOM _atomWndClassList = 0;;
ATOM _atomWndClassHide = 0;

namespace WINDOW {

#define HANDLE_MY_MSG(Ret, message, fn) \
    case (message): Ret = fn((hWnd), (message), (wParam), (lParam)); break;

    BaseWindow::BaseWindow() :m_hWnd(0), m_bShown(FALSE)
    {
    }

    BaseWindow::~BaseWindow()
    {
        Destroy();
    }

    LRESULT BaseWindow::Create(const BaseWindow* pParentWindow)
    {
        _InitWindowClass(DefineWindowClass());

        m_pParentWindow = (BaseWindow*)pParentWindow;

        MakeWindow();

        ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(this));

        OnInitialize__();

        return 0;
    }

    LRESULT BaseWindow::CreateMessageWindow(const DWORD nID)
    {
        WCHAR windowtitle[256] = { 0, };

        ::wsprintfW(windowtitle, L"XT9MESSAGEWND%08d", nID);

        m_hWnd = ::CreateWindowExW(0, (LPCWSTR)L"XT9MESSAGE", (LPCWSTR)windowtitle,
            0, 0, 0, 0, 0,
            HWND_MESSAGE, NULL, g_hInst, reinterpret_cast<LPVOID>(this));

        ::SetWindowLongW(m_hWnd, GWL_USERDATA, reinterpret_cast<LONG>(this));

        OnInitialize();

        return 0;
    }

    LRESULT BaseWindow::Destroy()
    {
        if (m_hWnd != NULL) {

            ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);

            DestroyWindow(m_hWnd);

            m_hWnd = NULL;
        }

        return 0;
    }

    void BaseWindow::UpdateView()
    {
        if (m_hWnd != NULL) {

            InvalidateRect(m_hWnd, NULL, FALSE);

            UpdateWindow(m_hWnd);
        }
    }

    LRESULT CALLBACK BaseWindow::gWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        BaseWindow* win = reinterpret_cast<BaseWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (win != NULL) {

            return win->WndProc(hWnd, uMsg, wParam, lParam);
        }

        return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    LRESULT BaseWindow::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT paint;

        BeginPaint(hWnd, &paint);

        OnDraw(paint.hdc);

        EndPaint(hWnd, &paint);

        return 0;
    }

    LRESULT BaseWindow::OnPrintClient(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        OnDraw((HDC)wParam);

        return 0;
    }

    LRESULT BaseWindow::OnNCDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG)NULL);

        m_hWnd = NULL;

        return ::DefWindowProcW(hWnd, message, wParam, lParam);
    }

    LRESULT BaseWindow::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        ::ShowWindow(m_hWnd, SW_HIDE);

        return 0;
    }

    LRESULT	BaseWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT Ret = 1;

        switch (uMsg)
        {
            HANDLE_MY_MSG(Ret, WM_NCCALCSIZE, OnNCCalcSize);
            HANDLE_MY_MSG(Ret, WM_GETMINMAXINFO, OnGetMinMaxInfo);
            HANDLE_MY_MSG(Ret, WM_NCPAINT, OnNCPaint);
            HANDLE_MY_MSG(Ret, WM_NCHITTEST, OnNCHitTest);
            HANDLE_MY_MSG(Ret, WM_NCLBUTTONDOWN, OnNCLBtnDown);
            HANDLE_MY_MSG(Ret, WM_NCLBUTTONUP, OnNCLBtnUp);
            HANDLE_MY_MSG(Ret, WM_NCLBUTTONDBLCLK, OnNCDCLBtn);
            HANDLE_MY_MSG(Ret, WM_NCRBUTTONDOWN, OnNCRBtnDown);
            HANDLE_MY_MSG(Ret, WM_NCRBUTTONUP, OnNCRBtnUp);
            HANDLE_MY_MSG(Ret, WM_NCRBUTTONDBLCLK, OnNCDCRBtn);
            HANDLE_MY_MSG(Ret, WM_NCACTIVATE, OnNCActive);
            HANDLE_MY_MSG(Ret, WM_NCMOUSEMOVE, OnNCMouseMove);
            HANDLE_MY_MSG(Ret, WM_WINDOWPOSCHANGING, OnPosChanging);
            HANDLE_MY_MSG(Ret, WM_SIZING, OnSizing);
            HANDLE_MY_MSG(Ret, WM_MOVING, OnMoving);
            HANDLE_MY_MSG(Ret, WM_SYSCOMMAND, OnSystemCommand);
            HANDLE_MY_MSG(Ret, WM_PAINT, OnPaint);
            HANDLE_MY_MSG(Ret, WM_PRINTCLIENT, OnPrintClient);
            HANDLE_MY_MSG(Ret, WM_DRAWITEM, OnDrawItem);
            HANDLE_MY_MSG(Ret, WM_ERASEBKGND, OnErase);
            HANDLE_MY_MSG(Ret, WM_SIZE, OnSize);
            HANDLE_MY_MSG(Ret, WM_DESTROY, OnDestroy);
            HANDLE_MY_MSG(Ret, WM_NCDESTROY, OnNCDestroy);
            HANDLE_MY_MSG(Ret, WM_VSCROLL, OnVScroll);
            HANDLE_MY_MSG(Ret, WM_HSCROLL, OnHScroll);
            HANDLE_MY_MSG(Ret, WM_MOUSEWHEEL, OnMouseWheel);
            HANDLE_MY_MSG(Ret, WM_MOUSEMOVE, OnMouseMove);
            HANDLE_MY_MSG(Ret, WM_MOUSEHOVER, OnMouseHover);
            HANDLE_MY_MSG(Ret, WM_MOUSELEAVE, OnMouseLeave);
            HANDLE_MY_MSG(Ret, WM_MOUSEACTIVATE, OnMouseActivate);
            HANDLE_MY_MSG(Ret, WM_SETFOCUS, OnSetFocus);
            HANDLE_MY_MSG(Ret, WM_KILLFOCUS, OnKillFocus);
            HANDLE_MY_MSG(Ret, WM_ACTIVATE, OnActivate);
            HANDLE_MY_MSG(Ret, WM_KEYUP, OnKeyUp);
            HANDLE_MY_MSG(Ret, WM_KEYDOWN, OnKeyDown);
            HANDLE_MY_MSG(Ret, WM_CHAR, OnChar);
            HANDLE_MY_MSG(Ret, WM_UNICHAR, OnUniChar);
            HANDLE_MY_MSG(Ret, WM_IME_CHAR, OnIMEChar);
            HANDLE_MY_MSG(Ret, WM_DEADCHAR, OnDeadChar);
            HANDLE_MY_MSG(Ret, WM_SYSKEYUP, OnSysKeyUp);
            HANDLE_MY_MSG(Ret, WM_SYSKEYDOWN, OnSysKeyDown);
            HANDLE_MY_MSG(Ret, WM_SYSCHAR, OnSysChar);
            HANDLE_MY_MSG(Ret, WM_SYSDEADCHAR, OnSysDeadChar);
            HANDLE_MY_MSG(Ret, WM_LBUTTONDOWN, OnLButtonDown);
            HANDLE_MY_MSG(Ret, WM_LBUTTONUP, OnLButtonUp);
            HANDLE_MY_MSG(Ret, WM_LBUTTONDBLCLK, OnDCLButton);
            HANDLE_MY_MSG(Ret, WM_RBUTTONDOWN, OnRButtonDown);
            HANDLE_MY_MSG(Ret, WM_RBUTTONUP, OnRButtonUp);
            HANDLE_MY_MSG(Ret, WM_RBUTTONDBLCLK, OnDCRButton);
            HANDLE_MY_MSG(Ret, WM_COMMAND, OnCommand);
            HANDLE_MY_MSG(Ret, WM_DROPFILES, OnDropFiles);
            HANDLE_MY_MSG(Ret, WM_SETTEXT, OnSetWindowText);
            HANDLE_MY_MSG(Ret, WM_IME_STARTCOMPOSITION, OnIMEStartComp);
            HANDLE_MY_MSG(Ret, WM_IME_COMPOSITION, OnIMEComp);
            HANDLE_MY_MSG(Ret, WM_IME_ENDCOMPOSITION, OnIMEEndComp);
            HANDLE_MY_MSG(Ret, WM_ENTERSIZEMOVE, OnEnterSizeMove);
            HANDLE_MY_MSG(Ret, WM_EXITSIZEMOVE, OnExitSizeMove);
            HANDLE_MY_MSG(Ret, WM_CREATED, OnCreated);
            HANDLE_MY_MSG(Ret, WM_TIMER, OnTimer);

            HANDLE_MY_MSG(Ret, WM_XT9KEYBARDLAYOUTUPDATE, OnXT9KeyboardLayoutUpdate);

        default:

            return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
        }

        return Ret;
    }

}
