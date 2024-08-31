#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace Flame {
  template <typename T>
  struct HitRecord final {
    // Default constructor (Zero-initialization)
    HitRecord() = default;

    // Parameterized data-copy constructor
    HitRecord(const glm::vec3& point, const glm::vec3& normal, float time, const T& data )
    : point(point)
    , normal(normal)
    , time(time)
    , data { data } {
    }

    // Parameterized data-move constructor
    HitRecord(const glm::vec3& point, const glm::vec3& normal, float time, T&& data )
    : point(point)
    , normal(normal)
    , time(time)
    , data { std::move(data) } {
    }

    // Copy constructor (copies data)
    HitRecord(const HitRecord<T>& other)
    : point(other.point)
    , normal(other.normal)
    , time(other.time)
    , data { other.data } {
    }

    // Move constructor (moves data)
    HitRecord(HitRecord<T>&& other) noexcept
    : point(other.point)
    , normal(other.normal)
    , time(other.time)
    , data { std::move(other.data) } {
    }

    // Templated copy constructor (doesn't touch data)
    template <typename U>
    explicit HitRecord(const HitRecord<U>& other)
    : point(other.point)
    , normal(other.normal)
    , time(other.time) {
    }

    // Copy assignment (copies data)
    HitRecord& operator=(const HitRecord& other) {
      if (this == &other) {
        return *this;
      }

      point = other.point;
      normal = other.normal;
      time = other.time;
      data = other.data;
      return *this;
    }

    // Move assignment (moves data)
    HitRecord& operator=(HitRecord&& other) noexcept {
      if (this == &other) {
        return *this;
      }
    
      point = other.point;
      normal = other.normal;
      time = other.time;
      data = std::move(other.data);
      other.point = { 0.0f };
      other.normal = { 0.0f };
      other.time = 0.0f;
      other.data = { };
      return *this;
    }

    // Templated copy assignment (doesn't touch data)
    template <typename U>
    HitRecord& operator=(const HitRecord<U>& other) {
      point = other.point;
      normal = other.normal;
      time = other.time;

      return *this;
    }

    friend std::ostream& operator<<(std::ostream& out, const HitRecord<T>& record) {
      return out << "HitRecord { " << record.point << record.normal << record.time << " }";
    }

  public:
    glm::vec3 point = glm::vec3 { 0.0f };
    glm::vec3 normal = glm::vec3 { 0.0f };
    float time = 0.0f;
    T data = { };
  };
}
