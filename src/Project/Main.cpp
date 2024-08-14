#include "ConsoleLib.h"
#include "Application.h"
#include <Windows.h>

#include "Flame/utils/SolidVector.h"

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::Get()->RedirectStdHandles();
  Flame::Engine::Init();

  Flame::SolidVector<std::string> v;
  std::cout << v.insert("ABOBA1") << '\n';
  std::cout << v.insert("ABOBA2") << '\n';
  std::cout << v.insert("ABOBA3") << '\n';
  std::cout << v[0] << '\n';
  std::cout << v[1] << '\n';
  std::cout << v[2] << '\n';
  v.erase(1);
  v.erase(2);
  auto id = v.insert("LAST_ABOBA");
  std::cout << id << '\n';
  std::cout << v[id] << '\n';
  id = v.insert("LASTEST_ABOBA_FINAL");
  std::cout << id << '\n';
  std::cout << v[id] << '\n';
  id = v.insert("LASTEST_ABOBA_FINAL_INAL_FINAL");
  std::cout << id << '\n';
  std::cout << v[id] << '\n';

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
