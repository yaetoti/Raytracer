#pragma once
#include "Flame/camera/AlignedCamera.h"
#include "Flame/layers/SceneOld.h"
#include "Flame/utils/ParallelExecutor.h"
#include "Flame/window/Window.h"

namespace Flame {
  struct Renderer final {
    explicit Renderer(const SceneOld* scene);

    void Render(Framebuffer& surface, const AlignedCamera& camera);
    void Resize(uint32_t width, uint32_t height);
    void ResetAccumulatedData();

  private:
    glm::vec3 ColorPerRay(const AlignedCamera& camera, const Ray& ray, uint32_t bounce, glm::vec3& lightTotal);
    glm::vec3 CalculatePointLightPerPoint(const AlignedCamera& camera, const HitRecordOld& record);
    glm::vec3 CalculateSpotLightPerPoint(const AlignedCamera& camera, const HitRecordOld& record);
    glm::vec3 CalculateDirectLightPerPoint(const AlignedCamera& camera, const HitRecordOld& record);

  private:
    const SceneOld* m_scene;
    ParallelExecutor m_executor;

    // Render data
    uint32_t m_surfaceWidth = 0;
    uint32_t m_surfaceHeight = 0;

    uint32_t m_bounces = 4;
    float m_lightSmooth = 0.3f;

    std::vector<float> m_accumulatedData;
    uint32_t m_framesCount = 1;
  };
}
