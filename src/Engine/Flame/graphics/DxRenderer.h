#pragma once

#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/window/Window.h"

#include <memory>
#include <Flame/engine/IShadowMapProvider.h>
#include <Flame/engine/LightSystem.h>
#include <Flame/engine/ReflectionCapture.h>
#include <Flame/engine/ShaderPipeline.h>
#include <Flame/engine/Texture.h>
#include <wrl/client.h>

#include "buffers/ConstantBuffer.h"
#include "buffers/VertexBuffer.h"
#include "buffers/data/PerFrame.h"
#include "buffers/data/PerView.h"
#include "Flame/camera/AlignedCamera.h"

namespace Flame {
  struct DxRenderer final : IShadowMapProvider {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

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
    // ShadowMaps
    void GenerateShadowMapsDirect();
    void UpdateMatricesDirect();
    void RenderShadowMapsDirect();

    void GenerateShadowMapsSpot();
    void UpdateMatricesSpot();
    void RenderShadowMapsSpot();

    void RenderSkybox();
    void UpdateFrameBuffer(float time);
    void UpdateViewBuffer();

  public:
    ID3D11ShaderResourceView* GetShadowMapSrvDirect() override;
    ID3D11ShaderResourceView* GetShadowMapSrvSpot() override;

  private:
    // Input
    std::shared_ptr<Window> m_window;
    std::shared_ptr<AlignedCamera> m_camera;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
    InputSystem* m_input;

    // CBuffers
    ConstantBuffer<PerFrame> m_frameCBuffer;
    ConstantBuffer<PerView> m_viewCBuffer;

    // ShadowMaps
    ComPtr<ID3D11Texture2D> m_shadowMapArrayDirect;
    ComPtr<ID3D11DepthStencilView> m_shadowMapDsvDirect[LightSystem::kDirectLightNum];
    ComPtr<ID3D11ShaderResourceView> m_shadowMapSrvDirect;
    uint32_t m_directLightsCount = 0;

    ComPtr<ID3D11Texture2D> m_shadowMapArraySpot;
    ComPtr<ID3D11DepthStencilView> m_shadowMapDsvSpot[LightSystem::kSpotLightNum];
    ComPtr<ID3D11ShaderResourceView> m_shadowMapSrvSpot;
    uint32_t m_spotLightsCount = 0;

    // Samplers
    ComPtr<ID3D11SamplerState> m_pointSampler;
    ComPtr<ID3D11SamplerState> m_linearSampler;
    ComPtr<ID3D11SamplerState> m_anisotropicSampler;

    glm::vec4 m_resolution;
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
    static constexpr uint32_t kShadowMapResolution = 8192;
    static constexpr float kDirectShadowPadding = 10.0f;
  };
}
