/*----- Lights -----*/


#define NUM_LIGHTS 8

struct DirectLight {
  float4 direction;
  float4 color;
  float intensity;

  float3 padding0;
};

struct PointLight {
  float4 position;
  float4 color;
  float intensity;

  float constantFadeoff;
  float linearFadeoff;
  float quadraticFadeoff;
};

struct SpotLight {
  float4 position;
  float4 direction;
  float4 color;
  float intensity;

  float cutoffCosineInner;
  float cutoffCosineOuter;

  float linearFadeoff;
  float constantFadeoff;
  float quadraticFadeoff;

  float2 padding0;
};


/*----- Buffers -----*/


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

cbuffer LightBuffer : register(b1) {
  DirectLight g_directLights[NUM_LIGHTS];
  PointLight g_pointLights[NUM_LIGHTS];
  SpotLight g_spotLights[NUM_LIGHTS];
  uint g_directLightsCount;
  uint g_pointLightsCount;
  uint g_spotLightsCount;
  float padding0;
};


/*----- SampleStates -----*/


SamplerState g_pointWrap : register(s0);
SamplerState g_linearWrap : register(s1);
SamplerState g_anisotropicWrap : register(s2);
