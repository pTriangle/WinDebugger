#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern ATOM _atomWndClassList;
extern ATOM _atomWndClassHide;
extern HINSTANCE g_hInst;

inline static BOOL IsKeyDown(int nKey)
{
    return (::GetAsyncKeyState(nKey) < 0);
}

inline static void EraseRect(HDC hDC, RECT* prect, COLORREF rgb)
{
    COLORREF clr = ::SetBkColor(hDC, rgb);
    ::ExtTextOutW(hDC, 0, 0, ETO_OPAQUE, prect, NULL, NULL, NULL);
    ::SetBkColor(hDC, clr);
}

inline void InitializeMenuItem(HMENU hmenu, LPTSTR lpszItemName, int nId, int nPos)
{
    MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID | MIIM_TYPE;
    mii.fType = MFT_STRING;
    mii.wID = nId;
    mii.dwTypeData = lpszItemName;

    InsertMenuItem(hmenu, nPos, TRUE, &mii);
}

inline static bool GetTaskbarRect(RECT& rect)
{
    HWND hWnd = FindWindow(L"Shell_TrayWnd", L"");

    if (hWnd != NULL)
    {
        GetWindowRect(hWnd, &rect);

        return true;
    }

    return false;
}

inline void GradientFill(HDC hDC, RECT& rect, COLORREF cr1, COLORREF cr2)
{
    static TRIVERTEX     vert[2] = { 0 };
    static GRADIENT_RECT gRect = { 0 };

    vert[0].x = rect.left;
    vert[0].y = rect.top;
    vert[0].Alpha = 0;

    vert[1].x = rect.right;
    vert[1].y = rect.bottom;
    vert[1].Alpha = 0;

    vert[0].Red = MAKEWORD(0, GetRValue(cr1));
    vert[0].Green = MAKEWORD(0, GetGValue(cr1));
    vert[0].Blue = MAKEWORD(0, GetBValue(cr1));
    vert[1].Red = MAKEWORD(0, GetRValue(cr2));
    vert[1].Green = MAKEWORD(0, GetGValue(cr2));
    vert[1].Blue = MAKEWORD(0, GetBValue(cr2));

    gRect.UpperLeft = 1;
    gRect.LowerRight = 0;

    ::GradientFill(hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

class DIBPlane
{

private:

    HBITMAP	m_hBitMap;
    HDC		m_hDC;
    void*  m_pBuffer;
    DWORD  m_dwWidth;
    DWORD  m_dwHeight;
    DWORD  m_dwBpp;
    RECT   m_Rect;

public:

    DIBPlane() :m_hBitMap(NULL), m_hDC(NULL)
    {
    }

    virtual ~DIBPlane()
    {
        Destroy();
    }

    void Destroy()
    {
        if (m_hBitMap) DeleteObject(m_hBitMap);
        m_hBitMap = NULL;

        if (m_hDC) DeleteDC(m_hDC);
        m_hDC = NULL;
    }

    BOOL Create(DWORD dwWidth, DWORD dwHeight)
    {
        DIBPlane::Destroy();

        m_dwBpp = 32;
        m_dwWidth = ((dwWidth + 3) / 4) * 4;
        m_dwHeight = ((dwHeight + 3) / 4) * 4;

        BITMAPINFO bi; memset(&bi, 0, sizeof(bi));

        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = m_dwWidth;
        bi.bmiHeader.biHeight = -(long)m_dwHeight;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = (WORD)m_dwBpp;
        bi.bmiHeader.biCompression = BI_RGB;

        HDC hdc = ::GetDC(NULL);
        m_hBitMap = (HBITMAP)CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (VOID **)&m_pBuffer, NULL, 0);
        ::ReleaseDC(NULL, hdc);

        if (m_hBitMap == NULL) return FALSE;

        m_hDC = ::CreateCompatibleDC(NULL);

        ::SelectObject(m_hDC, m_hBitMap);

        SetRect(&m_Rect, 0, 0, dwWidth, dwHeight);

        EraseRect(m_hDC, &m_Rect, RGB(0, 0, 0));

        return TRUE;
    }

    inline void Paste(HDC hDC, int x, int y, DWORD rop)
    {
        ::BitBlt(hDC, x, y, m_dwWidth, m_dwHeight, m_hDC, 0, 0, rop);
    }

    inline HDC GetDC()
    {
        return m_hDC;
    }

    inline void* GetPtr()
    {
        return m_pBuffer;
    }

    inline RECT GetRect()
    {
        return m_Rect;
    }
};

namespace WINDOW {

#define WM_CREATED				  (WM_USER+100)
#define WM_XT9KEYBARDLAYOUTUPDATE (WM_USER+200)

    class BaseWindow
    {

    public:

        virtual ~BaseWindow();

        inline BOOL _InitWindowClass(LPCWSTR lpszClassName)
        {
            WNDCLASSEXW wndclass;

            wndclass.cbSize = sizeof(WNDCLASSEX);
            wndclass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_DROPSHADOW;
            wndclass.lpfnWndProc = gWndProc;
            wndclass.cbClsExtra = 0;
            wndclass.cbWndExtra = 0;
            wndclass.hInstance = g_hInst;

            wndclass.hIcon = (HICON)LoadImage(g_hInst, L"IDI_DEMO", IMAGE_ICON, 32, 32, 0);
            wndclass.hIconSm = (HICON)LoadImage(g_hInst, L"IDI_DEMOSMALL", IMAGE_ICON, 16, 16, 0);
            wndclass.hCursor = ::LoadCursor(g_hInst, IDC_ARROW);;

            wndclass.hbrBackground = 0;
            wndclass.lpszMenuName = NULL;
            wndclass.lpszClassName = lpszClassName;

            _atomWndClassList = RegisterClassExW(&wndclass);

            if (_atomWndClassList == 0) {

                return FALSE;
            }

            return TRUE;
        }

        static void _UninitWindowClass()
        {
            if (_atomWndClassList != 0) {

                UnregisterClass((LPCTSTR)_atomWndClassList, g_hInst);
            }
        };

        static inline BOOL _InitHidenWindowClass()
        {
            WNDCLASSEXW wndclass;

            wndclass.cbSize = sizeof(WNDCLASSEX);
            wndclass.style = 0;
            wndclass.lpfnWndProc = gWndProc;
            wndclass.cbClsExtra = 0;
            wndclass.cbWndExtra = 0;
            wndclass.hInstance = g_hInst;
            wndclass.hIcon = NULL;
            wndclass.hIconSm = NULL;
            wndclass.hCursor = NULL;
            wndclass.hbrBackground = 0;
            wndclass.lpszMenuName = NULL;
            wndclass.lpszClassName = L"THEMESSAGE";

            _atomWndClassHide = RegisterClassExW(&wndclass);

            if (_atomWndClassHide == 0) {

                return FALSE;
            }

            return TRUE;
        }

        static void _UninitHidenWindowClass()
        {
            if (_atomWndClassHide != 0) {

                UnregisterClass((LPCTSTR)_atomWndClassHide, g_hInst);
            }
        };

        virtual LRESULT Create(const BaseWindow* pWindow = NULL);
        virtual LRESULT CreateMessageWindow(const DWORD ID);
        virtual LRESULT Destroy();
        virtual void UpdateView();
        virtual void CalculateSize() {};

        inline HWND GetHWnd() const { return m_hWnd; }

        static inline void GetScreenSize(int &x, int &y)
        {
            x = ::GetSystemMetrics(SM_CXSCREEN);
            y = ::GetSystemMetrics(SM_CYSCREEN);
        }

        inline BOOL IsShown() { return m_bShown; };

    protected:

        BaseWindow();

        virtual void MakeWindow() = 0;

        virtual void OnInitialize__()
        {
            OnInitialize();
            SendMessage(m_hWnd, WM_CREATED, 0, 0);
        }

        virtual void OnInitialize()
        {
        }

        virtual	LRESULT OnDraw(HDC hDC) { return 1; };
        virtual	LRESULT OnNCDraw(HDC hDC) { return 1; };

        static LRESULT CALLBACK gWndProc(HWND, UINT, WPARAM, LPARAM);

        virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        virtual LRESULT OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnPrintClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnNCDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        virtual LRESULT OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnDrawItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSystemCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSetWindowText(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCCalcSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnGetMinMaxInfo(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCHitTest(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCLBtnDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCLBtnUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCDCLBtn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCRBtnDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCRBtnUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCDCRBtn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCActive(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnNCMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnPosChanging(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSizing(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnMoving(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnErase(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSetFocus(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnKillFocus(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnActivate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnUniChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnIMEChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnDeadChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSysKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSysKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSysChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnSysDeadChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnVScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnHScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnMouseHover(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnMouseLeave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnMouseActivate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnDCLButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnDCRButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnDropFiles(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnIMEStartComp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnIMEComp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnIMEEndComp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }

        virtual LRESULT OnEnterSizeMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }
        virtual LRESULT OnExitSizeMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, uMsg, wParam, lParam); }

        virtual LRESULT OnScrollPosVert(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 1; }
        virtual LRESULT OnScrollPosHorz(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 1; }

        virtual LRESULT OnCreated(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 1; }
        virtual LRESULT OnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 1; }

        virtual LRESULT OnXT9KeyboardLayoutUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 1; }

        HWND m_hWnd;

        BOOL m_bShown;

        BaseWindow* m_pParentWindow;

        virtual LPCWSTR DefineWindowClass() { return L""; };
    };

}
