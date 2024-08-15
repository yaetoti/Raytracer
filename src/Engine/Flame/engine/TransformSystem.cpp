#include "TransformSystem.h"

namespace Flame {
  void TransformSystem::Cleanup() {
    m_transforms.clear();
  }

  TransformSystem::ID TransformSystem::Insert() {
    return m_transforms.emplace(std::make_unique<TransformData>());
  }

  TransformSystem::ID TransformSystem::Insert(const TransformData& data) {
    return m_transforms.emplace(std::make_unique<TransformData>(data));
  }

  void TransformSystem::Remove(ID id) {
    m_transforms.erase(id);
  }

  bool TransformSystem::Contains(ID id) const {
    return m_transforms.occupied(id);
  }

  TransformSystem::ID TransformSystem::Size() const {
    return m_transforms.size();
  }

  const TransformSystem::TransformData* TransformSystem::At(ID id) const {
    return m_transforms[id].get();
  }

  TransformSystem::TransformData* TransformSystem::At(ID id) {
    return m_transforms[id].get();
  }

  TransformSystem* TransformSystem::Get() {
    static TransformSystem m_instance;
    return &m_instance;
  }
}
