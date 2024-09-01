#pragma once
#include <array>
#include <memory>
#include <vector>
#include <Flame/graphics/buffers/ConstantBuffer.h>
#include <wrl/client.h>

#include "ShaderPipeline.h"
#include "Texture.h"
#include "Flame/graphics/DxContext.h"
#include "glm/glm.hpp"

namespace Flame {
  struct ReflectionCapture final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct IblDiffuseData final {
      glm::mat4 viewMatInv;
      glm::vec4 normal;
      uint32_t samples;
      float padding0[3];
    };

    struct IblSpecularData final {
      glm::mat4 viewMatInv;
      glm::vec4 normal;
      float roughness;
      uint32_t samples;
      float padding0[2];
    };

    struct IblReflectanceData final {
      uint32_t samples;
      float padding0[3];
    };

    void Init();
    void Cleanup();

    std::shared_ptr<Texture> GenerateDiffuseTexture(uint32_t samples, uint32_t textureSize, ID3D11ShaderResourceView* skyboxView);
    std::shared_ptr<Texture> GenerateSpecularTexture(uint32_t samples, uint32_t textureSize, ID3D11ShaderResourceView* skyboxView);
    std::shared_ptr<Texture> GenerateReflectanceTexture(uint32_t samples, uint32_t textureSize);

  private:
    static std::shared_ptr<Texture> CreateCubemap(uint32_t textureSize, DXGI_FORMAT format, uint32_t mipLevels);
    static std::array<ComPtr<ID3D11RenderTargetView>, 6> CreateCubemapRtv(ID3D11Resource* texture, DXGI_FORMAT format, uint32_t mipLevel);
    static std::array<glm::mat4, 6> GenerateTransformMatrices();
    static uint32_t GetTextureSizeLevel(uint32_t size, uint32_t mipLevel);

  private:
    // Diffuse
    ConstantBuffer<IblDiffuseData> diffuseBuffer;
    ShaderPipeline diffusePipeline;

    // Specular
    ConstantBuffer<IblSpecularData> specularBuffer;
    ShaderPipeline specularPipeline;

    // Reflectance
    ShaderPipeline reflectancePipeline;
    ConstantBuffer<IblReflectanceData> reflectanceBuffer;

    inline static const glm::vec4 kCubemapFront[6] = {
      { 1, 0, 0, 0 },
      { -1, 0, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, -1, 0, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, -1, 0 },
    };
    inline static const glm::vec4 kCubemapRight[6] = {
      { 0, 0, -1, 0 },
      { 0, 0, 1, 0 },
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { -1, 0, 0, 0 },
    };
    inline static const glm::vec4 kCubemapUp[6] = {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, -1, 0 },
      { 0, 0, 1, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
    };
  };
}
