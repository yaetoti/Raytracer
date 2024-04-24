#include "Window.h"
#include "events/KeyWindowEvent.h"
#include "events/MouseButtonWindowEvent.h"
#include "events/MouseMoveWindowEvent.h"
#include "events/ResizeWindowEvent.h"
#include <windowsx.h>

namespace Flame {
  Window::Window(const wchar_t* title, uint32_t width, uint32_t height, uint32_t resolutionDivisor)
  : m_hWnd(nullptr)
  , m_width(width)
  , m_height(height)
  , m_title(title)
  , m_resolutionDivisor(resolutionDivisor)
  , m_framebufferInfo { } {
    m_framebufferInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_framebufferInfo.bmiHeader.biPlanes = 1;
    // 4th component is used for alignment in BitBlit()
    m_framebufferInfo.bmiHeader.biBitCount = 32;
    m_framebufferInfo.bmiHeader.biCompression = BI_RGB;
  }

  Window::~Window() {
    DiscardResources();
  }

  bool Window::CreateResources() {
    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kClassName;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszMenuName = nullptr;
    if (!RegisterClassExW(&wc)) {
      return false;
    }

    RECT windowRect { 0, 0, m_width, m_height };
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, 0, 0);

    m_hWnd = CreateWindowExW(
      0,
      wc.lpszClassName,
      m_title,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      nullptr,
      nullptr,
      GetModuleHandleW(nullptr),
      this
    );
    if (m_hWnd == nullptr) {
      return false;
    }

    InitHandlers();
    m_dispatcher.AddListener(&m_input);

