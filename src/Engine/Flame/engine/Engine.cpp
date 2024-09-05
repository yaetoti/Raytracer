#include "Engine.h"

#include <Flame/graphics/DxContext.h>
#include <Flame/graphics/PostProcess.h>

#include "LightSystem.h"
#include "MeshSystem.h"
#include "ModelManager.h"
#include "ReflectionCapture.h"
#include "TextureManager.h"

#include "imgui.h"
#include "backends/imgui_impl_dx11.h"

namespace Flame {
  void Engine::Init() {
    DxContext::Get()->Init();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplDX11_Init(DxContext::Get()->d3d11Device.Get(), DxContext::Get()->d3d11DeviceContext.Get());

    // Dependencies: ReflectionCapture -> TextureManager -> DxContext
    ReflectionCapture capture;
    capture.Init();
    capture.GenerateAndSaveTextures();
    capture.Cleanup();

    ModelManager::Get()->Init();
    MeshSystem::Get()->Init();
    LightSystem::Get()->Init();
    PostProcess::Get()->Init();
  }

  void Engine::Cleanup() {
    PostProcess::Get()->Cleanup();
    TextureManager::Get()->Cleanup();
    LightSystem::Get()->Cleanup();
    MeshSystem::Get()->Cleanup();
    MeshSystem::Get()->Cleanup();
    ModelManager::Get()->Cleanup();
    TransformSystem::Get()->Cleanup();
    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();
    DxContext::Get()->Cleanup();
  }

  void Engine::SetWorkingDirectory(std::wstring workingDirectory) {
    m_workingDirectory = std::move(workingDirectory);
  }

  std::wstring Engine::GetWorkingDirectory() {
    return m_workingDirectory;
  }

  std::wstring Engine::GetDirectory(const std::wstring& directory) {
    return m_workingDirectory + directory;
  }
}
