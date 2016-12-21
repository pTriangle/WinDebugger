#include "MainWindow.h"

#include ".\\BaseUI\\Font.h"
#include ".\\BaseUI\\Color.h"

MainWindow::MainWindow(void) : nWritePos(0), nReadPos(0), m_bRunning(TRUE), _nWrLine(0), _nRdLine(0)
{
    InputInit();
    _Buffer[0][0] = 0;
    _BufferLen[0] = 0;
    _nRdLineOffset = 0;
    _gCurTime = ::GetTickCount();

    m_hReadingThread = ::CreateThread(NULL, 0, WinDebugMonitorThread, this, 0, NULL);
}

MainWindow::~MainWindow(void)
{
    m_bRunning = FALSE;
}

void MainWindow::InputInit()
{
}

void MainWindow::MakeWindow()
{
    m_hWnd = ::CreateWindowExW(
        DefineWindowExStyle(),
        (LPCWSTR)DefineWindowClass(),
        (LPCWSTR)DefineWindowTitle(),
        DefineWindowStyle(),
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        m_pParentWindow ? m_pParentWindow->GetHWnd() : 0, NULL,
        g_hInst, reinterpret_cast<LPVOID>(this));
}

LRESULT MainWindow::OnCreated(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ::ShowWindow(hWnd, SW_SHOWNORMAL);
    RECT rect; ::GetClientRect(m_hWnd, &rect);
    thePlane.Create(rect.right - rect.left, rect.bottom - rect.top);
    ::SetTimer(hWnd, 1000, 5, NULL);
    return 1;
}

LRESULT MainWindow::OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ::KillTimer(hWnd, 1000);
    ::PostQuitMessage(0);
    return 0;
}

LRESULT MainWindow::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _gInitTime = ::GetTickCount();
    POINTS points = MAKEPOINTS(lParam);
    POINT pt; POINTSTOPOINT(pt, points);
    return 0;
}

LRESULT MainWindow::OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UpdateView();
    return 0;
}

LRESULT MainWindow::OnChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _gCurTime = 0;
    ProcessKey(wParam);
    UpdateView();
    return 0;
}

LRESULT MainWindow::OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _gCurTime = 0;
    //ProcessKey(wParam);
    UpdateView();
    return 0;
}

BOOL MainWindow::ProcessKey(INT nKey)
{
    int nFuncKey = 0;
    UpdateView();
    return TRUE;
}

BOOL MainWindow::ProcessPoints(POINT points[], INT times[], INT ncount)
{
    UpdateView();
    return TRUE;
}

LRESULT MainWindow::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINTS points = MAKEPOINTS(lParam);
    POINT pt; POINTSTOPOINT(pt, points);
    RECT rect; ::SetRect(&rect, DefineListX(), DefineListY(), DefineListX() + DefineListW(), DefineListY() + DefineListH());

    if (::PtInRect(&rect, pt)) {
        // empty
    }

    static int _delay = 3;
    static int delay = _delay;

    delay++;

    if (delay > _delay) {
        if (IsKeyDown(VK_LBUTTON)) {
            // empty
        }
        delay = 0;
    }

    if (IsKeyDown(VK_LBUTTON)) {
        // empty
    }

    UpdateView();
    return 0;
}

LRESULT MainWindow::OnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _gTime = ::GetTickCount();
    UpdateView();
    return 0;
}

void MainWindow::GetVersionInfo(WCHAR* text, const int len)
{
}

LRESULT MainWindow::OnDraw(HDC hDC)
{
    HDC hdc = thePlane.GetDC();
    DrawBackground(hdc);
    //DrawKeyBackground(hdc);
    //DrawTextEditor(hdc);
    //DrawProcessedTrace(hdc);
    //DrawActiveTrace(hdc);
    //DrawCandidates(hdc);
    //DrawActiveKeyboard(hdc);
    DrawCurrentStatus(hdc);
    //DrawVersionInfo(hdc);
    thePlane.Paste(hDC, 0, 0, SRCCOPY);

    return 0;
}

