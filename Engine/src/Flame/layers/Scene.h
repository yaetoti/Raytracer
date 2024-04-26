#pragma once

#include "Flame/math/IHitable.h"

#include <memory>
#include <vector>
#include <Flame/math/Ray.h>
#include <Flame/render/Framebuffer.h>
#include <glm/glm.hpp>

#include "Flame/camera/Camera.h"
#include "Flame/utils/EventDispatcher.h"
#include "Flame/window/events/WindowEvent.h"

namespace Flame {
  struct Scene : EventListener<WindowEvent> {
    virtual ~Scene() override = default;

    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Cleanup() {}

    void Render(Framebuffer& surface, const Camera& camera);
    void HandleEvent(const WindowEvent& e) override;

    void ResetAccumulatedData();

    std::vector<std::unique_ptr<IHitable>>& GetHitables();

  private:
    glm::vec3 ColorPerRay(const Camera& camera, const Ray& ray, uint32_t bounce, glm::vec3& lightTotal);
    glm::vec3 LightPerPoint(const Camera& camera, const HitRecord& record);

  protected:
    // TODO Renderer
    uint32_t m_surfaceWidth = 0;
    uint32_t m_surfaceHeight = 0;
    std::vector<uint32_t> m_rowIndices;
    std::vector<uint32_t> m_columnIndices;

    uint32_t m_samples = 1;
    float m_sampleCountInv = 1.0f / static_cast<float>(m_samples);
    uint32_t m_bounces = 4;
    uint32_t m_lightSamples = 1;
    float m_lightSmooth = 0.4f;

    std::vector<float> m_accumulatedData;
    uint32_t m_framesCount = 1;

    // TODO Accumulation instead of samples

    // Scene
    std::vector<std::unique_ptr<IHitable>> m_hitables;
    std::vector<Material> m_materials;

    std::vector<DirectLight> m_directLights;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
  };
}
