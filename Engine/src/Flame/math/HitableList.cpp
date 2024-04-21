#include "HitableList.h"

namespace Flame {
  bool HitableList::Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const {
    HitRecord tempRecord;
    bool hitAnything = false;
    float closest = tMax;

    for (auto hitable : m_hitables) {
      if (hitable->Hit(r, tMin, closest, tempRecord)) {
        hitAnything = true;
        closest = tempRecord.time;
        record = tempRecord;
      }
    }

    return hitAnything;
  }

  void HitableList::Add(IHitable* element) {
    m_hitables.emplace_back(element);
  }
}