namespace MAIN {
    UINT _COLOR_BACKGROUND_ = Color::GetColor(Color::BLACK);
    UINT _COLOR_KEYBACKGROUND_ = Color::GetColor(Color::LIGHTBLUE);
    UINT _COLOR_KEYROUND_ = Color::GetColor(Color::GRAY);
    UINT _COLOR_ACTIVEKEY_ = Color::GetColor(Color::NAVY);
    UINT _COLOR_ACTIVETRACE_ = Color::GetColor(Color::ORANGE);
    UINT _COLOR_PROCESSEDTRACE_ = Color::GetColor(Color::DARKORANGE);
    UINT _COLOR_CANDIDATES_ = Color::GetColor(Color::WHITE);
    UINT _COLOR_ENGCANDIDATES_ = Color::GetColor(Color::GRAY);
    UINT _COLOR_ENGKEYBOARDTEXT_ = Color::GetColor(Color::GRAY);
    UINT _COLOR_ACTIVEKEYBOARDTEXT_ = Color::GetColor(Color::BLACK);
    UINT _COLOR_ACTIVEKEYBOARDFUNC_ = Color::GetColor(Color::GRAY);
};

void MainWindow::DrawBackground(HDC hDC)
{
    RECT rect; ::GetClientRect(m_hWnd, &rect);
    EraseRect(hDC, &rect, MAIN::_COLOR_BACKGROUND_);
}

void MainWindow::DrawKeyBackground(HDC hDC)
{
}

void MainWindow::DrawProcessedTrace(HDC hDC)
{
}

void MainWindow::DrawActiveTrace(HDC hDC)
{
}

void MainWindow::DrawCandidates(HDC hDC)
{
}

void MainWindow::DrawActiveKeyboard(HDC hDC)
{
}

void MainWindow::DrawTextEditor(HDC hDC)
{
}

DWORD WINAPI MainWindow::WinDebugMonitorThread(void *pData)
{
    MainWindow *_this = (MainWindow *)pData;

    if (_this != NULL) {
        while (_this->m_bRunning) {
            static const int BUFSIZE = 8192;
            DWORD _dwRead = 0;
            CHAR _chBuf[BUFSIZE] = { 0, };
            BOOL bSuccess = ReadFile(GUI_WIN_Monitor::g_hChildStd_OUT_Rd, (_chBuf), BUFSIZE, &(_dwRead), NULL);
            if (bSuccess) {
                for (DWORD n = 0; n < _dwRead;) {
                    for (int m = _this->_BufferLen[(_this->_nWrLine%LINECOUNT)]; n < _dwRead; n++) {
                        if (m < LINESIZE) {
                            _this->_Buffer[(_this->_nWrLine%LINECOUNT)][m] = _chBuf[n];
                            m++;
                            _this->_Buffer[(_this->_nWrLine%LINECOUNT)][m] = 0;
                            _this->_BufferLen[(_this->_nWrLine%LINECOUNT)] = m;
                            if (_chBuf[n] == 10) {
                                _this->_BufferLen[(_this->_nWrLine%LINECOUNT)] = m;
                                _this->_nWrLine++;
                                _this->_BufferLen[(_this->_nWrLine%LINECOUNT)] = 0;
                                n++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

void MainWindow::DrawCurrentStatus(HDC hDC)
{
    if (_nRdLine + 24 < _nWrLine) {
        _nRdLine++;
        //_nRdLineOffset+=8; _nRdLineOffset = _nRdLineOffset % 24;
        if (_nRdLineOffset == 0) {
            //_nRdLine++;
        }
    }

    ::SetTextColor(hDC, MAIN::_COLOR_CANDIDATES_);
    ::SetBkMode(hDC, TRANSPARENT);

    Font theFont(L"", 21);

    INT nRdLine = _nRdLine, nLine = 0, nLineHeight = theFont.GetHieght();
    HFONT hPrevFont = (HFONT) ::SelectObject(hDC, theFont.GethFont());
    DRAWTEXTPARAMS tabs = { sizeof(DRAWTEXTPARAMS), }; tabs.iTabLength = 16;

    for (int n = 0; n < 32; n++) {
        RECT rect = { 0, nLine - _nRdLineOffset, WIDTH, nLine - _nRdLineOffset + nLineHeight };
        ::DrawTextExA(hDC, (LPSTR)_Buffer[nRdLine%LINECOUNT], _BufferLen[nRdLine%LINECOUNT], &rect, DT_LEFT | DT_TOP | DT_EXPANDTABS, &tabs);
        nLine += nLineHeight; nRdLine++;
        if (nRdLine > _nWrLine) break;
    }
    ::SelectObject(hDC, hPrevFont);
}

void MainWindow::DrawVersionInfo(HDC hDC)
{
}
