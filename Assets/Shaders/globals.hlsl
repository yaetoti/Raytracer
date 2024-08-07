cbuffer ConstantBuffer : register(b0)
{
  float4x4 g_viewMatrix;
  float4x4 g_projectionMatrix;
  // Frustum corner directions in homogenous coordinates
  float4 g_frustumTL;
  float4 g_frustumBR;
  float4 g_frustumBL;
  float4 g_resolution;
  float4 g_cameraPosition;
  float g_time;
  bool g_isNormalVisMode;
};

SamplerState g_pointWrap {
  Filter = MIN_MAG_MIP_POINT;
  AddressU = Wrap;
  AddressV = Wrap;
  AddressW = Wrap;
};

SamplerState g_linearWrap {
  Filter = MIN_MAG_MIP_LINEAR;
  AddressU = Wrap;
  AddressV = Wrap;
  AddressW = Wrap;
};

SamplerState g_anisotropicWrap {
  Filter = ANISOTROPIC;
  MaxAnisotropy = 8;
  AddressU = Wrap;
  AddressV = Wrap;
  AddressW = Wrap;
};
