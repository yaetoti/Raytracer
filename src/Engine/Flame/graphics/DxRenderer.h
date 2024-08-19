#pragma once

#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/window/Window.h"

#include <memory>
#include <wrl/client.h>

#include "buffers/ConstantBuffer.h"
#include "Flame/camera/AlignedCamera.h"

namespace Flame {
  struct DxRenderer final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct PerFrame final {
      glm::mat4 viewMatrix;
      glm::mat4 projectionMatrix;
      float frustumTL[4];
      float frustumBR[4];
      float frustumBL[4];
      float resolution[4];
      float cameraPosition[4];
      float time;
      bool isNormalVisMode;
    };

    explicit DxRenderer(std::shared_ptr<Window> window, std::shared_ptr<AlignedCamera> camera);

    void Init();
    void Cleanup();
    void Update(float deltaTime);
    void Render(float time, float deltaTime);
    void Resize(uint32_t width, uint32_t height);

    void SetNormalVisMode(bool isNormalVisMode);
    bool GetNormalVisMode() const;

  private:
    std::shared_ptr<Window> m_window;
    std::shared_ptr<AlignedCamera> m_camera;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
    ConstantBuffer<PerFrame> m_constantBuffer;
    InputSystem* m_input;

    ComPtr<ID3D11SamplerState> m_pointSampler;
    ComPtr<ID3D11SamplerState> m_linearSampler;
    ComPtr<ID3D11SamplerState> m_anisotropicSampler;

    std::vector<float> m_resolution;
    bool m_isNormalVisMode = false;

    VertexShader m_resolveVertexShader;
    PixelShader m_resolvePixelShader;
  };
}
