#pragma once

#include <Flame/math/Sphere.h>

#include "Flame/objects/MeshObject.h"

namespace Flame {
  struct IDragger {
    virtual ~IDragger() = default;

    virtual void Drag(const Ray& r) = 0;
  };

  struct ISphereDragger : IDragger {
    explicit ISphereDragger(Sphere* draggable, const HitRecordOld& record)
    : m_draggable(draggable)
    , m_hitTime(record.time)
    , m_offset(draggable->center - record.point) {
    }

    void Drag(const Ray& r) override {
      m_draggable->center = r.AtParameter(m_hitTime) + m_offset;
    }

  private:
    Sphere* m_draggable;
    float m_hitTime;
    glm::vec3 m_offset;
  };

  struct IMeshDragger : IDragger {
    explicit IMeshDragger(MeshObject* draggable, const HitRecordOld& record)
    : m_draggable(draggable)
    , m_hitTime(record.time)
    , m_offset(draggable->Position() - record.point) {
    }

    void Drag(const Ray& r) override {
      m_draggable->SetPosition(r.AtParameter(m_hitTime) + m_offset);
    }

  private:
    MeshObject* m_draggable;
    float m_hitTime;
    glm::vec3 m_offset;
  };

  struct DraggerFactory final {
    static std::unique_ptr<IDragger> CreateDragger(const HitRecordOld& record) {
      if (auto sphere = dynamic_cast<Flame::Sphere*>(record.hitable)) {
        return std::make_unique<ISphereDragger>(sphere, record);
      }
      if (auto sphere = dynamic_cast<Flame::MeshObject*>(record.hitable)) {
        return std::make_unique<IMeshDragger>(sphere, record);
      }

      return nullptr;
    }
  };
}
