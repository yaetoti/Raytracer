#include "Application.h"
#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::GetInstance()->RedirectStdHandles();
  Flame::DxContext::Get()->Init();

  {
    Application application;
    application.Run();
  }

  Flame::DxContext::Get()->Deinit();
  Console::GetInstance()->Pause();
  return 0;
}
