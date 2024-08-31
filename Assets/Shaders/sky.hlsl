#include "globals.hlsl"

TextureCube<float4> skyTexture : register(t0);

struct VSOutput {
  float4 position : SV_POSITION;
  float3 cameraToPixelDir : TEXEL_DIRECTION;
};

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  VSOutput result;

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 0.0, 1.0);
    result.cameraToPixelDir = g_frustumTL.xyz;
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 0.0, 1.0);
    result.cameraToPixelDir = g_frustumBR.xyz;
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 0.0, 1.0);
    result.cameraToPixelDir = g_frustumBL.xyz;
    return result;
  }

  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET {
  return skyTexture.Sample(g_anisotropicWrap, input.cameraToPixelDir);
  //return skyTexture.SampleLevel(g_anisotropicWrap, input.cameraToPixelDir, 5);
  //return skyTexture.Sample(g_pointWrap, input.cameraToPixelDir);
}
