#define NOMINMAX

#include <Windows.h>
#include <ConsoleLib/Console.h>
#include "Project/Application.h"

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Console::GetInstance()->RedirectStdHandles();
    Console::GetInstance()->WPrintF(L"Çהאנמגא, קונעט!\n");

    std::shared_ptr<Application> m_application = std::make_shared<Application>();
    return m_application->runMainLoop();
}
