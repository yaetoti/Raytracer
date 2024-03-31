#define NOMINMAX

#include <random>
#include <iostream>
#include <Windows.h>
#include <ConsoleLib/Console.h>
#include <Engine/source/utils/FpsTimer.h>
#include <Engine/source/math/Vector.h>
#include <Engine/source/render/Framebuffer.h>

Framebuffer framebuffer(800, 600);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_SIZE) {
        WORD width = LOWORD(lParam);
        WORD height = HIWORD(lParam);
        framebuffer.Resize(width, height);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Console::GetInstance()->RedirectStdHandles();

    SphereF sphere(Vec3F(0, 0, -180), 200);
    std::wcout << sphere << L'\n';

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

    int width = static_cast<int>(framebuffer.GetWidth());
    int height = static_cast<int>(framebuffer.GetHeight());

    RECT windowRect {0, 0, width, height};
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, 0, 0);
    HWND hWnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"Dragons' Rays",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (hWnd == nullptr) {
        return 1;
    }

    //ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    MSG message;
    FpsTimer timer;

    // FPS limiter
    //double renderTimer = 0.0;
    int targetFps = 120;
    double renderDelta = 1.0 / targetFps;
    double renderAccumulator = 0.0;
    // FPS counter
    int frames = 0;
    int lastFps = 0;
    double fpsElapsed = 0.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis;

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
        static float coef = 0.5f;

        if (goUp) {
            coef += renderAccumulator / 4;
            if (coef >= 1.0f) {
                coef = 1.0f;
                goUp = false;
            }
        }
        else {
            coef -= renderAccumulator / 4;
            if (coef <= 0.5f) {
                coef = 0.5f;
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

        Vec3I skyColor(128, 178, 255);
        Vec3I sphereColor(70, 128, 255);

        Vec3F unit(1, 1, 0);
        int rays = 10;
        float raysScale = 1.0f / rays;

        width = static_cast<int>(framebuffer.GetWidth());
        height = static_cast<int>(framebuffer.GetHeight());

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                float x = j - width / 2;
                float y = i - height / 2;

                
                Vec3F resultColor;
                for (int k = 0; k < rays; ++k) {
                    Ray3F ray(Vec3F(x, y, 0) + unit * dis(gen), Vec3F(0, 0, -1));
                    if (sphere.Hit(ray)) {
                        resultColor += sphereColor;
                    } else {
                        resultColor += skyColor;
                    }
                }

                resultColor *= raysScale;
                framebuffer.SetPixel(j, i, static_cast<BYTE>(resultColor.r), static_cast<BYTE>(resultColor.g), static_cast<BYTE>(resultColor.b));
            }
        }

        BITMAPINFO info = { };
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(hWnd);
        StretchDIBits(dc, 0, 0, width, height, 0, 0, width, height, framebuffer.GetData(), &info, DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(hWnd, dc);

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
