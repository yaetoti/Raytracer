#pragma once
#include "IMaterial.h"

template <typename T>
struct LambertianMaterial final : IMaterial<T> {
    LambertianMaterial(const Vec3F& albedo, std::uniform_real_distribution<float>& distribution, std::mt19937& generator)
    : m_albedo(albedo), m_distribution(distribution), m_generator(generator) {
    }

    bool Scatter(const Ray3<T>& ray, const HitRecord<T>& record, Ray3<T>& scattered, Vec3F& attenuation) override {
        Vec3<T> target = record.point + record.normal + Vec3<T>::RandomInUnitSphere(m_distribution, m_generator);
        scattered = Ray3<T>(record.point, (target - record.point).Normalize());
        attenuation = m_albedo;
        return true;
    }

private:
    Vec3F m_albedo;
    std::uniform_real_distribution<float>& m_distribution;
    std::mt19937& m_generator;
};
