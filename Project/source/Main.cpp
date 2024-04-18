#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL

#include "Project/Application.h"
#include <ConsoleLib/Console.h>

#include <Windows.h>
#include <iostream>
#include <GLM/glm.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <GLM/gtx/vector_angle.hpp>

// WARNING: Run away, wayfarer!
// Problems (things that take long time to think about):
// 1. Architectural design (Placement of functional, interaction between nested components, reusing, planning handy interface for further access)
// 2. Choosing between pointer types and who should own an object
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::GetInstance()->RedirectStdHandles();
  Console::GetInstance()->WPrintF(L"Çהאנמגא, קונעט!\n");

  glm::vec3 v(1, 2, 3);
  std::cout << glm::to_string(glm::rotate(45.0f, v)) << '\n';

  auto m_application = std::make_shared<Application>();
  return m_application->RunMainLoop();
}
