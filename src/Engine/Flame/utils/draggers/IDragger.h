#pragma once

#include <Flame/math/Sphere.h>

#include "Flame/objects/MeshObject.h"

namespace Flame {
  struct IDragger {
    virtual ~IDragger() = default;

    virtual void Drag(const Ray& r) = 0;
  };

  struct OpaqueInstanceDragger final : IDragger {
    explicit OpaqueInstanceDragger(const HitRecord<MeshSystem::HitResult>& record)
    : m_draggable(record.data.perInstanceOpaque)
    , m_hitTime(record.time)
    , m_offset(m_draggable->data.transform.GetPosition() - record.point) {
    }

    void Drag(const Ray& r) override {
      m_draggable->data.transform.SetPosition(r.AtParameter(m_hitTime) + m_offset);
    }

  private:
    OpaqueGroup::PerInstance* m_draggable;
    float m_hitTime;
    glm::vec3 m_offset;
  };

  struct HologramInstanceDragger final : IDragger {
    explicit HologramInstanceDragger(const HitRecord<MeshSystem::HitResult>& record)
    : m_draggable(record.data.perInstanceHologram)
    , m_hitTime(record.time)
    , m_offset(m_draggable->data.transform.GetPosition() - record.point) {
    }

    void Drag(const Ray& r) override {
      m_draggable->data.transform.SetPosition(r.AtParameter(m_hitTime) + m_offset);
    }

  private:
    HologramGroup::PerInstance* m_draggable;
    float m_hitTime;
    glm::vec3 m_offset;
  };

  struct DraggerFactory final {
    static std::unique_ptr<IDragger> CreateDragger(const HitRecord<MeshSystem::HitResult>& record) {
      switch (record.data.groupType) {
        case GroupType::OPAQUE_GROUP:
          return std::make_unique<OpaqueInstanceDragger>(record);
        case GroupType::HOLOGRAM_GROUP:
          return std::make_unique<HologramInstanceDragger>(record);
        default:
          return nullptr;
      }
    }
  };
}
