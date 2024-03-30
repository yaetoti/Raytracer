#define NOMINMAX

#include <random>
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

    SphereF sphere(Vec3F(0, 0, -500), 200);
    //Ray3F ray(Vec3F(0, 0, 0), Vec3F(0, 0, -1).Normalized());

    //std::wcout << ray << L'\n';
    //std::wcout << ray.AtParameter(30) << L'\n';
    std::wcout << sphere << L'\n';
    //std::wcout << sphere.Hit(ray) << L'\n';

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

    int width = 500;
    int height = 500;

    HWND hWnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"Dragons' Rays",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (hWnd == nullptr) {
        return 1;
    }

    //ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

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
    int targetFps = 120;
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

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis;

        Vec3F unit(1, 1, 0);

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                //float x = j / static_cast<float>(height) * 2 - 1;
                //float y = i / static_cast<float>(height) * 2 - 1;
                float x = j - width / 2;
                float y = i - height / 2;

                int rays = 20;
                Vec3I resultColor;
                for (int k = 0; k < rays; ++k) {
                    Ray3F ray(Vec3F(x, y, 0) + unit * dis(gen), Vec3F(0, 0, -1).Normalized());
                    if (sphere.Hit(ray)) {
                        resultColor += sphereColor;
                    } else {
                        resultColor += skyColor;
                    }
                }

                resultColor /= rays;
                framebuffer[(i * width + j) * 3 + 0] = resultColor.b;
                framebuffer[(i * width + j) * 3 + 1] = resultColor.g;
                framebuffer[(i * width + j) * 3 + 2] = resultColor.r;

                // Ray3F ray(Vec3F(x, y, 0), Vec3F(0, 0, -1).Normalized());



                //if (sphere.Hit(ray)) {
                //    framebuffer[(i * width + j) * 3 + 0] = sphereColor.b;
                //    framebuffer[(i * width + j) * 3 + 1] = sphereColor.g;
                //    framebuffer[(i * width + j) * 3 + 2] = sphereColor.r;
                //} else {
                //    framebuffer[(i * width + j) * 3 + 0] = skyColor.b;
                //    framebuffer[(i * width + j) * 3 + 1] = skyColor.g;
                //    framebuffer[(i * width + j) * 3 + 2] = skyColor.r;
                //}
            }
        }

        StretchDIBits(dc, 0, 0, width, height, 0, 0, width, height, framebuffer, &info, DIB_RGB_COLORS, SRCCOPY);

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
