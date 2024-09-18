#ifndef CUBEMAP_UTILS_HLSL
#define CUBEMAP_UTILS_HLSL

uint FaceFromDir(float3 dir) {
  uint face = 0;
  float maxValue = dir.x;

  if (abs(maxValue) < abs(dir.y)) {
    maxValue = dir.y;
    face = 2;
  }

  if (abs(maxValue) < abs(dir.z)) {
    maxValue = dir.z;
    face = 4;
  }

  if (maxValue < 0) {
    face += 1;
  }

  return face;
}

float4x4 GetViewMatrix(float3 front, float3 up, float3 right, float3 position) {
  return float4x4(
    right.x, right.y, right.z, -dot(position, right),
    up.x, up.y, up.z, -dot(position, up),
    front.x, front.y, front.z, -dot(position, front),
    0.0, 0.0, 0.0, 1.0
  );
}

float4x4 GetViewMatrix(uint faceId, float3 position) {
  // Rotate 90: (x, y) -> (y, -x)

  // +X
  if (faceId == 0) {
    return GetViewMatrix(
      float3(1, 0, 0),
      float3(0, 1, 0),
      float3(0, 0, -1),
      position
    );
  }
  // -X
  if (faceId == 1) {
    return GetViewMatrix(
      float3(-1, 0, 0),
      float3(0, 1, 0),
      float3(0, 0, 1),
      position
    );
  }
  // +Y
  if (faceId == 2) {
    return GetViewMatrix(
      float3(0, 1, 0),
      float3(0, 0, -1),
      float3(1, 0, 0),
      position
    );
  }
  // -Y
  if (faceId == 3) {
    return GetViewMatrix(
      float3(0, -1, 0),
      float3(0, 0, 1),
      float3(1, 0, 0),
      position
    );
  }
  // +Z
  if (faceId == 4) {
    return GetViewMatrix(
      float3(0, 0, 1),
      float3(0, 1, 0),
      float3(1, 0, 0),
      position
    );
  }
  // -Z
  if (faceId == 5) {
    return GetViewMatrix(
      float3(0, 0, -1),
      float3(0, 1, 0),
      float3(-1, 0, 0),
      position
    );
  }

  return (float4x4)0;
}

#endif // CUBEMAP_UTILS_HLSL
