#include "globals.hlsl"

Texture2D<float4> lightTexture : register(t0);
Texture2D<float4> albedoTexture : register(t1);
Texture2D<float4> normalTexture : register(t2);
Texture2D<float4> metallicTexture : register(t3);
Texture2D<float4> roughnessTexture : register(t4);

TextureCube<float4> diffuseTexture : register(t5);
TextureCube<float4> specularTexture : register(t6);
Texture2D<float2> reflectanceTexture : register(t7);

struct VSInput {
  // MeshSpace
  float3 position : POSITION;
  // MeshSpace
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
  float2 uv : TEXCOORD;

  // Instance buffer
  float4x4 modelMatrix : MODEL;
};

struct VSOutput
{
  // WorldSpace
  float3x3 tbn : TBN;

  float4 positionLocal : POSITION_LOCAL;
  float4 positionWorld : POSITION_WORLD;
  float4 positionCameraCentered : POSITION_CAMERA_CENTERED;
  float4 positionProj : SV_POSITION;

  float3 normalLocal : NORMAL_LOCAL;
  float3 normalWorld : NORMAL_WORLD;

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
  result.normalLocal = normalize(mul((float3x3)g_meshToModel, input.normal));
  result.normalWorld = normalize(mul((float3x3)input.modelMatrix, result.normalLocal));

  result.tangent = normalize(mul((float3x3)input.modelMatrix, mul((float3x3)g_meshToModel, input.tangent)));
  result.bitangent = normalize(mul((float3x3)input.modelMatrix, mul((float3x3)g_meshToModel, input.bitangent)));
  result.uv = input.uv;

