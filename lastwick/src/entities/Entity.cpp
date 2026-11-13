#include "entities/Entity.h"

namespace lastwick {

Entity::Entity(Vector2 position) : m_position(position), m_active(true) {}

Vector2 Entity::getPosition() const {
    return m_position;
}

void Entity::setPosition(Vector2 position) {
    m_position = position;
}

bool Entity::isActive() const {
    return m_active;
}

void Entity::setActive(bool active) {
    m_active = active;
}

}  // namespace lastwick
