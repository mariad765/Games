#include "object22D.h"
#include <iostream>

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare1(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateTerrain1(const std::string& name, const std::vector<glm::vec2>& points, glm::vec3 color, float groundHeight, float heightOffset) {
    // Verificăm dacă sunt suficiente puncte pentru a forma terenul
    if (points.size() < 2) {
        std::cerr << "Not enough points to create terrain!" << std::endl;
        return nullptr;
    }

    // Creăm un nou mesh
    Mesh* mesh = new Mesh(name);
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Creăm vârfuri și indici pentru fiecare pătrat între punctele adiacente
    for (size_t i = 0; i < points.size() - 1; ++i) {
        glm::vec2 p1 = points[i];
        glm::vec2 p2 = points[i + 1];

        // Calculăm proprietățile dreptunghiului
        float width = glm::length(p2 - p1);
        float height1 = p1.y - groundHeight; // Înălțimea de la pământ la primul punct
        float height2 = p2.y - groundHeight; // Înălțimea de la pământ la al doilea punct

        // Calculăm colțurile pătratului
        glm::vec3 bottomLeft(p1.x, groundHeight, 0);
        glm::vec3 topLeft(p1.x, p1.y, 0);
        glm::vec3 topRight(p2.x, p2.y, 0);
        glm::vec3 bottomRight(p2.x, groundHeight, 0);

        // Adăugăm vârfurile la vectorul de vârfuri
        vertices.emplace_back(bottomLeft, color);
        vertices.emplace_back(topLeft, color);
        vertices.emplace_back(topRight, color);
        vertices.emplace_back(bottomRight, color);

        // Adăugăm indici pentru dreptunghi
        size_t baseIndex = i * 4; // Fiecare pătrat are 4 vârfuri
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);

        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }

    // Creăm mesh-ul cu vârfuri și indici
    mesh->InitFromData(vertices, indices);
    return mesh;
}

///////////////////////////////////////////////////////////////////////////////////////////////
Mesh* object2D::CreateTank1(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 colorLowerTrap, glm::vec3 colorBarrel, glm::vec3 colorUpperTrap, bool fill) {

    Mesh* tankMesh = new Mesh(name);

    // Dimensiunile pentru cele două trapeze și turelă
    float baseWidth = length;
    float baseHeight = 0.25f * length; // Înălțime mai mică pentru trapezul inferior
    float upperOffset = 0.1f * length; // Offset pentru trapezul superior
    float lowerOffset = 0.15f * length; // Offset pentru trapezul inferior (mai mic)

    // Dimensiunile turelei și ale țevii tunului
    float turretRadius = 0.2f * length;
    float barrelWidth = 0.05f * length;
    float barrelLength = 0.4f * length;


    // Pregătirea vertecșilor și indicilor
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Crearea vertecșilor pentru trapezul inferior (mai mic și cu baza mare orientată în sus)
    glm::vec3 lowerTrapezoidBase = glm::vec3(leftBottomCorner.x + lowerOffset, leftBottomCorner.y, 0);
    vertices.emplace_back(lowerTrapezoidBase, colorLowerTrap); // Stânga jos
    vertices.emplace_back(glm::vec3(lowerTrapezoidBase.x + baseWidth - 2 * lowerOffset, lowerTrapezoidBase.y, 0), colorLowerTrap); // Dreapta jos
    vertices.emplace_back(glm::vec3(leftBottomCorner.x + baseWidth, leftBottomCorner.y + baseHeight, 0), colorLowerTrap); // Dreapta sus
    vertices.emplace_back(glm::vec3(leftBottomCorner.x, leftBottomCorner.y + baseHeight, 0), colorLowerTrap); // Stânga sus

    // Indicii pentru trapezul inferior
    indices.insert(indices.end(), { 0, 1, 2, 0, 2, 3 });

    // Crearea vertecșilor pentru trapezul superior (mai mare și cu baza mare orientată în jos)
    glm::vec3 upperTrapezoidBase = glm::vec3(leftBottomCorner.x - upperOffset, leftBottomCorner.y + baseHeight, 0);
    vertices.emplace_back(upperTrapezoidBase, colorUpperTrap); // Stânga jos
    vertices.emplace_back(glm::vec3(upperTrapezoidBase.x + baseWidth + 2 * upperOffset, upperTrapezoidBase.y, 0), colorUpperTrap); // Dreapta jos
    vertices.emplace_back(glm::vec3(upperTrapezoidBase.x + baseWidth + upperOffset, upperTrapezoidBase.y + baseHeight, 0), colorUpperTrap); // Dreapta sus
    vertices.emplace_back(glm::vec3(upperTrapezoidBase.x + upperOffset, upperTrapezoidBase.y + baseHeight, 0), colorUpperTrap); // Stânga sus

    // Indicii pentru trapezul superior
    indices.insert(indices.end(), { 4, 5, 6, 4, 6, 7 });

  
   // Crearea vertecșilor pentru arc de cerc al turelei
  // Crearea vertecșilor pentru arc de cerc al turelei
    int numSegments = 30;
    float angleStep = glm::pi<float>() / numSegments; // Jumătate de cerc
    glm::vec3 turretCenter = glm::vec3(leftBottomCorner.x + baseWidth / 2, leftBottomCorner.y + 2 * baseHeight, 0);

    // Adăugăm centrul turelei ca prim vertex
    vertices.emplace_back(turretCenter, colorBarrel);
    int centerIndex = vertices.size() - 1;

    // Creăm vertecșii pentru fiecare punct de pe arc
    for (int i = 1; i <= numSegments; ++i) {
        float angle = i * angleStep;
        glm::vec3 pos = turretCenter + glm::vec3(turretRadius * cos(angle), turretRadius * sin(angle), 0);
        vertices.emplace_back(pos, colorBarrel);

        // Adăugăm indicii pentru triunghiul-fan
        if (i >= 0) {
            indices.push_back(centerIndex);          // Centrul turelei
            indices.push_back(centerIndex + i);      // Punctul curent pe arc
            indices.push_back(centerIndex + i - 1);  // Punctul anterior pe arc
        }
    }

    float lineHeight = -0.5f * turretRadius; // Adjust as needed for the line's height
    vertices.emplace_back(turretCenter + glm::vec3(0, lineHeight, 0), colorBarrel); // Vertex for the line down

    // Add indices to connect the last arc point to the bottom line
    indices.push_back(centerIndex); // Center of turret
    indices.push_back(centerIndex + numSegments + 1); // Bottom point
    indices.push_back(centerIndex + 1); // First arc point

    // Setarea modului de desenare în funcție de parametru
    if (!fill) {
        tankMesh->SetDrawMode(GL_LINE_LOOP); // Dacă nu e umplut, mod linie
    }

    // Inițializarea mesh-ului cu vertecși și indici
    tankMesh->InitFromData(vertices, indices);

    return tankMesh;
}

