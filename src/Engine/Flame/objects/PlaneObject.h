#pragma once
#include "Flame/math/IHitable.h"
#include "Flame/math/Plane.h"

namespace Flame {
  struct PlaneObject final : IHitable {
    explicit PlaneObject(const Plane& plane, int materialId);
    bool Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const override;

  private:
    Plane m_plane;
    int m_materialId;
  };
}
