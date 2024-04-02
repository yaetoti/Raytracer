#pragma once

#include <Windows.h>
#include <random>
#include <ConsoleLib/Console.h>
#include <Engine/source/utils/FpsTimer.h>
#include <Engine/source/math/Vector.h>
#include <Engine/source/render/Framebuffer.h>
#include <Engine/source/window/Window.h>
#include <Engine/source/math/HitableList.h>
#include <Engine/source/render/AlbedoMaterial.h>
#include <Engine/source/render/LambertianMaterial.h>
#include <Engine/source/render/MetalMaterial.h>

struct Application final {
    Application()
        : gen(rd()), framebuffer(800, 600) {
    }

    Vec3F Color(const Ray3F& ray, int depth) {
        HitRecord<float> record;
        if (hitables.Hit(ray, 0.01f, std::numeric_limits<float>::max(), record)) {
            Ray3F scattered;
            Vec3F attenuation;

            if (depth < 10 && record.material->Scatter(ray, record, scattered, attenuation)) {
                return attenuation * Color(scattered, depth + 1);
            }

            return Vec3F(0.0f);
        }

        Vec3F skyColor(0.5f, 0.7f, 1.0f);
        float t = (ray.direction.y + 1) * 0.5f;
        return skyColor * t + (1.0f - t) * Vec3F(1.0f);
    }

    int runMainLoop() {
        hitables.Add(sphere1);
        hitables.Add(sphere2);
        hitables.Add(sphere3);

        // TODO Make window name a parameter
        Window window(800, 600);
        window.GetSizeDispatcher()->AddListener([this](WORD width, WORD height) {
            framebuffer.Resize(width / 2, height / 2);
        });

        window.CreateResources();
        window.Show(SW_SHOW);

        MSG message;
        FpsTimer timer;

        // FPS limiter
        //double renderTimer = 0.0;
        int targetFps = 120;
        double renderDelta = 1.0 / targetFps;
        double elapsedTime = 0.0;
        // FPS counter
        int frames = 0;
        int lastFps = 0;
        double fpsElapsed = 0.0;

        bool wDown = false;
        bool aDown = false;
        bool sDown = false;
        bool dDown = false;
        bool rmbDown = false;
        int x = 0;
        int y = 0;
        int lastX = 0;
        int lastY = 0;

        window.GetKeyDispatcher()->AddListener([&wDown, &aDown, &sDown, &dDown](WORD vkCode, bool isPressed, bool wasPressed, WORD repeatCount, WORD scanCode) {
            if (vkCode == 'A') {
                aDown = isPressed;
            }
            else if (vkCode == 'D') {
                dDown = isPressed;
            }
            else if (vkCode == 'W') {
                wDown = isPressed;
            }
            else if (vkCode == 'S') {
                sDown = isPressed;
            }
        });

        window.GetMouseButtonDispatcher()->AddListener([&rmbDown](bool isLeft, bool isPressed) {
            if (!isLeft) {
                rmbDown = isPressed;
            }
        });

        window.GetMouseMoveDispatcher()->AddListener([&x, &y, &lastX, &lastY](int newX, int newY) {
            lastX = x;
            lastY = y;
            x = newX;
            y = newY;
        });

        while (true) {
            // Timer
            elapsedTime = std::max(timer.Tick(), renderDelta);

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

            if (aDown) {
                sphere2->center -= Vec3F(1.0f * elapsedTime, 0, 0);
            }
            if (dDown) {
                sphere2->center -= Vec3F(-1.0f * elapsedTime, 0, 0);
            }
            if (wDown) {
                sphere2->center -= Vec3F(0, -1.0f * elapsedTime, 0);
            }
            if (sDown) {
                sphere2->center -= Vec3F(0, 1.0f * elapsedTime, 0);
            }

            if (rmbDown) {
                sphere2->center -= Vec3F((lastX - x) * elapsedTime, -(lastY - y) * elapsedTime, 0);
                lastX = x;
                lastY = y;
            }

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

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    std::shared_ptr<SphereF> sphere1 = std::make_shared<SphereF>(
        Vec3F(0, 0, -2), 0.5f,
        std::make_unique<MetalMaterial<float>>(
            Vec3F(0.98f, 0.98f, 0.98f)));
    std::shared_ptr<SphereF> sphere2 = std::make_shared<SphereF>(
        Vec3F(0.8f, 0.2f, -1.0f), 0.4f,
        std::make_unique<AlbedoMaterial<float>>(
            Vec3F(0.28f, 1.0f, 0.5f)));
    std::shared_ptr<SphereF> sphere3 = std::make_shared<SphereF>(
        Vec3F(0, -10.5f, -2.0f), 10.0f,
        std::make_unique<LambertianMaterial<float>>(
            Vec3F(1.0f, 0.28f, 0.5f),
            dis, gen));
    HitableList<float> hitables;
    Framebuffer framebuffer;
};
