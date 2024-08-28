#include "ConsoleLib.h"
#include "Application.h"
#include <Windows.h>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <utility>
#include <vector>
#include <Flame/engine/Engine.h>

#include "Flame/engine/ModelManager.h"
#include "Flame/math/MathUtils.h"
#include "Flame/utils/PpmImage.h"
#include "Flame/engine/Transform.h"
#include "Flame/graphics/DxContext.h"
#include "Flame/utils/SolidVector.h"
#include "glm/ext.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "glm/trigonometric.hpp"

glm::vec3 RandomHemisphere(double i, double N, double* NoV = nullptr) {
  const double GOLDEN_RATIO = (1.0 + sqrt(5.0)) / 2.0;
	double theta = 2.0 * glm::pi<double>() * i / GOLDEN_RATIO;
	double phiCos = 1.0 - (i + 0.5) / N;
  if (NoV != nullptr) {
    *NoV = phiCos;
  }

	double phiSin = sqrt(1.0 - phiCos * phiCos);
  double thetaSin = std::sin(theta);
	double thetaCos = std::cos(theta);
	return glm::vec3(thetaCos * phiSin, thetaSin * phiSin, phiCos);
}

std::vector<std::wstring> ParseCommandArgs(LPWSTR lpCmdLine) {
  std::vector<std::wstring> args;
  std::wstring cmdLine(lpCmdLine);
  std::wstringstream argStream;
  size_t argSize = 0;
  bool inQuotes = false;

  for (size_t i = 0; i < cmdLine.length(); ++i) {
    if (cmdLine[i] == L'\"') {
      inQuotes = !inQuotes;
      continue;
    }
    if (std::iswspace(cmdLine[i]) && !inQuotes) {
      if (argSize != 0) {
        args.push_back(argStream.str());
        argStream.clear();
        argSize = 0;
      }

      continue;
    }

    argStream << cmdLine[i];
    ++argSize;
  }

  if (argSize != 0) {
    args.push_back(argStream.str());
  }

  return args;
}
void HandleCommandArgs(std::vector<std::wstring> args) {
  if (args.size() >= 1) {
    Flame::Engine::SetWorkingDirectory(args[0]);
    std::wcout << "Directory was set to: \"" << args[0] << '\"' << '\n';
  }
}

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int) {
  Console::Get()->RedirectStdHandles();

  HandleCommandArgs(ParseCommandArgs(lpCmdLine));
  Flame::Engine::Init();

  // Generation
  const uint32_t count = 100000;
  std::vector<glm::vec3> coords3d;
  std::vector<glm::vec2> coords;
  std::vector<double> NoVs;

  for (int i = 1; i <= count; ++i) {
    double NoV = 0.0f;
    glm::vec3 coord(RandomHemisphere(double(i), double(count), &NoV));
    coords.emplace_back(coord.x, coord.y);
    //coords.emplace_back(coord.x, coord.z);
    coords3d.emplace_back(coord);
    NoVs.emplace_back(NoV);
  }

// Image output
#if 0
  // Collecting meta-information
  float minX = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::min();
  float minY = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::min();

  for (uint32_t i = 0; i < coords.size(); ++i) {
    const glm::vec2& coord = coords[i];
    minX = glm::min(minX, coord.x);
    maxX = glm::max(maxX, coord.x);
    minY = glm::min(minY, coord.y);
    maxY = glm::max(maxY, coord.y);
  }

  // Normalization
  for (uint32_t i = 0; i < coords.size(); ++i) {
    glm::vec2& coord = coords[i];
    coord.x = (coord.x - minX) / (maxX - minX);
    coord.y = (coord.y - minY) / (maxY - minY);
  }

  // Rasterization
  Flame::PpmImage image(500, 500);
  const float epsilon = 0.01f;

  image.Fill(glm::vec3(0));
  for (uint32_t i = 0; i < coords.size(); ++i) {
    glm::ivec2 coord = coords[i] * glm::vec2(image.Width(), image.Height()) - 0.1f;
    image.Set(coord, Flame::MathUtils::HsvToRgb(glm::vec3(float(i) / float(coords.size()), 1.0f, 1.0f)));
  }

  image.SaveToFile(L"C:\\Users\\yaetoti\\Desktop\\image.ppm");
#endif

  double result = 0.0f;
  for (uint32_t i = 0; i < coords3d.size(); ++i) {
    auto coord = coords3d[i];
    result += double(NoVs[i]);
  }
  std::cout << "PI = " << result * (2.0 * glm::pi<double>() / double(count)) << '\n';

  // TODO Get ViewMatrix:
  // 1. Manually (rows are front, right and up axes)
  // 2. LookAt
  // 3. Transform * Rotate (Rotate - How? What? Z-Axis. By what? Difference between Z-Axis and Vector. How to find? ?)


  // glm::vec3 lightPos(2.0f, 2.0f, 20.0f);
  // glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
  // float lightAngleCos = glm::cos(glm::radians(25.0f));
  // glm::vec3 pointWS(0.0f, 0.0f, -4.0f);

  // std::cout << "lightPos: " << lightPos << '\n';
  // std::cout << "lightDir: " << lightDir << '\n';
  // std::cout << "pointWS: " << pointWS << '\n';
  // glm::quat rotationQuat(glm::vec3(0.0f, 0.0f, 1.0f), glm::normalize(lightDir));
  // std::cout << "Rotation Quat: " << rotationQuat << '\n';

  // glm::mat4 lightMat = glm::mat4(
  //   1, 0, 0, 0,
  //   0, 1, 0, 0,
  //   0, 0, 1, 0,
  //   lightPos.x, lightPos.y, lightPos.z, 1
  // ) * glm::mat4(rotationQuat);
  // std::cout << lightMat << '\n';

  // lightMat = glm::inverse(lightMat); // Create ViewMatrix
  // std::cout << lightMat << '\n';

  // std::cout << lightMat << " * " << pointWS << '\n';
  // glm::vec3 pointVS = glm::vec3(lightMat * glm::vec4(pointWS, 1.0f));
  // std::cout << pointWS << " WS -> VS " << pointVS << '\n';

  // // Now goes projection
  // float cosAbs = glm::abs(lightAngleCos);
  // float sinAbs = glm::sqrt(1 - cosAbs * cosAbs);
  // float ctgAbs = cosAbs / sinAbs;

  // std::cout << ctgAbs << '\n';

  // const float nearPlane = 0.001f;
  // const float farPlane = pointVS.z;
  // glm::mat4 perspective = glm::mat4(
  //   ctgAbs, 0, 0, 0,
  //   0, ctgAbs, 0, 0,
  //   0, 0, -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0,
  //   0, 0, -2.0f * farPlane * nearPlane / (farPlane - nearPlane), 0.0f
  // );
  
  // glm::vec4 pointCS = perspective * glm::vec4(pointVS, 1.0f);
  // std::cout << "PointCS:" << pointCS << '\n';
  // glm::vec4 pointNDC = pointCS / pointCS.w;
  // std::cout << "PointNDC:" << pointNDC << '\n';

  // glm::vec2 pointUV = glm::vec2(pointNDC) * 0.5f + glm::vec2(0.5f);
  // std::cout << "PointUV:" << pointUV << '\n';

  std::cout << "Press any key to continue..." << '\n';
  // TODO: Not working with CLion debug
  //Console::Get()->Pause();
  std::cout << "Loading..." << '\n';

  {
    Application application;
    application.Run();
  }

  Flame::Engine::Cleanup();
  Console::Get()->Pause();
  return 0;
}
