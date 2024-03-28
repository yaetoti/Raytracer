#include <Windows.h>
#include <ConsoleLib/Console.h>

#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Console::GetInstance()->RedirectStdHandles();
    Console::GetInstance()->WPrintF(L"Çהאנמגא, קונעט!\n");

    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"RaytracerClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszMenuName = nullptr;

    if (!RegisterClassExW(&wc)) {
        // TODO error
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW,
        wc.lpszClassName,
        L"Dragons Ray",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1366, 768,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (hWnd == nullptr) {
        return 1;
    }

    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hWnd);

    int width = 800;
    int height = 600;

    BYTE* framebuffer = new BYTE[width * height * 3];
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            framebuffer[(i * width + j) * 3 + 0] = 255;
            framebuffer[(i * width + j) * 3 + 1] = 178;
            framebuffer[(i * width + j) * 3 + 2] = 128;
        }
    }

    HDC dc = GetDC(hWnd);
    BITMAPINFO info = { };
    info.bmiHeader.biSize = sizeof(BITMAPINFO);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 24;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biSizeImage = 0;
    info.bmiHeader.biClrUsed = 0;
    info.bmiHeader.biClrImportant = 0;

    MSG message;

    while (true) {
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                return static_cast<int>(message.wParam);
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);

            StretchDIBits(dc, 0, 0, width, height, 0, 0, width, height, framebuffer, &info, DIB_RGB_COLORS, SRCCOPY);
        }
    }

    UnregisterClassW(wc.lpszClassName, GetModuleHandleW(nullptr));
    Console::GetInstance()->Pause();

    return 0;
}
