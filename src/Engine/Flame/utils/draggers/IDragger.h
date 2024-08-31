#pragma once

namespace Flame {
  struct IDragger {
    virtual ~IDragger() = default;

    virtual void Drag(const Ray& r, const glm::vec3& cameraDirection) = 0;
  };

  struct OpaqueInstanceDragger final : IDragger {
    explicit OpaqueInstanceDragger(const HitRecord<MeshSystem::HitResult>& record, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection)
    : m_draggable(record.data.perInstanceOpaque)
    , m_offset(m_draggable->GetData().transform.GetPosition() - record.point)
    , m_distanceToPlane(glm::dot(cameraDirection, record.point - cameraPosition)) {
    }

    void Drag(const Ray& r, const glm::vec3& cameraDirection) override {
      float approachToPlane = glm::dot(r.direction, cameraDirection);
      float approachTime = m_distanceToPlane / approachToPlane;
      m_draggable->GetData().transform.SetPosition(r.AtParameter(approachTime) + m_offset);
    }

  private:
    OpaqueGroup::PerInstance* m_draggable;
    glm::vec3 m_offset;
    float m_distanceToPlane;
  };

  struct HologramInstanceDragger final : IDragger {
    explicit HologramInstanceDragger(const HitRecord<MeshSystem::HitResult>& record, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection)
    : m_draggable(record.data.perInstanceHologram)
    , m_offset(m_draggable->GetData().transform.GetPosition() - record.point)
    , m_distanceToPlane(glm::dot(cameraDirection, record.point - cameraPosition)) {
    }

    void Drag(const Ray& r, const glm::vec3& cameraDirection) override {
      float approachToPlane = glm::dot(r.direction, cameraDirection);
      float approachTime = m_distanceToPlane / approachToPlane;
      m_draggable->GetData().transform.SetPosition(r.AtParameter(approachTime) + m_offset);
    }

  private:
    HologramGroup::PerInstance* m_draggable;
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
        default:
          return nullptr;
      }
    }
  };
}
