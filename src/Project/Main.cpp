#include "Application.h"
#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Application application;
  application.Run();
  return 0;
}
