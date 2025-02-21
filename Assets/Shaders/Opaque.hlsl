#include "globals.hlsl"
#include "CubemapUtils.hlsli"

Texture2D<float4> lightTexture : register(t0);
Texture2D<float4> albedoTexture : register(t1);
Texture2D<float4> normalTexture : register(t2);
Texture2D<float4> metallicTexture : register(t3);
Texture2D<float4> roughnessTexture : register(t4);

TextureCube<float4> diffuseTexture : register(t5);
TextureCube<float4> specularTexture : register(t6);
Texture2D<float2> reflectanceTexture : register(t7);

Texture2DArray<float> shadowMapDirect : register(t8);
Texture2DArray<float> shadowMapSpot : register(t9);
TextureCubeArray<float> shadowMapPoint : register(t10);

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
    0, 0, (-2.0 * nearPlane) / (farPlane - nearPlane), 2.0f * farPlane * nearPlane / (farPlane - nearPlane),
    0, 0, 1.0, 0.0f
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

// https://advances.realtimerendering.com/s2017/DecimaSiggraph2017.pdf
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
  float3 albedo = albedoTexture.Sample(g_linearWrap, input.uv).xyz;
  // TODO debug
  //albedo = float3(1, 0, 0);
  float3 normal = normalTexture.Sample(g_linearWrap, input.uv).xyz;
  float metallic = metallicTexture.Sample(g_linearWrap, input.uv).x;
  float roughness = roughnessTexture.Sample(g_linearWrap, input.uv).x;

  if (g_overwriteRoughness) {
    roughness = g_roughness;
  }

  //normal.y = -normal.y;
  normal = normal * 2.0 - 1.0;
  normal = normalize(mul(input.tbn, normal));
  // TODO remove
  input.normalWorld = normalize(input.normalWorld);
  //normal = -normal;

  if (g_isNormalVisMode) {
    float4 color = float4((normal + 1.0.rrr) * 0.5.rrr, 0.0);
    return color;
  }

  float3 light = 0.0;
  float3 F0 = lerp(0.04.xxx, albedo, metallic);
  float3 viewDir = normalize(g_cameraPosition.xyz - input.positionWorld.xyz);
  float NoV = dot(normal, viewDir);
  float GNoV = dot(input.normalWorld, viewDir);

  // Direct light
  for (uint i = 0; i < g_directLightsCount; ++i) {
    float3 lightDir = -g_directLights[i].direction.xyz;
    float solidAngle = g_directLights[i].solidAngle;
    float3 halfReflect = normalize(lightDir + viewDir);

    float NoL = max(dot(normal, lightDir), 0.001);
    float GNoL = dot(input.normalWorld, lightDir);
    float NoH = max(dot(normal, halfReflect), 0.001);
    float HoL = max(dot(halfReflect, lightDir), 0.001);

    float3 diffuse = 0.0.xxx;
    float3 specular = 0.0.xxx;

    if (GNoL >= 0.0 && NoL >= 0.0) {
      diffuse = ((solidAngle * albedo * (1 - metallic)) / PI) * (1 - Fresnel(NoL, F0)) * NoL;
    }
    if (NoV >= 0.0 && NoL >= 0.0) {
      specular = min(1, (solidAngle * Ndf(roughness, NoH)) / (4 * NoV)) * Gmf(roughness, NoV, NoL) * Fresnel(HoL, F0);
    }

    const uint offsetCount = 5;
    float2 offsets[offsetCount] = {
      float2(0.0, 0.0),
      float2(0.5, 0.0),
      float2(-0.5, 0.0),
      float2(0.0, 0.5),
      float2(0.0, -0.5),
    };

    float radius = length(g_frustumTL.xyz - g_cameraPosition.xyz);
    float mapSize = 8192;
    float texelSize = 1.0 / mapSize;
    float texelSizeWorld = 2 * radius * texelSize;
    float3 offset = texelSizeWorld * sqrt(2) * 0.5 * (input.normalWorld - 0.5 * lightDir * GNoL);

    float4 positionBiased = input.positionWorld + float4(offset, 0.0);
    float4 positionPS = mul(g_directLights[i].projectionMat, mul(g_directLights[i].viewMat, positionBiased));
    positionPS /= positionPS.w;

    float visibility = 0.0;
    for (uint sampleId = 0; sampleId < offsetCount; ++sampleId) {
      float2 shadowUv = positionPS.xy * float2(0.5, -0.5) + 0.5.xx + texelSize * offsets[sampleId];
      float depth = shadowMapDirect.Sample(g_linearWrap, float3(shadowUv, i));
      // reversed z
      visibility += positionPS.z < depth ? 1.0 : 0.0;
    }
    visibility /= offsetCount;
    visibility = 1 - smoothstep(0.33, 1.0, visibility);


    // TODO fix shadows


    if (g_diffuseEnabled) {
      light += g_directLights[i].radiance * diffuse * visibility;
    }
    if (g_specularEnabled) {
      light += g_directLights[i].radiance * specular * visibility;
    }
  }

  // Point light
  for (uint i = 0; i < g_pointLightsCount; ++i) {
    float3 diffuse = 0.0.xxx;
    float3 specular = 0.0.xxx;

    float radius = g_pointLights[i].radius;
    float3 lightVec = g_pointLights[i].position.xyz - input.positionWorld.xyz;
    float3 lightDir = normalize(lightVec);
    float lightDistance = length(lightVec);
    float solidAngle = SolidAngle(radius, lightDistance);
    if (lightDistance < radius) {
      solidAngle = 2 * PI;
    }

    // https://en.wikipedia.org/wiki/Solid_angle
    // Cos half 2D angle
    float sphereCos = (2 * PI - solidAngle) / (2 * PI);
    // cos2a = 2 cos^2(a) - 1
    sphereCos = 2 * sphereCos * sphereCos - 1;
    float sphereSin = sqrt(1 - sphereCos * sphereCos);

    // Offset sphere center by radius so that we have NoL > 0 while sphere isn't completely below the horizon.
    float3 diffuseLightVec = lightVec + input.normalWorld * g_pointLights[i].radius;
    float3 diffuseLightDir = normalize(diffuseLightVec);
    float diffuseNoL = dot(normal, diffuseLightDir);
    float diffuseGNoL = dot(input.normalWorld, diffuseLightDir);

    // If we see the surface and the light is above the horizon
    if (diffuseNoL >= 0.0 && diffuseGNoL >= 0) {
      diffuse += ((solidAngle * albedo * (1 - metallic)) / PI) * (1 - Fresnel(diffuseNoL, F0)) * diffuseNoL;
    }

    // inout
    float NoL = dot(normal, lightDir);
    float GNoL = dot(input.normalWorld, lightDir);
    float VoL = dot(viewDir, lightDir);
    // out
    float NoH = dot(normal, (viewDir + lightDir) / 2);
    float VoH = dot(viewDir, (viewDir + lightDir) / 2);

    /*
    //float3 ApproximateClosestSphereDir(out bool intersects, float3 reflectionDir, float sphereCos, float3 sphereRelPos, float3 sphereDir, float sphereDist, float sphereRadius)
    bool intersects;
    lightDir = ApproximateClosestSphereDir(intersects, -reflect(viewDir, normal), sphereCos, lightVec, lightDir, lightDistance, g_pointLights[i].radius);
    NoL = dot(normal, lightDir);
    if (intersects) {
      continue;
    }
    */

    //void SphereMaxNoH(float NoV, inout float NoL, inout float VoL, float sphereSin, float sphereCos, bool bNewtonIteration, out float NoH, out float VoH)
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

    // If lightDir is pointing below the horizon
    ClampDirToHorizon(lightDir, NoL, normal, 0.0);

    // From W.Merta
    // NoL = max(NoL, falloffMicro * sphereSin);

    if (NoV >= 0.0 && NoL >= 0.0) {
      specular += min(1, (solidAngle * Ndf(roughness * roughness, NoH)) / (4 * NoV)) * Gmf(roughness * roughness, NoV, diffuseNoL) * Fresnel(VoH, F0);
    }

    float falloffMicro = Falloff(normal, lightVec, lightDistance, radius);
    float falloffMacro = Falloff(input.normalWorld, lightVec, lightDistance, radius);
    // TODO remove
    //falloffMicro = falloffMacro = 1;

    // Shadows
    float nearPlane = 0.01;
    float farPlane = 1000.0;
    float4x4 lightView = GetViewMatrix(FaceFromDir(-lightDir), g_pointLights[i].position.xyz);
    float4x4 lightProj = float4x4(
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, (-2 * nearPlane) / (farPlane - nearPlane), 2 * farPlane * nearPlane / (farPlane - nearPlane),
      0, 0, 1.0, 0.0f
    );

    // Thank you, O.Hlushchuk
    const float kShadowOffset = 0.1;
    float4 positionWithOffset = input.positionWorld + float4(lightDir * kShadowOffset, 0);
    float4 positionCS = mul(lightProj, mul(lightView, positionWithOffset));
    float comparingDepth = positionCS.z / positionCS.w;
    float linearDepth = positionCS.w;
    float texelSize = linearDepth * 2.0 / 8192;
    float3 normalOffset = input.normalWorld * texelSize;
    float4 sampleLocation = float4(-lightDir + normalOffset, i);

/*
    float4 posInLightClipSpace = mul(float4(objectWorldPos, 1.f), viewProjPointLightMatrices[cubeFaceIndex]);
    float comparingDepth = posInLightClipSpace.z / posInLightClipSpace.w;
    float linearDepth = posInLightClipSpace.w;
    float texelSize = linearDepth * 2.f / g_pointLightDSResolution;
    float3 normal_offset = map_normal * texelSize;
    float4 sampleLocation = float4(toObject + normal_offset, pointLightIndex);

    float4 positionVS = mul(lightView, input.positionWorld);
    float halfSide = positionVS.z;

    const float mapSize = 8192;
    float texelSize = 1.0 / mapSize;
    float texelSizeWorld = 2 * halfSide * texelSize;
    float3 offset = texelSizeWorld * sqrt(2) * 0.5 * (input.normalWorld - 0.9 * lightDir * GNoL);

    float4 positionBiased = input.positionWorld + float4(offset, 0.0);
    float4 positionPS = mul(lightProj, mul(lightView, positionBiased));
    positionPS /= positionPS.w;
*/

    float visibility = 0.0;
    //float depth = shadowMapPoint.Sample(g_linearWrap, float4(-lightDir, i));
    float depth = shadowMapPoint.Sample(g_linearWrap, sampleLocation);
    // reversed z
    //visibility = positionPS.z <= depth ? 1.0 : 0.0;
    visibility = comparingDepth <= depth ? 1.0 : 0.0;
    visibility = 1 - smoothstep(0.33, 1.0, visibility);

    // TODO merge
    if (g_diffuseEnabled) {
      light += g_pointLights[i].radiance * diffuse * falloffMicro * falloffMacro * visibility;
    }
    if (g_specularEnabled) {
      light += g_pointLights[i].radiance * specular * falloffMicro * falloffMacro * visibility;
    }
  }

  // Spot light
  for (uint i = 0; i < g_spotLightsCount; ++i) {
    float3 diffuse = 0.0.xxx;
    float3 specular = 0.0.xxx;

    float radius = g_spotLights[i].radius;
    float3 lightVec = g_spotLights[i].position.xyz - input.positionWorld.xyz;
    float3 lightDir = normalize(lightVec);
    float lightDistance = length(lightVec);
    float solidAngle = SolidAngle(radius, lightDistance);
    if (lightDistance < radius) {
      solidAngle = 2 * PI;
    }

    // https://en.wikipedia.org/wiki/Solid_angle
    // Cos half 2D angle
    float sphereCos = (2 * PI - solidAngle) / (2 * PI);
    // cos2a = 2 cos^2(a) - 1
    sphereCos = 2 * sphereCos * sphereCos - 1;
    float sphereSin = sqrt(1 - sphereCos * sphereCos);

    // Offset sphere center by radius so that we have NoL > 0 while sphere isn't completely below the horizon.
    float3 diffuseLightVec = lightVec + input.normalWorld * g_pointLights[i].radius;
    float3 diffuseLightDir = normalize(diffuseLightVec);
    float diffuseNoL = dot(normal, diffuseLightDir);
    float diffuseGNoL = dot(input.normalWorld, diffuseLightDir);

    // If we see the surface and the light is above the horizon
    if (diffuseNoL >= 0.0 && diffuseGNoL >= 0) {
      diffuse += ((solidAngle * albedo * (1 - metallic)) / PI) * (1 - Fresnel(diffuseNoL, F0)) * diffuseNoL;
    }

    float theta = dot(lightDir, -g_spotLights[i].direction);
    float epsilon = g_spotLights[i].cutoffCosineInner - g_spotLights[i].cutoffCosineOuter;
    float intensity = saturate((theta - g_spotLights[i].cutoffCosineOuter) / epsilon);

    float2 textureUV = GetSpotLightUv(input.positionWorld, g_spotLights[i].viewMat, g_spotLights[i].cutoffCosineOuter);
    float4 textureColor = lightTexture.Sample(g_anisotropicWrap, textureUV);

    // inout
    float NoL = dot(normal, lightDir);
    float GNoL = dot(input.normalWorld, lightDir);
    float VoL = dot(viewDir, lightDir);
    // out
    float NoH = dot(normal, (viewDir + lightDir) / 2);
    float VoH = dot(viewDir, (viewDir + lightDir) / 2);

    /*
    //float3 ApproximateClosestSphereDir(out bool intersects, float3 reflectionDir, float sphereCos, float3 sphereRelPos, float3 sphereDir, float sphereDist, float sphereRadius)
    bool intersects;
    lightDir = ApproximateClosestSphereDir(intersects, -reflect(viewDir, normal), sphereCos, lightVec, lightDir, lightDistance, g_pointLights[i].radius);
    NoL = dot(normal, lightDir);
    if (intersects) {
      continue;
    }
    */

    //void SphereMaxNoH(float NoV, inout float NoL, inout float VoL, float sphereSin, float sphereCos, bool bNewtonIteration, out float NoH, out float VoH)
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

    // If lightDir is pointing below the horizon
    ClampDirToHorizon(lightDir, NoL, normal, 0.0);

    // From W.Merta
    // NoL = max(NoL, falloffMicro * sphereSin);

    if (NoV >= 0.0 && NoL >= 0.0) {
      specular += min(1, (solidAngle * Ndf(roughness * roughness, NoH)) / (4 * NoV)) * Gmf(roughness * roughness, NoV, diffuseNoL) * Fresnel(VoH, F0);
    }

    float falloffMicro = Falloff(normal, lightVec, lightDistance, radius);
    float falloffMacro = Falloff(input.normalWorld, lightVec, lightDistance, radius);
    // TODO remove
    //falloffMicro = falloffMacro = 1;

    // Shadows
    float4 positionVS = mul(g_spotLights[i].viewMat, input.positionWorld);
    float angleSin = sqrt(1 - g_spotLights[i].cutoffCosineOuter * g_spotLights[i].cutoffCosineOuter);
    float angleTan = angleSin / g_spotLights[i].cutoffCosineOuter;
    float halfSide = angleTan * positionVS.z;

    const float mapSize = 8192;
    float texelSize = 1.0 / mapSize;
    float texelSizeWorld = 2 * halfSide * texelSize;
    float3 offset = texelSizeWorld * sqrt(2) * 0.5 * (input.normalWorld - 0.5 * lightDir * GNoL);

    float4 positionBiased = input.positionWorld + float4(offset, 0.0);
    float4 positionPS = mul(g_spotLights[i].projectionMat, mul(g_spotLights[i].viewMat, positionBiased));
    positionPS /= positionPS.w;

    float visibility = 0.0;
    float2 shadowUv = positionPS.xy * float2(0.5, -0.5) + 0.5.xx;
    float depth = shadowMapSpot.Sample(g_linearWrap, float3(shadowUv, i));
    // reversed z
    visibility = positionPS.z < depth ? 1.0 : 0.0;
    visibility = 1 - smoothstep(0.33, 1.0, visibility);

    if (g_diffuseEnabled) {
      light += g_spotLights[i].radiance * textureColor * intensity * falloffMicro * falloffMacro * diffuse * visibility;
    }
    if (g_specularEnabled) {
      light += g_spotLights[i].radiance * textureColor * intensity * falloffMicro * falloffMacro * specular * visibility;
    }
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

  if (g_iblDiffuseEnabled) {
    light += diffuseReflection;
  }
  if (g_iblSpecularEnabled) {
    light += specularReflection;
  }

  return float4(light, 1.0);
}
