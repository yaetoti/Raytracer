#include "ConsoleLib.h"
#include "Application.h"
#include <Windows.h>

#include "Flame/engine/Transform.h"
#include "Flame/utils/SolidVector.h"

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::Get()->RedirectStdHandles();
  Flame::Engine::Init();

  auto id0 = Flame::TransformSystem::Get()->Insert();
  auto id1 = Flame::TransformSystem::Get()->Insert();
  Flame::TransformSystem::Get()->At(id0)->transform.SetPosition(2, 2, 8);
  auto id2 = Flame::TransformSystem::Get()->Insert();
  std::cout << id0 << '\n';
  std::cout << id1 << '\n';
  std::cout << id2 << '\n';
  Flame::TransformSystem::Get()->Remove(id1);
  std::cout << Flame::TransformSystem::Get()->At(id0)->transform.GetPosition() << '\n';

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
