#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <utility> // for std::pair
#include <random>  // for random number generation
#include <vector>
#include <glm/vec2.hpp>
#include <cmath> // For cos, sin

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object22D.h"

using namespace std;
using namespace m1;
int width = 720;  // Number of points in the x-direction
float scale = 80.0f; // Scale factor to spread the points
float groundHeight = 0.0f; // Set the ground level
float heightOffset = 0.0f; // Move the terrain up by 50 units
bool isBarrelActive = false; // Inițial, țeava nu este activată
float turretRadius = 0.2f * 50;
float projectileRadius = 0.1f * 50;
float tankRadius = 1.5f * 50;
glm::vec3 offsetBarrel = glm::vec3(25, 25,0);
glm::vec3 offsetTeajectory = glm::vec3(25, 50,0);
int projectileCounter = 0;
int projectileCounterTank2 = 0;
// Define tank dimensions
float  length = 50; // Example width
// Dimensiunile pentru cele două trapeze și turelă
float height = 0.27f * length; // Înălțimea trapezului
float baseWidth = length;
float baseHeight = 0.3f * length; // Înălțime mai mică pentru trapezul inferior
float upperOffset = 0.1f * length; // Offset pentru trapezul superior
float lowerOffset = 0.15f * length; // Offset pentru trapezul inferior (mai mic)
float explaosionDepth = 40.0f;
float explosionRadius = 100.0f;
float terraincounter = 0.0f;
bool terrainExploded = false;
float explosionCounter = 0.0f;
float explosionCounterTank2 = 0.0f;
// Dimensiunile turelei și ale țevii tunului
float barrelWidth = 0.05f * length;
float barrelLength = 0.4f * length;
float rotateBarrel = 0.0f;
float rotateBarrelTank2 = 0.0f;
//Lifebar
float lifeBarWidth = 80;
float lifeBarWidthTank2 = 80;
float lifeBarHeight = 15;
float lifeBarHeightTank2 = 15;
int collisionWasDoneToTank = 0;
int collisionWasDoneToTankTank2 = 0;
const int delayFrames = 15;
const int delayFramesmin = 5;
std::vector<bool> oldWay1(20, false);
std::vector<bool> lifeChanges(20, false);
std::vector<bool> startProjectile(20, false);
std::vector<float> counterDestroy(20, 0.0f);
std::vector<float> tankAngleOld(20, 0.0f);
std::vector<float> tankXOld(20, 0.0f);
std::vector<float> tankYOld(20, 0.0f);
std::vector<float> projectileInterpolationIndex(20, 0.0f);
std::vector< std::vector<glm::vec2>> projectilePointsOld(20); 
std::vector<bool> projectileCollidedWithTank(20, false);
std::vector<int> projectileCollisionDelayCounter(20, 0);  // Frame counter to delay ground collision check
std::vector<int> projectileCollisionDelayCounterTank2(20, 0);  // Frame counter to delay ground collision check
std::vector<int> collisionDelayCounter(20, 0);  // Frame counter to delay ground collision check
std::vector<int> collisionDelayCounterTank2(20, 0);  // Frame counter to delay ground collision check
std::vector<bool> oldWay1Tank2(20, false);
std::vector<bool> lifeChangesTank2(20, false);
std::vector<bool> startProjectileTank2(20, false);
std::vector<bool> projectileCollidedWithTank2(20, false);
std::vector<float> counterDestroyTank2(20, 0.0f);
std::vector<float> tankAngleOldTank2(20, 0.0f);
std::vector<float> tankXOldTank2(20, 0.0f);
std::vector<float> tankYOldTank2(20, 0.0f);
std::vector<float> projectileInterpolationIndexTank2(20, 0.0f);
std::vector< std::vector<glm::vec2>> projectilePointsOldTank2(20);

///////////////////////////////////////////////////////////Functions///////////////////////////////////////////////////////////////////////

void CreateExplosionInTerrain(std::vector<glm::vec2>& terrainPoints, float explosionX, float explosionY, float radius, float depth) {
    for (auto& point : terrainPoints) {
        // Calculăm distanța dintre punctul curent și centrul exploziei
        float distance = glm::distance(glm::vec2(explosionX, explosionY), point);

        // Dacă punctul se află în interiorul razei de explozie, reducem înălțimea lui `y`
        if (distance < radius) {
            // Calculăm un factor de scădere pe baza distanței față de centru
            float factor = 1.0f - (distance / radius);
            point.y -= depth * factor;  // Ajustăm înălțimea proporțional cu `factor`
        }
    }
}

bool CheckCollision(const glm::vec3& projectilePosition, float projectileRadius,
    const glm::vec3& tankPosition, float tankRadius) {
    // Calculate the distance between the projectile and the tank
    float distance = glm::distance(projectilePosition, tankPosition);

    // Check if the distance is less than the sum of the radii
    if (distance < (projectileRadius + tankRadius)) {
        return true; // Collision detected
    }
    return false; // No collision
}


bool CheckGroundCollision(const std::vector<glm::vec2>& terrainPoints, float projectileX, float projectileY, float projectileRadius, float threshold) {
    // Iterate over each segment between adjacent terrain points
    for (size_t i = 0; i < terrainPoints.size() - 1; ++i) {
        glm::vec2 p1 = terrainPoints[i];
        glm::vec2 p2 = terrainPoints[i + 1];

        // Check if the projectile's X is within the range of the current segment
        if ((projectileX >= p1.x && projectileX <= p2.x) || (projectileX >= p2.x && projectileX <= p1.x)) {
            // Check if the projectile is below or near the terrain at this X position
            // The projectile's "bottom" is its Y position minus its radius
            float projectileBottomY = projectileY - projectileRadius;

            // We check if the projectile is within the threshold of the terrain segment
            float minY = std::min(p1.y, p2.y);  // minimum height of the terrain segment
            float maxY = std::max(p1.y, p2.y);  // maximum height of the terrain segment

            // If the projectile's bottom Y is between the terrain's min and max Y, and within the threshold distance
            if (projectileBottomY >= minY - threshold && projectileBottomY <= maxY + threshold) {
                return true;  // Collision detected
            }
        }
    }

    return false;  // No collision detected
}



