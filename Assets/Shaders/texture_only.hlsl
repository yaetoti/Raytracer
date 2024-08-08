#include "globals.hlsl"

Texture2D<float4> texture0 : register(t0);

struct VSInput
{
  float3 position : POSITION;
  float3 normal : POSITION;
  float2 uv : TEXCOORD;
  float4x4 modelMatrix : MODEL;
};

struct VSOutput {
  float4 position : SV_POSITION;
  float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input) {
  VSOutput result;
  result.position = mul(g_projectionMatrix, mul(g_viewMatrix, mul(input.modelMatrix, float4(input.position, 1.0))));
  result.uv = input.uv;
  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET {
  //return texture0.Sample(g_pointWrap, input.uv);
  //return texture0.Sample(g_linearWrap, input.uv);
  return texture0.Sample(g_anisotropicWrap, input.uv);
}