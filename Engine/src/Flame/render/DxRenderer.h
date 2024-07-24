#pragma once

#include "Flame/window/Window.h"
#include <wrl/client.h>

namespace Flame {
  struct DxRenderer final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    explicit DxRenderer(Window* window);

    void Init();
    void Render(float time, float deltaTime);
    void Resize(uint32_t width, uint32_t height);

  private:
    struct PerVertex final {
      float position[4];
      float color[4];
    };

    struct PerFrame final {
      float time;
      float padding0[3];
      float resolution[4];
    };

    bool LoadShaders();
    bool CreateBuffers();

  private:
    Window* m_window;

    ComPtr<ID3DBlob> m_vertexShaderBlob;
    ComPtr<ID3DBlob> m_pixelShaderBlob;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11Buffer> m_vbo;
    ComPtr<ID3D11Buffer> m_constantBuffer;


    std::vector<float> m_resolution;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/task3.hlsl";
    inline static const uint32_t kDataOffset = 0;
    inline static const uint32_t kDataStride = 8 * sizeof(float);
  };
}
