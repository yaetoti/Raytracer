#include "Scene3D.h"

#include <execution>
#include <numeric>

#include "Flame/camera/Camera.h"
#include "Flame/math/MathUtils.h"
#include "Flame/math/Ray.h"
#include "Flame/utils/Random.h"
#include "Flame/window/events/KeyWindowEvent.h"

namespace Flame {
  void Scene3D::Render(RenderSurface& surface) {
    Layer::Render(surface);

    // TODO Pass application
    // TODO Add parameters
    int rays = 5;
    float raysScale = 1.0f / rays;

    uint32_t width = static_cast<int>(surface.GetWidth());
    uint32_t height = static_cast<int>(surface.GetHeight());

    std::vector<uint32_t> horizontalIter;
    horizontalIter.resize(height);
    std::iota(horizontalIter.begin(), horizontalIter.end(), 0);
    std::vector<uint32_t> verticalIter;
    verticalIter.resize(width);
    std::iota(verticalIter.begin(), verticalIter.end(), 0);

    // TODO DONT Move to scene (Pass as an argument)
    static Camera camera(width, height, 90.0f, 0.1f, 1000.0f);
    static float x = 0.0f;
    x += 0.05f;
    camera.Rotate(glm::eulerAngleXZ(0.02f, 0.05f));
    camera.SetPosition(glm::vec3(0.0f, 0.0f, -glm::sin(x) * 0.5f));

    std::for_each(std::execution::par, verticalIter.begin(), verticalIter.end(), [&](uint32_t row) {
      std::for_each(std::execution::par, horizontalIter.begin(), horizontalIter.end(), [&](uint32_t col) {
        glm::vec3 resultColor(0);
        for (int i = 0; i < rays; ++i) {
          resultColor += Color(camera.GetRay(row, col), 0);
        }

        resultColor *= raysScale;
        resultColor *= 255.0f;
        surface.SetPixel(row, col, static_cast<BYTE>(resultColor.r), static_cast<BYTE>(resultColor.g), static_cast<BYTE>(resultColor.b));
      });
    });
  }

  glm::vec3 Scene3D::Color(const Ray& ray, int depth) {
    // TODO Lighting, Materials, Meshes and Ray to ModelSpace
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
