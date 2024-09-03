#include "samplers.hlsli"

static const float PI = 3.1415926535897;

TextureCube<float4> skyTexture : register(t0);

cbuffer IblBuffer : register(b0) {
  float4 g_normal;
  uint g_samples;
  uint g_cubemapSize;
};

struct VSOutput {
  float4 position : SV_POSITION;
  float3 cameraToPixelDir : TEXEL_DIRECTION;
  float2 uv : UV;
};

// Frisvad with z == -1 problem avoidance
void BasisFromDir(out float3 right, out float3 top, in float3 dir) {
  float k = 1.0 / max(1.0 + dir.z, 0.00001);
  float a =  dir.y * k;
  float b =  dir.y * a;
  float c = -dir.x * a;
  right = float3(dir.z + b, c, -dir.x);
  top = float3(c, 1.0 - b, -dir.y);
}

// Frisvad with z == -1 problem avoidance
float3x3 BasisFromDir(float3 dir) {
  float3x3 rotation;
  rotation[2] = dir;
  BasisFromDir(rotation[0], rotation[1], dir);
  return rotation;
}

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  VSOutput result;
  float3x3 basisInv = transpose(BasisFromDir(g_normal.xyz));

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 1.0, 1.0);
    //result.cameraToPixelDir = mul(float4(-1.0f, 3.0f, 1.0f, 0.0f), g_viewMatInv).xyz;
	// Basis is also to the right in RandomGGX formula. That's because we rotate FROM basis to WorldSpace
	// (Basis is ViewMat. We skipped projection because we are drawing in 2D)
    result.cameraToPixelDir = mul(basisInv, float3(-1.0f, 3.0f, 1.0f));
    result.uv = float2(0, 2);
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 1.0, 1.0);
    //result.cameraToPixelDir = mul(float4(3.0, -1.0, 1.0, 0.0), g_viewMatInv).xyz;
    result.cameraToPixelDir = mul(basisInv, float3(3.0, -1.0, 1.0));
    result.uv = float2(2, 0);
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 1.0, 1.0);
    //result.cameraToPixelDir = mul(float4(-1.0, -1.0, 1.0, 0.0), g_viewMatInv).xyz;
    result.cameraToPixelDir = mul(basisInv, float3(-1.0, -1.0, 1.0));
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

// Determing which mip level to read in cubemap sampling with uniform/importance sampling
float HemisphereMip(float sampleProbability, float cubemapSize) {
  float hemisphereTexels = cubemapSize * cubemapSize * 3;
  float log4 = 0.5 * log2(sampleProbability * hemisphereTexels);
  return log4;
}

// Input = micronormal
float4 PSMain(VSOutput input) : SV_TARGET {
  float3 light = 0;
  float probability = 1.0 / g_samples;
  float3 normal = normalize(input.cameraToPixelDir);
  float3x3 basisInv = transpose(BasisFromDir(normal));

  for (uint i = 0; i < g_samples; ++i) {
    float NoV;
    float3 lightDir = normalize(mul(basisInv, RandomHemisphere(NoV, i, g_samples)));
    float3 irradiance = skyTexture.SampleLevel(g_linearWrap, lightDir, HemisphereMip(probability, g_cubemapSize));
    float NoL = max(dot(g_normal, lightDir), 0.0);

    light += ((irradiance * NoV) / PI) * (1 - Fresnel(NoV, 0.04));
  }

  light *= (2 * PI) / g_samples;

  return float4(light, 1.0);
}
