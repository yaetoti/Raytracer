#include "globals.hlsl"

struct VSInput
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4x4 modelMatrix : MODEL;
};

struct VSOutput
{
  float4x4 modelMatrix : MODEL;
  float3 positionLocal : POSITION_LOCAL;
  float4 positionWorld : POSITION_WORLD;
  float4 positionCameraCentered : POSITION_CAMERA_CENTERED;
  float4 positionProj : SV_POSITION;
  float3 normal : NORMAL;
  float3 normalLocal : NORMAL_LOCAL;
};

VSOutput VSMain(VSInput input)
{
  VSOutput result;
  result.positionLocal = input.position;
  result.positionWorld = mul(input.modelMatrix, float4(input.position, 1.0));
  result.positionCameraCentered = result.positionWorld - float4(g_cameraPosition.xyz, 0.0);
  result.positionProj = mul(g_projectionMatrix, mul(g_viewMatrix, result.positionWorld));

  float3 axisX = normalize(input.modelMatrix[0].xyz);
  float3 axisY = normalize(input.modelMatrix[1].xyz);
  float3 axisZ = normalize(input.modelMatrix[2].xyz);
  float3 worldN = input.normal.x * axisX + input.normal.y * axisY + input.normal.z * axisZ;
  
  result.modelMatrix = input.modelMatrix;
  result.normalLocal = input.normal;
  result.normal = worldN;
  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
  if (g_isNormalVisMode)
  {
    float4 color = float4((input.normal + 1.0.rrr) * 0.5.rrr, 0.0);
    return color;
  }

  float4 light = float4(0.0, 0.0, 0.0, 0.0);

  // Direct light
  for (uint i = 0; i < g_directLightsCount; ++i) {
    float4 diffuse = g_directLights[i].color * saturate(dot(-input.normal, g_directLights[i].direction.xyz));
    float4 specular = float4(0.0, 0.0, 0.0, 0.0);
    light += g_directLights[i].intensity * (diffuse + specular);
  }

  // Point light
  for (uint i = 0; i < g_pointLightsCount; ++i) {
    float3 lightPosCameraCentered = g_pointLights[i].position.xyz - g_cameraPosition.xyz;
    float3 lightVec = lightPosCameraCentered - input.positionCameraCentered.xyz;

    float lightDistance = length(lightVec);
    float3 lightDir = normalize(lightVec);

    float attenuation =  1.0f / (g_pointLights[i].constantFadeoff
      + g_pointLights[i].linearFadeoff * lightDistance
      + g_pointLights[i].quadraticFadeoff * lightDistance * lightDistance);
    
    float4 diffuse = g_pointLights[i].color * saturate(dot(input.normal, lightDir));
    float4 specular = float4(0.0, 0.0, 0.0, 0.0);
    light += g_pointLights[i].intensity * attenuation * (diffuse + specular);
  }

  return light;
}
