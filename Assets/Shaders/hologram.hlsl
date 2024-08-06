// Data

cbuffer ConstantBuffer : register(b0)
{
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
  float4 g_resolution;
  float4 g_cameraPosition;
  float g_time;
};

struct VSInput
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4x4 modelMatrix : MODEL;
  float3 mainColor : MAIN_COLOR;
  float3 secondaryColor : SECONDARY_COLOR;
};

struct VSOutput
{
  float4x4 modelMatrix : MODEL;
  float4 position : SV_POSITION;
  float3 normal : NORMAL;
  float3 positionLocal : POSITION_LOCAL;
  float3 normalLocal : NORMAL_LOCAL;
  nointerpolation float3 mainColor : MAIN_COLOR;
  nointerpolation float3 secondaryColor : SECONDARY_COLOR;
};

// BEGIN ShaderToy https://www.shadertoy.com/view/WttcRB
float hash4d(in float4 p)
{
  p = frac(p * 0.1031);
  p += dot(p, p.zwyx + 31.32);
  return frac((p.x + p.y) * p.z - p.x * p.w);
}

float noise4d(in float4 p)
{
  float4 cell = floor(p);
  float4 local = frac(p);
  local *= local * (3.0 - 2.0 * local);

  float ldbq = hash4d(cell);
  float rdbq = hash4d(cell + float4(1.0, 0.0, 0.0, 0.0));
  float ldfq = hash4d(cell + float4(0.0, 0.0, 1.0, 0.0));
  float rdfq = hash4d(cell + float4(1.0, 0.0, 1.0, 0.0));
  float lubq = hash4d(cell + float4(0.0, 1.0, 0.0, 0.0));
  float rubq = hash4d(cell + float4(1.0, 1.0, 0.0, 0.0));
  float lufq = hash4d(cell + float4(0.0, 1.0, 1.0, 0.0));
  float rufq = hash4d(cell + float4(1.0, 1.0, 1.0, 0.0));
  float ldbw = hash4d(cell + float4(0.0, 0.0, 0.0, 1.0));
  float rdbw = hash4d(cell + float4(1.0, 0.0, 0.0, 1.0));
  float ldfw = hash4d(cell + float4(0.0, 0.0, 1.0, 1.0));
  float rdfw = hash4d(cell + float4(1.0, 0.0, 1.0, 1.0));
  float lubw = hash4d(cell + float4(0.0, 1.0, 0.0, 1.0));
  float rubw = hash4d(cell + float4(1.0, 1.0, 0.0, 1.0));
  float lufw = hash4d(cell + float4(0.0, 1.0, 1.0, 1.0));
  float rufw = hash4d(cell + 1.0);

  return lerp(lerp(lerp(lerp(ldbq, rdbq, local.x),
                       lerp(lubq, rubq, local.x),
                       local.y),

                   lerp(lerp(ldfq, rdfq, local.x),
                       lerp(lufq, rufq, local.x),
                       local.y),

                   local.z),

               lerp(lerp(lerp(ldbw, rdbw, local.x),
                       lerp(lubw, rubw, local.x),
                       local.y),

                   lerp(lerp(ldfw, rdfw, local.x),
                       lerp(lufw, rufw, local.x),
                       local.y),

                   local.z),

               local.w);
}

float noise4d(in float4 p, uniform in uint octaves)
{
  float nscale = 1.0;
  float tscale = 0.0;
  float value = 0.0;

  for (uint octave = 0.0; octave < octaves; octave++)
  {
    value += noise4d(p) * nscale;
    tscale += nscale;
    nscale *= 0.5;
    p *= 2.0;
  }

  return value / tscale;
}
// END ShaderToy https://www.shadertoy.com/view/WttcRB

float distanceIntensity(float value, float target, float fade)
{
  return min(1.0, abs(value - target) / fade);
}

float periodIntensity(float value, float period, float fade)
{
  float target = round(value / period) * period;
  return distanceIntensity(value, target, fade);
}

float wave(float3 pos, float waveInterval, float waveYSpeed, float waveThickness, uniform bool distort)
{
  if (distort)
  {
    const float WAVE_XZ_SPEED = 3.0;
    const float WAVE_DISTORTION_SIZE = 0.035;
    const float WAVE_OSCILLATING_TIME = 4;

    float distortionSign = abs(frac(g_time / WAVE_OSCILLATING_TIME) - 0.5) * 4 - 1;
    float2 distortion = sin(pos.xz / WAVE_DISTORTION_SIZE + g_time * WAVE_XZ_SPEED) * WAVE_DISTORTION_SIZE * distortionSign;
    pos.y -= (distortion.x + distortion.y);
  }

  pos.y -= g_time * waveYSpeed;

  float intensity = 1.0 - periodIntensity(pos.y, waveInterval, waveThickness);
  return intensity;
}

// Note: in HLSL global constant is marked with "static const".
// One "const" is not enough, because it will be considered to be a uniform from a constant buffer.
// In HLSL const means that the value can not be changed by the shader.
// Adding "static" to global variable means that it is not visible to application, so doesn't belong to a constant buffer.
// A local constant inside a function can be marked just with "const".
// "static" for locals preserves value during current shader thread execution.

