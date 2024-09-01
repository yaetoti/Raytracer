static const float PI = 3.1415926535897;

cbuffer IblBuffer : register(b0) {
  uint g_samples;
  float3 g_padding0;
};

struct VSOutput {
  float4 position : SV_POSITION;
  float2 uv : UV;
};

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  VSOutput result;

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 1.0, 1.0);
    result.uv = float2(0, 2);
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 1.0, 1.0);
    result.uv = float2(2, 0);
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 1.0, 1.0);
    result.uv = float2(0, 0);
    return result;
  }

  return result;
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

// Height-correlated Smith G2 for GGX,
// Filament, 4.4.2 Geometric shadowing
float Gmf(float rough4, float NoV, float NoL) {
  NoV *= NoV;
  NoL *= NoL;
  return 2.0 / (sqrt(1 + rough4 * (1 - NoV) / NoV) * sqrt(1 + rough4 * (1 - NoL) / NoL));
}

float2 PSMain(VSOutput input) : SV_TARGET {
  float NoV = input.uv.y;
  float roughness = input.uv.x;
  float roughness2 = roughness * roughness;
  float roughness4 = roughness2 * roughness2;

  float3 viewDir = float3(sqrt(1.0 - NoV * NoV), 0, NoV);
  float3 normal = float3(0, 0, 1);
  float2 result = float2(0.0, 0.0);

  float samples = 0.0;
  for (uint i = 0; i < g_samples; ++i) {
    float3 halfVector = RandomGGX(RandomHammersley(i, g_samples), roughness4);
    // Reflect saves vector's side direction, so we need '-' and it's actually pointing to light
    float3 lightDir = -reflect(viewDir, halfVector);
    float NoH = dot(normal, halfVector);
    float NoL = dot(normal, lightDir);
    float HoV = dot(halfVector, viewDir);

    if (NoL > 0.000001 && HoV > 0.000001) {
      samples += 1;
      float gmf = Gmf(roughness4, NoV, NoL);
      //float gmf = GeometrySmith(normal, viewDir, lightDir, roughness);
      result.r += (gmf * (1 - pow(1 - HoV, 5)) * HoV) / (NoV * NoH);
      result.g += (gmf * pow(1 - HoV, 5) * HoV) / (NoV * NoH);
    }
  }

  result.r /= samples;
  result.g /= samples;

  return result;
}
