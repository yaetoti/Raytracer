#ifndef SAMPLERS_HLSL
#define SAMPLERS_HLSL

SamplerState g_pointWrap : register(s0);
SamplerState g_linearWrap : register(s1);
SamplerState g_anisotropicWrap : register(s2);

#endif
