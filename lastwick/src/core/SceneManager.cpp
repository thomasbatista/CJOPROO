#include "core/SceneManager.h"

#include <algorithm>
#include <utility>

#include "core/GameConfig.h"
#include "raylib.h"

namespace lastwick {

SceneManager::SceneManager()
    : m_currentScene(nullptr),
      m_pendingScene(nullptr),
      m_shouldClose(false),
      m_fadeState(FadeState::None),
      m_fadeTimer(0.0f) {}

SceneManager::~SceneManager() = default;

void SceneManager::changeScene(std::unique_ptr<Scene> nextScene) {
    if (!m_currentScene) {
        // Primeira inicialização direta da cena
        m_currentScene = std::move(nextScene);
        if (m_currentScene) {
            m_currentScene->init();
            m_fadeState = FadeState::FadeIn;
            m_fadeTimer = 0.0f;
        }
        return;
    }

    // Inicia a transição escurecendo a cena atual
    m_pendingScene = std::move(nextScene);
    m_fadeState = FadeState::FadeOut;
    m_fadeTimer = 0.0f;
}

void SceneManager::update(float deltaTime) {
    if (m_fadeState == FadeState::FadeOut) {
        m_fadeTimer += deltaTime;
        if (m_fadeTimer >= GameConfig::kSceneFadeDuration) {
            m_fadeTimer = 0.0f;
            m_fadeState = FadeState::FadeIn;
            m_currentScene = std::move(m_pendingScene);
            if (m_currentScene) {
                m_currentScene->init();
            }
        }
    } else if (m_fadeState == FadeState::FadeIn) {
        m_fadeTimer += deltaTime;
        if (m_currentScene) {
            m_currentScene->update(deltaTime);
        }
        if (m_fadeTimer >= GameConfig::kSceneFadeDuration) {
            m_fadeState = FadeState::None;
            m_fadeTimer = 0.0f;
        }
    } else {
        if (m_currentScene) {
            m_currentScene->update(deltaTime);
        }
    }
}

void SceneManager::draw() {
    if (m_currentScene) {
        m_currentScene->draw();
    }

    if (m_fadeState != FadeState::None) {
        float progress = m_fadeTimer / GameConfig::kSceneFadeDuration;
        progress = std::clamp(progress, 0.0f, 1.0f);

        float alpha = (m_fadeState == FadeState::FadeOut) ? progress : (1.0f - progress);
        alpha = std::clamp(alpha, 0.0f, 1.0f);

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, alpha));
    }
}

bool SceneManager::shouldClose() const {
    return m_shouldClose;
}

void SceneManager::requestClose() {
    m_shouldClose = true;
}

}  // namespace lastwick
