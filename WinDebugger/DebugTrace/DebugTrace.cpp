#include <windows.h>
#include <stdio.h>

#include "DebugTrace.h"

extern "C" {
#define TRACE_BUFFER_SIZE 1024
    void OutputMessageCallback(const char *format, ...)
    {
        va_list arguments; va_start(arguments, format);
        char buffer[TRACE_BUFFER_SIZE + 2];
        buffer[0] = 'c';
        buffer[1] = 'b';
        _vsnprintf_s(buffer + 2, TRACE_BUFFER_SIZE, TRACE_BUFFER_SIZE, format, arguments);
        ::OutputDebugStringA(buffer);
        ::OutputDebugStringA("\n");
        va_end(arguments);
    }
    void OutputMessageA(const char *format, ...)
    {
        va_list arguments; va_start(arguments, format);
        char buffer[TRACE_BUFFER_SIZE + 2];
        buffer[0] = 'm';
        buffer[1] = 'y';
        _vsnprintf_s(buffer + 2, TRACE_BUFFER_SIZE, TRACE_BUFFER_SIZE, format, arguments);
        ::OutputDebugStringA(buffer);
        ::OutputDebugStringA("\n");
        va_end(arguments);
    }
    void OutputMessageW(const wchar_t *format, ...)
    {
        va_list arguments; va_start(arguments, format);
        wchar_t buffer[TRACE_BUFFER_SIZE + 2];
        buffer[0] = 'm';
        buffer[1] = 'y';
        _vsnwprintf_s(buffer + 2, TRACE_BUFFER_SIZE, TRACE_BUFFER_SIZE, format, arguments);
        ::OutputDebugStringW(buffer);
        ::OutputDebugStringW(L"\n");
        va_end(arguments);
    }
}
