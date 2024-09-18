#pragma once
#include <array>
#include <d3d11.h>

#include "LightSystem.h"

namespace Flame {
  struct IShadowMapProvider {
    virtual ~IShadowMapProvider() = default;

    virtual ID3D11ShaderResourceView* GetShadowMapSrvDirect() = 0;
    virtual ID3D11ShaderResourceView* GetShadowMapSrvSpot() = 0;
    virtual ID3D11ShaderResourceView* GetShadowMapSrvPoint() = 0;

  protected:
    IShadowMapProvider() = default;
  };
}
