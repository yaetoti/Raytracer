#pragma once

#include "Flame/math/HitableList.h"
#include "Flame/math/HitRecord.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/Ray.h"
#include "Flame/math/Sphere.h"

#include "Flame/render/AlbedoMaterial.h"
#include "Flame/render/Framebuffer.h"
#include "Flame/render/IMaterial.h"
#include "Flame/render/LambertianMaterial.h"
#include "Flame/render/MetalMaterial.h"

#include "Flame/utils/EventDispatcher.h"
#include "Flame/utils/FunctionalDispatcher.h"
#include "Flame/utils/Timer.h"

#include "Flame/window/Window.h"

#include "Flame/window/events/KeyWindowEvent.h"
#include "Flame/window/events/MouseButtonWindowEvent.h"
#include "Flame/window/events/MouseMoveWindowEvent.h"
#include "Flame/window/events/ResizeWindowEvent.h"
#include "Flame/window/events/WindowEvent.h"
