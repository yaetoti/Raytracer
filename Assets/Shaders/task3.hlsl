cbuffer ConstantBuffer : register(b0)
{
  float g_time; // 0-3
  float4 g_resolution; // 16-31
};

//#define PART1
#ifdef PART1

struct VSInput
{
  float4 position : POSITION;
  float4 color : COLOR;
};

struct VSOutput
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
  VSOutput result;
  result.position = input.position;
  result.color = input.color;
  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
  return input.color;
}

#else

struct VSInput
{
  float4 position : POSITION;
  float4 color : COLOR;
};

struct VSOutput
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
  VSOutput result;
  result.position = input.position;
  result.color = input.color;
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
  
  return blended;
}

#endif

