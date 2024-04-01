#define NOMINMAX

#include <random>
#include <iostream>
#include <Windows.h>
#include <ConsoleLib/Console.h>
#include <Engine/source/utils/FpsTimer.h>
#include <Engine/source/math/Vector.h>
#include <Engine/source/render/Framebuffer.h>
#include <Engine/source/window/Window.h>

SphereF sphere(Vec3F(0, 0, -2), 0.5f);
SphereF sphere1(Vec3F(0.8f, 0.8f, -1.0f), 0.4f);
SphereF sphere2(Vec3F(0, -10.5f, -2.0f), 10.0f);
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

//Vec3I Color(const Ray3F& ray) {
//    Vec3I skyColor(128, 178, 255);
//    Vec3I sphereColor(70, 128, 255);
//
//    if (sphere.Hit(ray)) {
//        return sphereColor;
//    }
//    else {
//        return skyColor;
//    }
//}

bool Scatter(const Ray3F& ray, const HitRecord<float>& record, Ray3F& scattered, Vec3F& attenuation) {
    Vec3F reflected = ray.direction.Reflect(record.normal);
    scattered = Ray3F(record.point, reflected);
    attenuation = Vec3F(0.95f);
    return reflected.Dot(record.normal) > 0;
}

Vec3F Color(const Ray3F& ray, int depth) {
    Vec3F skyColor(0.5f, 0.7f, 1.0f);
    Vec3F sphereColor(0.28f, 0.5f, 1.0f);
    Vec3F sphere1Color(0.28f, 1.0f, 0.5f);
    Vec3F sphere2Color(1.0f, 0.28f, 0.5f);

    HitRecord<float> record;
    if (sphere1.Hit(ray, 0.01f, std::numeric_limits<float>::max(), record)) {
        return sphere1Color * (ray.direction.y + 1) * 0.5f;
    }

    //if (sphere2.Hit(ray, 0.01f, std::numeric_limits<float>::max(), record)) {
    //    return sphere2Color * (ray.origin.x + 1) * 0.5f;
    //}

    if (sphere.Hit(ray, 0.01f, std::numeric_limits<float>::max(), record)) {
        Ray3F scattered;
        Vec3F attenuation;

        if (depth < 3 && Scatter(ray, record, scattered, attenuation)) {
            return attenuation * Color(scattered, depth + 1);
        } else {
            return Vec3F(0.0f);
        }
    }

    float t = (ray.direction.y + 1) * 0.5f;
    return skyColor * t + (1.0f - t) * Vec3F(1.0f);
}

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Console::GetInstance()->RedirectStdHandles();
    Console::GetInstance()->WPrintF(L"Çהאנמגא, קונעט!\n");


    Window window;
    window.GetSizeDispatcher()->AddListener([](WORD width, WORD height) {
        framebuffer.Resize(width / 1, height / 1);
    });

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
        int rays = 10;
        float raysScale = 1.0f / rays;

        int width = static_cast<int>(framebuffer.GetWidth());
        int height = static_cast<int>(framebuffer.GetHeight());
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        float scaleX = std::min(aspectRatio, 1.0f);
        float scaleY = std::min(1 / aspectRatio, 1.0f);
        Vec3F unit(1.0f / width, 1.0f / height, 0);

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                //float x = j - width / 2;
                //float y = i - height / 2;
                //float x = 2.0f * j / width - 1.0f;
                //float y = 2.0f * i / height - 1.0f;
                float x = (2.0f * j / width - 1.0f) * scaleX;
                float y = (2.0f * i / height - 1.0f) * scaleY;

                
                Vec3F resultColor;
                for (int k = 0; k < rays; ++k) {
                    Ray3F ray(Vec3F(x, y, 0) + unit * dis(gen), Vec3F(0, 0, -1));
                    resultColor += Color(ray, 0);
                }

                resultColor *= raysScale;
                resultColor *= 255.0f;
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
