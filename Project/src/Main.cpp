#include "Application.h"

#include <ConsoleLib.h>
#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::GetInstance()->RedirectStdHandles();

  auto application = std::make_shared<Application>();
  return application->RunMainLoop();
}
