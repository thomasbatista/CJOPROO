#include "core/SceneManager.h"

#include <algorithm>
#include <utility>

#include "core/AudioManager.h"
#include "core/GameConfig.h"
#include "raylib.h"

namespace lastwick {

SceneManager::SceneManager()
    : m_currentScene(nullptr),
      m_pendingScene(nullptr),
      m_shouldClose(false),
      m_fadeState(FadeState::None),
      m_fadeTimer(0.0f),
      m_eventBus(),
      m_musicPlayer() {
    m_musicPlayer.registerEvents(m_eventBus);
    m_musicPlayer.playMenuTheme();
}

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
    // Tecla M alterna o silenciamento global de áudio em qualquer cena
    if (IsKeyPressed(KEY_M)) {
        toggleAudioMute();
    }

    // Atualização contínua do fluxo de música procedural
    m_musicPlayer.update();

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

    // Camada visual de transição suave
    if (m_fadeState != FadeState::None) {
        float progress = m_fadeTimer / GameConfig::kSceneFadeDuration;
        progress = std::clamp(progress, 0.0f, 1.0f);

        float alpha = (m_fadeState == FadeState::FadeOut) ? progress : (1.0f - progress);
        alpha = std::clamp(alpha, 0.0f, 1.0f);

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, alpha));
    }

    // Aviso discreto de silenciamento global de áudio
    if (isAudioMuted()) {
        const char* muteNotice = "Som desligado";
        int fontSize = 16;
        int textWidth = MeasureText(muteNotice, fontSize);
        int noticeX = GetScreenWidth() - textWidth - 16;
        int noticeY = 16;
        DrawRectangle(noticeX - 8, noticeY - 4, textWidth + 16, 26, Color{18, 20, 26, 215});
        DrawRectangleLines(noticeX - 8, noticeY - 4, textWidth + 16, 26, Color{100, 48, 48, 220});
        DrawText(muteNotice, noticeX, noticeY, fontSize, Color{235, 95, 90, 240});
    }
}

bool SceneManager::shouldClose() const {
    return m_shouldClose;
}

void SceneManager::requestClose() {
    m_shouldClose = true;
}

EventBus& SceneManager::getEventBus() {
    return m_eventBus;
}

MusicPlayer& SceneManager::getMusicPlayer() {
    return m_musicPlayer;
}

bool SceneManager::isAudioMuted() const {
    return AudioManager::isMuted();
}

void SceneManager::toggleAudioMute() {
    AudioManager::toggleMute();
    m_musicPlayer.setMuted(AudioManager::isMuted());
}

}  // namespace lastwick