Mesh* object2D::CreateTankBarrel(const std::string& name, glm::vec3 turretCenter, float turretRadius, float barrelWidth, float barrelLength, glm::vec3 colorBarrel) {
    Mesh* barrelMesh = new Mesh(name);

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Unghi de 45 de grade
    float barrelAngle = glm::pi<float>() / 4;
    // Punctul de start al țevii
    glm::vec3 barrelStart = turretCenter + glm::vec3(0, turretRadius, 0);

    int barrelStartIndex = vertices.size();

    // Adăugăm vertecșii pentru dreptunghiul țevii tunului
    vertices.emplace_back(barrelStart, colorBarrel); // Stânga jos
    vertices.emplace_back(barrelStart + glm::vec3(barrelWidth, 0, 0), colorBarrel); // Dreapta jos

    // Calculăm coordonatele pentru a înclina țevile la 45 de grade
    float deltaX = barrelLength * cos(barrelAngle); // componenta pe axa X
    float deltaY = barrelLength * sin(barrelAngle); // componenta pe axa Y

    // Ajustăm coordonatele pentru a roti țeava
    glm::vec3 barrelUpperRight = barrelStart + glm::vec3(barrelWidth, deltaY, 0); // Dreapta sus
    glm::vec3 barrelUpperLeft = barrelStart + glm::vec3(0, deltaY, 0); // Stânga sus

    // Adăugăm vertecșii pentru stânga sus și dreapta sus
    vertices.emplace_back(barrelUpperRight, colorBarrel); // Dreapta sus
    vertices.emplace_back(barrelUpperLeft, colorBarrel); // Stânga sus

    // Indicii pentru dreptunghiul țevii tunului
    indices.insert(indices.end(), {
        static_cast<unsigned int>(barrelStartIndex),     // Stânga jos
        static_cast<unsigned int>(barrelStartIndex + 1), // Dreapta jos
        static_cast<unsigned int>(barrelStartIndex + 2), // Dreapta sus
        static_cast<unsigned int>(barrelStartIndex),     // Stânga jos
        static_cast<unsigned int>(barrelStartIndex + 2), // Dreapta sus
        static_cast<unsigned int>(barrelStartIndex + 3)  // Stânga sus
        });

    // Initialize mesh with vertices and indices
    barrelMesh->InitFromData(vertices, indices);

    return barrelMesh;
}
///////////////////////////////////////////////////////////////////////////////////////////////

