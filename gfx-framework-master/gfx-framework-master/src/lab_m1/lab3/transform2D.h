#pragma once

#include "utils/glm_utils.h"


namespace transform2D
{
    // Translate matrix
    inline glm::mat3 Translate(float translateX, float translateY)
    {
        return glm::mat3(1, 0, 0,    // First row
            0, 1, 0,    // Second row
            translateX, translateY, 1); // Third row

    }

    // Scale matrix
    inline glm::mat3 Scale(float scaleX, float scaleY)
    {
        // TODO(student): Implement the scaling matrix
        return glm::mat3(scaleX, 0.0f, 0.0f,
            0.0f, scaleY, 0.0f,
            0.0f, 0.0f, 1.0f);
    }

    

    // Rotate matrix
    inline glm::mat3 Rotate(float radians)
    {
        return glm::mat3(
            cos(radians), -sin(radians), 0,    // First row
            sin(radians), cos(radians), 0,    // Second row
            0, 0, 1                               // Third row (for homogeneous coordinates)
        );

    }
}   // namespace transform2D
