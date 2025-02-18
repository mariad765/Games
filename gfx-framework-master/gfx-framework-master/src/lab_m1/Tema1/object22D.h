#pragma once

#include <string>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace object2D
{
	Mesh* CreateSquare1(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateTerrain1(const std::string& name, const std::vector<glm::vec2>& points, glm::vec3 color, float groundHeight, float heighOffset);
	Mesh* CreateTank1(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 colorLowerTrap, glm::vec3 colorBarrel, glm::vec3 colorUpperTrap, bool fill);
	Mesh* CreateTankBarrel(const std::string& name, glm::vec3 turretCenter, float turretRadius, float barrelWidth, float barrelLength, glm::vec3 colorBarrel);
	Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill = false);
	Mesh* CreateProjectilTrajectory(const std::string& name, const std::vector<glm::vec2>& points, glm::vec3 color, float groundHeight, float heightOffset);
	Mesh* CreateLifeBarOutline(const std::string& name, glm::vec3 position, float width, float height, glm::vec3 outlineColor);
	Mesh* CreateLifeBarFill(const std::string& name, glm::vec3 position, float width, float height, glm::vec3 fillColor);
}
