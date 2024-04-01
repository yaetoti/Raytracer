#define NOMINMAX

#include <random>
#include <iostream>
#include <Windows.h>
#include <ConsoleLib/Console.h>
#include <Engine/source/utils/FpsTimer.h>
#include <Engine/source/math/Vector.h>
#include <Engine/source/render/Framebuffer.h>
#include <Engine/source/window/Window.h>

#include "Engine/source/utils/TDispatcher.h"

SphereF sphere(Vec3F(0, 0, -180), 200);
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

void f1(WORD w1, WORD w2) {
    std::cout << w1 << ' ' << w2 << '\n';
}

void f2(WORD w1, WORD w2) {
    std::cout << "ABOBA\n";
}

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Console::GetInstance()->RedirectStdHandles();
    Console::GetInstance()->WPrintF(L"Çהאנמגא, קונעט!\n");

    TDispatcher<WORD, WORD> d1;
    auto e1 = d1.AddListener(f1);
    auto e2 = d1.AddListener(f2);
    auto e3 = d1.AddListener(f1);

    d1.RemoveListener(e1);
    d1.Dispatch(1, 2);

    Console::GetInstance()->Pause();

    Window window;
    window.CreateResources();
    window.Show(SW_SHOW);

    MSG message;
    FpsTimer timer;

    // FPS limiter
    //double renderTimer = 0.0;
    int targetFps = 120;
    double renderDelta = 1.0 / targetFps;
    // FPS counter
    int frames = 0;
    int lastFps = 0;
    double fpsElapsed = 0.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis;

    while (true) {
        // Timer
        double elapsedTime = std::max(timer.Tick(), renderDelta);

        // FPS counter
        fpsElapsed += elapsedTime;
        if (fpsElapsed >= 1.0) {
            fpsElapsed = 0.0;
            lastFps = frames;
            frames = 0;
            Console::GetInstance()->WPrintF(L"FPS: %d\n", lastFps);
        }

        // Input
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                return static_cast<int>(message.wParam);
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // FPS counter
        ++frames;

        /// Render
        Vec3I skyColor(128, 178, 255);
        Vec3I sphereColor(70, 128, 255);

        Vec3F unit(1, 1, 0);
        int rays = 10;
        float raysScale = 1.0f / rays;

        int width = static_cast<int>(framebuffer.GetWidth());
        int height = static_cast<int>(framebuffer.GetHeight());
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

        window.Blit(framebuffer);

        // Sleep
        while (FpsTimer::Duration(FpsTimer::Clock::now() - timer.GetCurrentTimePoint()).count() < renderDelta) {
            std::this_thread::yield();
        }
    }
}