std::vector<glm::vec2> GenerateProjectilePoints(float barrelX, float barrelY, float scale, float barrelAngle) {
    std::vector<glm::vec2> points;

    // Constants for the motion
    float gravity = 9.81f; // Acceleration due to gravity (m/s^2)
    float initialVelocity = 5.0f; // Initial velocity of the projectile (m/s)
    float angleRad = glm::radians(barrelAngle); // Convert angle to radians

    // Time step for generating points (smaller value for more points)
    float timeStep = 0.01f;

    // Generate points until the projectile hits the ground
    for (float t = 0.0f; t <= 5.0f; t += timeStep) {
        // Position at time t based on kinematic equations
        float x = barrelX + initialVelocity * t * cos(angleRad) * scale;
        float y = barrelY + (initialVelocity * t * sin(angleRad) - 0.5f * gravity * t * t) * scale;

        // Stop adding points if the projectile hits the ground (y < 0)
        if (y < 0) {
            break;
        }

        points.emplace_back(x, y);
    }

    return points;
}



glm::vec2 CalculateTankCenter(float tankX, float tankY) {
    // Assuming the tank is centered at (tankX, tankY) and the dimensions are:
    float halfLength = 0;
    float halfWidth = 0; // If your tank has a specific width

    // Calculate the center of the tank
    glm::vec2 centerPosition;
    centerPosition.x = tankX + halfLength; // Adjust for the length
    centerPosition.y = tankY + halfWidth - 16;  // Adjust for the width

    return centerPosition; // Return the center position
}


glm::vec2 CalculateTankTipPosition(float tankX, float tankY, float tankAngle) {
    glm::vec2 tankTipPosition;

    glm::vec2 tankCenter = CalculateTankCenter(tankX, tankY);

    tankTipPosition.x = tankCenter.x + cos(tankAngle) * height;
    tankTipPosition.y = tankCenter.y + sin(tankAngle) * height;

    return tankTipPosition; // Return the position of the barrel tip
}


glm::vec2 CalculateBarrelTip(float tankX, float tankY, float barrelAngle) {
    glm::vec2 tipPosition;

    glm::vec2 tankTip = CalculateTankTipPosition(tankX, tankY, barrelAngle);

    tipPosition.x = tankTip.x + cos(barrelAngle) * barrelLength;
    tipPosition.y = tankTip.y + sin(barrelAngle) * barrelLength;
  
    return tipPosition; // Return the tip position
}



// Function to calculate the angle of orientation based on terrain slope
float CalculateTerrainSlopeAngle(const std::vector<glm::vec2>& terrainPoints1, int ind) {

	// find the y values for the 3 points
	float xPos1 = terrainPoints1[ind].x;
	float xPos2 = terrainPoints1[ind+1].x;
	float xPos3 = terrainPoints1[ind-1].x;

    // find the y values for the 3 points
    float yPos1 = terrainPoints1[ind].y;
    float yPos2 = terrainPoints1[ind + 1].y;
    float yPos3 = terrainPoints1[ind - 1].y;
	
	// calculate the slope of the terrain
	float slope1 = (yPos2 - yPos1) / (xPos2 - xPos1);
	float slope2 = (yPos3 - yPos1) / (xPos3 - xPos1);

	// calculate the angle of orientation
	float angle1 = atan(slope1);
	float angle2 = atan(slope2);

	// calculate the average of the two angles
	float angle = (angle1 + angle2) / 2;

	return angle;
 
}



