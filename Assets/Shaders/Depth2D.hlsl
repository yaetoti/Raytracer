#include "globals.hlsl"

struct VSInput {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
  float2 uv : TEXCOORD;

  // Instance buffer
  float4x4 modelMatrix : MODEL;
};

struct VSOutput {
  float4 positionProj : SV_POSITION;
};

VSOutput VSMain(VSInput input) {
  VSOutput result;
  result.positionProj = mul(g_projectionMatrix, mul(g_viewMatrix, mul(input.modelMatrix, mul(g_meshToModel, float4(input.position, 1.0)))));
  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET {
  return float4(1, 1, 1, 1);
}
