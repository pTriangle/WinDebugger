#pragma once
#include <windows.h>
class CWinDebugMonitor
{
public:
    CWinDebugMonitor();
    virtual ~CWinDebugMonitor();
    virtual void OutputWinDebugString(const char *str) {};
private:
    enum {
        TIMEOUT_WIN_DEBUG = 1000,
    };
    struct dbwin_buffer {
        DWORD   dwProcessId;
        char    data[4096 - sizeof(DWORD)];
    };
private:
    DWORD Initialize();
    void  Unintialize();
    DWORD WinDebugMonitorProcess();
    static DWORD WINAPI WinDebugMonitorThread(void *pData);
    HANDLE m_hDBWinMutex;
    HANDLE m_hDBMonBuffer;
    HANDLE m_hEventBufferReady;
    HANDLE m_hEventDataReady;
    HANDLE m_hWinDebugMonitorThread;
    struct dbwin_buffer *m_pDBBuffer;
    BOOL m_bWinDebugMonStopped;
};

