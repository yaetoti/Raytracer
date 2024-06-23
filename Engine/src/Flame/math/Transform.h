#pragma once

#include <ostream>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace Flame {
  struct Transform {
    explicit Transform();

    void SetPosition(const glm::vec3& position);
    void SetPosition(float x, float y, float z);
    void SetScale(const glm::vec3& scale);
    void SetScale(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);
    void SetRotation(const glm::quat& rotation);
    void Rotate(float pitch, float yaw, float roll);
    /// \param roll Roll in degrees
    void SetRoll(float roll);
    /// \param pitch Pitch in degrees
    void SetPitch(float pitch);
    /// \param yaw Yaw in degrees
    void SetYaw(float yaw);

    const glm::vec3& GetPosition() const;
    const glm::vec3& GetScale() const;
    const glm::vec3& GetRotation() const;
    const glm::quat& GetRotationQuat() const;
    const glm::mat4& GetRotationMat() const;
    /// \return Roll in degrees
    float GetRoll() const;
    /// \return Pitch in degrees
    float GetPitch() const;
    /// \return Yaw in degrees
    float GetYaw() const;

    friend std::ostream& operator<<(std::ostream& out, const Transform& t);

  private:
    /// InvalidateMatrices rotation data from roll, pitch and yaw
    void InvalidateRotation() const;

  private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_rotation;
    mutable glm::quat m_rotationQuat;
    mutable glm::mat4 m_rotationMat;
    mutable bool m_rotationDirty;
  };
}
