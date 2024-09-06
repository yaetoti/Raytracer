#include "globals.hlsl"

TextureCube<float4> skyTexture : register(t0);

struct VSOutput {
  float4 position : SV_POSITION;
  float3 cameraToPixelDir : TEXEL_DIRECTION;
  float2 uv : UV;
};

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  VSOutput result;

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 1.0, 1.0);
    result.cameraToPixelDir = g_frustumTL.xyz;
    result.uv = float2(0, 2);
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 1.0, 1.0);
    result.cameraToPixelDir = g_frustumBR.xyz;
    result.uv = float2(2, 0);
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 1.0, 1.0);
    result.cameraToPixelDir = g_frustumBL.xyz;
    result.uv = float2(0, 0);
    return result;
  }

  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET {
  return float4(input.uv.x, input.uv.y, 0.0, 1.0);
  //return skyTexture.Sample(g_pointWrap, input.cameraToPixelDir);
}
