#pragma once

#include <Flame/Layer.h>

struct MainLayer : Flame::Layer {
  ~MainLayer() override = default;

  void Initialize() override;
  void Update() override;
  void Render(RenderSurface& surface) override;
  void Cleanup() override;
  void HandleEvent(const WindowEvent& e) override;

private:

};
