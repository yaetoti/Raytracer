#pragma once

#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/window/Window.h"

#include <memory>
#include <Flame/engine/ReflectionCapture.h>
#include <Flame/engine/ShaderPipeline.h>
#include <Flame/engine/Texture.h>
#include <wrl/client.h>

#include "buffers/ConstantBuffer.h"
#include "buffers/VertexBuffer.h"
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
      bool isNormalVisMode[4];

      bool diffuseEnabled[4];
      bool specularEnabled[4];
      bool iblDiffuseEnabled[4];
      bool iblSpecularEnabled[4];
      bool overwriteRoughness[4];
      float roughness;
    };

    explicit DxRenderer(std::shared_ptr<Window> window, std::shared_ptr<AlignedCamera> camera);
    ~DxRenderer();

    void Init();
    void Cleanup();
    void Update(float deltaTime);
    void Render(float time, float deltaTime);
    void Resize(uint32_t width, uint32_t height);

    void SetNormalVisMode(bool isNormalVisMode);
    bool GetNormalVisMode() const;

    bool DiffuseEnabled() const;
    void SetDiffuseEnabled(bool diffuseEnabled);
    bool SpecularEnabled() const;
    void SetSpecularEnabled(bool specularEnabled);
    bool IblDiffuseEnabled() const;
    void SetIblDiffuseEnabled(bool iblDiffuseEnabled);
    bool IblSpecularEnabled() const;
    void SetIblSpecularEnabled(bool iblSpecularEnabled);
    bool OverwriteRoughness() const;
    void SetOverwriteRoughness(bool overwriteRoughness);
    float Roughness() const;
    void SetRoughness(float roughness);

  private:
    void RenderSkybox();
    void UpdateConstantBuffer(float time);

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

    // Skybox
    ShaderPipeline m_skyboxPipeline;
    ID3D11Resource* m_skyTexture;
    ID3D11ShaderResourceView* m_skyTextureView;

    // Foreground Output TODO remove
    ShaderPipeline m_testPipeline;

    // ImGUI
    bool m_diffuseEnabled = true;
    bool m_specularEnabled = true;
    bool m_iblDiffuseEnabled = true;
    bool m_iblSpecularEnabled = true;
    bool m_overwriteRoughness = false;
    float m_roughness = 0.0;

    static constexpr const wchar_t* kSkyboxPath = L"Assets/Textures/lake_beach.dds";
    //static constexpr const wchar_t* kSkyboxPath = L"Assets/Textures/night_street.dds";
    static constexpr const wchar_t* kSkyShaderPath = L"Assets/Shaders/sky.hlsl";
  };
}
