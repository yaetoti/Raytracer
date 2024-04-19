#pragma once

#ifdef FLAME_BUILD_DLL
#define FLAME_API __declspec(dllexport)
#else
#define FLAME_API __declspec(dllimport)
#endif
