#pragma once

namespace Flame {
  template <typename T>
  struct PtrProxy final {
    PtrProxy()
    : m_data(static_cast<T>(0)) {
    }

    explicit PtrProxy(T data)
    : m_data(data) {
    }

    T* Ptr() {
      return &m_data;
    }

    const T* Ptr() const {
      return &m_data;
    }

  private:
    T m_data;
  };
}
