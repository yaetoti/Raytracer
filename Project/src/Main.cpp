#define GLM_ENABLE_EXPERIMENTAL

#include "Application.h"

#include <ConsoleLib.h>
#include <iostream>
#include <Windows.h>


int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::GetInstance()->RedirectStdHandles();

  auto m_application = std::make_shared<Application>();
  return m_application->RunMainLoop();
}