Mesh* object2D::CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill) {
	Mesh* circleMesh = new Mesh(name);

	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	// Adăugăm centrul cercului ca prim vertex
	vertices.emplace_back(center, color);
	int centerIndex = vertices.size() - 1;

	// Numărul de segmente pentru cerc
	int numSegments = 30;
	float angleStep = 2 * glm::pi<float>() / numSegments; // Un cerc complet

	// Creăm vertecșii pentru fiecare punct de pe cerc
	for (int i = 0; i <= numSegments; ++i) {
		float angle = i * angleStep;
		glm::vec3 pos = center + glm::vec3(radius * cos(angle), radius * sin(angle), 0);
		vertices.emplace_back(pos, color);

		// Adăugăm indicii pentru triunghiul-fan
		if (i > 0) {
			indices.push_back(centerIndex);          // Centrul cercului
			indices.push_back(centerIndex + i);      // Punctul curent pe cerc
			indices.push_back(centerIndex + i - 1);  // Punctul anterior pe cerc
		}
	}

	// Setarea modului de desenare în funcție de parametru
	if (!fill) {
		circleMesh->SetDrawMode(GL_LINE_LOOP); // Dacă nu e umplut, mod linie
	}

	// Inițializarea mesh-ului cu vertecși și indici
	circleMesh->InitFromData(vertices, indices);

	return circleMesh;
}

///////////////////////////////////////////////////////////////////////////////////////////////
Mesh* object2D::CreateProjectilTrajectory(const std::string& name, const std::vector<glm::vec2>& points, glm::vec3 color, float groundHeight, float heightOffset)
{
    // Create a new mesh object
    Mesh* trajectoryMesh = new Mesh(name);
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Add vertices based on the given points and adjust their height with heightOffset
    for (size_t i = 0; i < points.size(); ++i) {
        vertices.emplace_back(glm::vec3(points[i].x, points[i].y + heightOffset, groundHeight), color);
    }

    // Add indices to create line segments between points
    for (size_t i = 0; i < points.size() - 1; ++i) {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Set the vertices and indices in the mesh
    trajectoryMesh->InitFromData(vertices, indices);
    trajectoryMesh->SetDrawMode(GL_LINES);

    return trajectoryMesh;
}

////////////////////////////////////////////////////////////////////////////////////////////////
Mesh* object2D::CreateLifeBarOutline(const std::string& name, glm::vec3 position, float width, float height, glm::vec3 outlineColor)
{

    std::vector<VertexFormat> outlineVertices = {
       VertexFormat(position, outlineColor),                                 // Bottom-left
       VertexFormat(position + glm::vec3(width, 0, 0), outlineColor),        // Bottom-right
       VertexFormat(position + glm::vec3(width, height, 0), outlineColor),   // Top-right
       VertexFormat(position + glm::vec3(0, height, 0), outlineColor)        // Top-left
    };

    std::vector<unsigned int> outlineIndices = {
           0, 1,  // Bottom edge
           1, 2,  // Right edge
           2, 3,  // Top edge
           3, 0   // Left edge
    };

    // Create and initialize the mesh with vertices and indices
    Mesh* lifeBar = new Mesh(name);
    lifeBar->InitFromData(outlineVertices, outlineIndices);

    // Set draw mode to GL_LINE_LOOP to draw as an outline
    lifeBar->SetDrawMode(GL_LINE_LOOP);

    return lifeBar;
}

Mesh* object2D::CreateLifeBarFill( const std::string& name, glm::vec3 position, float width, float height, glm::vec3 fillColor)
{
    // Define vertices for the filled rectangle with fill color
    std::vector<VertexFormat> vertices = {
        VertexFormat(position, fillColor),
        VertexFormat(position + glm::vec3(width, 0, 0), fillColor),
        VertexFormat(position + glm::vec3(width, height, 0), fillColor),
        VertexFormat(position + glm::vec3(0, height, 0), fillColor)
    };

    // Indices to draw the rectangle as two triangles
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

    // Create and initialize the mesh with vertices and indices
    Mesh* healthBar = new Mesh(name);
    healthBar->InitFromData(vertices, indices);

    return healthBar;
}

