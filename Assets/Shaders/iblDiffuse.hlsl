#include "samplers.hlsli"

static const float PI = 3.1415926535897;

TextureCube<float4> skyTexture : register(t0);

cbuffer IblBuffer : register(b0) {
  float4x4 g_viewMatInv;
  float4 g_normal;
  float g_cubemapSize;
  float3 IblBufferPadding0;
};

struct VSOutput {
  float4 position : SV_POSITION;
  float3 cameraToPixelDir : TEXEL_DIRECTION;
  float2 uv : UV;
  nointerpolation float3 normal : NORMAL;
};

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  VSOutput result;
  result.normal = g_normal.xyz;

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 1.0, 1.0);
    result.cameraToPixelDir = mul(float4(-1.0f, 3.0f, 1.0f, 0.0f), g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_frustumTL, g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_viewMatInv, g_frustumTL).xyz;
    //result.cameraToPixelDir = g_frustumTL.xyz;
    result.uv = float2(0, 2);
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 1.0, 1.0);
    result.cameraToPixelDir = mul(float4(3.0, -1.0, 1.0, 0.0), g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_frustumBR, g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_viewMatInv, g_frustumBR).xyz;
    //result.cameraToPixelDir = g_frustumBR.xyz;
    result.uv = float2(2, 0);
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 1.0, 1.0);
    result.cameraToPixelDir = mul(float4(-1.0, -1.0, 1.0, 0.0), g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_frustumBL, g_viewMatInv).xyz;
    //result.cameraToPixelDir = mul(g_viewMatInv, g_frustumBL).xyz;
    //result.cameraToPixelDir = g_frustumBL.xyz;
    result.uv = float2(0, 0);
    return result;
  }

  return result;
}

// Fibonacci hemisphere point uniform distribution
float3 RandomHemisphere(out float NdotV, float i, float N) {
  const float GOLDEN_RATIO = (1.0 + sqrt(5.0)) / 2.0;
  float theta = 2.0 * PI * i / GOLDEN_RATIO;
  float phiCos = NdotV = 1.0 - (i + 0.5) / N;
  float phiSin = sqrt(1.0 - phiCos * phiCos);
  float thetaCos, thetaSin;
  sincos(theta, thetaSin, thetaCos);
  return float3(thetaCos * phiSin, thetaSin * phiSin, phiCos);
}

// Schlick's approximation of Fresnel reflectance
float3 Fresnel(float NoL, float3 F0) {
  return F0 + (1 - F0) * pow(1 - NoL, 5);
}

// Frisvad with z == -1 problem avoidance
void BasisFromDir(out float3 right, out float3 top, in float3 dir)
{
  float k = 1.0 / max(1.0 + dir.z, 0.00001);
  float a =  dir.y * k;
  float b =  dir.y * a;
  float c = -dir.x * a;
  right = float3(dir.z + b, c, -dir.x);
  top = float3(c, 1.0 - b, -dir.y);
}

// Frisvad with z == -1 problem avoidance
float3x3 BasisFromDir(float3 dir)
{
  float3x3 rotation;
  rotation[2] = dir;
  BasisFromDir(rotation[0], rotation[1], dir);
  return rotation;
}

// Determing which mip level to read in cubemap sampling with uniform/importance sampling
float HemisphereMip(float sampleProbability, float cubemapSize)
{
  float hemisphereTexels = cubemapSize * cubemapSize * 3;
  float log4 = 0.5 * log2(sampleProbability * hemisphereTexels);
  return log4;
}

static const uint kSamples = 1000000;
//static const float kProbability = 1 / (2 * PI);
static const float kProbability = 1 / kSamples;

float4 PSMain(VSOutput input) : SV_TARGET {
  float3 light = 0;
  float3 normal = normalize(input.cameraToPixelDir);
  float3x3 basis = BasisFromDir(normal);

  for (uint i = 0; i < kSamples; ++i) {
    float NdotV;
    float3 lightDir = mul(RandomHemisphere(NdotV, i, kSamples), basis);

    float3 irradiance = skyTexture.SampleLevel(g_pointWrap, lightDir, HemisphereMip(kProbability, g_cubemapSize));
    float NoL = max(dot(g_normal, lightDir), 0.01);

    light += ((irradiance * NdotV) / PI) * (1 - Fresnel(NoL, 0.04));
  }

  light *= (2 * PI) / kSamples;
  //light /= (2 * PI);

  //float3 color = normalize(input.cameraToPixelDir) * 0.5 + 0.5;
  //float3 color = g_normal * 0.5 + 0.5;
  //return float4(color.r, color.g, color.b, 1.0);
  //return float4(1.0, 0.0, 0.0, 1.0);

  //return skyTexture.Sample(g_anisotropicWrap, input.cameraToPixelDir);
  return float4(light, 1.0);
}
