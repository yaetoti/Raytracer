#include "globals.hlsl"

struct VSInput {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4x4 modelMatrix : MODEL_MATRIX;
};

struct VSOutput {
  float3 position : SV_POSITION;
  float3 normalWorld : NORMAL;
};

VSOutput VSMain(VSInput input) {
  float3 axisX = normalize(input.modelMatrix[0].xyz);
  float3 axisY = normalize(input.modelMatrix[1].xyz);
  float3 axisZ = normalize(input.modelMatrix[2].xyz);
  float3 worldN = input.normal.x * axisX + input.normal.y * axisY + input.normal.z * axisZ;

  VSOutput o;
  o.position = mul(g_projectionMatrix, mul(g_viewMatrix, mul(input.modelMatrix, float4(input.position, 1.0))));
  o.normalWorld = input.position;
  return o;
}

float4 PSMain(VSOutput input) : SV_TARGET {
    return float4((input.normalWorld + 1.0.rrr) * 0.5.rrr, 1.0);
}
