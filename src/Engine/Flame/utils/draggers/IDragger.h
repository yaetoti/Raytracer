#pragma once

#include "Flame/graphics/groups/EmissionOnlyGroup.h"
#include "Flame/graphics/groups/TextureOnlyGroup.h"
#include "Flame/math/Ray.h"
#include "Flame/engine/MeshSystem.h"
namespace Flame {
  struct IDragger {
    virtual ~IDragger() = default;

    virtual void Drag(const Ray& r, const glm::vec3& cameraDirection) = 0;
  };

  struct OpaqueInstanceDragger final : IDragger {
    explicit OpaqueInstanceDragger(const HitRecord<MeshSystem::HitResult>& record, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection)
    : m_draggable(record.data.perInstanceOpaque)
    , m_offset(TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.GetPosition() - record.point)
    , m_distanceToPlane(glm::dot(cameraDirection, record.point - cameraPosition)) {
    }

    void Drag(const Ray& r, const glm::vec3& cameraDirection) override {
      float approachToPlane = glm::dot(r.direction, cameraDirection);
      float approachTime = m_distanceToPlane / approachToPlane;
      TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.SetPosition(r.AtParameter(approachTime) + m_offset);
    }

  private:
    OpaqueGroup::PerInstance* m_draggable;
    glm::vec3 m_offset;
    float m_distanceToPlane;
  };

  struct HologramInstanceDragger final : IDragger {
    explicit HologramInstanceDragger(const HitRecord<MeshSystem::HitResult>& record, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection)
    : m_draggable(record.data.perInstanceHologram)
    , m_offset(TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.GetPosition() - record.point)
    , m_distanceToPlane(glm::dot(cameraDirection, record.point - cameraPosition)) {
    }

    void Drag(const Ray& r, const glm::vec3& cameraDirection) override {
      float approachToPlane = glm::dot(r.direction, cameraDirection);
      float approachTime = m_distanceToPlane / approachToPlane;
      TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.SetPosition(r.AtParameter(approachTime) + m_offset);
    }

  private:
    HologramGroup::PerInstance* m_draggable;
    glm::vec3 m_offset;
    float m_distanceToPlane;
  };

  struct TextureOnlyInstanceDragger final : IDragger {
    explicit TextureOnlyInstanceDragger(const HitRecord<MeshSystem::HitResult>& record, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection)
    : m_draggable(record.data.perInstanceTextureOnly)
    , m_offset(TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.GetPosition() - record.point)
    , m_distanceToPlane(glm::dot(cameraDirection, record.point - cameraPosition)) {
    }

    void Drag(const Ray& r, const glm::vec3& cameraDirection) override {
      float approachToPlane = glm::dot(r.direction, cameraDirection);
      float approachTime = m_distanceToPlane / approachToPlane;
      TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.SetPosition(r.AtParameter(approachTime) + m_offset);
    }

  private:
    TextureOnlyGroup::PerInstance* m_draggable;
    glm::vec3 m_offset;
    float m_distanceToPlane;
  };

  struct EmissionOnlyInstanceDragger final : IDragger {
    explicit EmissionOnlyInstanceDragger(const HitRecord<MeshSystem::HitResult>& record, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection)
    : m_draggable(record.data.perInstanceEmissionOnly)
    , m_offset(TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.GetPosition() - record.point)
    , m_distanceToPlane(glm::dot(cameraDirection, record.point - cameraPosition)) {
    }

    void Drag(const Ray& r, const glm::vec3& cameraDirection) override {
      float approachToPlane = glm::dot(r.direction, cameraDirection);
      float approachTime = m_distanceToPlane / approachToPlane;
      TransformSystem::Get()->At(m_draggable->GetData().transformId)->transform.SetPosition(r.AtParameter(approachTime) + m_offset);
    }

  private:
    EmissionOnlyGroup::PerInstance* m_draggable;
    glm::vec3 m_offset;
    float m_distanceToPlane;
  };

  struct DraggerFactory final {
    static std::unique_ptr<IDragger> CreateDragger(const HitRecord<MeshSystem::HitResult>& record, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection) {
      switch (record.data.groupType) {
        case GroupType::OPAQUE_GROUP:
          return std::make_unique<OpaqueInstanceDragger>(record, cameraPosition, cameraDirection);
        case GroupType::HOLOGRAM_GROUP:
          return std::make_unique<HologramInstanceDragger>(record, cameraPosition, cameraDirection);
        case GroupType::TEXTURE_ONLY_GROUP:
          return std::make_unique<TextureOnlyInstanceDragger>(record, cameraPosition, cameraDirection);
        case GroupType::EMISSION_ONLY_GROUP:
          return std::make_unique<EmissionOnlyInstanceDragger>(record, cameraPosition, cameraDirection);
        default:
          return nullptr;
      }
    }
  };
}
