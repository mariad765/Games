#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


#include <vector>
#include <glm/glm.hpp>

class Object3D {
public:
    // Constructor care prime?te dimensiunile grid-ului
    Object3D(int m, int n);

    // Functie pentru a genera mesh-ul
    void generateMesh();

    // Func?ii pentru accesarea vertec?ilor ?i indicilor
    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;

private:
    int m, n; // Dimensiunile grid-ului
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
};