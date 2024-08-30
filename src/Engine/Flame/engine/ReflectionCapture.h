#pragma once
#include <array>
#include <complex.h>
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
    };

    struct IblSpecularData final {
      glm::mat4 viewMatInv;
      glm::vec4 normal;
    };

    void Init();
    void Cleanup();

    std::shared_ptr<Texture> GenerateDiffuseTexture(uint32_t textureSize, ID3D11ShaderResourceView* skyboxView);
    std::shared_ptr<Texture> GenerateSpecularTexture(uint32_t textureSize, ID3D11ShaderResourceView* skyboxView);

  private:
    static std::shared_ptr<Texture> CreateCubemap(uint32_t textureSize, DXGI_FORMAT format);
    static std::array<ComPtr<ID3D11RenderTargetView>, 6> CreateCubemapRtv(ID3D11Resource* texture, DXGI_FORMAT format);
    static std::array<glm::mat4, 6> GenerateTransformMatrices();

  private:
    // Diffuse IBL
    ConstantBuffer<IblDiffuseData> diffuseBuffer;
    ShaderPipeline diffusePipeline;

    // Specular IBL
    ConstantBuffer<IblSpecularData> specularBuffer;
    ShaderPipeline specularPipeline;

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
