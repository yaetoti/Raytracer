#pragma once

struct Window final {
    
private:
    HWND m_hWnd;
    const static wchar_t* kClassName = L"DlRaycasterWindow";
};