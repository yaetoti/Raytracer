#pragma once
#include <algorithm>
#include <string>
#include <fstream>
#include <cwctype>
#include <vector>

#include "Flame/math/MeshData.h"

namespace Flame {
  struct ObjUtils final {
    static std::wstring TrimLeft(std::wstring s) {
      s.erase(s.begin(), std::ranges::find_if(s, [](wchar_t c) {
        return !std::iswspace(c);
      }));
      return s;
    }

    static std::wstring TrimRight(std::wstring s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](wchar_t c) {
        return !std::iswspace(c);
      }).base(), s.end());
      return s;
    }

    static std::wstring Trim(const std::wstring& s) {
      return TrimRight(TrimLeft(s));
    }

    template <typename Predicate>
    static std::vector<std::wstring> Split(const std::wstring& s, Predicate predicate) {
      std::vector<std::wstring> result;
      auto prevPos = s.begin();
      auto pos = s.begin();

      do {
        pos = std::find_if(prevPos, s.end(), predicate);
        auto endPos = std::find_if_not(pos, s.end(), predicate);
        result.emplace_back(std::wstring(prevPos, pos));
        prevPos = endPos;
      } while(prevPos != s.end());

      return result;
    }

    static bool ParseObj(const std::wstring& filename, MeshData& mesh) {
      // Open file
      std::wifstream in(filename);
      if (!in.is_open()) {
        return false;
      }

      MeshData result;
      std::wstring line;
      // Parse lines
      while (std::getline(in, line)) {
        std::vector<std::wstring> tokens = Split(line, std::iswspace);
        if (tokens.size() == 0) {
          continue;
        }

        // PerVertex
        if (tokens[0] == L"v") {
          if (tokens.size() != 4) {
            continue;
          }

          result.vertices.emplace_back(
            std::stof(tokens[1]),
            std::stof(tokens[2]),
            std::stof(tokens[3])
          );
        }
        // Normal
        else if (tokens[0] == L"vn") {
          if (tokens.size() != 4) {
            continue;
          }

          result.normals.emplace_back(
            std::stof(tokens[1]),
            std::stof(tokens[2]),
            std::stof(tokens[3])
          );
        }
        // FaceOld
        else if (tokens[0] == L"f") {
          if (tokens.size() != 4) {
            continue;
          }

          bool innerError = false;
          uint32_t verticesId[3];
          uint32_t normalId;

          // Parse IDs "f 1//1 2//1 3//1"
          for (int tokenId = 0; tokenId < 3; ++tokenId) {
            std::vector<std::wstring> faceTokens = Split(tokens[tokenId + 1], [](wchar_t c) {
              return c == '/';
            });

            int tokensCount = faceTokens.size();
            if (tokensCount != 2 && tokensCount != 3) {
              innerError = true;
              break;
            }

            verticesId[tokenId] = std::stoul(faceTokens[0]) - 1;
            normalId = std::stoul(faceTokens[tokensCount - 1]) - 1;
          }

          if (innerError) {
            continue;
          }

          // Create face
          result.faces.emplace_back(
            verticesId[0],
            verticesId[1],
            verticesId[2],
            normalId
          );
        }
        // Ignore others
        else {
          continue;
        }
      }

      mesh = std::move(result);
      return true;
    }
  };
}
