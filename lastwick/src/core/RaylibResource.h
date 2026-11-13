#ifndef LASTWICK_CORE_RAYLIBRESOURCE_H
#define LASTWICK_CORE_RAYLIBRESOURCE_H

#include <string>
#include <utility>

#include "raylib.h"

namespace lastwick {

/**
 * @brief Invólucro RAII para textura de renderização em memória (RenderTexture2D).
 *
 * Garante a criação da textura secundária na placa gráfica e a sua correta
 * liberação da memória de vídeo ao fim do ciclo de vida da instância.
 */
class RenderTextureWrapper {
public:
    /**
     * @brief Construtor padrão que inicializa uma textura vazia.
     */
    RenderTextureWrapper() : m_renderTexture{}, m_loaded(false) {}

    /**
     * @brief Construtor que aloca uma textura de renderização com dimensões especificadas.
     * @param width Largura da textura em pixels.
     * @param height Altura da textura em pixels.
     */
    RenderTextureWrapper(int width, int height)
        : m_renderTexture(LoadRenderTexture(width, height)), m_loaded(true) {}

    /**
     * @brief Destrutor que descarrega a textura de renderização da VRAM caso esteja ativa.
     */
    ~RenderTextureWrapper() { unload(); }

    RenderTextureWrapper(const RenderTextureWrapper&) = delete;
    RenderTextureWrapper& operator=(const RenderTextureWrapper&) = delete;

    RenderTextureWrapper(RenderTextureWrapper&& other) noexcept
        : m_renderTexture(other.m_renderTexture), m_loaded(other.m_loaded) {
        other.m_renderTexture = {};
        other.m_loaded = false;
    }

    RenderTextureWrapper& operator=(RenderTextureWrapper&& other) noexcept {
        if (this != &other) {
            unload();
            m_renderTexture = other.m_renderTexture;
            m_loaded = other.m_loaded;
            other.m_renderTexture = {};
            other.m_loaded = false;
        }
        return *this;
    }

    /**
     * @brief Recria a textura com novas dimensões caso necessário.
     * @param width Nova largura em pixels.
     * @param height Nova altura em pixels.
     */
    void reset(int width, int height) {
        unload();
        m_renderTexture = LoadRenderTexture(width, height);
        m_loaded = true;
    }

    /**
     * @brief Libera a textura atualmente alocada.
     */
    void unload() {
        if (m_loaded) {
            UnloadRenderTexture(m_renderTexture);
            m_renderTexture = {};
            m_loaded = false;
        }
    }

    /**
     * @brief Obtém referência constante para a estrutura interna da raylib.
     * @return Referência constante para RenderTexture2D.
     */
    const RenderTexture2D& get() const { return m_renderTexture; }

    /**
     * @brief Obtém referência modificável para a estrutura interna da raylib.
     * @return Referência para RenderTexture2D.
     */
    RenderTexture2D& get() { return m_renderTexture; }

    /**
     * @brief Verifica se a textura foi carregada e está válida.
     * @return Verdadeiro se a textura estiver alocada.
     */
    bool isLoaded() const { return m_loaded; }

private:
    RenderTexture2D m_renderTexture;
    bool m_loaded;
};

/**
 * @brief Guarda RAII para o subsistema de áudio da biblioteca raylib.
 *
 * Inicializa os canais de áudio na construção e finaliza o dispositivo
 * no momento da destruição.
 */
class AudioDeviceGuard {
public:
    /**
     * @brief Construtor que inicializa o dispositivo de áudio.
     */
    AudioDeviceGuard() { InitAudioDevice(); }

    /**
     * @brief Destrutor que encerra o dispositivo de áudio.
     */
    ~AudioDeviceGuard() { CloseAudioDevice(); }

    AudioDeviceGuard(const AudioDeviceGuard&) = delete;
    AudioDeviceGuard& operator=(const AudioDeviceGuard&) = delete;
    AudioDeviceGuard(AudioDeviceGuard&&) = delete;
    AudioDeviceGuard& operator=(AudioDeviceGuard&&) = delete;

