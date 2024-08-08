cbuffer ConstantBuffer : register(b0)
{
  float4x4 g_viewMatrix;
  float4x4 g_projectionMatrix;
  // Frustum corner directions in homogenous coordinates
  float4 g_frustumTL;
  float4 g_frustumBR;
  float4 g_frustumBL;
  float4 g_resolution;
  float4 g_cameraPosition;
  float g_time;
  bool g_isNormalVisMode;
};

SamplerState g_pointWrap : register(s0);
SamplerState g_linearWrap : register(s1);
SamplerState g_anisotropicWrap : register(s2);
