#include "Transform.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Flame {
  Transform::Transform(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation)
  : m_position(position)
  , m_scale(scale)
  , m_rotation(rotation) {
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
    m_rotation = glm::eulerAngleYXZ(glm::radians(yaw), glm::radians(pitch), glm::radians(roll));
  }

  void Transform::SetRotation(const glm::vec3& rotation) {
    SetRotation(rotation.x, rotation.y, rotation.z);
  }

  void Transform::SetRotation(const glm::quat& rotation) {
    m_rotation = rotation;
  }

  void Transform::Rotate(float pitch, float yaw, float roll) {
    Rotate(glm::eulerAngleYXZ(glm::radians(yaw), glm::radians(pitch), glm::radians(roll)));
  }

  void Transform::Rotate(const glm::vec3& rotation) {
    Rotate(rotation.x, rotation.y, rotation.z);
  }

  void Transform::Rotate(const glm::quat& rotation) {
    m_rotation = glm::normalize(m_rotation * rotation);
  }

  void Transform::SetPitch(float pitch) {
    glm::vec3 rotation = GetRotationEuler();
    rotation.x = pitch;
    SetRotation(rotation);
  }

  void Transform::SetYaw(float yaw) {
    glm::vec3 rotation = GetRotationEuler();
    rotation.y = yaw;
    SetRotation(rotation);
  }

  void Transform::SetRoll(float roll) {
    glm::vec3 rotation = GetRotationEuler();
    rotation.z = roll;
    SetRotation(rotation);
  }

  const glm::vec3& Transform::GetPosition() const {
    return m_position;
  }

  const glm::vec3& Transform::GetScale() const {
    return m_scale;
  }

  const glm::quat& Transform::GetRotation() const {
    return m_rotation;
  }

  glm::vec3 Transform::GetRotationEuler() const {
    return glm::eulerAngles(m_rotation);
  }

  glm::mat4 Transform::GetRotationMat() const {
    return glm::mat4(m_rotation);
  }

  glm::mat4 Transform::GetMat() const {
    glm::mat4 result = glm::translate(glm::mat4(1.0f), m_position)
      * GetRotationMat()
      * glm::scale(glm::mat4(1.0f), m_scale);
    return result;
  }

  float Transform::GetPitch() const {
    return GetRotationEuler().x;
  }

  float Transform::GetYaw() const {
    return GetRotationEuler().y;
  }

  float Transform::GetRoll() const {
    return GetRotationEuler().z;
  }

  std::ostream& operator<<(std::ostream& out, const Transform& t) {
    out << "Transform { Position: " << glm::to_string(t.m_position)
        << ", Scale: " << glm::to_string(t.m_scale)
        << ", Rotation: " << glm::to_string(t.GetRotationEuler())
        << " }";
    return out;
  }
}