std::vector<glm::vec2> GenerateTerrainPoints(int width, int height, float scale) {
    std::vector<glm::vec2> points;
    float x = 0;

        for (int j = 0; j < width; j++) {
            // Convertim coordonatele în coordonate logice
            
            float xPos =  x;
         
             float heightValue =0.9f* sin(x*1.4f) + 1.2f * sin(0.7f * x)  ;

            // Exemplu de funcție complexă pentru a genera înălțimi
           //float heightValue = 50 * sin(0.1f * xPos) + 30 * cos(0.05f * yPos);

            points.emplace_back(xPos*scale, heightValue*scale+250);
		    x = x + 0.1;
           
        }

    
        return points;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
    
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;
    projectileCounter = 0;
    projectileCounterTank2 = 0;
    // Initialize translation variables
    translateX1 = 0;
    translateY1 = 0; 
	translateX2 = 0;
	translateY2 = 0;

    for (int i = 0; i < tankAngleOld.size(); i++) {
        tankAngleOld[i] = 0;
      
    }

    for (int i = 0; i < tankAngleOldTank2.size(); i++) {
        tankAngleOldTank2[i] = 0;

    }
 
	point = glm::vec2(0, 0);
    // Initialize scale factors (if needed)
    scaleX = 1;
    scaleY = 1;
    // Initialize angularStep
    angularStep = 0;
    bAngle = 0.0f;
    bAngleTank2 = 0.0f;

	for (int i = 0; i < projectileInterpolationIndex.size(); i++) {
        projectileInterpolationIndex[i] = 0.0f;
	}
    for (int i = 0; i < projectileInterpolationIndexTank2.size(); i++) {
        projectileInterpolationIndexTank2[i] = 0.0f;
    }
    isMovingRight = false; // Inițial, tancul nu se mișcă la dreapta
    isMovingRightTank2 = false; // Inițial, tancul nu se mișcă la dreapta
    isMovingLeft = false; // Inițial, tancul nu se mișcă la stânga
    isMovingLeftTank2 = false; // Inițial, tancul nu se mișcă la stânga
	for (int i = 0; i < startProjectile.size(); i++)
	{
        startProjectile[i]=false;
	}
    for (int i = 0; i < startProjectileTank2.size(); i++)
    {
        startProjectileTank2[i] = false;
    }
    modifyTrajectory = false;
    modifyTrajectoryTank2 = false;

    for (int i = 0; i < oldWay1.size(); i++) {
		oldWay1[i] = false;
    }
    for (int i = 0; i < oldWay1Tank2.size(); i++) {
        oldWay1Tank2[i] = false;
    }
    barrelToTheLeft = false;
    barrelToTheLeftTank2 = false;
    barrelToTheRight = false;
    barrelToTheRightTank2 = false;
	for (int i = 0; i < lifeChanges.size(); i++) {
		lifeChanges[i] = false;
	}
    for (int i = 0; i < lifeChangesTank2.size(); i++) {
        lifeChangesTank2[i] = false;
    }  
    tankDied = false;
    tankDiedTank2 = false;

    terrainPoints = GenerateTerrainPoints(1280, 720, scale);
  

    // Crearea mesh-ului de teren
    Mesh* terrain1 = object2D::CreateTerrain1("terrain1", terrainPoints, glm::vec3(1, 1, 0), groundHeight, heightOffset);
    AddMeshToList(terrain1);

  
    index = 5;
    index2 = 84;
    tankX = terrainPoints[index].x;
    tankY = terrainPoints[index].y;
	tankXTank2 = terrainPoints[index2].x;
	tankYTank2 = terrainPoints[index2].y;
	for (int i = 0; i < tankXOld.size(); i++) {
		tankXOld[i] = 0;
		tankYOld[i] = 0;
	}

	for (int i = 0; i < tankXOldTank2.size(); i++) {
		tankXOldTank2[i] = 0;
		tankYOldTank2[i] = 0;
	}
    lifeBarPercentage = 0;
    lifeBarPercentageTank2 = 0;
    projectileCounterMine = 0;
    projectileCounterMineTank2 = 0;
    for (int i = 0; i < counterDestroy.size(); i++) {
		counterDestroy[i] = 0;
    }
	for (int i = 0; i < counterDestroyTank2.size(); i++) {
		counterDestroyTank2[i] = 0;
	}
    for (int i = 0; i < startProjectile.size(); i++) {
		startProjectile[i] = false;
    }
	for (int i = 0; i < startProjectileTank2.size(); i++) {
		startProjectileTank2[i] = false;
	}
  
    tankAngle = CalculateTerrainSlopeAngle(terrainPoints, index ); // Get the orientation angle
	tankAngleTank2 = CalculateTerrainSlopeAngle(terrainPoints, index2); // Get the orientation angle
    glm::vec3 corner5 = glm::vec3(tankX-length/2, tankY-height/2, 0);
	glm::vec3 corner6 = glm::vec3(tankXTank2 - length / 2, tankYTank2 - height / 2, 0);
    // Create the tank mesh with brown color
    glm::vec3 brown = glm::vec3(0.4f, 0.2f, 0.1f);
	glm::vec3 purpleDark = glm::vec3(0.53f, 0.12f, 0.47f);
    glm::vec3 brownLight = glm::vec3(0.6f, 0.3f, 0.15f);
	glm::vec3 purpleLight = glm::vec3(0.87f, 0.58f, 0.98f);
    
	Mesh* tank1 = object2D::CreateTank1("tank1", corner5, 50, brown, brownLight,brownLight, true);
	Mesh* tank2 = object2D::CreateTank1("tank2", corner6, 50, purpleDark, purpleLight, purpleLight, true);
	AddMeshToList(tank1);
	AddMeshToList(tank2);

    // Create the turret
   
    glm::vec3 turretCenter = corner5 + offsetBarrel; // Center of the turret relative to the tank
	glm::vec3 turretCenterTank2 = corner6 + offsetBarrel; // Center of the turret relative to the tank
    


    Mesh* barrel1 = object2D::CreateTankBarrel("barrel1", turretCenter, turretRadius, barrelWidth, barrelLength, glm::vec3(1.0f, 0.5f, 0));
    AddMeshToList(barrel1);

	Mesh* barrel2 = object2D::CreateTankBarrel("barrel2", turretCenterTank2, turretRadius, barrelWidth, barrelLength, glm::vec3(0.85f, 0.43f, 0.85f));
	AddMeshToList(barrel2);

	// create the projectile
	//glm::vec3 projectileCenter = turretCenter + glm::vec3(0, 50, 0); // Center of the projectile relative to the barrel

	glm::vec3 projectileCenter =  glm::vec3(700, 700, 0);
	Mesh* projectile = object2D::CreateCircle("projectile", projectileCenter, projectileRadius, glm::vec3(0, 0, 1));
	AddMeshToList(projectile);

	// Create the projectile trajectory
    // clear prev projectil Points
   //projectilePoints.clear();
   projectilePoints = GenerateProjectilePoints(corner5.x + offsetTeajectory.x, corner5.y + offsetTeajectory.y, scale,30.0f);

    Mesh* projectileTrajectory = object2D::CreateProjectilTrajectory("projectileTrajectory", projectilePoints, glm::vec3(0, 0, 1), groundHeight, groundHeight);
    AddMeshToList(projectileTrajectory);

    // Create Lifebar

	lifeBarPosition = corner5 + glm::vec3(-10, 80, 0);
	lifeBarPositionTank2 = corner6 + glm::vec3(-10, 80, 0);

	glm::vec3 outlineColor = glm::vec3(1, 0, 0);
	glm::vec3 outlineColorTank2 = glm::vec3(0.85,0.85, 0.95);

	Mesh* lifeBarOutline = object2D::CreateLifeBarOutline("lifeBarOutline", lifeBarPosition, lifeBarWidth, lifeBarHeight, outlineColor);
	AddMeshToList(lifeBarOutline);

	Mesh* lifeBarOutlineTank2 = object2D::CreateLifeBarOutline("lifeBarOutlineTank2", lifeBarPositionTank2, lifeBarWidth, lifeBarHeight, outlineColorTank2);
	AddMeshToList(lifeBarOutlineTank2);
	

}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.0, 0.3, 0.86, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
  
	std::string terrainName = "terrain1_" + std::to_string(terraincounter);
	Mesh* terrain1 = object2D::CreateTerrain1(terrainName, terrainPoints, glm::vec3(1, 1, 0), groundHeight, heightOffset);
	AddMeshToList(terrain1);
	RenderMesh2D(meshes[terrainName], shaders["VertexColor"], glm::mat3(1));

    terraincounter++;

	modelMatrix = glm::mat3(1);

    modelMatrix *= transform2D::Translate(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y); // Translate tank to the current position
    modelMatrix *= transform2D::Rotate(tankAngle);
    modelMatrix *= transform2D::Translate(-CalculateTankCenter(tankX, tankY).x, -CalculateTankCenter(tankX, tankY).y); // Translate back


	modelMatrix *= transform2D::Translate(translateX1, translateY1); 

    if(!tankDied)
    	RenderMesh2D(meshes["tank1"], shaders["VertexColor"], modelMatrix);

    
	modelMatrixTank2 = glm::mat3(1);
	modelMatrixTank2 *= transform2D::Translate(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y); // Translate tank to the current position
	modelMatrixTank2 *= transform2D::Rotate(tankAngleTank2);
	modelMatrixTank2 *= transform2D::Translate(-CalculateTankCenter(tankXTank2, tankYTank2).x, -CalculateTankCenter(tankXTank2, tankYTank2).y); // Translate back
  
	modelMatrixTank2 *= transform2D::Translate(translateX2, translateY2);

	if (!tankDiedTank2)
		RenderMesh2D(meshes["tank2"], shaders["VertexColor"], modelMatrixTank2);
    
    // Life Bar
    glLineWidth(3.0f); // Set line width to 2 pixels (or any desired value)

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   
    modelMatrixLifeBar = glm::mat3(1);
	modelMatrixLifeBarTank2 = glm::mat3(1);

    modelMatrixLifeBar *= transform2D::Translate(translateX1, translateY1);
	modelMatrixLifeBarTank2 *= transform2D::Translate(translateX2, translateY2);
    if(!tankDied)
	RenderMesh2D(meshes["lifeBarOutline"], shaders["VertexColor"], modelMatrixLifeBar);
    if(!tankDiedTank2)
	RenderMesh2D(meshes["lifeBarOutlineTank2"], shaders["VertexColor"], modelMatrixLifeBarTank2);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // Check if any life change occurred and the collision was done to the tank
    if (std::any_of(lifeChanges.begin(), lifeChanges.end(), [](bool change) { return change; }) &&
        collisionWasDoneToTank == 1 &&
        std::any_of(counterDestroy.begin(), counterDestroy.end(), [](int count) { return count == 1; })) {

        int multiply = 1;

        // Check for consecutive `counterDestroy` values that are equal to 1
        for (int i = 0; i < counterDestroy.size() - 1; ++i) {
            if (counterDestroy[i] == counterDestroy[i + 1] && counterDestroy[i] == 1) {
                multiply++;
            }
        }

        // Update the life bar percentage and width based on the calculated multiplier
        lifeBarPercentage += 5 * multiply;
        lifeBarWidth -= (lifeBarPercentage / 100.0) * 80;

        // Check if the life bar has reached zero
        if (lifeBarWidth <= 0) {
            tankDied = true;
            lifeBarWidth = 0;
        }

        // Reset all lifeChanges values to false
        std::fill(lifeChanges.begin(), lifeChanges.end(), false);
    }

	if (std::any_of(lifeChangesTank2.begin(), lifeChangesTank2.end(), [](bool change) { return change; }) &&
		collisionWasDoneToTankTank2 == 1 &&
		std::any_of(counterDestroyTank2.begin(), counterDestroyTank2.end(), [](int count) { return count == 1; })) {

		int multiply = 1;

		// Check for consecutive `counterDestroy` values that are equal to 1
		for (int i = 0; i < counterDestroyTank2.size() - 1; ++i) {
			if (counterDestroyTank2[i] == counterDestroyTank2[i + 1] && counterDestroyTank2[i] == 1) {
				multiply++;
			}
		}

		// Update the life bar percentage and width based on the calculated multiplier
		lifeBarPercentageTank2 += 2 * multiply;
		lifeBarWidthTank2 -= (lifeBarPercentageTank2 / 100.0) * 80;

		// Check if the life bar has reached zero
		if (lifeBarWidthTank2 <= 0) {
			tankDiedTank2 = true;
			lifeBarWidthTank2 = 0;
		}

		// Reset all lifeChanges values to false
		std::fill(lifeChangesTank2.begin(), lifeChangesTank2.end(), false);
	}
    if (tankY < 5)
        tankDied = true;
    if (tankYTank2 < 5)
        tankDiedTank2 = true;

  

    // print the width
	std::cout << lifeBarWidth << std::endl;
    std::string lifeBarName = "LifeBarFill_" + std::to_string(projectileCounter);
    Mesh* lifeBarFill = object2D::CreateLifeBarFill(lifeBarName, lifeBarPosition, lifeBarWidth, lifeBarHeight, glm::vec3(0, 1, 0));
    AddMeshToList(lifeBarFill);
    if(!tankDied)
    RenderMesh2D(meshes[lifeBarName], shaders["VertexColor"], modelMatrixLifeBar);

	std::string lifeBarNameTank2 = "LifeBarFillTank2_" + std::to_string(projectileCounter);
	Mesh* lifeBarFillTank2 = object2D::CreateLifeBarFill(lifeBarNameTank2, lifeBarPositionTank2, lifeBarWidthTank2, lifeBarHeightTank2, glm::vec3(1, 0.43, 0.78));
	AddMeshToList(lifeBarFillTank2);
    if(!tankDiedTank2)
	RenderMesh2D(meshes[lifeBarNameTank2], shaders["VertexColor"], modelMatrixLifeBarTank2);
   

    barrelModelMatrix = glm::mat3(1);
    barrelModelMatrix *= transform2D::Translate(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y); // Translate tank to the current position
    barrelModelMatrix *= transform2D::Rotate(tankAngle);
    barrelModelMatrix *= transform2D::Translate(-CalculateTankCenter(tankX, tankY).x, -CalculateTankCenter(tankX, tankY).y); // Translate back
  
    barrelModelMatrix *= transform2D::Translate(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y + 32); // Translate tank to the current position
    barrelModelMatrix *= transform2D::Rotate(rotateBarrel);
    barrelModelMatrix *= transform2D::Translate(-CalculateTankCenter(tankX, tankY).x, -CalculateTankCenter(tankX, tankY).y - 32); // Translate tank to the current position

    barrelModelMatrix *= transform2D::Translate(translateX1, translateY1);
   
	if (!tankDied)
    RenderMesh2D(meshes["barrel1"], shaders["VertexColor"], barrelModelMatrix);

	barrelModelMatrixTank2 = glm::mat3(1);
	barrelModelMatrixTank2 *= transform2D::Translate(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y); // Translate tank to the current position
	barrelModelMatrixTank2 *= transform2D::Rotate(tankAngleTank2);
	barrelModelMatrixTank2 *= transform2D::Translate(-CalculateTankCenter(tankXTank2, tankYTank2).x, -CalculateTankCenter(tankXTank2, tankYTank2).y); // Translate back

	barrelModelMatrixTank2 *= transform2D::Translate(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y + 32); // Translate tank to the current position
	barrelModelMatrixTank2 *= transform2D::Rotate(rotateBarrelTank2);
	barrelModelMatrixTank2 *= transform2D::Translate(-CalculateTankCenter(tankXTank2, tankYTank2).x, -CalculateTankCenter(tankXTank2, tankYTank2).y - 32); // Translate tank to the current position

	barrelModelMatrixTank2 *= transform2D::Translate(translateX2, translateY2);

	if (!tankDiedTank2)
	RenderMesh2D(meshes["barrel2"], shaders["VertexColor"], barrelModelMatrixTank2);

    
    bAngle = +(tankAngle+rotateBarrel)*20 +90 +(tankAngle+rotateBarrel)*20 ;
	bAngleTank2 = +(tankAngleTank2 + rotateBarrelTank2) * 20 + 90 + (tankAngleTank2 + rotateBarrelTank2) * 20;
    
 
   projectilePoints = GenerateProjectilePoints(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y+60, scale, bAngle);
   projectilePointsTank2 = GenerateProjectilePoints(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y + 60, scale, bAngleTank2);

   std::string projectileName = "projectileTrajectory_" + std::to_string(projectileCounter);
   projectileCounter++;
   trajectoryModelMatrix = glm::mat3(1);
   trajectoryModelMatrix *= transform2D::Translate(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y);
   trajectoryModelMatrix *= transform2D::Rotate(tankAngle);
   trajectoryModelMatrix *= transform2D::Translate(-CalculateTankCenter(tankX, tankY).x, -CalculateTankCenter(tankX, tankY).y);

   trajectoryModelMatrix *= transform2D::Translate(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y + 32); // Translate tank to the current position
   trajectoryModelMatrix *= transform2D::Rotate(rotateBarrel);
   trajectoryModelMatrix *= transform2D::Translate(-CalculateTankCenter(tankX, tankY).x, -CalculateTankCenter(tankX, tankY).y - 32); // Translate tank to the current position

   std::string projectileNameTank2 = "projectileTrajectoryTank2_" + std::to_string(projectileCounterTank2);
   projectileCounterTank2++;
   trajectoryModelMatrixTank2 = glm::mat3(1);
   trajectoryModelMatrixTank2 *= transform2D::Translate(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y);
   trajectoryModelMatrixTank2 *= transform2D::Rotate(tankAngleTank2);
   trajectoryModelMatrixTank2 *= transform2D::Translate(-CalculateTankCenter(tankXTank2, tankYTank2).x, -CalculateTankCenter(tankXTank2, tankYTank2).y);

   trajectoryModelMatrixTank2 *= transform2D::Translate(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y + 32); // Translate tank to the current position
   trajectoryModelMatrixTank2 *= transform2D::Rotate(rotateBarrelTank2);
   trajectoryModelMatrixTank2 *= transform2D::Translate(-CalculateTankCenter(tankXTank2, tankYTank2).x, -CalculateTankCenter(tankXTank2, tankYTank2).y - 32); // Translate tank to the current position


   Mesh* projectileTrajectory = object2D::CreateProjectilTrajectory(projectileName, projectilePoints, glm::vec3(0.37, 0.62, 0.62), groundHeight, groundHeight);
   AddMeshToList(projectileTrajectory);

   Mesh* projectileTrajectoryTank2 = object2D::CreateProjectilTrajectory(projectileNameTank2, projectilePointsTank2, glm::vec3(0.37, 0.62, 0.62), groundHeight, groundHeight);
   AddMeshToList(projectileTrajectoryTank2);
	
   if (!tankDied)
   RenderMesh2D(meshes[projectileName], shaders["VertexColor"], trajectoryModelMatrix);
   if (!tankDiedTank2)
   RenderMesh2D(meshes[projectileNameTank2], shaders["VertexColor"], trajectoryModelMatrixTank2);

   for (int i = 0; i < startProjectile.size() && projectileCounterMine > 0; ++i) {
       // Activate the current projectile if the previous ones are all activated
       bool canActivate = true;
       for (int j = 0; j < i; ++j) {
           if (!startProjectile[j]) {
               canActivate = false;
               break;
           }
       }

       if (canActivate && !startProjectile[i]) {
           startProjectile[i] = true;
           projectileCounterMine--;
       }
   }

   
   for (int i = 0; i < startProjectileTank2.size() && projectileCounterMineTank2 > 0; ++i) {
	   // Activate the current projectile if the previous ones are all activated
	   bool canActivate = true;
	   for (int j = 0; j < i; ++j) {
		   if (!startProjectileTank2[j]) {
			   canActivate = false;
			   break;
		   }
	   }

	   if (canActivate && !startProjectileTank2[i]) {
		   startProjectileTank2[i] = true;
		   projectileCounterMineTank2--;
	   }
   }

   /////////////////////////////////1st 20 projectile////////////////////////////////////////
   for (int k = 0; k < 20; k++) {
       if (!oldWay1[k] && startProjectile[k]) {
           /*    projectilePointsOld.clear();*/
           projectilePointsOld[k] = GenerateProjectilePoints(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y + 60, scale, bAngle);
           tankXOld[k] = tankX;
           tankYOld[k] = tankY;
           tankAngleOld[k] = tankAngle + rotateBarrel;
           projectileInterpolationIndex[k] = 0;
           oldWay1[k] = true;
       }

       if (startProjectile[k] && oldWay1[k]) {
           // for each point in the projectil trajectory, create the obect and mesh make sure it has different names.
           pointName = "projectilePoint_" + std::to_string(projectileCounter);
           glm::vec3 projectilePoint1 = glm::vec3(projectilePointsOld[k][projectileInterpolationIndex[k]].x, projectilePointsOld[k][projectileInterpolationIndex[k]].y, 0);
           Mesh* projectilePoint = object2D::CreateCircle(pointName, projectilePoint1, projectileRadius, glm::vec3(0.37, 0.62, 0.6));
           AddMeshToList(projectilePoint);

           projectileModelMatrix = glm::mat3(1);
           projectileModelMatrix *= transform2D::Translate(CalculateTankCenter(tankXOld[k], tankYOld[k]).x, CalculateTankCenter(tankXOld[k], tankYOld[k]).y);
           projectileModelMatrix *= transform2D::Rotate(tankAngleOld[k]);
           projectileModelMatrix *= transform2D::Translate(-CalculateTankCenter(tankXOld[k], tankYOld[k]).x, -CalculateTankCenter(tankXOld[k], tankYOld[k]).y);

           RenderMesh2D(meshes[pointName], shaders["VertexColor"], projectileModelMatrix); // Assuming no additional transforms  

           if (CheckCollision(glm::vec3(projectilePointsOld[k][projectileInterpolationIndex[k]].x, projectilePointsOld[k][projectileInterpolationIndex[k]].y, 0), projectileRadius, glm::vec3(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y, 0), tankRadius)) {
               // Collision detected
               std::cout << "Collision detected p1-t2!" << std::endl;

               collisionWasDoneToTankTank2 = 1;
               lifeChangesTank2[k] = true;  // posibil GREASIT
               counterDestroyTank2[k]++;
               if (counterDestroyTank2[k] == 15) {
                   counterDestroyTank2[k] = 0;
               }
			   projectileCollidedWithTank2[k] = true;

           }

		/*  if (CheckCollision(glm::vec3(projectilePointsOld[k][projectileInterpolationIndex[k]].x, projectilePointsOld[k][projectileInterpolationIndex[k]].y, 0), projectileRadius, glm::vec3(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y, 0), tankRadius)) {
			  
               collisionDelayCounter[k]++;
               if (collisionDelayCounter[k] > delayFramesmin+5) {
                   projectileCollidedWithTank[k] = true;
                   collisionDelayCounter[k] = 0;

               }
			 
		   }*/

           if (CheckGroundCollision(terrainPoints, projectilePointsOld[k][projectileInterpolationIndex[k]].x, projectilePointsOld[k][projectileInterpolationIndex[k]].y, projectileRadius, 10) && ((projectileCollidedWithTank[k] && !tankDied)|| (projectileCollidedWithTank2[k] && !tankDiedTank2))) {
             
               projectileCollisionDelayCounter[k]++;

			   if (projectileCollisionDelayCounter[k] > delayFrames) {
                   explosionCounter++;
                   terrainExploded = false;
                   if (explosionCounter > 3) {
                       std::cout << "Coolision ground" << std::endl;
                       explosionCounter = 0;
                       terrainExploded = true;
                       CreateExplosionInTerrain(terrainPoints, projectilePointsOld[k][projectileInterpolationIndex[k]].x, projectilePointsOld[k][projectileInterpolationIndex[k]].y, explosionRadius, explaosionDepth);
                   }
				   projectileCollisionDelayCounter[k] = 0;
				   projectileCollidedWithTank[k] = false;
				   projectileCollidedWithTank2[k] = false;
			   }
             
		   }
		   else if (CheckGroundCollision(terrainPoints, projectilePointsOld[k][projectileInterpolationIndex[k]].x, projectilePointsOld[k][projectileInterpolationIndex[k]].y, projectileRadius, 10)) {
			   explosionCounter++;
			   terrainExploded = false;
			   std::cout << "Coolision ground" << std::endl;
			   if (explosionCounter > 3) {
				   explosionCounter = 0;
				   terrainExploded = true;
				   CreateExplosionInTerrain(terrainPoints, projectilePointsOld[k][projectileInterpolationIndex[k]].x, projectilePointsOld[k][projectileInterpolationIndex[k]].y, explosionRadius, explaosionDepth);
			   }
		   }

       }
   
   }

   for (int k = 0; k < 20; k++) {
       if (!oldWay1Tank2[k] && startProjectileTank2[k]) {
           projectilePointsOldTank2[k] = GenerateProjectilePoints(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y + 60, scale, bAngleTank2);
           tankXOldTank2[k] = tankXTank2;
           tankYOldTank2[k] = tankYTank2;
           tankAngleOldTank2[k] = tankAngleTank2 + rotateBarrelTank2;
           projectileInterpolationIndexTank2[k] = 0;
           oldWay1Tank2[k] = true;
       }

       if (startProjectileTank2[k] && oldWay1Tank2[k]) {
           // for each point in the projectil trajectory, create the obect and mesh make sure it has different names
           pointNameT2 = "projectilePointTank2_" + std::to_string(projectileCounterTank2);
           glm::vec3 projectilePoint2 = glm::vec3(projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].x, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].y, 0);
           Mesh* projectilePointT2 = object2D::CreateCircle(pointNameT2, projectilePoint2, projectileRadius, glm::vec3(0.37, 0.62, 0.6));
           AddMeshToList(projectilePointT2);

           projectileModelMatrixTank2 = glm::mat3(1);
           projectileModelMatrixTank2 *= transform2D::Translate(CalculateTankCenter(tankXOldTank2[k], tankYOldTank2[k]).x, CalculateTankCenter(tankXOldTank2[k], tankYOldTank2[k]).y);
           projectileModelMatrixTank2 *= transform2D::Rotate(tankAngleOldTank2[k]);
           projectileModelMatrixTank2 *= transform2D::Translate(-CalculateTankCenter(tankXOldTank2[k], tankYOldTank2[k]).x, -CalculateTankCenter(tankXOldTank2[k], tankYOldTank2[k]).y);

           RenderMesh2D(meshes[pointNameT2], shaders["VertexColor"], projectileModelMatrixTank2); // Assuming no additional transforms  

		   if (CheckCollision(glm::vec3(projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].x, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].y, 0), projectileRadius, glm::vec3(CalculateTankCenter(tankX, tankY).x, CalculateTankCenter(tankX, tankY).y, 0), tankRadius+5)) {
			   // Collision detected
			   std::cout << "Collision detected p2-t1!" << std::endl;

			   collisionWasDoneToTank = 1;
			   lifeChanges[k] = true;  // posibil GREASIT
			   counterDestroy[k]++;
			   if (counterDestroy[k] == 15) {
				   counterDestroy[k] = 0;
			   }
			   projectileCollidedWithTank[k] = true;
		   }

          /* if (CheckCollision(glm::vec3(projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].x, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].y, 0), projectileRadius, glm::vec3(CalculateTankCenter(tankXTank2, tankYTank2).x, CalculateTankCenter(tankXTank2, tankYTank2).y, 0), tankRadius + 5)) {
               collisionDelayCounterTank2[k]++;
               if (collisionDelayCounterTank2[k] > delayFramesmin+5) {
                   projectileCollidedWithTank2[k] = true;
                   collisionDelayCounterTank2[k] = 0;
               }
             

           }*/
           
           if (CheckGroundCollision(terrainPoints, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].x, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].y, projectileRadius, 10) &&(( projectileCollidedWithTank[k] && !tankDied) || (projectileCollidedWithTank2[k] && !tankDiedTank2))) {
               projectileCollisionDelayCounterTank2[k]++; 
               if (projectileCollisionDelayCounterTank2[k] > delayFrames) {
                   explosionCounterTank2++;
                   terrainExploded = false;
                   std::cout << "Coolision ground tank2" << std::endl;
                   if (explosionCounterTank2 > 3) {
					   projectileCollisionDelayCounterTank2[k] = 0;
                       explosionCounterTank2 = 0;
                       terrainExploded = true;
                       CreateExplosionInTerrain(terrainPoints, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].x, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].y, explosionRadius, explaosionDepth);
                   }
                   projectileCollidedWithTank2[k] = false;
                   projectileCollisionDelayCounterTank2[k] = 0;
                   projectileCollidedWithTank[k] = false;
               }
		   }
		   else if (CheckGroundCollision(terrainPoints, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].x, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].y, projectileRadius, 10)) {
			   explosionCounterTank2++;
			   terrainExploded = false;
			   std::cout << "Coolision ground tank2" << std::endl;
			   if (explosionCounterTank2 > 3) {
				   explosionCounterTank2 = 0;
				   terrainExploded = true;
				   CreateExplosionInTerrain(terrainPoints, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].x, projectilePointsOldTank2[k][projectileInterpolationIndexTank2[k]].y, explosionRadius, explaosionDepth);
			   }
		   }
       }

   } 
   

}

