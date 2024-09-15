/**
* Dependencies:
* PerView CBuffer
* MeshBuffer CBuffer
* DepthCubemap CBuffer
*/

#include "globals.hlsl"
#include "CubemapUtils.hlsli"

/**
* Shader can be used for rendering into a CubemapTextureArray
* g_cubemapPosition - viewMatrix position
* g_cubemapId - cubemap offset in the array
*/
cbuffer DepthCubemapBuffer : register(CBUFFER_DEPTH_CUBEMAP) {
  float3 g_cubemapPosition;
  uint g_cubemapId;
};

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

struct GSInput {
  float4 positionWS : POSITION_WS;
};

struct PSInput {
  float4 positionProj : SV_POSITION;
  uint arrayId : SV_RenderTargetArrayIndex;
};

/* Vertex */

GSInput VSMain(VSInput input) {
  GSInput result;
  result.positionWS = mul(input.modelMatrix, mul(g_meshToModel, float4(input.position, 1.0)));
  return result;
}

/* Geometry */

// 3 vertices * 6 sides
[maxvertexcount(18)]
void GSMain(triangle GSInput input[3], inout TriangleStream<PSInput> output) {
  PSInput result;

  for (uint faceId = 0; faceId < 6; ++faceId) {
    float4x4 WStoVS = GetViewMatrix(faceId, g_cubemapPosition);

    for (uint vertexId = 0; vertexId < 3; ++vertexId) {
      result.positionProj = mul(g_projectionMatrix, mul(WStoVS, input[vertexId].positionWS));
      result.arrayId = g_cubemapId * 6 + faceId;
      output.Append(result);
    }

    output.RestartStrip();
  }
}
