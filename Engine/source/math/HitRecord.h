#pragma once

#include "Vec3.h"

template <typename T>
struct IMaterial;

template <typename T>
struct HitRecord final {
    T time;
    Vec3<T> point;
    Vec3<T> normal;
    IMaterial<T>* material;
};
