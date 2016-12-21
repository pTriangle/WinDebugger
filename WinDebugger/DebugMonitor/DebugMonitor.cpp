#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "WinDebugMonitor.h"
#include "DebugMonitor.h"
#define BUFFER_SIZE 256

HANDLE GUI_WIN_Monitor::g_hChildStd_IN_Rd = NULL;
HANDLE GUI_WIN_Monitor::g_hChildStd_IN_Wr = NULL;
HANDLE GUI_WIN_Monitor::g_hChildStd_OUT_Rd = NULL;
HANDLE GUI_WIN_Monitor::g_hChildStd_OUT_Wr = NULL;
HANDLE GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Rd = NULL;
HANDLE GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Wr = NULL;
HANDLE GUI_WIN_Monitor::g_hInputFile = NULL;
HANDLE GUI_WIN_Monitor::g_hTempFile = NULL;

HANDLE GUI_WIN_Monitor::m_hStdIn = NULL;
HANDLE GUI_WIN_Monitor::m_hStdOut = NULL;
HANDLE GUI_WIN_Monitor::m_hStdErr = NULL;

HANDLE GUI_WIN_Monitor::m_hStdInWrite = NULL;
HANDLE GUI_WIN_Monitor::m_hStdOutRead = NULL;
HANDLE GUI_WIN_Monitor::m_hStdErrRead = NULL;

HANDLE GUI_WIN_Monitor::m_hEventADBDataReady = NULL;

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

#define BUFSIZE 4096 