void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{


    // Only move the tank if the D key is held down
    if (isMovingRight) {
     
        if (index < 160) {
            translateX1 += terrainPoints[index + 1].x - terrainPoints[index].x;
            translateY1 += terrainPoints[index + 1].y - terrainPoints[index].y;

            tankX = terrainPoints[index + 1].x;
            tankY = terrainPoints[index + 1].y;


            // Update the tank's angle by adding the angle change
            tankAngle = CalculateTerrainSlopeAngle(terrainPoints, index + 1);

            // Normalize the angle if necessary (keep it within 0 to 2*PI or -PI to PI)
            if (tankAngle > M_PI) {
                tankAngle -= 2 * M_PI; // Normalize to [-PI, PI]
                std::cout << "tankAngle > M_PI" << "\n";
            }
            else if (tankAngle < -M_PI) {
                tankAngle += 2 * M_PI; // Normalize to [-PI, PI]
            }

            index++;
        }

    }

    if (isMovingRightTank2)
    {
        if (index2 < 160) {
            translateX2 += terrainPoints[index2 + 1].x - terrainPoints[index2].x;
            translateY2 += terrainPoints[index2 + 1].y - terrainPoints[index2].y;

            tankXTank2 = terrainPoints[index2 + 1].x;
            tankYTank2 = terrainPoints[index2 + 1].y;

            tankAngleTank2 = CalculateTerrainSlopeAngle(terrainPoints, index2 + 1);

            // Normalize the angle if necessary (keep it within 0 to 2*PI or -PI to PI)
            if (tankAngleTank2 > M_PI) {
                tankAngleTank2 -= 2 * M_PI; // Normalize to [-PI, PI]
                std::cout << "tankAngle > M_PI" << "\n";
            }
            else if (tankAngleTank2 < -M_PI) {
                tankAngleTank2 += 2 * M_PI; // Normalize to [-PI, PI]
            }

            index2++;
        }
	
    }

    if (isMovingLeft) {
   
        if (index > 1) {
            translateX1 += terrainPoints[index - 1].x - terrainPoints[index].x;
            translateY1 += terrainPoints[index - 1].y - terrainPoints[index].y;

            tankX = terrainPoints[index - 1].x;
            tankY = terrainPoints[index - 1].y;

            // Update the tank's angle by adding the angle change
            tankAngle = CalculateTerrainSlopeAngle(terrainPoints, index - 1);

            // Normalize the angle if necessary (keep it within 0 to 2*PI or -PI to PI)
            if (tankAngle > M_PI) {
                tankAngle -= 2 * M_PI; // Normalize to [-PI, PI]
                std::cout << "tankAngle > M_PI" << "\n";
            }
            else if (tankAngle < -M_PI) {
                tankAngle += 2 * M_PI; // Normalize to [-PI, PI]
            }

            index--;
        }
    }

    if (isMovingLeftTank2) {

        if (index2  >1) {
            translateX2 += terrainPoints[index2 - 1].x - terrainPoints[index2].x;
            translateY2 += terrainPoints[index2 - 1].y - terrainPoints[index2].y;

            tankXTank2 = terrainPoints[index2 - 1].x;
            tankYTank2 = terrainPoints[index2 - 1].y;

            // Update the tank's angle by adding the angle change
            tankAngleTank2 = CalculateTerrainSlopeAngle(terrainPoints, index2 - 1);

            // Normalize the angle if necessary (keep it within 0 to 2*PI or -PI to PI)
            if (tankAngleTank2 > M_PI) {
                tankAngleTank2 -= 2 * M_PI; // Normalize to [-PI, PI]
                std::cout << "tankAngle > M_PI" << "\n";
            }
            else if (tankAngleTank2 < -M_PI) {
                tankAngleTank2 += 2 * M_PI; // Normalize to [-PI, PI]
            }

            index2--;
        }
    }

    for (int k = 0; k < 20; k++) 
    {
        if (startProjectile[k]) {

            if (projectileInterpolationIndex[k] < projectilePointsOld[k].size() - 2) {
                projectileInterpolationIndex[k]++;
            }

            if (projectileInterpolationIndex[k] == projectilePointsOld[k].size() - 2) {
                startProjectile[k] = false;
                projectileInterpolationIndex[k] = 0;
                tankXOld[k] = 0;
                tankYOld[k] = 0;
                tankAngleOld[k] = 0;
                oldWay1[k] = false;

            }

        }
    }

    for (int k = 0; k < 20; k++) {
        if (startProjectileTank2[k]) {

            if (projectileInterpolationIndexTank2[k] < projectilePointsOldTank2[k].size() - 2) {
                projectileInterpolationIndexTank2[k]++;
            }

            if (projectileInterpolationIndexTank2[k] == projectilePointsOldTank2[k].size() - 2) {
                startProjectileTank2[k] = false;
                projectileInterpolationIndexTank2[k] = 0;
                tankXOldTank2[k] = 0;
                tankYOldTank2[k] = 0;
                tankAngleOldTank2[k] = 0;
                oldWay1Tank2[k] = false;

            }

        }
    }
    

	if (barrelToTheRight) {
		rotateBarrel += 0.01f;
		if (rotateBarrel > 0.8f) {
			rotateBarrel = 0.8f;
		}
	}

    if (barrelToTheRightTank2)
    {
        rotateBarrelTank2 += 0.01f;
        if (rotateBarrelTank2 > 0.8f) {
            rotateBarrelTank2 = 0.8f;
        }
    }

    if (barrelToTheLeft) {
        rotateBarrel -= 0.01f;
        if (rotateBarrel < -0.8f) {
            rotateBarrel = -0.8f;
        }
    }

        if (barrelToTheLeftTank2)
        {
            rotateBarrelTank2 -= 0.01f;
            if (rotateBarrelTank2 < -0.8f) {
                rotateBarrelTank2 = -0.8f;
            }
        }


}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
    // Check if the 'D' key is pressed
    if (key == GLFW_KEY_D) {
        isMovingRight = true; // Start moving right
    }
    if (key == GLFW_KEY_RIGHT)
    {
        isMovingRightTank2 = true;
    }
    if (key == GLFW_KEY_A) {
        isMovingLeft = true; // Stop moving when key is released
    }
    if (key == GLFW_KEY_LEFT)
    {
        isMovingLeftTank2 = true;
    }
    if (key == GLFW_KEY_SPACE) {
        if(projectileCounterMine < 4 && !tankDied)
            projectileCounterMine++;

    }
	if (key == GLFW_KEY_ENTER)
	{
		if (projectileCounterMineTank2 < 4 && !tankDiedTank2)
			projectileCounterMineTank2++;
	}
    if (key == GLFW_KEY_S) {
        barrelToTheRight = true;

    }
	if (key == GLFW_KEY_UP)
	{
		barrelToTheRightTank2 = true;
	}
    if (key == GLFW_KEY_W) {
        barrelToTheLeft = true;
    }
	if (key == GLFW_KEY_DOWN)
	{
		barrelToTheLeftTank2 = true;
	}

}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
    if (key == GLFW_KEY_D) {
        isMovingRight = false; // Stop moving when key is released
    }

	if (key == GLFW_KEY_RIGHT)
	{
        isMovingRightTank2 = false;
	}

    if (key == GLFW_KEY_A) {
        isMovingLeft = false; // Stop moving when key is released
    }

    if (key == GLFW_KEY_LEFT)
    {
        isMovingLeftTank2 = false;
    }

    if (key == GLFW_KEY_W) {
        barrelToTheLeft = false;
    }
	if (key == GLFW_KEY_UP)
	{
		barrelToTheRightTank2 = false;
	}
    if (key == GLFW_KEY_S) {
        barrelToTheRight = false;

    }
	if (key == GLFW_KEY_DOWN)
	{
		barrelToTheLeftTank2 = false;
	}
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
