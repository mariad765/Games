#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

class Terrain {
public:
    Terrain(float size, const glm::vec2& position);  // Constructor
    ~Terrain();                                       // Destructor

    void Render() const;                             // Render function
    const std::vector<glm::vec2>& GetPoints() const; // Getter for points

private:
    void GenerateTerrain();                          // Function to generate terrain data

    float size;                                      // Size of the terrain
    glm::vec2 position;                              // Position of the terrain
    GLuint vbo;                                      // Vertex Buffer Object
    std::vector<glm::vec2> points;                  // Vector of terrain points
};
