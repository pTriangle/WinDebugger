#include "WinDebugMonitor.h"

CWinDebugMonitor::CWinDebugMonitor()
{
    if (Initialize() != 0) {
        ::OutputDebugStringW(L"CWinDebugMonitor::Initialize failed.\n");
    }
}

CWinDebugMonitor::~CWinDebugMonitor()
{
    Unintialize();
}

DWORD CWinDebugMonitor::Initialize()
{
    DWORD errorCode = 0;
    BOOL bSuccessful = FALSE;

    SetLastError(0);

    CONST WCHAR* DBWIN_MUTEX = L"DBWinMutex";
    CONST WCHAR* DBWIN_BUFFER_READY = L"DBWIN_BUFFER_READY";
    CONST WCHAR* DBWIN_DATA_READY = L"DBWIN_DATA_READY";
    CONST WCHAR* DBWIN_BUFFER = L"DBWIN_BUFFER";

    m_hDBWinMutex = ::OpenMutex(SYNCHRONIZE | READ_CONTROL, FALSE, DBWIN_MUTEX);

    if (m_hDBWinMutex == NULL) {
        errorCode = GetLastError();
        return errorCode;
    }

    m_hEventBufferReady = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, DBWIN_BUFFER_READY);

    if (m_hEventBufferReady == NULL) {
        m_hEventBufferReady = ::CreateEvent(NULL, FALSE, TRUE, DBWIN_BUFFER_READY);
        if (m_hEventBufferReady == NULL) {
            errorCode = GetLastError();
            return errorCode;
        }
    }

    m_hEventDataReady = ::OpenEvent(SYNCHRONIZE, FALSE, DBWIN_DATA_READY);

    if (m_hEventDataReady == NULL) {
        m_hEventDataReady = ::CreateEvent(NULL, FALSE, FALSE, DBWIN_DATA_READY);
        if (m_hEventDataReady == NULL) {
            errorCode = GetLastError();
            return errorCode;
        }
    }

    m_hDBMonBuffer = ::OpenFileMapping(FILE_MAP_READ, FALSE, DBWIN_BUFFER);

    if (m_hDBMonBuffer == NULL) {
        m_hDBMonBuffer = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct dbwin_buffer), DBWIN_BUFFER);
        if (m_hDBMonBuffer == NULL) {
            errorCode = GetLastError();
            return errorCode;
        }
    }

    m_pDBBuffer = (struct dbwin_buffer *)::MapViewOfFile(m_hDBMonBuffer, SECTION_MAP_READ, 0, 0, 0);

    if (m_pDBBuffer == NULL) {
        errorCode = GetLastError();
        return errorCode;
    }

    m_bWinDebugMonStopped = FALSE;
    m_hWinDebugMonitorThread = ::CreateThread(NULL, 0, WinDebugMonitorThread, this, 0, NULL);

    if (m_hWinDebugMonitorThread == NULL) {
        m_bWinDebugMonStopped = TRUE;
        errorCode = GetLastError();
        return errorCode;
    }

    bSuccessful = ::SetPriorityClass(::GetCurrentProcess(), IDLE_PRIORITY_CLASS);
    bSuccessful = ::SetThreadPriority(m_hWinDebugMonitorThread, THREAD_PRIORITY_BELOW_NORMAL);

    return errorCode;
}

void CWinDebugMonitor::Unintialize()
{
    if (m_hWinDebugMonitorThread != NULL) {
        m_bWinDebugMonStopped = TRUE;
        ::WaitForSingleObject(m_hWinDebugMonitorThread, INFINITE);
    }

    if (m_hDBWinMutex != NULL) {
        ::CloseHandle(m_hDBWinMutex);
        m_hDBWinMutex = NULL;
    }

    if (m_hDBMonBuffer != NULL) {
        ::UnmapViewOfFile(m_pDBBuffer);
        ::CloseHandle(m_hDBMonBuffer);
        m_hDBMonBuffer = NULL;
    }

    if (m_hEventBufferReady != NULL) {
        ::CloseHandle(m_hEventBufferReady);
        m_hEventBufferReady = NULL;
    }

    if (m_hEventDataReady != NULL) {
        ::CloseHandle(m_hEventDataReady);
        m_hEventDataReady = NULL;
    }

    m_pDBBuffer = NULL;
}

DWORD CWinDebugMonitor::WinDebugMonitorProcess()
{
    DWORD ret = ::WaitForSingleObject(m_hEventDataReady, TIMEOUT_WIN_DEBUG);
    if (ret == WAIT_OBJECT_0) {
        OutputWinDebugString(m_pDBBuffer->data);
        SetEvent(m_hEventBufferReady);
    }
    return ret;
}

DWORD WINAPI CWinDebugMonitor::WinDebugMonitorThread(void *pData)
{
    CWinDebugMonitor *_this = (CWinDebugMonitor *)pData;
    if (_this != NULL) {
        while (!_this->m_bWinDebugMonStopped) {
            _this->WinDebugMonitorProcess();
        }
    }
    return 0;
}
