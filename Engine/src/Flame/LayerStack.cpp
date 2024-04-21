#include "LayerStack.h"

namespace Flame {
  LayerStack::LayerStack() {
    m_layersEnd = m_layers.begin();
  }

  void LayerStack::PushLayer(std::shared_ptr<Layer> layer) {
    m_layersEnd = m_layers.emplace(m_layersEnd, std::move(layer));
  }

  void LayerStack::PushOverlay(std::shared_ptr<Layer> layer) {
    m_layers.emplace_back(std::move(layer));
  }

  void LayerStack::Clear() {
    m_layers.clear();
  }

  std::vector<std::shared_ptr<Layer>>::iterator LayerStack::begin() {
    return m_layers.begin();
  }

  std::vector<std::shared_ptr<Layer>>::iterator LayerStack::end() {
    return m_layers.end();
  }

  std::vector<std::shared_ptr<Layer>>::reverse_iterator LayerStack::rbegin() {
    return m_layers.rbegin();
  }

  std::vector<std::shared_ptr<Layer>>::reverse_iterator LayerStack::rend() {
    return m_layers.rend();
  }
}