HANDLE CreateChildProcess(const char szCmdline[])
{
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;

    BOOL bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Wr;
    siStartInfo.hStdOutput = GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Wr;
    siStartInfo.hStdInput = 0;//GUI_WIN_Monitor::g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USEFILLATTRIBUTE | STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow = SW_HIDE;

    bSuccess = CreateProcessA(NULL,
        (LPSTR)szCmdline,     // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION

    return piProcInfo.hProcess;
}

void WriteToPipe(void)
{
    CHAR chBuf[BUFSIZE] = { 0, };
    DWORD dwRead, dwWritten;
    BOOL bSuccess = FALSE;

    for (;;)
    {
        bSuccess = ReadFile(GUI_WIN_Monitor::g_hInputFile, chBuf, BUFSIZE, &dwRead, NULL);

        if (!bSuccess || dwRead == 0) break;

        bSuccess = WriteFile(GUI_WIN_Monitor::g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);

        if (!bSuccess) break;
    }

    CloseHandle(GUI_WIN_Monitor::g_hChildStd_IN_Wr);
}

DWORD WINAPI ReadFromPipe(void *pData)
{
    GUI_WIN_Monitor *_this = (GUI_WIN_Monitor *)pData;

    if (_this != NULL) {

        DWORD dwRead;

        CHAR chBuf[512], line[BUFSIZE], newline[BUFSIZE];

        BOOL bSuccess = FALSE;

        int nLinePos = 0, nCur = 0;

        for (;;)
        {
            DWORD nPos = 0;

            bSuccess = ReadFile(GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Rd, chBuf, 512, &dwRead, NULL);

            if (!bSuccess) {

                break;
            }

            BOOL bSend = FALSE;

            while (nPos < dwRead) {

                BOOL bSpace = FALSE;

                while (nPos < dwRead) {

                    CHAR a = *(chBuf + nPos);

                    if (a == 0x20) {

                        if (bSpace) {
                            nPos++;
                            continue;
                        }

                        bSpace = TRUE;
                    }
                    else {

                        bSpace = FALSE;
                    }

                    line[nLinePos++] = a;

                    nPos++;

                    if (*(chBuf + nPos - 1) == '\n') break;
                }

                if (*(chBuf + nPos - 1) == '\n') {

                    static char prev = 0;

                    *(line + nLinePos) = 0;

                    std::string aline(line);

                    std::vector<std::string> fields;

                    Split(aline, 0x20, fields);

                    if (fields.size() > 4) {

                        static int stackdepth = 0;

                        static char prevc = 0;

                        char cType = fields[4][0];

                        if (cType == 'E' || cType == 'W' || cType == 'D' || cType == 'V' || cType == 'I') {

                            if (prevc != cType) {
                            }

                            prevc = cType;

                            if (prev != 0 && prev != *(line)) {
                            }

                            prev = *(line);

                            bool bOut = false;

                            switch (cType) {
                            case 'E':
                            case 'W':
                                break;
                            case 'D':
                            case 'V':
                            case 'I':
                                bOut = true;
                                break;
                            }

                            if (bOut && fields.size() > 0) {

                                nCur = 0;

                                for (unsigned int i = 5; i < fields.size(); i++) {
                                    if (i > 5) {
                                        nCur += ::sprintf_s(newline + nCur, BUFSIZE - nCur, " ");
                                    }
                                    nCur += ::sprintf_s(newline + nCur, BUFSIZE - nCur, "%s", fields[i].c_str());
                                }

                                _this->OutputWinDebugString(newline);

                                bSend = true;
                            }

                            if (fields.size() > 6) {
                                if (fields[6][0] == '>') stackdepth++;
                            }

                            if (fields.size() > 6) {
                                if (fields[6][0] == '<') stackdepth--;
                            }
                        }
                    }

                    nLinePos = 0;

                    nPos += nLinePos;
                }
            }

            if (bSend) SetEvent(_this->m_hEventADBDataReady);

        }
    }

    return 0;
}

static int _CreatePipes_()
{
    SECURITY_ATTRIBUTES saAttr;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    GUI_WIN_Monitor::g_hTempFile = CreateFile(TEXT("adb_out.txt"), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    DuplicateHandle(GetCurrentProcess(), GUI_WIN_Monitor::g_hTempFile, GetCurrentProcess(), &GUI_WIN_Monitor::g_hChildStd_OUT_Wr, 0, FALSE, DUPLICATE_SAME_ACCESS);

    if (!CreatePipe(&(GUI_WIN_Monitor::g_hChildStd_OUT_Rd), &(GUI_WIN_Monitor::g_hChildStd_OUT_Wr), &saAttr, 0)) {
        return 0;
    }
    if (!SetHandleInformation(GUI_WIN_Monitor::g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        return 0;
    }

    if (!CreatePipe(&(GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Rd), &(GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Wr), &saAttr, 0)) {
        return 0;
    }
    if (!SetHandleInformation(GUI_WIN_Monitor::g_hChildStd_OUT_ADB_Rd, HANDLE_FLAG_INHERIT, 0)) {
        return 0;
    }

    if (!CreatePipe(&(GUI_WIN_Monitor::g_hChildStd_IN_Rd), &(GUI_WIN_Monitor::g_hChildStd_IN_Wr), &saAttr, 0)) {
        return 0;
    }
    if (!SetHandleInformation((GUI_WIN_Monitor::g_hChildStd_IN_Wr), HANDLE_FLAG_INHERIT, 0)) {
        return 0;
    }

    return 1;
}

GUI_WIN_Monitor::GUI_WIN_Monitor() : _RdCnt(0), _WrCnt(0), m_ChildHandle(0)
{
    _CreatePipes_();

    m_ChildHandle = CreateChildProcess("adb logcat");

    ::CreateThread(NULL, 0, ReadFromPipe, this, 0, NULL);

    m_hWritingThread = ::CreateThread(NULL, 0, WinDebugWritingThread, this, 0, NULL);

    CONST WCHAR* ADB_DATA_READY = L"ADB_DATA_READY";

    m_hEventADBDataReady = ::OpenEvent(SYNCHRONIZE, FALSE, ADB_DATA_READY);

    if (m_hEventADBDataReady == NULL) {

        m_hEventADBDataReady = ::CreateEvent(NULL, FALSE, FALSE, ADB_DATA_READY);

        if (m_hEventADBDataReady == NULL) {
            GetLastError();
            return;
        }
    }
}

GUI_WIN_Monitor::~GUI_WIN_Monitor()
{
    if (m_ChildHandle) {

        TerminateProcess(m_ChildHandle, 0);
    }
}

DWORD WINAPI GUI_WIN_Monitor::WinDebugWritingThread(void *pData)
{
    GUI_WIN_Monitor *_this = (GUI_WIN_Monitor *)pData;

    while (_this != NULL) {

        DWORD dwWritten = 0;

        DWORD ret = ::WaitForSingleObject(m_hEventADBDataReady, 10);

        if (ret == WAIT_TIMEOUT) continue;

        BOOL bSuccess = WriteFile(GUI_WIN_Monitor::g_hChildStd_OUT_Wr, _this->_str[_this->_RdCnt], _this->_len[_this->_RdCnt], &dwWritten, NULL);

        if (!bSuccess) {

            continue;
        }

        _this->_len[_this->_RdCnt] = 0;

        _this->_RdCnt++; _this->_RdCnt %= 256;
    }

    return 0;
}

void GUI_WIN_Monitor::OutputWinDebugString(const char *str)
{
    size_t len = ::strlen(str);

    _len[_WrCnt] = 0;

    if (len > 0) {

        _len[_WrCnt] = ::sprintf_s(_str[_WrCnt], SIZE, "%s", str);

        _str[_WrCnt][_len[_WrCnt]] = 0;

        _WrCnt++; _WrCnt %= 256;

        SetEvent(m_hEventADBDataReady);
    }
}