    return true;
  }

  void Window::DiscardResources() {
    DestroyWindow(m_hWnd);
    UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
  }

  void Window::Show(int nCmdShow) const {
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
  }

  void Window::Blit() const {
    HDC dc = GetDC(m_hWnd);
    StretchDIBits(
      dc,
      0,
      0,
      static_cast<int>(m_width),
      static_cast<int>(m_height),
      0,
      0,
      static_cast<int>(m_framebuffer.GetWidth()),
      static_cast<int>(m_framebuffer.GetHeight()),
      m_framebuffer.GetData(),
      &m_framebufferInfo,
      DIB_RGB_COLORS,
      SRCCOPY
    );
    ReleaseDC(m_hWnd, dc);
  }

  Framebuffer& Window::GetFramebuffer() {
    return m_framebuffer;
  }

  InputSystem& Window::GetInputSystem() {
    return m_input;
  }

  EventDispatcher<WindowEvent>& Window::GetDispatcher() {
    return m_dispatcher;
  }

  uint32_t Window::GetWidth() const {
    return m_width;
  }

  uint32_t Window::GetHeight() const {
    return m_height;
  }

  uint32_t Window::GetResolutionDivisor() const {
    return m_resolutionDivisor;
  }


  /*----- EVENTS -----*/


  bool Window::HandleWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam) const {
    const auto& it = m_messageHandlers.find(msg);
    if (it == m_messageHandlers.end()) {
      return false;
    }

    it->second(msg, wParam, lParam);
    return true;
  }

  void Window::InvalidateFramebufferSize() {
    m_framebuffer.Resize(m_width / m_resolutionDivisor, m_height / m_resolutionDivisor);
    m_framebufferInfo.bmiHeader.biWidth = static_cast<LONG>(m_framebuffer.GetWidth());
    m_framebufferInfo.bmiHeader.biHeight = static_cast<LONG>(m_framebuffer.GetHeight());
  }

  void Window::InitHandlers() {
    m_messageHandlers[WM_SIZE] = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
      HandleResizeMessage(msg, wParam, lParam);
    };

    m_messageHandlers[WM_KEYDOWN]
    = m_messageHandlers[WM_KEYUP]
    = m_messageHandlers[WM_SYSKEYDOWN]
    = m_messageHandlers[WM_SYSKEYUP]
    = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
        HandleKeyMessage(msg, wParam, lParam);
      };

    m_messageHandlers[WM_LBUTTONDOWN]
    = m_messageHandlers[WM_LBUTTONUP]
    = m_messageHandlers[WM_RBUTTONDOWN]
    = m_messageHandlers[WM_RBUTTONUP]
    = m_messageHandlers[WM_MBUTTONDOWN]
    = m_messageHandlers[WM_MBUTTONUP]
    = m_messageHandlers[WM_XBUTTONDOWN]
    = m_messageHandlers[WM_XBUTTONUP]
    = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
        HandleMouseButtonMessage(msg, wParam, lParam);
      };

    m_messageHandlers[WM_MOUSEMOVE] = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
      HandleMouseMoveMessage(msg, wParam, lParam);
    };
  }

  void Window::HandleResizeMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    m_width = LOWORD(lParam);
    m_height = HIWORD(lParam);
    InvalidateFramebufferSize();
    m_dispatcher.Dispatch(ResizeWindowEvent(m_width, m_height, m_framebuffer.GetWidth(), m_framebuffer.GetHeight()));
  }

  void Window::HandleKeyMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    WORD vkCode = LOWORD(wParam);
    WORD keyFlags = HIWORD(lParam);
    BOOL wasPressed = (keyFlags & KF_REPEAT) == KF_REPEAT;
    BOOL isReleased = (keyFlags & KF_UP) == KF_UP;
    m_dispatcher.Dispatch(KeyWindowEvent(vkCode, !isReleased, wasPressed));
  }

  void Window::HandleMouseButtonMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    float xCursor = static_cast<float>(GET_X_LPARAM(lParam));
    float yCursor = static_cast<float>(GET_Y_LPARAM(lParam));
    switch (msg) {
      case WM_LBUTTONDOWN:
        m_dispatcher.Dispatch(MouseButtonWindowEvent(MouseButton::LEFT, true, xCursor, yCursor));
        return;
      case WM_RBUTTONDOWN:
        m_dispatcher.Dispatch(MouseButtonWindowEvent(MouseButton::RIGHT, true, xCursor, yCursor));
        return;
      case WM_MBUTTONDOWN:
        m_dispatcher.Dispatch(MouseButtonWindowEvent(MouseButton::MIDDLE, true, xCursor, yCursor));
        return;
      case WM_XBUTTONDOWN: {
        MouseButton xButtonType = HIWORD(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
        m_dispatcher.Dispatch(MouseButtonWindowEvent(xButtonType, true, xCursor, yCursor));
        return;
      }
      case WM_LBUTTONUP:
        m_dispatcher.Dispatch(MouseButtonWindowEvent(MouseButton::LEFT, false, xCursor, yCursor));
        return;
      case WM_RBUTTONUP:
        m_dispatcher.Dispatch(MouseButtonWindowEvent(MouseButton::RIGHT, false, xCursor, yCursor));
        return;
      case WM_MBUTTONUP:
        m_dispatcher.Dispatch(MouseButtonWindowEvent(MouseButton::MIDDLE, false, xCursor, yCursor));
        return;
      case WM_XBUTTONUP: {
        MouseButton xButtonType = HIWORD(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
        m_dispatcher.Dispatch(MouseButtonWindowEvent(xButtonType, false, xCursor, yCursor));
        return;
      }
      default:
        break;
    }
  }

  void Window::HandleMouseMoveMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    // TODO Will I need the real coordinates?
    float xCursor = static_cast<float>(GET_X_LPARAM(lParam)) / m_resolutionDivisor;
    float yCursor = static_cast<float>(GET_Y_LPARAM(lParam)) / m_resolutionDivisor;
    m_dispatcher.Dispatch(MouseMoveWindowEvent(xCursor, yCursor));
  }

  LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
      CREATESTRUCTW* data = reinterpret_cast<CREATESTRUCTW*>(lParam);
      SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data->lpCreateParams));
      return 0;
    }

    if (msg == WM_DESTROY) {
      PostQuitMessage(0);
      return 0;
    }

    Window* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (window != nullptr && window->HandleWindowMessage(msg, wParam, lParam)) {
      return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
  }
}
