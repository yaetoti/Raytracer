#include "globals.hlsl"

//#define PART1
#ifdef PART1

struct VSInput
{
  float3 position : POSITION;
  float3 color : COLOR;
  float2 uv : TEXCOORD;
};

struct VSOutput
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
  VSOutput result;
  result.position = float4(input.position, 1.0);
  result.color = float4(input.color, 1.0);
  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
  return input.color;
}

#else

struct VSInput
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4x4 modelMatrix : MODEL;
};

struct VSOutput
{
  float4x4 modelMatrix : MODEL;
  float4 position : SV_POSITION;
  float3 normal : NORMAL;
  float3 positionLocal : POSITION_LOCAL;
  float3 normalLocal : NORMAL_LOCAL;
};

VSOutput VSMain(VSInput input)
{
  VSOutput result;
  // RHS multiplication since matrices are column-major
  result.position = mul(g_projectionMatrix, mul(g_viewMatrix, mul(input.modelMatrix, float4(input.position, 1.0))));
  
  float3 axisX = normalize(input.modelMatrix[0].xyz);
  float3 axisY = normalize(input.modelMatrix[1].xyz);
  float3 axisZ = normalize(input.modelMatrix[2].xyz);
  float3 worldN = input.normal.x * axisX + input.normal.y * axisY + input.normal.z * axisZ;
  
  result.modelMatrix = input.modelMatrix;
  result.positionLocal = input.position;
  result.normalLocal = input.normal;
  result.normal = worldN;
  return result;
}

// Pixel

float3 palette(float d)
{
  return lerp(float3(0.2, 0.7, 0.9), float3(1.0, 0.0, 1.0), d);
}

float2 rotate(float2 p, float a)
{
  float c = cos(a);
  float s = sin(a);
  return mul(p, float2x2(c, s, -s, c));
}

float map(float3 p)
{
  for (int i = 0; i < 8; ++i)
  {
    float t = g_time * 0.2;
    p.xz = rotate(p.xz, t);
    p.xy = rotate(p.xy, t * 1.89);
    p.xz = abs(p.xz);
    p.xz -= 0.5;
  }

  return dot(sign(p), p) / 5.0;
}

float4 rm(float3 ro, float3 rd)
{
  float t = 0.0;
  float3 col = 0.0;
  float d;

  for (float i = 0.0; i < 64.0; i++)
  {
    float3 p = ro + rd * t;
    d = map(p) * 0.5;
    if (d < 0.02)
    {
      break;
    }
    if (d > 100.0)
    {
      break;
    }

    col += palette(length(p) * 0.1) / (400.0 * (d));
    t += d;
  }

  return float4(col, 1.0 / (d * 100.0));
}
 

float4 PSMain(VSOutput input) : SV_TARGET
{
  float2 uv = (input.position.xy - (g_resolution.xy / 2.0)) / g_resolution.x;
  float3 ro = float3(0.0, 0.0, -50.0);
  ro.xz = rotate(ro.xz, g_time);
  float3 cf = normalize(-ro);
  float3 cs = normalize(cross(cf, float3(0.0, 1.0, 0.0)));
  float3 cu = normalize(cross(cf, cs));
  float3 uuv = ro + cf * 3.0 + uv.x * cs + uv.y * cu;
  float3 rd = normalize(uuv - ro);
  float4 col = rm(ro, rd);
    
  float4 background = float4(0.0, 0.0, 0.0, 1.0);
  float4 blended = 1.0.rrrr - (1.0.rrrr - col) * (1.0.rrrr - background);
  
  //return blended;
  //return float4((input.normal + 1.0.rrr) * 0.5.rrr, 1.0);
  
  if (g_isNormalVisMode)
  {
    float4 color = float4((input.normal + 1.0.rrr) * 0.5.rrr, 0.0);
    return color;
  }
  else
  {
    float4 color = float4(0.95, 0.95, 0.85, 1.0);
    return color * dot(normalize(g_cameraPosition - mul(input.modelMatrix, float4(input.positionLocal, 1.0))), normalize(input.normal));
  }
}

#endif

