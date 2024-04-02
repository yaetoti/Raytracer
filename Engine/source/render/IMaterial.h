#pragma once

#include "Engine/source/math/Vector.h"

template <typename T>
struct IMaterial {
    virtual ~IMaterial() = default;

    virtual bool Scatter(const Ray3<T>& ray, const HitRecord<T>& record, Ray3<T>& scattered, Vec3F& attenuation) = 0;
};
