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

cbuffer ConstantBuffer : register(b0)
{
  float g_time; // 0-3
  float4 g_resolution; // 16-31
};

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
  return float4(input.position.rg / 255.0, 0.0, 1.0);
}

#endif

