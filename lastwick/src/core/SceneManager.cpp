#include "core/SceneManager.h"

#include <utility>

namespace lastwick {

SceneManager::SceneManager()
    : m_currentScene(nullptr), m_pendingScene(nullptr), m_shouldClose(false) {}

SceneManager::~SceneManager() = default;

void SceneManager::changeScene(std::unique_ptr<Scene> nextScene) {
    m_pendingScene = std::move(nextScene);
}

void SceneManager::update(float deltaTime) {
    if (m_pendingScene) {
        m_currentScene = std::move(m_pendingScene);
        if (m_currentScene) {
            m_currentScene->init();
        }
    }

    if (m_currentScene) {
        m_currentScene->update(deltaTime);
    }
}

void SceneManager::draw() {
    if (m_currentScene) {
        m_currentScene->draw();
    }
}

bool SceneManager::shouldClose() const {
    return m_shouldClose;
}

void SceneManager::requestClose() {
    m_shouldClose = true;
}

}  // namespace lastwick
