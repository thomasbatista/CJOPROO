#include "core/ParticleSystem.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"

namespace lastwick {

ParticleSystem::ParticleSystem() {
    m_particles.reserve(256);
}

void ParticleSystem::registerEvents(EventBus& eventBus) {
    eventBus.subscribe(EventType::EnemyKilled,
                       [this](const Event& event) { emitEnemyExplosion(event.position); });

    eventBus.subscribe(EventType::PickupCollected,
                       [this](const Event& event) { emitPickupSparks(event.position); });
}

void ParticleSystem::emitEnemyExplosion(Vector2 origin) {
    for (int i = 0; i < GameConfig::kParticleEnemyExplosionCount; ++i) {
        float angle = static_cast<float>(GetRandomValue(0, 359)) * (3.14159265f / 180.0f);
        float speed = static_cast<float>(GetRandomValue(40, 130));
        float radius = static_cast<float>(GetRandomValue(25, 45)) * 0.1f;
        float maxLife = static_cast<float>(GetRandomValue(35, 60)) * 0.01f;

        // Partículas em tons avermelhados e sombrios da dissipação das sombras
        unsigned char red = static_cast<unsigned char>(GetRandomValue(180, 240));
        unsigned char green = static_cast<unsigned char>(GetRandomValue(30, 60));
        unsigned char blue = static_cast<unsigned char>(GetRandomValue(30, 60));
        Color col{red, green, blue, 240};

        Vector2 vel{std::cos(angle) * speed, std::sin(angle) * speed};
        m_particles.push_back(Particle{origin, vel, col, radius, 0.0f, maxLife});
    }
}

void ParticleSystem::emitPickupSparks(Vector2 origin) {
    for (int i = 0; i < GameConfig::kParticlePickupSparksCount; ++i) {
        float angle = static_cast<float>(GetRandomValue(0, 359)) * (3.14159265f / 180.0f);
        float speed = static_cast<float>(GetRandomValue(30, 100));
        float radius = static_cast<float>(GetRandomValue(20, 35)) * 0.1f;
        float maxLife = static_cast<float>(GetRandomValue(30, 50)) * 0.01f;

        // Faíscas douradas e alaranjadas reluzentes
        unsigned char red = static_cast<unsigned char>(GetRandomValue(230, 255));
        unsigned char green = static_cast<unsigned char>(GetRandomValue(190, 230));
        unsigned char blue = static_cast<unsigned char>(GetRandomValue(50, 90));
        Color col{red, green, blue, 255};

        Vector2 vel{std::cos(angle) * speed, std::sin(angle) * speed};
        m_particles.push_back(Particle{origin, vel, col, radius, 0.0f, maxLife});
    }
}

void ParticleSystem::update(float deltaTime) {
    for (auto& p : m_particles) {
        p.position.x += p.velocity.x * deltaTime;
        p.position.y += p.velocity.y * deltaTime;
        p.velocity.x *= 0.92f;
        p.velocity.y *= 0.92f;
        p.life += deltaTime;
    }

    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
                                     [](const Particle& p) { return p.life >= p.maxLife; }),
                      m_particles.end());
}

void ParticleSystem::draw() const {
    for (const auto& p : m_particles) {
        float progress = p.life / p.maxLife;
        if (progress > 1.0f) {
            progress = 1.0f;
        }

        float curRadius = p.radius * (1.0f - progress * 0.45f);
        Color curColor = p.color;
        curColor.a = static_cast<unsigned char>(static_cast<float>(p.color.a) * (1.0f - progress));

        DrawCircleV(p.position, curRadius, curColor);
    }
}

void ParticleSystem::clear() {
    m_particles.clear();
}

size_t ParticleSystem::getParticleCount() const {
    return m_particles.size();
}

}  // namespace lastwick
