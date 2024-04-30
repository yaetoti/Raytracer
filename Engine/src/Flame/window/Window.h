#pragma once

#include "events/WindowEvent.h"
#include "Flame/render/Framebuffer.h"
#include "Flame/utils/EventDispatcher.h"
#include "InputSystem.h"

#include <functional>
#include <unordered_map>
#include <Windows.h>

namespace Flame {
  struct Window final {
    explicit Window(const wchar_t* title, uint32_t width, uint32_t height, uint32_t resolutionDivisor);
    ~Window();

    bool CreateResources();
    void DiscardResources();

    void Show(int nCmdShow) const;
    void Blit() const;

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetResolutionDivisor() const;
    Framebuffer& GetFramebuffer();
    InputSystem& GetInputSystem();
    EventDispatcher<WindowEvent>& GetDispatcher();

    bool HandleWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam) const;

  private:
    void InvalidateFramebufferSize();

    void InitHandlers();
    void HandleResizeMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void HandleKeyMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void HandleMouseButtonMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void HandleMouseMoveMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void HandleMouseScrollMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  private:
    HWND m_hWnd;
    uint32_t m_width;
    uint32_t m_height;
    const wchar_t* m_title;
    uint32_t m_resolutionDivisor;

    Framebuffer m_framebuffer;
    BITMAPINFO m_framebufferInfo;

    EventDispatcher<WindowEvent> m_dispatcher;
    std::unordered_map<UINT, std::function<void(UINT, WPARAM, LPARAM)>> m_messageHandlers;
    InputSystem m_input;

    inline constexpr const static wchar_t* kClassName = L"DlRaytracerWindow";
  };
}
