#pragma once

#include <list>

#include "IHitable.h"

struct HitableList final : IHitable {
  bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) override {
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

  // TODO Handy add()/remove() regarding the scene and variability of game objects
  void Add(std::shared_ptr<IHitable> element) {
    m_hitables.emplace_back(element);
  }

private:
  std::list<std::shared_ptr<IHitable>> m_hitables;
};
