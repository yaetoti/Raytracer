#pragma once

#include <Windows.h>
#include "../render/Framebuffer.h"
#include "Engine/source/utils/TDispatcher.h"

struct Window final {
    using SizeDispatcher = TDispatcher<WORD, WORD>;

    Window()
    : m_hWnd(nullptr), m_width(800), m_height(600) {
    }

    ~Window() {
        DiscardResources();
    }

    bool CreateResources() {
        WNDCLASSEX wc = { };
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"DlRaytracer";
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
        wc.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszMenuName = nullptr;
        if (!RegisterClassExW(&wc)) {
            return false;
        }

        RECT windowRect { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
        AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, 0, 0);

        m_hWnd = CreateWindowExW(
            0,
            wc.lpszClassName,
            L"Dragons' Rays",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
            nullptr, nullptr, GetModuleHandleW(nullptr), this);
        if (m_hWnd == nullptr) {
            return false;
        }

        return true;
    }

    void DiscardResources() {
        UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
    }

    void Show(int nCmdShow) const {
        ShowWindow(m_hWnd, nCmdShow);
        UpdateWindow(m_hWnd);
    }

    void Blit(const Framebuffer& framebuffer) const {
        BITMAPINFO info = { };
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = static_cast<LONG>(framebuffer.GetWidth());
        info.bmiHeader.biHeight = static_cast<LONG>(framebuffer.GetHeight());
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(m_hWnd);
        StretchDIBits(dc,
                      0, 0, static_cast<int>(m_width), static_cast<int>(m_height),
                      0, 0, static_cast<int>(framebuffer.GetWidth()), static_cast<int>(framebuffer.GetHeight()),
                      framebuffer.GetData(), &info, DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(m_hWnd, dc);
    }

    // Dispatchers
    SizeDispatcher* GetSizeDispatcher() {
        return &m_sizeDispatcher;
    }

    // Handlers
    void OnResize(WORD width, WORD height) {
        m_width = width;
        m_height = height;
        m_sizeDispatcher.Dispatch(width, height);
    }

    void OnKey(WORD vkCode, BYTE scanCode, bool isPressed, bool wasPressed, WORD repeatCount) {
        
    }

    // TODO KeyDown
    // TODO KeyUp
    // TODO SysKeyDown
    // TODO SysKeyUp
    // TODO MouseLeftDown
    // TODO MouseLeftUp
    // TODO MouseRightDown
    // TODO MouseRightUp

private:
    HWND m_hWnd;
    size_t m_width;
    size_t m_height;
    SizeDispatcher m_sizeDispatcher;
    inline const static wchar_t* kClassName = L"DlRaycasterWindow";

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
        
        switch (msg) {
        case WM_CREATE: {
            CREATESTRUCTW* data = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data->lpCreateParams));
            return 0;
        }
        case WM_SIZE: {
            window->OnResize(LOWORD(lParam), HIWORD(lParam));
            return 0;
        }
        default: 
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }
    }
};
