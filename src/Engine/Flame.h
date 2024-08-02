#pragma once

#include "Flame/window/Window.h"
#include "Flame/window/InputSystem.h"

#include "Flame/layers/Scene.h"

#include "Flame/math/HitRecordOld.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/MathUtils.h"
#include "Flame/math/Ray.h"
#include "Flame/math/Sphere.h"

#include "Flame/render/Framebuffer.h"
#include "Flame/render/Material.h"

#include "Flame/utils/EventDispatcher.h"
#include "Flame/utils/FunctionalDispatcher.h"
#include "Flame/utils/Timer.h"

#include "Flame/window/events/KeyWindowEvent.h"
#include "Flame/window/events/MouseButtonWindowEvent.h"
#include "Flame/window/events/MouseMoveWindowEvent.h"
#include "Flame/window/events/ResizeWindowEvent.h"
#include "Flame/window/events/WindowEvent.h"
