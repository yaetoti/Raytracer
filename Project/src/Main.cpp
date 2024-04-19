#define GLM_ENABLE_EXPERIMENTAL

#include "Application.h"
#include <ConsoleLib.h>

#include <iostream>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::GetInstance()->RedirectStdHandles();

  glm::vec3 v(1, 2, 3);
  std::cout << glm::to_string(glm::rotate(45.0f, v)) << '\n';

  auto m_application = std::make_shared<Application>();
  return m_application->RunMainLoop();
}
