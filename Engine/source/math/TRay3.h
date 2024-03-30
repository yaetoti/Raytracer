#pragma once

#include <ostream>

#include "TVec3.h"

template <typename T>
struct TRay3 final {
    TVec3<T> center;
    TVec3<T> direction;

    explicit TRay3(const TVec3<T>& center, const TVec3<T>& direction)
    : center(center), direction(direction) {
    }

    TVec3<T> AtParameter(T t) const {
        return center + t * direction;
    }

    friend std::ostream& operator<<(std::ostream& out, const TRay3 ray) {
        out << "{ C: " << ray.center << ", D: " << ray.direction << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const TRay3 ray) {
        out << L"{ C: " << ray.center << L", D: " << ray.direction << L" }";
        return out;
    }
};
