#pragma once

#include <Windows.h>
#include "../render/Framebuffer.h"
#include "Engine/source/utils/EventDispatcher.h"
#include "events/KeyWindowEvent.h"
#include "events/MouseButtonWindowEvent.h"
#include "events/MouseMoveWindowEvent.h"
#include "events/ResizeWindowEvent.h"
#include "events/WindowEvent.h"

struct Window final {
    explicit Window(const wchar_t* title, size_t width, size_t height)
    : m_hWnd(nullptr), m_width(width), m_height(height), m_title(title) {
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
            m_title,
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
    EventDispatcher<WindowEvent>* GetDispatcher() {
        return &m_dispatcher;
    }

    // Handlers
    void OnResize(WORD width, WORD height) {
        m_width = width;
        m_height = height;

        ResizeWindowEvent event(width, height);
        m_dispatcher.Dispatch(&event);
    }

    void OnKey(WORD vkCode, bool isPressed, bool wasPressed, WORD repeatCount, WORD scanCode) {
        KeyWindowEvent event(vkCode, isPressed, wasPressed, repeatCount, scanCode);
        m_dispatcher.Dispatch(&event);
    }

    void OnMouseButton(bool isLeft, bool isPressed) {
        MouseButtonWindowEvent event(isLeft, isPressed);
        m_dispatcher.Dispatch(&event);
    }

    void OnMouseMove(int x, int y) {
        MouseMoveWindowEvent event(x, y);
        m_dispatcher.Dispatch(&event);
    }

private:
    HWND m_hWnd;
    size_t m_width;
    size_t m_height;
    EventDispatcher<WindowEvent> m_dispatcher;
    const wchar_t* m_title;
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
        case WM_KEYDOWN: // fallthrough
        case WM_KEYUP: // fallthrough
        case WM_SYSKEYDOWN: // fallthrough
        case WM_SYSKEYUP: {
            WORD vkCode = LOWORD(wParam);
            WORD keyFlags = HIWORD(lParam);
            WORD scanCode = LOBYTE(keyFlags);
            if ((keyFlags & KF_EXTENDED) == KF_EXTENDED) {
                scanCode = MAKEWORD(scanCode, 0xE0);
            }
            BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;
            WORD repeatCount = LOWORD(lParam);
            BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;

            window->OnKey(vkCode, !isKeyReleased, wasKeyDown, repeatCount, scanCode);
            return 0;
        }
        case WM_LBUTTONDOWN:
            window->OnMouseButton(true, true);
            return 0;
        case WM_LBUTTONUP:
            window->OnMouseButton(true, false);
            return 0;
        case WM_RBUTTONDOWN:
            window->OnMouseButton(false, true);
            return 0;
        case WM_RBUTTONUP:
            window->OnMouseButton(false, false);
            return 0;
        case WM_MOUSEMOVE:
            window->OnMouseMove(LOWORD(lParam), HIWORD(lParam));
            return 0;
        default: 
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }
    }
};
