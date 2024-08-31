Texture2DMS<float4> targetTexture : register(t0);

cbuffer ResolveBuffer : register(b0) {
  float g_evFactor;
};

struct VSOutput {
  float4 position : SV_POSITION;
  float2 uv : UV;
};

VSOutput VSMain(uint vertexId : SV_VERTEXID) {
  VSOutput result;

  // TopLeft
  if (vertexId == 0) {
    result.position = float4(-1.0, 3.0, 0.0, 1.0);
    result.uv = float2(0, -1);
    return result;
  }

  // BottomRight
  if (vertexId == 1) {
    result.position = float4(3.0, -1.0, 0.0, 1.0);
    result.uv = float2(2, 1);
    return result;
  }

  // BottomLeft
  if (vertexId == 2) {
    result.position = float4(-1.0, -1.0, 0.0, 1.0);
    result.uv = float2(0, 1);
    return result;
  }

  return result;
}

float3 acesHdr2Ldr(float3 hdr) {
  float3x3 m1 = float3x3(
    float3(0.59719f, 0.07600f, 0.02840f),
    float3(0.35458f, 0.90834f, 0.13383f),
    float3(0.04823f, 0.01566f, 0.83777f)
  );
  float3x3 m2 = float3x3(
    float3(1.60475f, -0.10208, -0.00327f),
    float3(-0.53108f, 1.10813, -0.07276f),
    float3(-0.07367f, -0.00605, 1.07602f)
  );

  float3 v = mul(hdr, m1);
  float3 a = v * (v + float3(0.0245786f, 0.0245786f, 0.0245786f)) - float3(0.000090537f, 0.000090537f, 0.000090537f);
  float3 b = v * (float3(0.983729f, 0.983729f, 0.983729f) * v + float3(0.4329510f, 0.4329510f, 0.4329510f)) + float3(0.238081f, 0.238081f, 0.238081f);
  float3 ldr = saturate(mul(a / b, m2));

  return ldr;
}

float3 adjustExposure(float3 color, float EV100) {
  float LMax = (78.0f / (0.65f * 100.0f)) * pow(2.0f, EV100);
  return color * (1.0f / LMax);
}

float3 correctGamma(float3 color, float gamma) {
  return pow(color, 1.0f / gamma);
}


float4 PSMain(VSOutput input) : SV_TARGET {
  float3 color = 0.0.xxx;
  uint width;
  uint height;
  uint samples;
  targetTexture.GetDimensions(width, height, samples);

  for (uint sample = 0; sample < samples; ++sample) {
	  color += targetTexture.Load(input.uv * float2(width, height), sample).xyz;
  }

  color /= samples;

  color = acesHdr2Ldr(color);
  color = adjustExposure(color, g_evFactor);
  color = correctGamma(color, 2.2);
  return float4(color, 1.0);
}
