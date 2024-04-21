#include "Window.h"
#include "events/KeyWindowEvent.h"
#include "events/MouseButtonWindowEvent.h"
#include "events/MouseMoveWindowEvent.h"
#include "events/ResizeWindowEvent.h"
#include <windowsx.h>

namespace Flame {
  Window::Window(const wchar_t* title, size_t width, size_t height)
  : m_hWnd(nullptr)
  , m_width(width)
  , m_height(height)
  , m_title(title) {
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

  void Window::Blit(const RenderSurface& surface) const {
    BITMAPINFO info = { };
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = static_cast<LONG>(surface.GetWidth());
    info.bmiHeader.biHeight = static_cast<LONG>(surface.GetHeight());
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    HDC dc = GetDC(m_hWnd);
    StretchDIBits(
      dc,
      0,
      0,
      static_cast<int>(m_width),
      static_cast<int>(m_height),
      0,
      0,
      static_cast<int>(surface.GetWidth()),
      static_cast<int>(surface.GetHeight()),
      surface.GetData(),
      &info,
      DIB_RGB_COLORS,
      SRCCOPY
    );
    ReleaseDC(m_hWnd, dc);
  }

  InputSystem& Window::GetInput() {
    return m_input;
  }

  EventDispatcher<WindowEvent>* Window::GetDispatcher() {
    return &m_dispatcher;
  }

  size_t Window::GetWidth() const {
    return m_width;
  }

  size_t Window::GetHeight() const {
    return m_height;
  }

  bool Window::DispatchEvents() {
    // TODO Not implemented
    return false;
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

  void Window::InitHandlers() {
    m_messageHandlers[WM_SIZE] = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
      HandleResizeMessage(msg, wParam, lParam);
    };
    {
      UINT messages[] = { WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP };
      auto lambda = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
        HandleKeyMessage(msg, wParam, lParam);
      };

      for (auto it = std::begin(messages), end = std::end(messages); it != end; ++it) {
        m_messageHandlers[*it] = lambda;
      }
    }
    {
      UINT messages[] = {
        WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN, WM_XBUTTONDOWN,
        WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP, WM_XBUTTONUP
      };
      auto lambda = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
        HandleMouseButtonMessage(msg, wParam, lParam);
      };

      for (auto it = std::begin(messages), end = std::end(messages); it != end; ++it) {
        m_messageHandlers[*it] = lambda;
      }
    }
    m_messageHandlers[WM_MOUSEMOVE] = [this](UINT msg, WPARAM wParam, LPARAM lParam) {
      HandleMouseMoveMessage(msg, wParam, lParam);
    };
  }

  void Window::HandleResizeMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    m_width = LOWORD(lParam);
    m_height = HIWORD(lParam);
    m_dispatcher.Dispatch(ResizeWindowEvent(m_width, m_height));
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
    float xCursor = static_cast<float>(GET_X_LPARAM(lParam));
    float yCursor = static_cast<float>(GET_Y_LPARAM(lParam));
    m_dispatcher.Dispatch(MouseMoveWindowEvent(xCursor, yCursor));
  }

  LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
      const auto& data = reinterpret_cast<CREATESTRUCTW*>(lParam);
      SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data->lpCreateParams));
      return 0;
    }

    const auto& window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (window != nullptr && window->HandleWindowMessage(msg, wParam, lParam)) {
      return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
  }
}
