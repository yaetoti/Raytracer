#pragma once

#include <list>

template <typename E>
struct EventListener {
    virtual ~EventListener() = default;

    virtual void Handle(const E* e) = 0;
};

template <typename E>
struct EventDispatcher final {
    using CbType = EventListener<E>*;
    using CbId = typename std::list<CbType>::iterator;

    CbId AddListener(CbType callback) {
        return m_listeners.emplace(m_listeners.end(), callback);
    }

    void RemoveListener(CbId id) {
        m_listeners.erase(id);
    }

    void Dispatch(const E* e) {
        for (const auto& listener : m_listeners) {
            listener->Handle(e);
        }
    }

private:
    std::list<CbType> m_listeners;
};
