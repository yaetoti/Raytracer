#pragma once

#include "Flame/camera/AlignedCamera.h"
#include "Flame/camera/CameraController.h"
#include "Flame/camera/SpaceshipCamera.h"
#include "Flame/engine/HologramGroup.h"
#include "Flame/engine/Mesh.h"
#include "Flame/engine/MeshBvh.h"
#include "Flame/engine/MeshSystem.h"
#include "Flame/engine/Model.h"
#include "Flame/engine/ModelManager.h"
#include "Flame/engine/OpaqueGroup.h"
#include "Flame/engine/Transform.h"
#include "Flame/graphics/buffers/ConstantBuffer.h"
#include "Flame/graphics/buffers/IndexBuffer.h"
#include "Flame/graphics/buffers/VertexBuffer.h"
#include "Flame/graphics/DxContext.h"
#include "Flame/graphics/DxRenderer.h"
#include "Flame/graphics/Framebuffer.h"
#include "Flame/graphics/Material.h"
#include "Flame/graphics/Renderer.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/graphics/Vertex.h"
#include "Flame/layers/SceneOld.h"
#include "Flame/math/Aabb.h"
#include "Flame/math/AabbOld.h"
#include "Flame/math/BvhOld.h"
#include "Flame/math/HitRecord.h"
#include "Flame/math/HitRecordOld.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/MathUtils.h"
#include "Flame/math/MeshData.h"
#include "Flame/math/MeshOld.h"
#include "Flame/math/Plane.h"
#include "Flame/math/Ray.h"
#include "Flame/math/Sphere.h"
#include "Flame/math/TriangleOld.h"
#include "Flame/objects/MeshObject.h"
#include "Flame/objects/PlaneObject.h"
#include "Flame/objects/SphereObject.h"
#include "Flame/objects/TriangleObject.h"
#include "Flame/utils/draggers/IDragger.h"
#include "Flame/utils/EventDispatcher.h"
#include "Flame/utils/FunctionalDispatcher.h"
#include "Flame/utils/ObjUtils.h"
#include "Flame/utils/ParallelExecutor.h"
#include "Flame/utils/PtrProxy.h"
#include "Flame/utils/Random.h"
#include "Flame/utils/ScopeTimer.h"
#include "Flame/utils/Timer.h"
#include "Flame/window/events/KeyWindowEvent.h"
#include "Flame/window/events/MouseButtonWindowEvent.h"
#include "Flame/window/events/MouseMoveWindowEvent.h"
#include "Flame/window/events/MouseScrollWindowEvent.h"
#include "Flame/window/events/ResizeWindowEvent.h"
#include "Flame/window/events/WindowEvent.h"
#include "Flame/window/InputSystem.h"
#include "Flame/window/Window.h"

namespace Flame {
  struct Engine final {
    static void Init() {
      DxContext::Get()->Init();
      MeshSystem::Get()->Init();
    }

    static void Cleanup() {
      MeshSystem::Get()->Cleanup();
      MeshSystem::Get()->Cleanup();
      ModelManager::Get()->Cleanup();
      DxContext::Get()->Cleanup();
    }
  };
}
