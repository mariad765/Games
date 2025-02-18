// ParticleSystem.h
#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <glm/glm.hpp>
#include <cstdlib>  // for rand()

struct VertexFormat; // Assuming VertexFormat is defined elsewhere

class ParticleSystem {
public:
    // Particle structure
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec4 color;
        float lifespan;
        float size;
    };

    ParticleSystem(int maxParticles, glm::vec3 origin);

    void emitParticle();
    void updateParticles(float deltaTime);
    void drawParticles();

private:
    float randFloat(float min, float max);

    std::vector<Particle> particles;
    int maxParticles;
    glm::vec3 origin;
};

#endif // PARTICLE_SYSTEM_H
