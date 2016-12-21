#pragma once
#if defined(__cplusplus)
extern "C" {
#endif
    void OutputMessageA(const char  *format, ...);
    void OutputMessageW(const wchar_t *format, ...);
    void OutputMessageCallback(const char *format, ...);
#if defined(__cplusplus)
}
#endif
