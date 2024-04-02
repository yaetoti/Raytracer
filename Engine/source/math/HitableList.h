#pragma once

#include <list>

#include "IHitable.h"

template <typename T>
struct HitableList final : IHitable<T> {
    bool Hit(const Ray3<T>& r, T tMin, T tMax, HitRecord<T>& record) override {
        HitRecord<T> tempRecord;
        bool hitAnything = false;
        T closest = tMax;

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
    void Add(std::shared_ptr<IHitable<T>> element) {
        m_hitables.emplace_back(element);
    }

private:
    std::list<std::shared_ptr<IHitable<T>>> m_hitables;
};
