#pragma once

#include <ostream>

#include "IHitable.h"
#include "Ray3.h"
#include "Vec3.h"

template <typename T>
struct IMaterial;

template <typename T>
struct Sphere final : IHitable<T> {
    Vec3<T> center;
    T radius;
    std::unique_ptr<IMaterial<T>> m_material;

    explicit Sphere(const Vec3<T>& center, const T& radius, std::unique_ptr<IMaterial<T>>&& material)
    : center(center), radius(radius), m_material(std::move(material)) {
    }

    bool Hit(const Ray3<T>& r, T tMin, T tMax, HitRecord<T>& record) override {
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
                record.material = m_material.get();
                return true;
            }

            t = (-b + sqrt(d)) / (2.0f * a);
            if (t > tMin && t < tMax) {
                record.time = t;
                record.point = r.AtParameter(t);
                record.normal = (record.point - center) / radius;
                record.material = m_material.get();
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
