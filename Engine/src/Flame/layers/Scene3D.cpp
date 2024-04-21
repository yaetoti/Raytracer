#include "Scene3D.h"

#include <execution>
#include <numeric>

#include "Flame/math/MathUtils.h"
#include "Flame/math/Ray.h"
#include "Flame/utils/Random.h"

namespace Flame {
  void Scene3D::Render(RenderSurface& surface) {
    Layer::Render(surface);

    // TODO Pass application
    // TODO Add parameters
    int rays = 5;
    float raysScale = 1.0f / rays;

    int width = static_cast<int>(surface.GetWidth());
    int height = static_cast<int>(surface.GetHeight());
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float scaleX = std::min(aspectRatio, 1.0f);
    float scaleY = std::min(1 / aspectRatio, 1.0f);
    glm::vec3 unit(1.0f / width, 1.0f / height, 0);

    std::vector<int> horizontalIter;
    horizontalIter.resize(height);
    std::iota(horizontalIter.begin(), horizontalIter.end(), 0);
    std::vector<int> verticalIter;
    verticalIter.resize(width);
    std::iota(verticalIter.begin(), verticalIter.end(), 0);

    std::for_each(std::execution::par, horizontalIter.begin(), horizontalIter.end(), [&](int col) {
      std::for_each(std::execution::par, verticalIter.begin(), verticalIter.end(), [&](int row) {
        float x = (2.0f * row / width - 1.0f) * scaleX;
        float y = (2.0f * col / height - 1.0f) * scaleY;

        glm::vec3 resultColor(0);
        for (int i = 0; i < rays; ++i) {
          Ray ray(glm::vec3(x, y, 0) + unit * Random::Float(), glm::vec3(0, 0, -1));
          resultColor += Color(ray, 0);
        }

        resultColor *= raysScale;
        resultColor *= 255.0f;
        surface.SetPixel(row, col, static_cast<BYTE>(resultColor.r), static_cast<BYTE>(resultColor.g), static_cast<BYTE>(resultColor.b));
      });
    });

#if 0
      for (int col = 0; col < height; ++col) {
      for (int row = 0; row < width; ++row) {
        // TODO Camera->GetRay(u, v)
        float x = (2.0f * row / width - 1.0f) * scaleX;
        float y = (2.0f * col / height - 1.0f) * scaleY;

        glm::vec3 resultColor(0);
        for (int i = 0; i < rays; ++i) {
          Ray ray(glm::vec3(x, y, 0) + unit * Random::Float(), glm::vec3(0, 0, -1));
          resultColor += Color(ray, 0);
        }

        resultColor *= raysScale;
        resultColor *= 255.0f;
        surface.SetPixel(row, col, static_cast<BYTE>(resultColor.r), static_cast<BYTE>(resultColor.g), static_cast<BYTE>(resultColor.b));
      }
    }
#endif
  }

  glm::vec3 Scene3D::Color(const Ray& ray, int depth) {
    HitRecord record;
    if (MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), ray, 0.01f, std::numeric_limits<float>::max(), record)) {
      Ray scattered;
      glm::vec3 attenuation;

      if (depth < 10 && record.material->Scatter(ray, record, scattered, attenuation)) {
        return attenuation * Color(scattered, depth + 1);
      }

      return glm::vec3(0.0f);
    }

    glm::vec3 skyColor(0.5f, 0.7f, 1.0f);
    float t = (ray.direction.y + 1) * 0.5f;
    return skyColor * t + (1.0f - t) * glm::vec3(1.0f);
  }
}