    /**
     * @brief Verifica se o dispositivo de áudio foi inicializado com sucesso.
     * @return Verdadeiro caso o subsistema de som esteja operante.
     */
    bool isReady() const { return IsAudioDeviceReady(); }
};

/**
 * @brief Invólucro RAII para efeitos sonoros (Sound).
 *
 * Controla o carregamento de arquivos de áudio em memória e sua respectiva
 * desocupação ao término da utilização.
 */
class SoundWrapper {
public:
    /**
     * @brief Construtor padrão sem efeito associado.
     */
    SoundWrapper() : m_sound{}, m_loaded(false) {}

    /**
     * @brief Construtor que carrega um arquivo sonoro a partir do caminho informado.
     * @param filePath Caminho para o arquivo de áudio.
     */
    explicit SoundWrapper(const std::string& filePath)
        : m_sound(LoadSound(filePath.c_str())), m_loaded(IsSoundValid(m_sound)) {}

    /**
     * @brief Destrutor que descarrega a amostra de som.
     */
    ~SoundWrapper() { unload(); }

    SoundWrapper(const SoundWrapper&) = delete;
    SoundWrapper& operator=(const SoundWrapper&) = delete;

    SoundWrapper(SoundWrapper&& other) noexcept : m_sound(other.m_sound), m_loaded(other.m_loaded) {
        other.m_sound = {};
        other.m_loaded = false;
    }

    SoundWrapper& operator=(SoundWrapper&& other) noexcept {
        if (this != &other) {
            unload();
            m_sound = other.m_sound;
            m_loaded = other.m_loaded;
            other.m_sound = {};
            other.m_loaded = false;
        }
        return *this;
    }

    /**
     * @brief Toca a amostra sonora associada.
     */
    void play() const {
        if (m_loaded) {
            PlaySound(m_sound);
        }
    }

    /**
     * @brief Descarrega o som da memória.
     */
    void unload() {
        if (m_loaded) {
            UnloadSound(m_sound);
            m_sound = {};
            m_loaded = false;
        }
    }

    /**
     * @brief Verifica se o som está devidamente carregado.
     * @return Verdadeiro caso o som esteja pronto para reprodução.
     */
    bool isLoaded() const { return m_loaded; }

private:
    Sound m_sound;
    bool m_loaded;
};

/**
 * @brief Invólucro RAII para tipografias personalizadas (Font).
 *
 * Gerencia o ciclo de vida e a liberação de glifos carregados do disco.
 */
class FontWrapper {
public:
    /**
     * @brief Construtor padrão de fonte não carregada.
     */
    FontWrapper() : m_font{}, m_loaded(false) {}

    /**
     * @brief Carrega uma fonte tipográfica com altura fixa de glifos.
     * @param filePath Caminho do arquivo tipográfico TTF/OTF.
     * @param fontSize Tamanho base dos caracteres em pixels.
     */
    FontWrapper(const std::string& filePath, int fontSize)
        : m_font(LoadFontEx(filePath.c_str(), fontSize, nullptr, 0)),
          m_loaded(IsFontValid(m_font)) {}

    /**
     * @brief Destrutor que descarrega a fonte da memória.
     */
    ~FontWrapper() { unload(); }

    FontWrapper(const FontWrapper&) = delete;
    FontWrapper& operator=(const FontWrapper&) = delete;

    FontWrapper(FontWrapper&& other) noexcept : m_font(other.m_font), m_loaded(other.m_loaded) {
        other.m_font = {};
        other.m_loaded = false;
    }

    FontWrapper& operator=(FontWrapper&& other) noexcept {
        if (this != &other) {
            unload();
            m_font = other.m_font;
            m_loaded = other.m_loaded;
            other.m_font = {};
            other.m_loaded = false;
        }
        return *this;
    }

    /**
     * @brief Obtém a estrutura interna de fonte da raylib.
     * @return Estrutura Font correspondente.
     */
    const Font& get() const { return m_font; }

    /**
     * @brief Descarrega a fonte.
     */
    void unload() {
        if (m_loaded) {
            UnloadFont(m_font);
            m_font = {};
            m_loaded = false;
        }
    }

    /**
     * @brief Verifica se a fonte foi carregada com sucesso.
     * @return Verdadeiro se a fonte for válida.
     */
    bool isLoaded() const { return m_loaded; }

private:
    Font m_font;
    bool m_loaded;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_RAYLIBRESOURCE_H
