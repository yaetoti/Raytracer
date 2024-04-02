#define NOMINMAX

#include <Windows.h>
#include <ConsoleLib/Console.h>
#include "Project/Application.h"

// WARNING: Run away, wayfarer!
// Problems (things that take long time to think about):
// 1. Architectural design (Placement of functional, interaction between nested components, reusing, planning handy interface for further access)
// 2. Choosing between pointer types and who should own an object
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Console::GetInstance()->RedirectStdHandles();
    Console::GetInstance()->WPrintF(L"Çהאנמגא, קונעט!\n");

    std::shared_ptr<Application> m_application = std::make_shared<Application>();
    return m_application->RunMainLoop();
}
