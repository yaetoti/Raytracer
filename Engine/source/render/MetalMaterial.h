#pragma once

#include "IMaterial.h"

template <typename T>
struct MetalMaterial final : IMaterial<T> {
    MetalMaterial(const Vec3F& albedo)
        : m_albedo(albedo) {
    }

    bool Scatter(const Ray3<T>& ray, const HitRecord<T>& record, Ray3<T>& scattered, Vec3F& attenuation) override {
        Vec3F reflected = ray.direction.Reflect(record.normal).Normalize();
        scattered = Ray3F(record.point, reflected);
        attenuation = m_albedo;
        return reflected.Dot(record.normal) > 0;
    }

private:
    Vec3F m_albedo;
};
