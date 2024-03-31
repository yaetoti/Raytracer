#pragma once

#include <ostream>

#include "TRay3.h"
#include "TVec3.h"

template <typename T>
struct TSphere final {
    TVec3<T> center;
    T radius;

    explicit TSphere(const TVec3<T>& center, const T& radius)
    : center(center), radius(radius) {
    }

    bool Hit(const TRay3<T>& r) {
        // Quadratic equation
        TVec3<T> oc = r.origin - center;
        T a = r.direction.SquaredLength();
        T b = 2.0 * oc.Dot(r.direction);
        T c = oc.SquaredLength() - radius * radius;
        T d = b * b - 4 * a * c;

        if (d > 0) {
            T t = (-b - sqrt(d)) / (2 * a);
            return t >= 0;
        }

        return false;
    }

    friend std::ostream& operator<<(std::ostream& out, const TSphere& s) {
        out << "{ C: " << s.center << ", R: " << s.radius << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const TSphere& s) {
        out << L"{ C: " << s.center << L", R: " << s.radius << L" }";
        return out;
    }
};
