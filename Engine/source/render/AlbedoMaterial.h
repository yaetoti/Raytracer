#pragma once
#include "IMaterial.h"

template <typename T>
struct AlbedoMaterial final : IMaterial<T> {
    AlbedoMaterial(const Vec3F& albedo)
        : m_albedo(albedo) {
    }

    bool Scatter(const Ray3<T>& ray, const HitRecord<T>& record, Ray3<T>& scattered, Vec3F& attenuation) override {
        scattered = Ray3<T>();
        attenuation = m_albedo;
        return true;
    }

private:
    Vec3F m_albedo;
};