static const float BLUE_WAVE_INTERVAL = 0.8;
static const float BLUE_WAVE_SPEED = 0.25;
static const float BLUE_WAVE_THICKNESS = 0.05;

static const float RED_WAVE_INTERVAL = 10;
static const float RED_WAVE_SPEED = 2;
static const float RED_WAVE_THICKNESS = 0.2;

// called in vertex shader
float3 vertexDistortion(float3 pos, float3 normal)
{
  float3 offset = 0.0;
  offset += normal * 0.025 * wave(pos, BLUE_WAVE_INTERVAL, BLUE_WAVE_SPEED, BLUE_WAVE_THICKNESS, true);
  offset += normal * 0.05 * wave(pos, RED_WAVE_INTERVAL, RED_WAVE_SPEED, RED_WAVE_THICKNESS, false);
  return offset;
}

// called in pixel shader
float3 colorDistortion(float3 mainColor, float3 secondaryColor, float3 pos, float3 normal)
{
  float blueWave = wave(pos, BLUE_WAVE_INTERVAL, BLUE_WAVE_SPEED, BLUE_WAVE_THICKNESS, true);
  float redWave = wave(pos, RED_WAVE_INTERVAL, RED_WAVE_SPEED, RED_WAVE_THICKNESS, false);

  float3 toCamera = normalize(g_cameraPosition.xyz - pos);
  float contourGlow = pow(1.0 - abs(dot(normal, toCamera)), 2);

    // when contourInterference is 0, ripple effect contourWave is added to contourGlow, otherwise contourWave is 1
  float contourWave = wave(pos, 0.1, 0.1, 0.05, false);
  float contourInterference = periodIntensity(g_time, 4, 1);
  contourWave = lerp(contourWave, 1.0, contourInterference);
    // when contourWave is 0.0, contourGlow becomes darker, otherwise contourGlow color is plain, without ripple
  contourGlow = lerp(contourGlow / 10, contourGlow, contourWave);

  float3 color = mainColor * min(1, contourGlow + blueWave * 0.5);
  float colorNoise = sqrt(noise4d(float4(pos, frac(g_time)) * 100, 1));
  color *= lerp(colorNoise, 1.0, contourInterference);
    
  color = lerp(color, secondaryColor, redWave * 0.25);
  return color;
}

// Vertex

VSOutput VSMain(VSInput input)
{
  VSOutput result;
  
  float3 axisX = normalize(input.modelMatrix[0].xyz);
  float3 axisY = normalize(input.modelMatrix[1].xyz);
  float3 axisZ = normalize(input.modelMatrix[2].xyz);
  float3 worldN = input.normal.x * axisX + input.normal.y * axisY + input.normal.z * axisZ;
  
  result.modelMatrix = input.modelMatrix;
  result.position = float4(input.position, 1.0);
  result.positionLocal = input.position;
  result.normal = worldN;
  result.normalLocal = input.normal;
  result.mainColor = input.mainColor;
  result.secondaryColor = input.secondaryColor;
  return result;
}

// Geometry

[maxvertexcount(3)]
void GSMain(triangle VSOutput input[3] : SV_POSITION, inout TriangleStream<VSOutput> output)
{
  //float3 normalTri = (input[0].normalLocal + input[1].normalLocal + input[2].normalLocal) / 3;
  float3 v0v1 = input[1].position.xyz - input[0].position.xyz;
  float3 v0v2 = input[2].position.xyz - input[0].position.xyz;
  float3 normalTri = normalize(cross(v0v1, v0v2));
  float3 positionTri = (input[0].position.xyz + input[1].position.xyz + input[2].position.xyz) / 3;

  for (int i = 0; i < 3; ++i)
  {
    float4 pos = input[i].position;
    //pos += float4(vertexDistortion(input[i].position, input[i].normal), 1.0);
    pos += float4(vertexDistortion(positionTri, normalTri), 0.0);
    //pos += clamp(10 - pow(5 * sin(0.5 * g_time), 2), 0.0, 10.0) * float4(normalTri, 0.0);
    
    pos = mul(input[i].modelMatrix, pos);
    input[i].position = mul(projectionMatrix, mul(viewMatrix, pos));
    
    output.Append(input[i]);
  }
  
  output.RestartStrip();
}

// Pixel

float4 PSMain(VSOutput input) : SV_TARGET
{
  float2 uv = (input.position.xy - (g_resolution.xy / 2.0)) / g_resolution.x;
    
  float4 color1 = float4(0.05, 0.05, 0.05, 1.0);
  float4 color2 = float4((input.normal + 1.0.rrr) * 0.5.rrr, 0.0);
  float4 distortedColor = float4(colorDistortion(input.mainColor, input.secondaryColor, mul(input.modelMatrix, float4(input.positionLocal, 1.0)).xyz, input.normal), 1.0);
  
  return color1 + distortedColor;
  //return distortedColor;
}
