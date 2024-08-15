#pragma once

#include "Flame/engine/Transform.h"
#include "Flame/utils/SolidVector.h"
#include <memory>

namespace Flame {
  struct TransformSystem final {
    struct TransformData final {
      Transform transform;
      // InverseTransform, Parent, Childos, etc...
    };

    using Container = SolidVector<std::unique_ptr<TransformData>>;
    using ID = Container::ID;

    void Cleanup();
    ID Insert();
    ID Insert(const TransformData& data);
    void Remove(ID id);
    bool Contains(ID id) const;
    ID Size() const;

    const TransformData* At(ID id) const;
    TransformData* At(ID id);

    static TransformSystem* Get();

  private:
    TransformSystem() = default;

  public:
    Container m_transforms;
  };
}
