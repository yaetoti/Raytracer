#include "indices.hlsli"
#include "samplers.hlsli"

/*----- Defines -----*/

static const float PI = 3.14159265f;


/*----- Lights -----*/


#define NUM_DIRECT_LIGHTS 1
#define NUM_POINT_LIGHTS 8
#define NUM_SPOT_LIGHTS 1

struct DirectLight {
  float4 direction;
  float3 radiance;
  float solidAngle;
};

struct PointLight {
  float4 position;
  float3 radiance;
  float radius;
};

struct SpotLight {
  float4 position;
  float4 direction;
  float3 radiance;
  float radius;

  float cutoffCosineInner;
  float cutoffCosineOuter;

  float2 padding0;

  // For flashlight
  float4x4 lightViewMat; // WStoVS
};


/*----- Buffers -----*/

// https://maraneshi.github.io/HLSL-ConstantBufferLayoutVisualizer/

cbuffer FrameBuffer : register(CBUFFER_FRAME) {
  float g_time;
  bool g_isNormalVisMode;
  // ImGUI settings
  bool g_diffuseEnabled;
  bool g_specularEnabled;
  bool g_iblDiffuseEnabled;
  bool g_iblSpecularEnabled;
  bool g_overwriteRoughness;
  float g_roughness;
};

cbuffer ViewBuffer : register(CBUFFER_VIEW) {
  float4x4 g_viewMatrix;
  float4x4 g_projectionMatrix;
  float4 g_frustumTL;
  float4 g_frustumBR;
  float4 g_frustumBL;
  float4 g_resolution;
  float4 g_cameraPosition;
};

cbuffer LightBuffer : register(CBUFFER_LIGHT) {
  DirectLight g_directLights[NUM_DIRECT_LIGHTS];
  PointLight g_pointLights[NUM_POINT_LIGHTS];
  SpotLight g_spotLights[NUM_SPOT_LIGHTS];
  uint g_directLightsCount;
  uint g_pointLightsCount;
  uint g_spotLightsCount;
  float padding0;
};

cbuffer MeshBuffer : register(CBUFFER_MESH) {
  float4x4 g_meshToModel;
  float4x4 g_modelToMesh;
};
