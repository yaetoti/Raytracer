#include "globals.hlsl"

// TODO pass frustum corners in a constant buffer

TextureCube<float4> skyTexture : register(t0);

struct VSOutput {
  float4 position : SV_POSITION;
  float4 uv : TEXCOORD0;
  float3 cameraToPixelDir;
};

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  // Why do we need UV?

  VSOutput result;

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 1.0, 1.0);
    result.uv = float4(0.0, 2.0, 0.0, 0.0);
    result.cameraToPixelDir = g_frustumTL.xyz;
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 1.0, 1.0);
    result.uv = float4(2.0, 0.0, 0.0, 0.0);
    result.cameraToPixelDir = g_frustumBR.xyz;
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 1.0, 1.0);
    result.uv = float4(0.0, 0.0, 0.0, 0.0);
    result.cameraToPixelDir = g_frustumBL.xyz;
    return result;
  }
}

float4 PSMain(VSOutput input) : SV_TARGET {
  return skyTexture.Sample(g_pointWrap, input.cameraToPixelDir);
}
