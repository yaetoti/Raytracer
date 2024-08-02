#include "Application.h"
#include <Windows.h>

#include "Flame/systems/Engine.h"

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::Get()->RedirectStdHandles();
  Flame::Engine::Init();

  std::cout << "Press any key to continue..." << '\n';
  Console::Get()->Pause();

  {
    Application application;
    application.Run();
  }

  Flame::Engine::Cleanup();
  Console::Get()->Pause();
  return 0;
}
