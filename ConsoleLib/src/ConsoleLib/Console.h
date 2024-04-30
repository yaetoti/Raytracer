#pragma once

#include <mutex>
#include <string>
#include <Windows.h>

class Console final {
public:
  ~Console();

  void WWrite(const void* buffer, DWORD size) const;
  void Write(const void* buffer, DWORD size) const;
  void WPut(wchar_t c) const;
  void Put(char c) const;

  void WPrintF(const wchar_t* format, ...) const;
  void WPrintFV(const wchar_t* format, va_list args) const;
  void PrintF(const char* format, ...) const;
  void PrintFV(const char* format, va_list args) const;

  void Pause() const;
  std::wstring WReadLine() const;
  std::string ReadLine() const;

  void RedirectStdHandles() const;

  static const Console* GetInstance();

private:
  Console();
  Console(const Console&) = delete;
  void operator=(const Console&) = delete;

  static Console* m_instance;
  HANDLE m_hOutput;
  HANDLE m_hInput;
  mutable std::recursive_mutex m_mutex;
};
