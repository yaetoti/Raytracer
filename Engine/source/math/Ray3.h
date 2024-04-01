#pragma once

#include <ostream>

#include "Vec3.h"

template <typename T>
struct Ray3 final {
    Vec3<T> origin;
    Vec3<T> direction;

    Ray3()
    : origin(Vec3<T>()), direction(Vec3<T>()) {
    }

    explicit Ray3(const Vec3<T>& origin, const Vec3<T>& direction)
    : origin(origin), direction(direction) {
    }

    Vec3<T> AtParameter(T t) const {
        return origin + t * direction;
    }

    friend std::ostream& operator<<(std::ostream& out, const Ray3 ray) {
        out << "{ C: " << ray.origin << ", D: " << ray.direction << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const Ray3 ray) {
        out << L"{ C: " << ray.origin << L", D: " << ray.direction << L" }";
        return out;
    }
};
