#pragma once
#include <string>

namespace Flame {
  struct Engine final {
    static void Init();
    static void Cleanup();

    static void SetWorkingDirectory(std::wstring workingDirectory);
    static std::wstring GetWorkingDirectory();
    static std::wstring GetDirectory(const std::wstring& directory);

  private:
    inline static std::wstring m_workingDirectory;
  };
}
