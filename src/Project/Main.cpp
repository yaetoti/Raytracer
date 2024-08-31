#include "ConsoleLib.h"
#include "Application.h"
#include <Windows.h>

// #include "assimp/Importer.hpp"
// #include "assimp/scene.h"
// #include "assimp/postprocess.h"

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
  Console::Get()->RedirectStdHandles();
  Flame::Engine::Init();

  // Assimp::Importer importer;
  // auto scene = importer.ReadFile("Assets/Models/OtherCube/OtherCube.obj", aiProcess_Triangulate);
  
  // std::cout << "Materials: " << scene->mNumMaterials << '\n';
  // std::cout << "Meshes: " << scene->mNumMeshes << '\n';
  // std::cout << "Index: " << scene->mMeshes[0]->mMaterialIndex << '\n';

  // std::cout << "Textures: " << scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) << '\n';
  // std::cout << "Properties: " << scene->mMaterials[0]->mNumProperties << '\n';
  // for (int i = 0; i < scene->mMaterials[0]->mNumProperties; ++i) {
  //   std::cout << "Property: " << scene->mMaterials[0]->mProperties[i]->mKey.C_Str() << '\n';
  // }

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
