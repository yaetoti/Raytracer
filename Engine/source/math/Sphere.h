#pragma once

#include <ostream>

#include "IHitable.h"
#include "Ray3.h"
#include "Vec3.h"

template <typename T>
struct Sphere final : IHitable<T> {
    Vec3<T> center;
    T radius;

    explicit Sphere(const Vec3<T>& center, const T& radius)
    : center(center), radius(radius) {
    }

    bool Hit(const Ray3<T>& r, float tMin, float tMax, HitRecord<T>& record) override {
        // Quadratic equation
        Vec3<T> oc = r.origin - center;
        T a = r.direction.SquaredLength();
        T b = static_cast<T>(2.0 * oc.Dot(r.direction));
        T c = oc.SquaredLength() - radius * radius;
        T d = b * b - 4 * a * c;

        if (d > 0) {
            T t = (-b - sqrt(d)) / (2.0f * a);
            if (t > tMin && t < tMax) {
                record.time = t;
                record.point = r.AtParameter(t);
                record.normal = (record.point - center) / radius;
                return true;
            }

            t = (-b + sqrt(d)) / (2.0f * a);
            if (t > tMin && t < tMax) {
                record.time = t;
                record.point = r.AtParameter(t);
                record.normal = (record.point - center) / radius;
                return true;
            }
        }

        return false;
    }

    friend std::ostream& operator<<(std::ostream& out, const Sphere& s) {
        out << "{ C: " << s.center << ", R: " << s.radius << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const Sphere& s) {
        out << L"{ C: " << s.center << L", R: " << s.radius << L" }";
        return out;
    }
};
