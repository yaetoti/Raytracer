#pragma once

#include "AabbOld.h"
#include "HitRecordOld.h"
#include "Ray.h"
#include "Sphere.h"
#include "TriangleOld.h"
#include "glm/exponential.hpp"

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

    static float SolidAngle(float radius, float distance) {
      float sin = (radius / distance);
      return (1.0f - glm::sqrt(1.0f - sin * sin));
    }

    static glm::vec3 RadianceFromIrradiance(glm::vec3 irradiance, float radius, float distance) {
      return irradiance / SolidAngle(radius, distance);
    }

    static glm::vec3 HsvToRgb(const glm::vec3& hsv) {
      int i = int(std::floor(hsv.x * 6));
      float f = hsv.x * 6 - i;
      float p = hsv.z * (1 - hsv.y);
      float q = hsv.z * (1 - f * hsv.y);
      float t = hsv.z * (1 - (1 - f) * hsv.y);

      switch(i % 6){
        case 0: return glm::vec3(hsv.z, t, p);
        case 1: return glm::vec3(q, hsv.z, p);
        case 2: return glm::vec3(p, hsv.z, t);
        case 3: return glm::vec3(p, q, hsv.z);
        case 4: return glm::vec3(t, p, hsv.z);
        case 5: return glm::vec3(hsv.z, p, q);
        default: return {};
      }
    }

    static void BasisFromDir(glm::vec3 front, glm::vec3& right, glm::vec3& up) {
      // Frisvad with z == -1 problem avoidance
      float k = 1.0 / glm::max(1.0 + front.z, 0.00001);
      float a =  front.y * k;
      float b =  front.y * a;
      float c = -front.x * a;
      right = glm::vec3(front.z + b, c, -front.x);
      up = glm::vec3(c, 1.0 - b, -front.y);
    }

    static glm::mat3 ViewFromDir(glm::vec3 front) {
      glm::vec3 right;
      glm::vec3 up;
      BasisFromDir(front, right, up);
      return glm::mat3(
        right.x, up.x, front.x,
        right.y, up.y, front.y,
        right.z, up.z, front.z
      );
    }

    static glm::mat4 ViewFromDir(glm::vec3 front, glm::vec3 position) {
      glm::vec3 right;
      glm::vec3 up;
      BasisFromDir(front, right, up);
      return glm::mat4(
        right.x, up.x, front.x, 0,
        right.y, up.y, front.y, 0,
        right.z, up.z, front.z, 0,
        -dot(position, right), -dot(position, up), -dot(position, front), 1.0f
      );
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
      // Your camera worked wrong in HW2 had a reversed z scale and offset here, stupid moron
      return glm::mat4(
        ctgHalfFov / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, ctgHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f * near / (far - near), 1.0f,
        0.0f, 0.0f, 2.0f * far * near / (far - near), 0.0f
      );
    }

    static glm::mat4 Orthographic(float right, float left, float top, float bottom, float far, float near) {
      // See yaetoti___ CheatSheet for reference ;)
      // X [-1; 1]: 2 * P - 1
      // Y [-1; 1]: 2 * P - 1
      // Z [1; 0]: -1 * P + 1
      return glm::mat4(
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f / (far - near), 0.0f,
        (-right - left) / (right - left), (-top - bottom) / (top - bottom), (far + near) / (far - near), 1.0f
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
