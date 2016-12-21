#pragma once
#define WIN32_LEAN_AND_MEAN
#include<windows.h>

class Font {
public:
    Font(const WCHAR *pFontName = L"", const int nSize = 24);
    virtual~Font();
    inline HFONT GethFont()
    {
        return _hFont_;
    }
    void Bold();
    inline const int GetHieght()
    {
        return _LogFontW_.lfHeight;
    }
protected:
    void Initialize(LOGFONTW& logFont);
    LOGFONTW	_LogFontW_;
    HFONT		_hFont_;
};
