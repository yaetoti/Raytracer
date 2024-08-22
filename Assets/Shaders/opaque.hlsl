#include "globals.hlsl"

Texture2D<float4> albedoTexture : register(t0);
Texture2D<float4> normalTexture : register(t1);
Texture2D<float4> metallicTexture : register(t2);
Texture2D<float4> roughnessTexture : register(t3);

// TODO generate metallic
// TODO change layout

struct VSInput
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
  float2 uv : TEXCOORD;

  // Instance buffer
  float4x4 modelMatrix : MODEL;
};

struct VSOutput
{
  float4x4 modelMatrix : MODEL;

  float4 positionLocal : POSITION_LOCAL;
  float4 positionWorld : POSITION_WORLD;
  float4 positionCameraCentered : POSITION_CAMERA_CENTERED;
  float4 positionProj : SV_POSITION;

  float3 normal : NORMAL;
  float3 normalLocal : NORMAL_LOCAL;

  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
  float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
  VSOutput result;
  // Position
  result.positionLocal = mul(g_meshToModel, float4(input.position, 1.0));
  result.positionWorld = mul(input.modelMatrix, result.positionLocal);
  result.positionCameraCentered = result.positionWorld - float4(g_cameraPosition.xyz, 0.0);
  result.positionProj = mul(g_projectionMatrix, mul(g_viewMatrix, result.positionWorld));

  // Normal
  result.normalLocal = mul(g_meshToModel, float4(input.normal, 0.0)).xyz;

  float3 axisX = normalize(input.modelMatrix[0].xyz);
  float3 axisY = normalize(input.modelMatrix[1].xyz);
  float3 axisZ = normalize(input.modelMatrix[2].xyz);
  float3 worldN = result.normalLocal.x * axisX + result.normalLocal.y * axisY + result.normalLocal.z * axisZ;
  
  result.modelMatrix = input.modelMatrix;
  result.normal = worldN;

  result.tangent = mul(g_meshToModel, float4(input.tangent, 0.0)).xyz;
  result.bitangent = mul(g_meshToModel, float4(input.bitangent, 0.0)).xyz;
  result.uv = input.uv;
  return result;
}

float2 GetSpotLightUv(float4 pointWS, float4x4 lightMat, float lightAngleCos) {
  float4 pointVS = mul(lightMat, pointWS);

  // Now goes projection
  float cosAbs = abs(lightAngleCos);
  float sinAbs = sqrt(1 - cosAbs * cosAbs);
  float ctgAbs = cosAbs / sinAbs;

  float nearPlane = 0.001f;
  float farPlane = pointVS.z;
  float4x4 perspective = float4x4(
    ctgAbs, 0, 0, 0,
    0, ctgAbs, 0, 0,
    0, 0, -(farPlane + nearPlane) / (farPlane - nearPlane), -2.0f * farPlane * nearPlane / (farPlane - nearPlane),
    0, 0, -1.0, 0.0f
  );
  
  float4 pointCS = mul(perspective, pointVS);
  float4 pointNDC = pointCS / pointCS.w;
  float2 pointUV = pointNDC.xy * 0.5 + 0.5;
  return pointUV;
}

// Schlick's approximation of Fresnel reflectance
float3 Fresnel(float NoL, float3 F0)
{
	return F0 + (1 - F0) * pow(1 - NoL, 5);
}

// Height-correlated Smith G2 for GGX,
// Filament, 4.4.2 Geometric shadowing
float Gmf(float rough, float NoV, float NoL)
{
	NoV *= NoV;
	NoL *= NoL;
  float rough4 = pow(rough, 4);
	return 2.0 / (sqrt(1 + rough4 * (1 - NoV) / NoV) + sqrt(1 + rough4 * (1 - NoL) / NoL));
}

// GGX normal distribution,
// Real-Time Rendering 4th Edition, page 340, equation 9.41
float Ndf(float rough, float NoH)
{
	float denom = NoH * NoH * (pow(rough, 4) - 1.0) + 1.0;
	denom = PI * denom * denom;
	return pow(rough, 2) / denom;
}

float SolidAngle(float radius, float distance) {
  return 1 - sqrt(1 - pow(radius / distance, 2));
}

