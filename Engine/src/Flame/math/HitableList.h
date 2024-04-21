#pragma once

#include "IHitable.h"
#include <list>

namespace Flame {
  struct HitableList final : IHitable {
    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const override;
    void Add(IHitable* element);

  private:
    std::list<IHitable*> m_hitables;
  };
}
