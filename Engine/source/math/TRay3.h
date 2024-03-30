#pragma once

#include <ostream>

#include "TVec3.h"

template <typename T>
struct TRay3 final {
    TVec3<T> origin;
    TVec3<T> direction;

    explicit TRay3(const TVec3<T>& origin, const TVec3<T>& direction)
    : origin(origin), direction(direction) {
    }

    TVec3<T> AtParameter(T t) const {
        return origin + t * direction;
    }

    friend std::ostream& operator<<(std::ostream& out, const TRay3 ray) {
        out << "{ C: " << ray.origin << ", D: " << ray.direction << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const TRay3 ray) {
        out << L"{ C: " << ray.origin << L", D: " << ray.direction << L" }";
        return out;
    }
};
