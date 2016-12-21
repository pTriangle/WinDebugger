#include "Font.h"

Font::Font(const WCHAR *pFontName, const int nSize) :_hFont_(0)
{
    _hFont_ = 0;
    Initialize(_LogFontW_);
    if (pFontName && (*pFontName)) ::wcscpy_s((_LogFontW_.lfFaceName), LF_FACESIZE, pFontName);
    HDC hdc = GetDC(0);
    _LogFontW_.lfHeight = ((nSize * GetDeviceCaps(hdc, LOGPIXELSY)) / 72);
    _hFont_ = CreateFontIndirectW(&_LogFontW_);
    HGDIOBJ oldFont = SelectObject(hdc, _hFont_);
    GetTextFaceW(hdc, LF_FACESIZE, _LogFontW_.lfFaceName);
    SelectObject(hdc, oldFont);
    ReleaseDC(0, hdc);
}

Font::~Font()
{
    if (_hFont_) DeleteObject(_hFont_);
}

void Font::Bold()
{
    if (_hFont_) DeleteObject(_hFont_);
    _LogFontW_.lfWeight = FW_BOLD;
    HDC hdc = GetDC(0);
    _hFont_ = CreateFontIndirectW(&_LogFontW_);
    HGDIOBJ oldFont = SelectObject(hdc, _hFont_);
    GetTextFaceW(hdc, LF_FACESIZE, _LogFontW_.lfFaceName);
    SelectObject(hdc, oldFont);
    ReleaseDC(0, hdc);
}

void Font::Initialize(LOGFONTW& logFont)
{
    HDC hdc = GetDC(0);
    logFont.lfHeight = -((14 * GetDeviceCaps(hdc, LOGPIXELSY)) / 72);
    logFont.lfWidth = 0;
    logFont.lfEscapement = 0;
    logFont.lfOrientation = 0;
    logFont.lfWeight = FW_NORMAL;
    logFont.lfItalic = FALSE;
    logFont.lfUnderline = FALSE;
    logFont.lfStrikeOut = FALSE;
    logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont.lfQuality = ANTIALIASED_QUALITY;
    logFont.lfPitchAndFamily = DEFAULT_PITCH;
    logFont.lfCharSet = DEFAULT_CHARSET;
    ReleaseDC(0, hdc);
    ::wcscpy_s((logFont.lfFaceName), LF_FACESIZE, L"Segoe UI Symbol");
}
