#include <iostream>
#include <stdexcept>
#include <strsafe.h>

#include "Console.h"

Console* Console::m_instance = nullptr;

Console::Console() {
  if (!AllocConsole()) {
    throw new std::runtime_error("Can't allocate console");
  }

  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  m_hInput = GetStdHandle(STD_INPUT_HANDLE);
}

Console::~Console() {
  FreeConsole();
}

const Console* Console::Get() {
  if (m_instance == nullptr) {
    static std::mutex mutex;
    std::lock_guard lock(mutex);

    if (m_instance == nullptr) {
      m_instance = new Console();
    }
  }

  return m_instance;
}

void Console::WWrite(const void* buffer, DWORD size) const {
  std::lock_guard lock(m_mutex);
  WriteConsoleW(m_hOutput, buffer, size, nullptr, nullptr);
}

void Console::Write(const void* buffer, DWORD size) const {
  std::lock_guard lock(m_mutex);
  WriteConsoleA(m_hOutput, buffer, size, nullptr, nullptr);
}

void Console::WPut(wchar_t c) const {
  WWrite(&c, 1);
}

void Console::Put(char c) const {
  Write(&c, 1);
}

void Console::Pause() const {
  INPUT_RECORD inputRecord;
  DWORD numEventsRead;
  DWORD previousMode;
  std::lock_guard lock(m_mutex);

  GetConsoleMode(m_hInput, &previousMode);
  SetConsoleMode(m_hInput, previousMode & ~(ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_LINE_INPUT));
  FlushConsoleInputBuffer(m_hInput);
  do {
    ReadConsoleInputW(m_hInput, &inputRecord, 1, &numEventsRead);
  } while (inputRecord.EventType != KEY_EVENT || !inputRecord.Event.KeyEvent.bKeyDown);

  SetConsoleMode(m_hInput, previousMode);
}

void Console::WPrintF(const wchar_t* format, ...) const {
  va_list args;
  va_start(args, format);
  WPrintFV(format, args);
  va_end(args);
}

void Console::WPrintFV(const wchar_t* format, va_list args) const {
  int bufferSize = _vscwprintf(format, args) + 1;
  auto buffer = new wchar_t[bufferSize];
  StringCbVPrintfW(buffer, bufferSize * sizeof(wchar_t), format, args);
  std::lock_guard lock(m_mutex);
  WriteConsoleW(m_hOutput, buffer, bufferSize - 1, nullptr, nullptr);
  delete[] buffer;
}

void Console::PrintF(const char* format, ...) const {
  va_list args;
  va_start(args, format);
  PrintFV(format, args);
  va_end(args);
}

void Console::PrintFV(const char* format, va_list args) const {
  int bufferSize = _vscprintf(format, args) + 1;
  auto buffer = new char[bufferSize];
  StringCbVPrintfA(buffer, bufferSize, format, args);
  std::lock_guard lock(m_mutex);
  WriteConsoleA(m_hOutput, buffer, bufferSize - 1, nullptr, nullptr);
  delete[] buffer;
}

std::wstring Console::WReadLine() const {
  std::wstring buffer;
  INPUT_RECORD inputRecord;
  DWORD numEventsRead;
  DWORD previousMode;
  std::lock_guard lock(m_mutex);

  GetConsoleMode(m_hInput, &previousMode);
  SetConsoleMode(m_hInput, previousMode & ~(ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_LINE_INPUT));
  FlushConsoleInputBuffer(m_hInput);
  while (0 != ReadConsoleInputW(m_hInput, &inputRecord, 1, &numEventsRead)) {
    if (inputRecord.EventType != KEY_EVENT || !inputRecord.Event.KeyEvent.bKeyDown) {
      continue;
    }

    if (inputRecord.Event.KeyEvent.uChar.UnicodeChar == L'\r') {
      WPut(L'\n');
      break;
    }

    if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_BACK && buffer.size() != 0) {
      buffer.pop_back();
      CONSOLE_SCREEN_BUFFER_INFO info;
      GetConsoleScreenBufferInfo(m_hOutput, &info);
      if (info.dwCursorPosition.X != 0) {
        --info.dwCursorPosition.X;
      }
      else {
        info.dwCursorPosition.X = info.dwSize.X - 1;
        --info.dwCursorPosition.Y;
      }
      DWORD written;
      WriteConsoleOutputCharacterW(m_hOutput, L" ", 1, info.dwCursorPosition, &written);
      SetConsoleCursorPosition(m_hOutput, info.dwCursorPosition);
    }

    if (iswprint(inputRecord.Event.KeyEvent.uChar.UnicodeChar)) {
      buffer += inputRecord.Event.KeyEvent.uChar.UnicodeChar;
      WPut(inputRecord.Event.KeyEvent.uChar.UnicodeChar);
    }
  }

  SetConsoleMode(m_hInput, previousMode);
  return buffer;
}

std::string Console::ReadLine() const {
  std::string buffer;
  INPUT_RECORD inputRecord;
  DWORD numEventsRead;
  DWORD previousMode;
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  GetConsoleMode(m_hInput, &previousMode);
  SetConsoleMode(m_hInput, previousMode & ~(ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_LINE_INPUT));
  FlushConsoleInputBuffer(m_hInput);
  while (0 != ReadConsoleInputA(m_hInput, &inputRecord, 1, &numEventsRead)) {
    if (inputRecord.EventType != KEY_EVENT || !inputRecord.Event.KeyEvent.bKeyDown) {
      continue;
    }

    if (inputRecord.Event.KeyEvent.uChar.AsciiChar == '\r') {
      Put('\n');
      break;
    }

    if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_BACK && buffer.size() != 0) {
      buffer.pop_back();
      CONSOLE_SCREEN_BUFFER_INFO info;
      GetConsoleScreenBufferInfo(m_hOutput, &info);
      if (info.dwCursorPosition.X != 0) {
        --info.dwCursorPosition.X;
      }
      else {
        info.dwCursorPosition.X = info.dwSize.X - 1;
        --info.dwCursorPosition.Y;
      }
      DWORD written;
      WriteConsoleOutputCharacterW(m_hOutput, L" ", 1, info.dwCursorPosition, &written);
      SetConsoleCursorPosition(m_hOutput, info.dwCursorPosition);
    }

    if (isprint(inputRecord.Event.KeyEvent.uChar.AsciiChar)) {
      buffer += inputRecord.Event.KeyEvent.uChar.AsciiChar;
      Put(inputRecord.Event.KeyEvent.uChar.AsciiChar);
    }
  }

  SetConsoleMode(m_hInput, previousMode);
  return buffer;
}

void Console::RedirectStdHandles() const {
  std::lock_guard lock(m_mutex);
  FILE* stream;
  freopen_s(&stream, "CONIN$", "r", stdin);
  freopen_s(&stream, "CONOUT$", "w", stdout);
  freopen_s(&stream, "CONOUT$", "w", stderr);
  std::cout.clear();
  std::wcout.clear();
  std::cin.clear();
  std::wcin.clear();
  std::cerr.clear();
  std::wcerr.clear();
  std::clog.clear();
  std::wclog.clear();
}
