#include "Transform.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Flame {
  Transform::Transform()
  : m_position(0.0f)
  , m_scale(1.0f)
  , m_rotation(0.0f)
  , m_rotationDirty(true) {
  }

  void Transform::SetPosition(const glm::vec3& position) {
    m_position = position;
  }

  void Transform::SetPosition(float x, float y, float z) {
    m_position = glm::vec3(x, y, z);
  }

  void Transform::SetScale(const glm::vec3& scale) {
    m_scale = scale;
  }

  void Transform::SetScale(float x, float y, float z) {
    m_scale = glm::vec3(x, y, z);
  }

  void Transform::SetRotation(float pitch, float yaw, float roll) {
    m_rotation = glm::vec3(pitch, yaw, roll);
    m_rotationDirty = true;
  }

  void Transform::SetRotation(const glm::quat& rotation) {
    m_rotationQuat = rotation;
    m_rotationMat = glm::mat4(rotation);
    m_rotation = glm::degrees(glm::eulerAngles(rotation));
    m_rotationDirty = false;
  }

  void Transform::Rotate(float pitch, float yaw, float roll) {
    m_rotation.x += pitch;
    m_rotation.y += yaw;
    m_rotation.z += roll;
    m_rotationDirty = true;
  }

  void Transform::SetRoll(float roll) {
    m_rotation.z = roll;
    m_rotationDirty = true;
  }

  void Transform::SetPitch(float pitch) {
    m_rotation.x = pitch;
    m_rotationDirty = true;
  }

  void Transform::SetYaw(float yaw) {
    m_rotation.y = yaw;
    m_rotationDirty = true;
  }

  const glm::vec3& Transform::GetPosition() const {
    return m_position;
  }

  const glm::vec3& Transform::GetScale() const {
    return m_scale;
  }

  const glm::vec3& Transform::GetRotation() const {
    return m_rotation;
  }

  const glm::quat& Transform::GetRotationQuat() const {
    InvalidateRotation();
    return m_rotationQuat;
  }

  const glm::mat4& Transform::GetRotationMat() const {
    InvalidateRotation();
    return m_rotationMat;
  }

  float Transform::GetRoll() const {
    return m_rotation.z;
  }

  float Transform::GetPitch() const {
    return m_rotation.x;
  }

  float Transform::GetYaw() const {
    return m_rotation.y;
  }

  void Transform::InvalidateRotation() const {
    if (!m_rotationDirty) {
      return;
    }

    m_rotationQuat = glm::eulerAngleYXZ(glm::radians(m_rotation.y), glm::radians(m_rotation.x), glm::radians(m_rotation.z));
    m_rotationMat = glm::mat4(m_rotationQuat);
    m_rotationDirty = false;
  }

  std::ostream& operator<<(std::ostream& out, const Transform& t) {
    out << "Transform { Position: " << glm::to_string(t.m_position)
        << ", Scale: " << glm::to_string(t.m_scale)
        << ", Rotation: " << glm::to_string(t.m_rotation)
        << " }";
    return out;
  }
}
