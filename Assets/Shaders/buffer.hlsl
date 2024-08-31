cbuffer ConstantBuffer : register(b0)
{
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
  float4 g_resolution;
  float4 g_cameraPosition;
  float g_time;
  bool g_isNormalVisMode;
};
