#include "globals.hlsl"

TextureCube<float4> skyTexture : register(t0);

cbuffer IblBuffer : register(b13) {
  float4x4 g_viewMatInv;
  float4 g_normal;
};

struct VSOutput {
  float4 position : SV_POSITION;
  float3 cameraToPixelDir : TEXEL_DIRECTION;
  float2 uv : UV;
  nointerpolation float3 normal : NORMAL;
};

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  VSOutput result;
  result.normal = mul(g_normal, g_viewMatInv);

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 1.0, 1.0);
    result.cameraToPixelDir = mul(g_frustumTL, g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_viewMatInv, g_frustumTL).xyz;
    //result.cameraToPixelDir = g_frustumTL.xyz;
    result.uv = float2(0, 2);
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 1.0, 1.0);
    result.cameraToPixelDir = mul(g_frustumBR, g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_viewMatInv, g_frustumBR).xyz;
    //result.cameraToPixelDir = g_frustumBR.xyz;
    result.uv = float2(2, 0);
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 1.0, 1.0);
    result.cameraToPixelDir = mul(g_frustumBL, g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_viewMatInv, g_frustumBL).xyz;
    //result.cameraToPixelDir = g_frustumBL.xyz;
    result.uv = float2(0, 0);
    return result;
  }

  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET {
  float3 color = normalize(input.cameraToPixelDir) * 0.5 + 0.5;
  //float3 color = g_normal * 0.5 + 0.5;
  //return float4(color.r, color.g, color.b, 1.0);
  //return float4(1.0, 0.0, 0.0, 1.0);
  return skyTexture.Sample(g_pointWrap, input.cameraToPixelDir);
}
