#include "samplers.hlsli"

static const float PI = 3.1415926535897;

TextureCube<float4> skyTexture : register(t0);

cbuffer IblBuffer : register(b0) {
  float4 g_normal;
  float g_roughness;
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
    result.cameraToPixelDir = mul(basisInv, float3(-1.0f, 3.0f, 1.0f));
    result.uv = float2(0, 2);
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 1.0, 1.0);
    result.cameraToPixelDir = mul(basisInv, float3(3.0, -1.0, 1.0));
    result.uv = float2(2, 0);
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 1.0, 1.0);
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

float RandomVanDeCorput(uint bits) {
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// random2D from random1D
float2 RandomHammersley(float i, float N) {
  return float2(i / N, RandomVanDeCorput(i));
}

// GGX importance sampling, returns microsurface normal (half-vector)
// rough4 is initial roughness value in power of 4
float3 RandomGGX(float2 random, float rough4) {
  float phi = 2.0 * PI * random.x;
  float cosTheta = sqrt((1.0 - random.y) / (1.0 + (rough4 - 1.0) * random.y));
  float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

  float3 dir;
  dir.x = cos(phi) * sinTheta;
  dir.y = sin(phi) * sinTheta;
  dir.z = cosTheta;
  return dir;
}

// GGX normal distribution,
// Real-Time Rendering 4th Edition, page 340, equation 9.41
float Ndf(float rough2, float NoH) {
  float denom = NoH * NoH * (rough2 * rough2 - 1.0) + 1.0;
  denom = PI * denom * denom;
  return rough2 / denom;
}

// Input = reflectedDir
float4 PSMain(VSOutput input) : SV_TARGET {
  float3 light = 0;
  float probability = 1.0 / g_samples;
  float3 viewDir = normalize(input.cameraToPixelDir);
  float3 normal = viewDir;
  float3x3 basisInv = transpose(BasisFromDir(normal.xyz));

  uint samples = 0;
  for (uint i = 0; i < g_samples; ++i) {
    float3 halfVector = normalize(mul(basisInv, RandomGGX(RandomHammersley(i, g_samples), pow(g_roughness, 4))));
    float3 lightDir = -reflect(viewDir, halfVector);

    if (dot(normal, lightDir) >= 0.000001) {
      ++samples;

      float NoH = dot(normal, halfVector);
      float ndf = Ndf(g_roughness * g_roughness, NoH);
      float sampleProbability = (2 * probability) / (PI * ndf);
      light += skyTexture.SampleLevel(g_linearWrap, lightDir, HemisphereMip(sampleProbability, g_cubemapSize));
    }
  }

  light /= samples;

  return float4(light, 1.0);
}
