#pragma once

#include <list>
#include <functional>

template <typename... Args>
struct EventDispatcher final {
    using CbType = std::function<void(Args...)>;
    using CbId = typename std::list<CbType>::iterator;

    CbId AddListener(CbType callback) {
        return m_callbacks.insert(m_callbacks.end(), callback);
    }

    void RemoveListener(CbId id) {
        m_callbacks.erase(id);
    }

    void Dispatch(Args... args) {
        for (auto callback : m_callbacks) {
            callback(args...);
        }
    }

private:
    std::list<CbType> m_callbacks;
};
