#pragma once

#include <ostream>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace Flame {
  struct Transform final {
    explicit Transform();

    void SetPosition(const glm::vec3& position);
    void SetPosition(float x, float y, float z);
    void SetScale(const glm::vec3& scale);
    void SetScale(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);
    void SetRotation(const glm::vec3& rotation);
    void SetRotation(const glm::quat& rotation);
    void Rotate(float pitch, float yaw, float roll);
    void Rotate(const glm::vec3& rotation);
    void Rotate(const glm::quat& rotation);
    /// \param pitch Pitch in degrees
    void SetPitch(float pitch);
    /// \param yaw Yaw in degrees
    void SetYaw(float yaw);
    /// \param roll Roll in degrees
    void SetRoll(float roll);

    const glm::vec3& GetPosition() const;
    const glm::vec3& GetScale() const;
    const glm::quat& GetRotation() const;
    glm::vec3 GetRotationEuler() const;
    glm::mat4 GetRotationMat() const;
    glm::mat4 GetMat() const;
    /// \return Pitch in degrees
    float GetPitch() const;
    /// \return Yaw in degrees
    float GetYaw() const;
    /// \return Roll in degrees
    float GetRoll() const;

    friend std::ostream& operator<<(std::ostream& out, const Transform& t);
  private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::quat m_rotation;
  };
}
