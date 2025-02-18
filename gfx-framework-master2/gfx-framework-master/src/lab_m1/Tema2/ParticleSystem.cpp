// ParticleSystem.cpp
#include "ParticleSystem.h"
#include "utils/glm_utils.h"
#include <glm/gtc/constants.hpp> // for glm::pi
#include <GL/glew.h>-

ParticleSystem::ParticleSystem(int maxParticles, glm::vec3 origin)
    : maxParticles(maxParticles), origin(origin) {
    particles.reserve(maxParticles);
}

void ParticleSystem::emitParticle() {
    if (particles.size() < maxParticles) {
        Particle particle;
        particle.position = origin;
        particle.velocity = glm::vec3(
            randFloat(-0.5f, 0.5f),
            randFloat(-0.1f, 0.5f),
            randFloat(-0.5f, 0.5f)
        );
        particle.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Bright yellow
        particle.lifespan = randFloat(1.0f, 3.0f);
        particle.size = randFloat(0.05f, 0.1f);
        particles.push_back(particle);
    }
}

void ParticleSystem::updateParticles(float deltaTime) {
    for (auto& particle : particles) {
        particle.position += particle.velocity * deltaTime;
        particle.color.a -= deltaTime / particle.lifespan; // Fade out
        particle.lifespan -= deltaTime;
    }
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](Particle& p) {
            return p.lifespan <= 0.0f;
            }),
        particles.end()
    );
}

void ParticleSystem::drawParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);

    for (const auto& particle : particles) {
        glPointSize(particle.size * 100.0f); // Scale size for visibility
        glBegin(GL_POINTS);
        glColor4f(particle.color.r, particle.color.g, particle.color.b, particle.color.a);
        glVertex3f(particle.position.x, particle.position.y, particle.position.z);
        glEnd();
    }

    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
}

float ParticleSystem::randFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}
