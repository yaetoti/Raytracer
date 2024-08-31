#pragma once

#include "AabbOld.h"
#include "HitRecordOld.h"
#include "Ray.h"
#include "Sphere.h"
#include "TriangleOld.h"

#undef near
#undef far

namespace Flame {
  struct MathUtils {
    template <typename It>
    static bool HitClosest(It begin, It end, const Ray& r, float tMin, float tMax, HitRecordOld& record) {
      HitRecordOld tempRecord;
      bool hitAnything = false;
      float closest = tMax;

      while (begin != end) {
        if ((*begin)->Hit(r, tempRecord, tMin, closest)) {
          hitAnything = true;
          closest = tempRecord.time;
          record = tempRecord;
        }
        ++begin;
      }

      return hitAnything;
    }

    static glm::vec3 ColorFromHex(uint32_t color) {
      return glm::vec3(
        static_cast<float>(color >> 16 & 0xFF) / 255.0f,
        static_cast<float>(color >> 8 & 0xFF) / 255.0f,
        static_cast<float>(color & 0xFF)  / 255.0f
      );
    }

    static AabbOld AabbFromSphere(const Sphere& sphere) {
      glm::vec3 min;
      glm::vec3 max;

      for (int dim = 0; dim < 3; ++dim) {
        min[dim] = sphere.center[dim] - sphere.radius;
        max[dim] = sphere.center[dim] + sphere.radius;
      }

      return AabbOld(min, max);
    }

    static AabbOld AabbFromTriangle(const TriangleOld& triangle) {
      glm::vec3 min(std::numeric_limits<float>::infinity());
      glm::vec3 max(-std::numeric_limits<float>::infinity());
      for (int point = 0; point < 3; ++point) {
        for (int dim = 0; dim < 3; ++dim) {
          min[dim] = glm::min(triangle.points[point][dim], min[dim]);
          max[dim] = glm::max(triangle.points[point][dim], max[dim]);
        }
      }

      return AabbOld(min, max);
    }

    static glm::vec3 ToBarycentric(const glm::vec3& point, const glm::vec3* triangle) {
      glm::vec3 v0 = triangle[1] - triangle[0];
      glm::vec3 v1 = triangle[2] - triangle[0];
      glm::vec3 v2 = point - triangle[0];
      float d00 = glm::dot(v0, v0);
      float d01 = glm::dot(v0, v1);
      float d11 = glm::dot(v1, v1);
      float d20 = glm::dot(v2, v0);
      float d21 = glm::dot(v2, v1);
      float denom = d00 * d11 - d01 * d01;
      float v = (d11 * d20 - d01 * d21) / denom;
      float w = (d00 * d21 - d01 * d20) / denom;
      float u = 1.0f - v - w;
      return glm::vec3(u, v, w);
    }

    // GLM temporary replacement
    static glm::mat4 Perspective(float fov, float aspect, float near, float far) {
      float ctgHalfFov = glm::cot(fov * 0.5f);
      //return glm::mat4(
      //  ctgHalfFov / aspect, 0.0f, 0.0f, 0.0f,
      //  0.0f, ctgHalfFov, 0.0f, 0.0f,
      //  0.0f, 0.0f, -(far + near) / (far - near), -1.0f,
      //  0.0f, 0.0f, -2.0f * far * near / (far - near), 0.0f
      //);

      //return glm::mat4(
      //  1, 0, 0, 0,
      //  0, 1, 0, 0,
      //  0, 0, -1, 0,
      //  0, 0, 1, 1
      //) *
      //glm::mat4(
      //  ctgHalfFov / aspect, 0.0f, 0.0f, 0.0f,
      //  0.0f, ctgHalfFov, 0.0f, 0.0f,
      //  0.0f, 0.0f, -(far + near) / (far - near), -1.0,
      //  0.0f, 0.0f, -2.0f * far * near / (far - near), 0.0f
      //);

      // => Z Reversed LHS
      return glm::mat4(
        ctgHalfFov / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, ctgHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f * near / (far - near), -1.0f,
        0.0f, 0.0f, 2.0f * far * near / (far - near), 0.0f
      );
    }

    static glm::mat4 Translate(const glm::vec3& offset) {
      return glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        offset.x, offset.y, offset.z, 1.0f
      );
    }

    static glm::mat4 Scale(const glm::vec3& scale) {
      return glm::mat4(
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      );
    }
  };
}
