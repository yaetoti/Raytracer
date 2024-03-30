#define NOMINMAX

#include <iostream>
#include <Windows.h>
#include <ConsoleLib/Console.h>
#include <Engine/source/utils/FpsTimer.h>
#include <Engine/source/math/Vector.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Console::GetInstance()->RedirectStdHandles();

    Vec4d vec(1, 0, 0, 22);
    Vec3f vec2(0, 0, 1);

    std::wcout << vec << L'\n';
    std::wcout << vec2 << L'\n';
    std::wcout << vec.Cross(static_cast<Vec4f>(vec2)) << L'\n';

    Console::GetInstance()->Pause();

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
        0,
        wc.lpszClassName,
        L"Dragons' Rays",
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
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 24;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biSizeImage = 0;
    info.bmiHeader.biClrUsed = 0;
    info.bmiHeader.biClrImportant = 0;

    MSG message;
    FpsTimer timer;

    // FPS limiter
    //double renderTimer = 0.0;
    int targetFps = 10;
    double renderDelta = 1.0 / targetFps;
    double renderAccumulator = 0.0;
    // FPS counter
    int frames = 0;
    int lastFps = 0;
    double fpsElapsed = 0.0;

    while (true) {
        // Timer
        double elapsedTime = timer.Tick();
        fpsElapsed += elapsedTime;
        renderAccumulator = std::max(renderAccumulator + elapsedTime, renderDelta);

        // Input
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                return static_cast<int>(message.wParam);
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // Update
        static bool goUp = true;
        static float coef = 0.0f;

        if (goUp) {
            coef += renderAccumulator;
            if (coef >= 1.0f) {
                coef = 1.0f;
                goUp = false;
            }
        }
        else {
            coef -= renderAccumulator;
            if (coef <= 0.0f) {
                coef = 0.0f;
                goUp = true;
            }
        }

        // FPS counter
        if (fpsElapsed >= 1.0) {
            fpsElapsed = 0.0;
            lastFps = frames;
            frames = 0;
            Console::GetInstance()->WPrintF(L"FPS: %d\n", lastFps);
        }

        // Render
        //renderTimer = 0.0;
        // FPS counter
        ++frames;

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                framebuffer[(i * width + j) * 3 + 0] = coef * 255;
                framebuffer[(i * width + j) * 3 + 1] = coef * 178;
                framebuffer[(i * width + j) * 3 + 2] = coef * 128;
            }
        }

        StretchDIBits(dc, 0, 0, 1920, 1080, 0, 0, width, height, framebuffer, &info, DIB_RGB_COLORS, SRCCOPY);

        renderAccumulator = 0.0;

        // Sleep
        while (FpsTimer::Duration(FpsTimer::Clock::now() - timer.GetCurrentTimePoint()).count() < renderDelta) {
            std::this_thread::yield();
        }
    }

    UnregisterClassW(wc.lpszClassName, GetModuleHandleW(nullptr));
    Console::GetInstance()->Pause();

    return 0;
}
