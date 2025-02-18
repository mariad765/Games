#include "object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

#include <cmath>
#include <glm/glm.hpp>

Object3D::Object3D(int m, int n) : m(m), n(n) {
    generateMesh();
}

void Object3D::generateMesh() {
    // Generare vertec?i pentru un grid m x n
    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            // Coordonatele (x, z) ale fiec?rui vertex, y ini?ial 0
            vertices.push_back(glm::vec3(i, 0.0f, j));
        }
    }

    // Generare indici pentru triunghiuri
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            int topLeft = i * (n + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * (n + 1) + j;
            int bottomRight = bottomLeft + 1;

            // Primul triunghi
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Al doilea triunghi
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

const std::vector<glm::vec3>& Object3D::getVertices() const {
    return vertices;
}

const std::vector<unsigned int>& Object3D::getIndices() const {
    return indices;
}
