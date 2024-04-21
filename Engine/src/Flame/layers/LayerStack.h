#pragma once

#include <memory>

#include "Layer.h"
#include <vector>

namespace Flame {
  struct LayerStack final {
    LayerStack();

    void PushLayer(std::shared_ptr<Layer> layer);
    void PushOverlay(std::shared_ptr<Layer> layer);
    void Clear();

    std::vector<std::shared_ptr<Layer>>::iterator begin();
    std::vector<std::shared_ptr<Layer>>::iterator end();
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin();
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rend();

  private:
    std::vector<std::shared_ptr<Layer>> m_layers;
    std::vector<std::shared_ptr<Layer>>::iterator m_layersEnd;
  };
}
