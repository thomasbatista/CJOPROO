#include "core/EventBus.h"

namespace lastwick {

void EventBus::subscribe(EventType type, Callback callback) {
    m_listeners[type].push_back(std::move(callback));
}

void EventBus::emit(const Event& event) {
    auto it = m_listeners.find(event.type);
    if (it != m_listeners.end()) {
        for (const auto& callback : it->second) {
            callback(event);
        }
    }
}

void EventBus::clear() {
    m_listeners.clear();
}

}  // namespace lastwick
