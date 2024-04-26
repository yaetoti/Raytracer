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

    std::vector<std::unique_ptr<IHitable>>& GetHitables();

  private:
    glm::vec3 ColorPerSample(const Camera& camera, uint32_t x, uint32_t y);

  protected:
    // TODO Renderer
    uint32_t m_surfaceWidth = 0;
    uint32_t m_surfaceHeight = 0;
    std::vector<uint32_t> m_rowIndices;
    std::vector<uint32_t> m_columnIndices;

    uint32_t m_samples = 10;
    float m_sampleCountInv = 1.0f / static_cast<float>(m_samples);
    uint32_t m_bounces = 10;
    uint32_t m_lightSamples = 4;
    float m_lightSmooth = 1.0f;

    // Scene
    std::vector<std::unique_ptr<IHitable>> m_hitables;
    std::vector<Material> m_materials;

    std::vector<DirectLight> m_directLights;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
  };
}
