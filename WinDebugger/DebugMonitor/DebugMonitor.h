#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include "WinDebugMonitor.h"
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static WORD backclr = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;

static std::vector<std::string>& Split(std::string s, char c, std::vector<std::string>& v)
{
    for (size_t p = 0; (p = s.find(c)) != s.npos; ) {
        v.push_back(s.substr(0, p));
        s = s.substr(p + 1);
    }
    v.push_back(s);
    return v;
}
class GUI_WIN_Monitor : public CWinDebugMonitor
{
public:
    GUI_WIN_Monitor();
    ~GUI_WIN_Monitor();

    static DWORD WINAPI WinDebugWritingThread(void *pData);

    HANDLE m_hWritingThread;

    BOOL m_bRunning;

    virtual void OutputWinDebugString(const char *str);

    static HANDLE g_hTempFile;

    static HANDLE g_hChildStd_IN_Rd;
    static HANDLE g_hChildStd_IN_Wr;
    static HANDLE g_hChildStd_OUT_Rd;
    static HANDLE g_hChildStd_OUT_Wr;
    static HANDLE g_hChildStd_OUT_ADB_Rd;
    static HANDLE g_hChildStd_OUT_ADB_Wr;
    static HANDLE g_hChildStd_ERR_Rd;
    static HANDLE g_hChildStd_ERR_Wr;
    static HANDLE g_hInputFile;

    static HANDLE m_hStdIn;
    static HANDLE m_hStdOut;
    static HANDLE m_hStdErr;
    static HANDLE m_hStdInWrite;
    static HANDLE m_hStdOutRead;
    static HANDLE m_hStdErrRead;

    HANDLE m_ChildHandle;
    static HANDLE m_hEventADBDataReady;

    static const int SIZE = 1024;
    CHAR _str[256][SIZE + 1];
    INT _len[256], _WrCnt, _RdCnt;
};
