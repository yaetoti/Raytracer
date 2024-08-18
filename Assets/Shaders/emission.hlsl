#include "globals.hlsl"

struct VSInput
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4x4 modelMatrix : MODEL;
  float3 emission : EMISSION;
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
  nointerpolation float3 emission : EMISSION;
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
  result.emission = input.emission;
  return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
  if (g_isNormalVisMode)
  {
    float4 color = float4((input.normal + 1.0.rrr) * 0.5.rrr, 0.0);
    return color;
  }

  float3 normedEmission = input.emission / max(input.emission.x, max(input.emission.y, max(input.emission.z, 1.0)));
  float3 cameraDir = normalize(-input.positionCameraCentered);

  float NoV = dot(cameraDir, input.normal);
  return float4(lerp(normedEmission * 0.33, input.emission, pow(max(0.0, NoV), 4.0)), 1.0);


  //return float4(1.0, 1.0, 1.0, 1.0);
}