float4 PSMain(VSOutput input) : SV_TARGET
{
  if (g_isNormalVisMode)
  {
    float4 color = float4((input.normal + 1.0.rrr) * 0.5.rrr, 0.0);
    return color;
  }

  float3 light = 0.0;
  float3 viewDir = normalize(g_cameraPosition.xyz - input.positionWorld.xyz);

  // a albedo
  // m metallicity
  // n normal

  // Direct light
  // for (uint i = 0; i < g_directLightsCount; ++i) {
  //   float3 radiance = g_directLights[i].radiance;
  //   float3 lightDir = -g_directLights[i].direction.xyz;
  //   float solidAngle = g_directLights[i].solidAngle;

  //   // l lightDir

  //   light += g_directLights[i].radiance * ();

  //   float4 diffuse = g_directLights[i].color * saturate(dot(-input.normal, g_directLights[i].direction.xyz));
  //   float3 halfReflect = normalize(g_directLights[i].direction + viewDir);
  //   float4 specular = g_directLights[i].color * pow(max(dot(input.normal, halfReflect), 0.0f), input.specularExponent);
  //   light += g_directLights[i].intensity * (diffuse + specular);
  // }

  // // Point light
  // for (uint i = 0; i < g_pointLightsCount; ++i) {
  //   float3 lightPosCameraCentered = g_pointLights[i].position.xyz - g_cameraPosition.xyz;
  //   float3 lightVec = lightPosCameraCentered - input.positionCameraCentered.xyz;

  //   float lightDistance = length(lightVec);
  //   float3 lightDir = normalize(lightVec);

  //   float attenuation =  1.0f / (g_pointLights[i].constantFadeoff
  //     + g_pointLights[i].linearFadeoff * lightDistance
  //     + g_pointLights[i].quadraticFadeoff * lightDistance * lightDistance);
    
  //   float4 diffuse = g_pointLights[i].color * saturate(dot(input.normal, lightDir));
  //   float3 halfReflect = normalize(lightDir + viewDir);
  //   float4 specular = g_pointLights[i].color * pow(max(dot(input.normal, halfReflect), 0.0f), input.specularExponent);
  //   light += g_pointLights[i].intensity * attenuation * (diffuse + specular);
  // }

  // // Spot light
  // for (uint i = 0; i < g_spotLightsCount; ++i) {
  //   float3 lightPosCameraCentered = g_spotLights[i].position.xyz - g_cameraPosition.xyz;
  //   float3 lightVec = lightPosCameraCentered - input.positionCameraCentered.xyz;

  //   float lightDistance = length(lightVec);
  //   float3 lightDir = normalize(lightVec);

  //   float theta = dot(lightDir, -g_spotLights[i].direction);
  //   float epsilon = g_spotLights[i].cutoffCosineInner - g_spotLights[i].cutoffCosineOuter;
  //   float intensity = saturate((theta - g_spotLights[i].cutoffCosineOuter) / epsilon);

  //   float attenuation =  1.0f / (g_spotLights[i].constantFadeoff
  //     + g_spotLights[i].linearFadeoff * lightDistance
  //     + g_spotLights[i].quadraticFadeoff * lightDistance * lightDistance);

  //   float2 textureUV = GetSpotLightUv(input.positionWorld, g_spotLights[i].lightMat, g_spotLights[i].cutoffCosineOuter);
  //   float4 textureColor = texture0.Sample(g_anisotropicWrap, textureUV);

  //   float4 diffuse = g_spotLights[i].color * saturate(dot(input.normal, lightDir));
  //   float3 halfReflect = normalize(lightDir + viewDir);
  //   float4 specular = g_spotLights[i].color * pow(max(dot(input.normal, halfReflect), 0.0f), input.specularExponent);
  //   light += g_spotLights[i].intensity * intensity * attenuation * (diffuse + specular) * textureColor;
  // }

  //light = float3(1.0, 1.0, 1.0) * dot(input.normal, viewDir);
  light = albedoTexture.Sample(g_anisotropicWrap, input.uv).xyz * dot(input.normal, viewDir);
  return float4(light, 1.0);
}
