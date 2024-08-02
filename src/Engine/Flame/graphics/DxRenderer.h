#pragma once

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
      float resolution[4];
      float cameraPosition[4];
      float time;
    };

    explicit DxRenderer(Window* window);

    void Init();
    void Cleanup();
    void UpdateCamera(float deltaTime);
    void Update(float deltaTime);
    void Render(float time, float deltaTime);
    void Resize(uint32_t width, uint32_t height);

  private:
    Window* m_window;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
    ConstantBuffer<PerFrame> m_constantBuffer;

    // AlignedCamera test
    InputSystem* m_input;
    std::shared_ptr<AlignedCamera> m_camera;

    std::vector<float> m_resolution;
  };
}
