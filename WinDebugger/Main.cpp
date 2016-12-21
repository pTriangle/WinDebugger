#include "Main.h"
#include ".\\MainWindow.h"
#include ".\\DebugMonitor\\DebugMonitor.h"

HINSTANCE g_hInst;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    g_hInst = hInstance;
    MainWindow theMainWindow;
    theMainWindow.Create(NULL);
    static MSG msg;
    while (::GetMessageW(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
