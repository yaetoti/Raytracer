#pragma once

#include "HitRecord.h"
#include "Ray3.h"

template <typename T>
struct IHitable {
    virtual ~IHitable() = default;

    virtual bool Hit(const Ray3<T>& r, float tMin, float tMax, HitRecord<T>& record) = 0;
};