  // Despite matrices are col_major, per-element constructor is always row-major
  result.tbn = float3x3(
    result.tangent.x, result.bitangent.x, result.normalWorld.x,
    result.tangent.y, result.bitangent.y, result.normalWorld.y,
    result.tangent.z, result.bitangent.z, result.normalWorld.z
  );

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
float3 Fresnel(float NoL, float3 F0) {
	return F0 + (1 - F0) * pow(1 - NoL, 5);
}

// Height-correlated Smith G2 for GGX,
// Filament, 4.4.2 Geometric shadowing
float Gmf(float rough, float NoV, float NoL) {
  NoV *= NoV;
  NoL *= NoL;
  float rough4 = pow(rough, 4);
  return 2.0 / (sqrt(1 + rough4 * (1 - NoV) / NoV) * sqrt(1 + rough4 * (1 - NoL) / NoL));
}

// GGX normal distribution,
// Real-Time Rendering 4th Edition, page 340, equation 9.41
float Ndf(float rough, float NoH) {
  float denom = NoH * NoH * (pow(rough, 4) - 1.0) + 1.0;
  denom = PI * denom * denom;
  return pow(rough, 2) / denom;
}

float SolidAngle(float radius, float distance) {
  return 1 - sqrt(1 - pow(radius / distance, 2));
}

// May return direction pointing beneath surface horizon (dot(N, dir) < 0), use clampDirToHorizon to fix it.
// sphereCos is cosine of the light source angular halfsize (2D angle, not solid angle).
// sphereRelPos is position of a sphere relative to surface:
// 'sphereDir == normalize(sphereRelPos)' and 'sphereDir * sphereDist == sphereRelPos'
float3 ApproximateClosestSphereDir(out bool intersects, float3 reflectionDir, float sphereCos,
  float3 sphereRelPos, float3 sphereDir, float sphereDist, float sphereRadius)
{
  float RoS = dot(reflectionDir, sphereDir);

  intersects = RoS >= sphereCos;
  if (intersects) {
    return reflectionDir;
  }
  if (RoS < 0.0) {
    return sphereDir;
  }

  float3 closestPointDir = normalize(reflectionDir * sphereDist * RoS - sphereRelPos);
  return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

// Input dir and NoD is N and NoL in a case of lighting computation
void ClampDirToHorizon(inout float3 dir, inout float NoD, float3 normal, float minNoD) {
  if (NoD < minNoD) {
    dir = normalize(dir + (minNoD - NoD) * normal);
    NoD = minNoD;
  }
}

float Falloff(float3 surfaceNormal, float3 sphereVec, float sphereDistance, float sphereRadius) {
  float height = max(dot(surfaceNormal, sphereVec), -sphereRadius);
  return min((sphereRadius + height) / (2 * sphereRadius), 1);
}

// [ de Carpentier 2017, "Decima Engine: Advances in Lighting and AA" ]
// sphereSin and sphereCos are sin and cos of the light source angular halfsize (2D angle, not solid angle).
void SphereMaxNoH(float NoV, inout float NoL, inout float VoL, float sphereSin, float sphereCos, bool bNewtonIteration, out float NoH, out float VoH) {
  float RoL = 2 * NoL * NoV - VoL;
  if( RoL >= sphereCos )
  {
    NoH = 1;
    VoH = abs( NoV );
  }
  else
  {
    float rInvLengthT = sphereSin * rsqrt( 1 - RoL*RoL );
    float NoTr = rInvLengthT * ( NoV - RoL * NoL );
    float VoTr = rInvLengthT * ( 2 * NoV*NoV - 1 - RoL * VoL );

    if (bNewtonIteration && sphereSin != 0)
    {
      // dot( cross(N,L), V )
      float NxLoV = sqrt( saturate( 1 - pow(NoL, 2) - pow(NoV, 2) - pow(VoL, 2) + 2 * NoL * NoV * VoL ) );

      float NoBr = rInvLengthT * NxLoV;
      float VoBr = rInvLengthT * NxLoV * 2 * NoV;

      float NoLVTr = NoL * sphereCos + NoV + NoTr;
      float VoLVTr = VoL * sphereCos + 1   + VoTr;

      float p = NoBr   * VoLVTr;
      float q = NoLVTr * VoLVTr;
      float s = VoBr   * NoLVTr;

      float xNum = q * ( -0.5 * p + 0.25 * VoBr * NoLVTr );
      float xDenom = p*p + s * (s - 2*p) + NoLVTr * ( (NoL * sphereCos + NoV) * pow(VoLVTr, 2) + q * (-0.5 * (VoLVTr + VoL * sphereCos) - 0.5) );
      float TwoX1 = 2 * xNum / ( pow(xDenom, 2) + pow(xNum, 2) );
      float SinTheta = TwoX1 * xDenom;
      float CosTheta = 1.0 - TwoX1 * xNum;
      NoTr = CosTheta * NoTr + SinTheta * NoBr;
      VoTr = CosTheta * VoTr + SinTheta * VoBr;
    }

    NoL = NoL * sphereCos + NoTr;
    VoL = VoL * sphereCos + VoTr;

    float InvLenH = rsqrt( 2 + 2 * VoL );
    NoH = saturate( ( NoL + NoV ) * InvLenH );
    VoH = saturate( InvLenH + InvLenH * VoL );
  }
}

float4 PSMain(VSOutput input) : SV_TARGET
{
  //float3 albedo = albedoTexture.Sample(g_linearWrap, input.uv).xyz;
  float3 albedo = float3(1, 0, 0);
  float3 normal = normalTexture.Sample(g_linearWrap, input.uv).xyz;
  float metallic = metallicTexture.Sample(g_linearWrap, input.uv).x;
  float roughness = roughnessTexture.Sample(g_linearWrap, input.uv).x;

  //normal.y = -normal.y;
  normal = normal * 2.0 - 1.0;
  normal = normalize(mul(input.tbn, normal));
  //normal = -normal;

  if (g_isNormalVisMode) {
    float4 color = float4((normal + 1.0.rrr) * 0.5.rrr, 0.0);
    return color;
  }

  float3 light = 0.0;
  float3 F0 = lerp(0.04.xxx, albedo, metallic);
  float3 viewDir = normalize(g_cameraPosition.xyz - input.positionWorld.xyz);
  float NoV = dot(normal, viewDir);

  // Direct light
  for (uint i = 0; i < g_directLightsCount; ++i) {
    float3 lightDir = -g_directLights[i].direction.xyz;
    float solidAngle = g_directLights[i].solidAngle;
    float3 halfReflect = normalize(lightDir + viewDir);

    float NoH = max(dot(normal, halfReflect), 0.001);
    float NoL = max(dot(normal, lightDir), 0.001);
    float HoL = max(dot(halfReflect, lightDir), 0.001);

    float3 diffuse = ((solidAngle * albedo * (1 - metallic)) / PI) * (1 - Fresnel(NoL, F0)) * NoL;
    float3 specular = min(1, (solidAngle * Ndf(roughness, NoH)) / (4 * NoV)) * Gmf(roughness, NoV, NoL) * Fresnel(HoL, F0);

    //light += g_directLights[i].radiance * (diffuse + specular);
  }

  // Point light
  for (uint i = 0; i < g_pointLightsCount; ++i) {
    float3 lightVec = g_pointLights[i].position.xyz - input.positionWorld.xyz;
    float3 lightDir = normalize(lightVec);
    float lightDistance = length(lightVec);
    float NoL = dot(normal, lightDir);
    float solidAngle = SolidAngle(g_pointLights[i].radius, lightDistance);

    // https://en.wikipedia.org/wiki/Solid_angle
    // Cos half 2D angle
    float sphereCos = (2 * PI - solidAngle) / (2 * PI);
    // cos2a = 2 cos^2(a) - 1
    sphereCos = 2 * sphereCos * sphereCos - 1;
    float sphereSin = sqrt(1 - sphereCos * sphereCos);

    //float3 ApproximateClosestSphereDir(out bool intersects, float3 reflectionDir, float sphereCos, float3 sphereRelPos, float3 sphereDir, float sphereDist, float sphereRadius)
    //bool intersects;
    //lightDir = ApproximateClosestSphereDir(intersects, -reflect(viewDir, normal), sphereCos, lightVec, lightDir, lightDistance, g_pointLights[i].radius);
    //NoL = dot(normal, lightDir);
    //if (intersects) {
    //  continue;
    //}

    // If a sphere is lower that surface
    ClampDirToHorizon(lightDir, NoL, normal, 0.0);

    float3 diffuse = 0.0.xxx;
    if (NoL >= 0.0) {
      diffuse += ((solidAngle * albedo * (1 - metallic)) / PI) * (1 - Fresnel(NoL, F0)) * NoL;
    }
    //void ClampDirToHorizon(inout float3 dir, inout float NoD, float3 normal, float minNoD)

    float VoL = dot(viewDir, lightDir);
    float NoH = dot(normal, (viewDir + lightDir) / 2);
    float VoH = dot(viewDir, (viewDir + lightDir) / 2);;

    float falloffMicro = Falloff(normal, lightVec, lightDistance, g_pointLights[i].radius);
    float falloffMacro = Falloff(input.normalWorld, lightVec, lightDistance, g_pointLights[i].radius);
    //NoL = max(NoL, falloffMicro * sphereSin);

    SphereMaxNoH(
      NoV,
      NoL,
      VoL,
      sphereSin,
      sphereCos,
      true,
      NoH,
      VoH
    );


    float3 specular = 0.0.xxx;


    if (NoV >= 0.0 && NoL >= 0.0) {
      specular += min(1, (solidAngle * Ndf(roughness * roughness, NoH)) / (4 * NoV)) * Gmf(roughness * roughness, NoV, NoL) * Fresnel(VoH, F0);
    }

    //light += g_pointLights[i].radiance * (diffuse + specular);
    light += g_pointLights[i].radiance * diffuse * falloffMicro * falloffMacro;
    //light += g_pointLights[i].radiance * specular * falloffMicro * falloffMacro;
  }

  // Spot light
  for (uint i = 0; i < g_spotLightsCount; ++i) {
    float3 lightVec = g_spotLights[i].position.xyz - input.positionWorld.xyz;
    float3 lightDir = normalize(lightVec);
    float lightDistance = length(lightVec);
    float solidAngle = SolidAngle(g_spotLights[i].radius, lightDistance);

    float NoL = dot(normal, lightDir);
    float VoL = dot(viewDir, lightDir);
    float NoH;
    float VoH;

    float sphereCos = 1 - 0.5 * solidAngle / PI;
    float sphereSin = sqrt(1 - sphereCos * sphereCos);
    SphereMaxNoH(
      NoV,
      NoL,
      VoL,
      sphereSin,
      sphereCos,
      true,
      NoH,
      VoH
    );
    float falloff = Falloff(normal, lightVec, lightDistance, g_pointLights[i].radius);
    falloff *= Falloff(input.normalWorld, lightVec, lightDistance, g_pointLights[i].radius);

    float theta = dot(lightDir, -g_spotLights[i].direction);
    float epsilon = g_spotLights[i].cutoffCosineInner - g_spotLights[i].cutoffCosineOuter;
    float intensity = saturate((theta - g_spotLights[i].cutoffCosineOuter) / epsilon);

    float2 textureUV = GetSpotLightUv(input.positionWorld, g_spotLights[i].lightViewMat, g_spotLights[i].cutoffCosineOuter);
    float4 textureColor = lightTexture.Sample(g_anisotropicWrap, textureUV);

    float3 diffuse = ((solidAngle * albedo * (1 - metallic)) / PI) * (1 - Fresnel(NoL, F0)) * NoL;
    float3 specular = min(1, (solidAngle * Ndf(roughness, NoH)) / (4 * NoV)) * Gmf(roughness, NoV, NoL) * Fresnel(VoH, F0);

    //light += g_spotLights[i].radiance * textureColor * intensity * (diffuse/* + specular*/);
    // TODO falloff
  }

  // Add IBL
  float3 diffuseReflection = albedo * (1.0 - metallic) * diffuseTexture.SampleLevel(g_linearWrap, normal, 0.0);

  // roughnessLinear is initial roughness value set by artist, not rough^2 or rough^4
  float2 reflectanceLUT = reflectanceTexture.SampleLevel(g_linearWrap, float2(roughness, NoV), 0.001);
  float3 reflectance = reflectanceLUT.r * F0 + reflectanceLUT.g;

  // TODO set in constantbuffer or vertexshader
  float width;
  float height;
  float levels;
  specularTexture.GetDimensions(0, width, height, levels);
  // v.reflectionDir - from hints. In the presentation it was stated that we need to pass the viewDir, however (that gives wrong results)

  float3 reflectedDir = -reflect(viewDir, normal);
  float NoR = dot(input.normalWorld, reflectedDir);
  ClampDirToHorizon(reflectedDir, NoR, input.normalWorld, 0.0);
  float specularReflection = reflectance * specularTexture.SampleLevel(g_linearWrap, reflectedDir, roughness * (levels - 1));

  //light += diffuseReflection;
  //light += specularReflection;

  return float4(light, 1.0);
}
