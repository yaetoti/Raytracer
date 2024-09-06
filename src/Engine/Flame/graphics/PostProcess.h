#pragma once

#include <Flame/engine/ShaderPipeline.h>
#include "buffers/ConstantBuffer.h"

namespace Flame {
  struct PostProcess final {
    struct ResolveBuffer final {
      float evFactor;
    };

    void Init();
    void Cleanup();

    float GetEvFactor() const;
    void SetEvFactor(float evFactor);

    void Resolve(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst) const;

    static PostProcess* Get();

  private:
    PostProcess() = default;

  private:
    ShaderPipeline m_pipeline;
    ConstantBuffer<ResolveBuffer> m_buffer;
    mutable bool m_bufferDirty = true;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/resolve.hlsl";
  };
}
