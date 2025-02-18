#include "lab_m1/Tema2/Tema2.h"
#include "ParticleSystem.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;
using namespace m1;

#include "lab_m1/lab4/transform3D.h"

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


 /////////////////////////////////////////////////////////////////VARIABLES///////////////////////////////////////////////
float rotationAngleElice = 0.0f; // Angle of rotation, initially 0
float rotationAngleDrona = 0.0f;
float translateX = 0.0f;
float translateY = 0.0f;
float translateZ = 0.0f;
int nrHouses = 20;
int nrTrees = 30;
int nrCheckpoints = 10;
float oldCameraX = 0.0f;
float oldCameraY = 0.0f;
float oldCameraZ = 0.0f;
float  foliageBaseRadius = 1.5f;
float foliageHeight = 2.0f;
float trunkRadius = 0.2f;
float trunkHeight = 3.0f;
// Dimensions for the drone
float width1 = 0.6f;   // Width of the first part of the drone
float height = 0.04f;  // Height of the drone (same for both parts)
float depth1 = 0.04f;  // Depth of the first part of the drone
float width2 = 0.04f;  // Width of the second part of the drone
float depth2 = 0.6f;   // Depth of the second part of the drone
int currentCheckpoint = 0;
glm::mat4 oldModelMatrix;
glm::mat4 droneModelMatrix;
glm::mat4 arrowMatrix = glm::mat4(1.0f);
glm::mat4 oldArrowMatrix = glm::mat4(1.0f);
std::vector<glm::mat4> propellerModelMatrices(4, glm::mat4(1.0f));
std::vector<glm::mat4> oldPropellerModelMatrices(4, glm::mat4(1.0f));
std::vector<glm::vec3> housePositions;
std::vector<glm::vec3> treePositions;
std::vector<glm::vec3> nextCheckpoints;
std::vector<glm::vec3> checkpointsYellowGreen;
//checkpoint stuff
/* Initial toate checkPointurile sunt DEACTIVTED
* Un checkpoint care este DEACTIVATED nu poate fi trecut
* El va fi in nuante de gri si nu va avea efect asupra jocului
* Un checkpoint care este NOT_ENTERED poate fi trecut dar juca-
* torul nu are ca scop sa treaca prin el daca nu este un
* checkpoint urmator. Un checkpoint urmator este colorat
* cu galben si verde si este scopul jucatorului sa treaca prin el
* de la culoarea verde spre culoarea galben. NU ESTE POSIBILA trecerea
* de la galben spre verde. Pe langa next checkpoint, va mai fi un checkpoint
* in culoare albastru si roz care este urmatorul checkpoint dupanext-checkpoint
* Un checkpoint care este in stadiul COMPLETED va fi in nuante de gri si
* nu va mai putea fi trecut. Un checkpoint care este in stadiul ENTERED_GREEN
* inseamna ca drona a trecut prin collision-box-ul verde al unui checkpoint.
*/
enum CheckpointState {
    NOT_ENTERED,
    ENTERED_GREEN,
    COMPLETED,
    DEACTIVATED
};
std::vector<CheckpointState> checkpointStates(nrCheckpoints, DEACTIVATED);

struct AABB {
    glm::vec3 min;  // Min corner (x, y, z)
    glm::vec3 max;  // Max corner (x, y, z)
};
std::vector<AABB> houseAABBs;
std::vector<std::pair<AABB, AABB>> treeAABBs;
AABB droneAABB;
std::vector<std::tuple<AABB, AABB, AABB>> checkpointAABB;
std::vector<std::tuple<AABB, AABB, AABB>> checkpointNextAABB;


// Propellers
glm::vec3 propellerOffsets[] = {
    glm::vec3(0.4f, 0.05f, 0.4f),   // Front-right
    glm::vec3(-0.4f, 0.05f, 0.4f),  // Front-left
    glm::vec3(-0.4f, 0.05f, -0.4f), // Rear-left
    glm::vec3(0.4f, 0.05f, -0.4f)   // Rear-right
};





/////////////////////////////////////////////////////////FUNCTIONS///////////////////////////////////////////////
// Utility function to rotate a vector around the Y-axis
glm::vec3 RotateAroundYAxis(const glm::vec3& position, float angleRadians) {
    float cosTheta = cos(angleRadians);
    float sinTheta = sin(angleRadians);
    return glm::vec3(
        position.x * cosTheta - position.z * sinTheta,
        position.y,
        position.x * sinTheta + position.z * cosTheta
    );
}

//-------------Functii pentru zgomot Pelin----------------

/* Zgomotul Perlin este un generator de secven?e aleatorii care produce o 
* succesiune de numere mai natural? ?i armonic? decât func?ia standard random().
*/

/* Functie care face o "curbura" a valorii de intrare t pentru a face tranzi?iile
* dintre valori mai line. Aceasta functie este adesea folosita in generarea zgomo-
* tului Perlin pentru a face trecerile mai naturale.
*/
float fade(float t) {
    /* creeaza o curba de tip "smoothstep" care face ca valorile de la 0 la 1 sa aiba o crestere treptata.
    * t = un coeficient care reprezinta pozitia relativa a unui punct in raport cu vecinii sai.
    * la o interpolare, procesul de tranzitie este facut mai armonios*/
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

/*  Aceasta functie calculeaza un gradient pe baza unei valori de "hash" si a unui punct p.
* Gradientul este folosit in generarea zgomotului Perlin pentru a determina directia variatiei.
* Se face o operatie & (AND bit cu bit) cu valoarea de hash pentru a selecta una dintre cele patru directii.
* u si v sunt valorile coordonatelor din p, iar semnul acestora depinde de hash.
* aceasta functie calculeaza un "dot product" intre vectorul de gradient si vectorul pozitional,
* obtinand valoarea finala a gradientului
* Formula dot product pentru doi vectori in 2D:
* A = (a?, a?)
* B = (b?, b?)
* Dot product-ul se calculeaza astfel:
* A · B = a? * b? + a? * b?
* aka functia imita un comportament de dot product
*/
float grad(int hash, glm::vec2 p) {
    int h = hash & 3;
    float u = (h < 2) ? p.x : p.y;
    float v = (h < 2) ? p.y : p.x;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

/* Algoritmul Perlin Noise 2D este folosit pentru a genera zgomot continuu si neted.
* Vectorul st reprezinta coordonatele unui punct in spatiul 2D pentru care se doreste 
* calcularea valorii de zgomot Perlin.
* glm::dot(i, glm::vec2(127.1f, 311.7f)) - calculeaza un produs scalar (dot product) 
* intre vectorul i si un vector constant. Acesta este folosit pentru a crea un numar 
* pseudo-aleator pe care se va aplica un modul (operatia std::fmod) pentru a-l limita
* la o valoare in intervalul [0, 255].
* glm::mix este o functie care interpoleaza liniar doua valori in functie de un factor 
* de interpolare.
*/
float perlinNoise(glm::vec2 st) {
    /*i: Reprezinta partea intreaga a coordonatelor vectorului st.*/
    glm::vec2 i = glm::floor(st);
	/*f: Reprezinta partea fractionara a coordonatelor vectorului st.*/
    glm::vec2 f = glm::fract(st);

    /*Aici se calculeaza patru valori hash pentru cele patru colturi ale unui patrat definit de pozitia st.*/
    int h00 = static_cast<int>(std::fmod(glm::dot(i, glm::vec2(127.1f, 311.7f)), 256.0f));
    int h10 = static_cast<int>(std::fmod(glm::dot(i + glm::vec2(1.0f, 0.0f), glm::vec2(127.1f, 311.7f)), 256.0f));
    int h01 = static_cast<int>(std::fmod(glm::dot(i + glm::vec2(0.0f, 1.0f), glm::vec2(127.1f, 311.7f)), 256.0f));
    int h11 = static_cast<int>(std::fmod(glm::dot(i + glm::vec2(1.0f, 1.0f), glm::vec2(127.1f, 311.7f)), 256.0f));

    /*f - glm::vec2(1.0f, 0.0f) si f - glm::vec2(0.0f, 1.0f) ajusteaza 
    * vectorul f pentru a obtine vectorii de pozitie ai celorlalte colturi ale patratului.*/
    float g00 = grad(h00, f);
    float g10 = grad(h10, f - glm::vec2(1.0f, 0.0f));
    float g01 = grad(h01, f - glm::vec2(0.0f, 1.0f));
    float g11 = grad(h11, f - glm::vec2(1.0f, 1.0f));

    glm::vec2 fadeXY = glm::vec2(fade(f.x), fade(f.y));

    // interpolarea valorilor de gradient
    float n0 = glm::mix(g00, g10, fadeXY.x); //  interpolarea pe OX
    float n1 = glm::mix(g01, g11, fadeXY.x); // interpolatea pe OY
    return glm::mix(n0, n1, fadeXY.y);  // interpolatea celor 2 rezultate
}

/* Adauga detalii suplimentare la un zgomot de baza  prin combinarea mai
* multor "niveluri" (sau octaves) de zgomot cu diferente de frecventa si 
* amplitudine.
*/
float fractalNoise(glm::vec2 st, int octaves, float persistence) {
    float total = 0.0f; //valoarea acumulata a zgomotului fractal pentru toate octaves. 
    //In fiecare pas, vom adauga zgomotul generat de un anumit octave la acest total.
    float amplitude = 1.0f; // intensitatea zgomotului la fiecare octave.
    float frequency = 1.0f;
    float maxValue = 0.0f; // folosit pentru normalizare
    // octaves = numarul de niveluri de zgomot pe care dorim sa le combinam. 
    // Fiecare octave adauga detalii suplimentare la zgomotul fractal.
    for (int i = 0; i < octaves; i++) {
        //La fiecare octave, coordonatele st sunt scalate cu un factor de frecventa, 
        // ceea ce inseamna ca frecventa detaliilor devine din ce in ce mai mare pe 
        // masura ce urcam prin octaves.
        // persistence = determin? cât de mult va sc?dea amplitudinea fiec?rui nivel de zgomot 
        // Dac? persistence este aproape de 1: Zgomotul de la octavele superioare va avea o amplitudine 
        // aproape la fel de mare ca la octavele inferioare.Acest lucru va face ca detaliile fine
        // (frecven?ele mari) s? aib? o influen?? semnificativ? asupra rezultatului final.
        // Dac? persistence este mic(aproape de 0) : Zgomotul la octavele superioare va avea o amplitudine
        // mult mai mic? decât zgomotul de la octavele inferioare, astfel încât detaliile fine vor avea o 
        // influen?? mult mai redus? asupra rezultatului final.
        total += perlinNoise(st * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

/*Functia enhanceWithLakes este o functie care modifica valoarea unei inaltimi
* pentru a introduce elemente de lacuri in teren. Aceasta este folosita pentru a
* adauga o adancire a terenului (lac) intr-o anumita zona, in functie de inaltimea 
* specificata, si include o tranzi?ie lina intre zonele de teren si zonele de lac.*/
float enhanceWithLakes(float height) {
    float lakeThreshold = -0.15f; // reprezinta pragul de inaltime de la care incepem sa consideram ca terenul se transforma intr-un lac.
    float lakeDepth = -0.3f;
    float smoothness = 0.05f; // reprezinta cat de lina va fi tranzitia dintre zona de teren si zona de lac. 
    // Cu cat acest parametru este mai mare, cu atat tranzitia va fi mai brusca (schimbarea intre teren si
    // lac va fi mai abrupta). Cu cat este mai mic, tranzitia va fi mai lina si mai graduala.

    float transition = glm::smoothstep(lakeThreshold - smoothness, lakeThreshold + smoothness, height);

    if (height < lakeThreshold) {
        return glm::mix(lakeDepth, height, transition);
    }

    // Daca height este mai mic decat lakeThreshold - smoothness, transition va fi 0
    // (adic? terenul va fi considerat complet parte din lac).
    // Daca height este mai mare decat lakeThreshold + smoothness, transition va fi 1
    // (adic? terenul nu va fi modificat, ramanand la valoarea sa initiala).
    // Daca height se afla intre lakeThreshold - smoothness si lakeThreshold + smoothness, 
    // transition va avea o valoare intermediar?, asigurând o tranzitie lina intre terenul 
    // normal si zona de lac.

    return height;
}

/* Functie care returneaza inaltimea terenului la o anumita pozitie (x, z).
*/
float GetTerrainHeight(float x, float z) {
    float frequency = 0.26f;
    int octaves = 7;
    float persistence = 0.5f;

	// Inaltimea terenului depinde de zgomotul fractal generat de functia fractalNoise.
    float height = fractalNoise(glm::vec2(x, z) * frequency, octaves, persistence);
    // Luam in considerare si lacurile
    height = enhanceWithLakes(height);

	// Adaugam un offset pentru a ridica terenul la o inaltime mai mare
    float terrainOffset = 0.2f;

    return height + terrainOffset; // z of terrain max
}

/* Functia are greija ca atunci cand spawnam un obiect intr-un loc random,
* acesta sa nu se spauneze peste alt obiect
*/
bool IsPositionTooClose(glm::vec3 newPosition, float minDistance) {
	// new position is too close to any existing object ?
	// comparam cu toate pozitiile existente ale turnurilor
    for (const glm::vec3& pos : housePositions) {
        // Calculate distance between new position and existing position
        float distance = glm::length(newPosition - pos);
        if (distance < minDistance) {
            return true;  // Too close to another object
        }
    }
	// comparam cu toate pozitiile existente ale copacilor
	for (const glm::vec3& pos : treePositions) {
		// Calculate distance between new position and existing position
		float distance = glm::length(newPosition - pos);
		if (distance < minDistance) {
			return true;  // Too close to another object
		}
	}
    return false;  // No objects within the radius
}

/* Function to generate a random float in a range */
glm::vec3 RandomObjectPosition(float minX, float maxX, float minY, float maxY) {
    // generate random x and random y within the specified range
    float randomX = (rand() / (float)RAND_MAX) * (maxX - minX) + minX;
    float randomY = (rand() / (float)RAND_MAX) * (maxY - minY) + minY;

    // Get terrain height at the random position
    float terrainHeight = GetTerrainHeight(randomX, randomY);

    // print position vector
    cout << "Random position: " << randomX << " " << terrainHeight << " " << randomY << endl;

    glm::vec3 newPos (randomX, terrainHeight, randomY);

    // Check if the terrain height is valid, if not, generate new position
    if (terrainHeight > 0.0f && !IsPositionTooClose(newPos,3))
        return glm::vec3(randomX, terrainHeight, randomY);
    else
        return RandomObjectPosition(minX, maxX, minY, maxY);  // Recurse if terrain height is too low
}

/* Functie care verifica daca drona se afla in coliziune cu solul.
* Primeste ca parametru pozitia dronei si verifica daca aceasta 
* se afla sub nivelul solului. Daca este aproape de punctul de sub sau egal cu solul,
* atunci se considera ca drona a intrat in coliziune cu solul.
*/
bool CheckGroundCollision(glm::vec3 dronePosition) {

        // Check multiple points along the perimeter of the circle with radius
        int numPoints = 100; // Number of points to check - daca crestem avem precizie mai mare
        float radius = 0.5;

        // folosesc un perimetru de coliziune pe o raza radius in jurul dronei
        // folosesc un numar limitat de puncte din acest perimetru pentru a verifica coliziunea
        // Loop through each point on the perimeter
        for (int i = 0; i < numPoints; ++i) {
            // Calculate the angle for each point along the circle's perimeter
            float angle = i * 2.0f * glm::pi<float>() / numPoints;

            // Calculate the position of each point on the perimeter
            glm::vec3 checkPosition = glm::vec3(
                dronePosition.x + radius * cos(angle),
                dronePosition.y ,  // We keep the drone's y position as is for height checking
                dronePosition.z + radius * sin(angle)
            );

            // Get the terrain height at this position
            float terrainHeight = GetTerrainHeight(checkPosition.x, checkPosition.z);

            // If the drone is below the terrain (considering tolerance), there is a collision
            if (checkPosition.y  <= terrainHeight) {
                return true;  // Collision with ground
            }
        }

        // No collision detected with the ground for any of the points
        return false;
}

/* Aceasta functie verifica daca doua AABB-uri (Axis-Aligned Bounding Boxes) se ciocnesc.
* AABB-urile sunt definite de coordonatele minime si maxime ale unui dreptunghi 3D.
* aka niste collision box-uri
*/
bool CheckAABBCollision(const AABB& a, const AABB& b) {
    // Check if there is no overlap along any axis
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false; // X-axis overlap
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false; // Y-axis overlap
    if (a.max.z < b.min.z || a.min.z > b.max.z) return false; // Z-axis overlap

    // If no condition above is true, the AABBs are colliding
    return true;
}

/* Functie care verifica daca drona se afla in coliziune cu obstacolele din scena.*/
bool CheckCollisionWithObstacles(const glm::vec3& playerPosition) {
	// playerPosition = pozitia dronei
    // Create an AABB for the player's bounding box (based on the player's size and position)
    glm::vec3 playerMin = playerPosition - glm::vec3(0.5f, 0.0f, 0.5f); // player AABB size
    glm::vec3 playerMax = playerPosition + glm::vec3(0.5f, 1.8f, 0.5f); // player AABB size
    // Cream collision box-ul pentru drona
    AABB playerAABB{ playerMin, playerMax };

    // Iterate over the treeAABBs vector (which contains pairs of AABBs for trunk and cone)
    // coliziunea cu copacul
    for (const auto& treePair : treeAABBs) {
        // Check for collision between player and both the trunk and cone AABBs
        if (CheckAABBCollision(playerAABB, treePair.first) || CheckAABBCollision(playerAABB, treePair.second)) {
            return true; // Collision detected
        }
    }

    // coliziunea cu turnurile
	// Iterate over the houseAABBs vector
	for (const auto& houseAABB : houseAABBs) {
		// Check for collision between player and house AABB
		if (CheckAABBCollision(playerAABB, houseAABB)) {
			return true; // Collision detected
		}
	}

    // coliziunea cu corpul checkpoint-ului
    for (int i = 0; i < checkpointAABB.size(); i++) {
        // coliziunea cu suportul checkpoint-ului
        // Check for collision between player and both the stick and circle AABBs
        if (CheckAABBCollision(playerAABB, std::get<0>(checkpointAABB[i]))) {
            return true; // Collision detected
        }

        if (checkpointStates[i] == DEACTIVATED) {
			// daca checkpoint-ul este DEACTIVATED, nu poate fi trecut
            if (CheckAABBCollision(playerAABB, std::get<2>((checkpointAABB[i])))) {
                return true;
            }
        }

        if ((checkpointStates[i]) == NOT_ENTERED) {
			// Daca checkpoint-ul este NOT_ENTERED, atunci drona poate trece prin el
			// dar doar daca trece prin zona verde a checkpoint-ului
			// altfel se detecteaza coliziunea cu collision-box-ul galben
            if (CheckAABBCollision(playerAABB, std::get<2>(checkpointAABB[i]))) {
                printf("intra galben not_entered");
                return true;
            }
        }
        if (checkpointStates[i] == NOT_ENTERED) {
			// Daca chckpoint-ul este NOT_ENTERED, atunci drona poate trece prin el
			// dar doar daca trece prin zona verde a checkpoint-ului
			// Checkpoint-ul devine ENTERED_GREEN
			// Checkpoint-ul devine COMPLETED
			// Urmatorul checkpoint devine tinta dronei
            // Check if the drone collides with the green circle
            if (CheckAABBCollision(playerAABB, std::get<1>((checkpointAABB[i])))) {
                checkpointStates[i] = ENTERED_GREEN;
				checkpointStates[i] = COMPLETED;
				currentCheckpoint++;
            }
        }
      
    }

	for (const auto& checkpointPair : checkpointNextAABB) {
		// coliziunea cu un checkpoint de tipul next va fi detectata
		if (CheckAABBCollision(playerAABB, std::get<0>(checkpointPair))) {
			return true; // Collision detected
		}
	}
    // No collision detected with any obstacles
    return false;
}


////////////////////////////////////////////////Main_Functions///////////////////////////////////////////////



Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = false;
    orthoProjection = false;
    projectionFov = 60.0f;
    projectionWidth = 16.0f;
    projectionHeight = 9.0f;

    // AABB drona
    glm::vec3 minPoint = glm::vec3(-std::max(width1, width2), -height, -std::max(depth1, depth2));  // The minimum point of the AABB
    glm::vec3 maxPoint = glm::vec3(std::max(width1, width2), height, std::max(depth1, depth2));     // The maximum point of the AABB

    // Create the AABB for the drone
	droneAABB = AABB{ minPoint, maxPoint };

	// Generate random positions for the houses and trees
	// Si calculeaza collision box-urile pentru acestea
	for (int i = 0; i < nrHouses; i++)
	{
        glm::vec3 position = RandomObjectPosition(-50, 130, -50, 130);
        housePositions.push_back(position);

        // Define AABB for the house (adjusted size based on suggested dimensions)
        glm::vec3 minPoint = position - glm::vec3(1.0f, 0.0f, 1.0f); // Half the width and depth
        glm::vec3 maxPoint = position + glm::vec3(1.0f, 6.0f, 1.0f); // Half the width and depth, plus full height

        houseAABBs.push_back(AABB{ minPoint, maxPoint });
	}
    for (int i = 0; i < nrTrees; i++) {
		
        glm::vec3 position = RandomObjectPosition(-50, 130, -50, 130);
        treePositions.push_back(position);

        // Define AABB for the tree (adjust size for both the cylinder trunk and cone top)
        glm::vec3 trunkMin = position - glm::vec3(trunkRadius, 0.0f, trunkRadius);
        glm::vec3 trunkMax = position + glm::vec3(trunkRadius, trunkHeight + 3.0f, trunkRadius);

		// foliage = coroana copacului
        glm::vec3 coneMin = position - glm::vec3(foliageBaseRadius, 0.0f, foliageBaseRadius);
        coneMin.y += trunkHeight + 1.8f; // Adjust Y-coordinate to start at trunkHeight
        glm::vec3 coneMax = position + glm::vec3(foliageBaseRadius, foliageHeight-0.5f, foliageBaseRadius);
        coneMax.y += trunkHeight; // Adjust Y-coordinate to include foliage height

        // Store AABBs for the trunk and cone as a pair
        treeAABBs.push_back(std::make_pair(AABB{ trunkMin, trunkMax }, AABB{ coneMin, coneMax }));
    }

	// Generate random positions for the checkpoints
    for (int i = 0; i < nrCheckpoints; i++)
    {
        glm::vec3 position = RandomObjectPosition(-50, 130, -50, 130);
        nextCheckpoints.push_back(position);
        checkpointsYellowGreen.push_back(position);

		// Ceare AABB-uri pentru checkpoint-uri
        // Define dimensions for the stick
        float stickLength = 1.5f;   // Length of the stick
        float stickThickness = 0.1f; // Thickness of the stick

        // Define dimensions for the circle
        float circleRadius = 1.0f;  // Radius of the circle
        float circleThickness = 0.3f; // Depth of the circle

        // Calculate AABB for the stick
        glm::vec3 stickMin = position - glm::vec3(stickThickness,0.0f, stickThickness);
        glm::vec3 stickMax = position + glm::vec3(stickThickness, stickLength, stickThickness);

        // Calculate AABB for the green side of the circle
        glm::vec3 greenCircleCenter = position; // Circle is at the top of the stick
        glm::vec3 greenCircleMin = greenCircleCenter - glm::vec3(circleRadius, stickLength, circleThickness);
        glm::vec3 greenCircleMax = greenCircleCenter + glm::vec3(circleRadius, 2*circleRadius+ stickLength, circleThickness);
        glm::vec3 yellowCircleCenter = position; // Center of the yellow circle (same as green circle)
        // Calculate the yellow circle AABB
        glm::vec3 yellowCircleMin = yellowCircleCenter - glm::vec3(circleRadius, stickLength, circleThickness);
        yellowCircleMin.z -= 2*circleThickness / 2.0f; // Adjust for circle depth

        glm::vec3 yellowCircleMax = yellowCircleCenter + glm::vec3(circleRadius, circleRadius*2+ stickLength, circleThickness);
        yellowCircleMax.z -= 2*circleThickness / 2.0f; // Adjust for circle depth

        // Store as a tuple of AABBs (stick, green circle, yellow circle)
        checkpointAABB.push_back(std::make_tuple(AABB{ stickMin, stickMax }, AABB{ greenCircleMin, greenCircleMax }, AABB{ yellowCircleMin, yellowCircleMax }));
        checkpointNextAABB.push_back(std::make_tuple(AABB{ stickMin, stickMax }, AABB{ greenCircleMin, greenCircleMax }, AABB{ yellowCircleMin, yellowCircleMax }));
    }


    camera = new implemented::Camera1();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    // Corp Drona
    {
        float x = 0.6f;
        float y = 0.04f;
        float z = 0.04f;

        float x2 = 0.04f;
        float z2 = 0.6f;

        glm::vec3 colorDrona = glm::vec3(0, 0, 1);
        vector<VertexFormat> vertices
        {
            // Prima parte
            /*Se folose?te un set de patru vârfuri pentru partea superioar? ?i patru pentru 
            partea inferioar?. Aceste vârfuri sunt definite astfel încât s? formeze un cub de 
            dimensiuni x ?i y pentru l??ime ?i în?l?ime, respectiv z pentru adâncime.
            Punctele sunt plasate în spa?iu astfel:
            Partea superioar? (cu y pozitiv) ?i partea inferioar? (cu y negativ).
            Fa?etele laterale sunt definite de puncte care sunt deplasate în func?ie de axele 
            x ?i z.*/
            VertexFormat(glm::vec3(-x, -y,  z), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 0
            VertexFormat(glm::vec3(x, -y,  z),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 1
            VertexFormat(glm::vec3(-x,  y,  z), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 2
            VertexFormat(glm::vec3(x,  y,  z),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 3
            VertexFormat(glm::vec3(-x, -y, -z), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 4
            VertexFormat(glm::vec3(x, -y, -z),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 5
            VertexFormat(glm::vec3(-x,  y, -z), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 6
            VertexFormat(glm::vec3(x,  y, -z),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 7

            // A doua parte
            VertexFormat(glm::vec3(-x2, -y,  z2), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 8
            VertexFormat(glm::vec3(x2, -y,  z2),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 9
            VertexFormat(glm::vec3(-x2,  y,  z2), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 10
            VertexFormat(glm::vec3(x2,  y,  z2),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 11
            VertexFormat(glm::vec3(-x2, -y, -z2), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 12
            VertexFormat(glm::vec3(x2, -y, -z2),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 13
            VertexFormat(glm::vec3(-x2,  y, -z2), colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 14
            VertexFormat(glm::vec3(x2,  y, -z2),  colorDrona, glm::vec3(0.2, 0.8, 0.6)), // 15
        };

        vector<unsigned int> indices =
        {
            /*Indicii definesc triunghiurile care formeaz? suprafa?a dronei, 
            legând vârfurile în spa?iu pentru a crea fa?etele formei 3D. 
            Fiecare grup de trei indici formeaz? un triunghi.*/
            // Indices for prima parte
            0, 1, 2, //0, 1, 2  definesc triangula?iile pentru partea cubic? a dronei, formând fe?ele acestui cub.
            1, 3, 2,
            2, 3, 7,
            2, 7, 6,
            1, 7, 3,
            1, 5, 7,
            6, 7, 4,
            7, 5, 4,
            0, 4, 1,
            1, 4, 5,
            2, 6, 4,
            0, 2, 4,

            // Indices for a doua parte
            8, 9, 10, // definesc triangula?iile pentru partea mai sub?ire, creând fe?ele acestei sec?iuni.
            9, 11, 10,
            10, 11, 15,
            10, 15, 14,
            9, 15, 11,
            9, 13, 15,
            14, 15, 12,
            15, 13, 12,
            8, 12, 9,
            9, 12, 13,
            10, 14, 12,
            8, 10, 12,
        };

        meshes["Drona"] = new Mesh("gc1");
        meshes["Drona"]->InitFromData(vertices, indices);
    }

    // Elice
    {
        float x = 0.14f;
        float y = 0.001f;
        float z = 0.01f;
        glm::vec3 colorElice = glm::vec3(1, 0, 0);
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(x, y, z), colorElice, glm::vec3(0)), // 0
            VertexFormat(glm::vec3(-x, y, z), colorElice, glm::vec3(0.2, 0.8, 0.6)), // 1
            VertexFormat(glm::vec3(x, -y, z), colorElice, glm::vec3(0.2, 0.8, 0.6)), // 2
            VertexFormat(glm::vec3(-x, -y, z), colorElice, glm::vec3(0.2, 0.8, 0.6)), // 3
            VertexFormat(glm::vec3(x, y, -z), colorElice, glm::vec3(0.2, 0.8, 0.6)), // 4
            VertexFormat(glm::vec3(-x, y, -z), colorElice, glm::vec3(0.2, 0.8, 0.6)), // 5
            VertexFormat(glm::vec3(x, -y, -z), colorElice, glm::vec3(0.2, 0.8, 0.6)), // 6
            VertexFormat(glm::vec3(-x, -y, -z), colorElice, glm::vec3(0.2, 0.8, 0.6)), // 7
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,    // indices for first triangle
            1, 3, 2,    // indices for second triangle
            2, 3, 7,
            2, 7, 6,
            1, 7, 3,
            1, 5, 7,
            6, 7, 4,
            7, 5, 4,
            0, 4, 1,
            1, 4, 5,
            2, 6, 4,
            0, 2, 4,
        };

        meshes["elice"] = new Mesh("generated elice");
        meshes["elice"]->InitFromData(vertices, indices);
    }
    // Teren
    /* Un teren 3D simplu, reprezentat ca o plasa de triunghiuri. 
    Codul este structurat astfel incat sa construiasca o retea de puncte 
    (vârfuri) si sa le conecteze in triunghiuri pentru a forma o suprafata 3D.*/
    {
        /*n: Reprezinta numarul de segmente pe fiecare latura a gridului (plasei).
        Deci, numarul total de puncte va fi (n+1)*(n+1) pentru o plasa cu n segmente pe fiecare latura.
        vertices: Vectorul care va contine toate vârfurile (punctele) 3D ale terenului.
        indices: Vectorul care va contine indicii pentru a conecta vârfurile in triunghiuri.*/
        int n = 200;
        vector<VertexFormat> vertices;
        vector<unsigned int> indices;

        // Generare vertecsi
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= n; j++) {
                vertices.push_back(VertexFormat(glm::vec3(i, 0.0, j), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)));
            }
        }
        /*Se creeaza o plasa de puncte 3D, avand coordonatele (i, 0, j) pentru fiecare punct, 
        unde i si j sunt indecsi care parcurg axele X si Z. Fiecare vârf este definit de:
        Pozitia: glm::vec3(i, 0.0, j) - pozitia fiecarui punct este pe planul XZ, la Y=0, formand astfel o retea de puncte.
        Culoarea: glm::vec3(0, 1, 0) - culoarea este setata la verde, adica fiecare punct are culoare verde.
        Coordenata de textura: glm::vec3(0, 0, 1) - aceasta poate fi utilizata pentru a mapa texturi pe teren, dar nu este explicata aici.*/

        // Generare indici
        /*Se foloseste formula start = i * (n + 1) + j pentru a gasi indexul primului vârf dintr-un patrat. 
        Patratul are 4 colturi, iar fiecare colt este conectat prin indicii ce formeaza doua triunghiuri.
        Pentru fiecare patrat definit de 4 puncte, se formeaza doua triunghiuri prin adaugarea indicilor respectivi 
        in vectorul indices:
        Primul triunghi: (start, start+1, start+n+1)
        Al doilea triunghi: (start+1, start+n+2, start+n+1)*/
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                int start = i * (n + 1) + j;
                indices.push_back(start);
                indices.push_back(start + 1);
                indices.push_back(start + n + 1);

                indices.push_back(start + 1);
                indices.push_back(start + n + 2);
                indices.push_back(start + n + 1);
            }
        }


        meshes["grid"] = new Mesh("generated grid");
        meshes["grid"]->InitFromData(vertices, indices);

    }
    //Watchtower
    {
        vector<VertexFormat> watchtowerVertices;
        vector<unsigned int> watchtowerIndices;

        // Dimensiuni
        float towerBaseSize = 1.0f; // Dimensiunea bazei turnului (mai mica decât casa)
        float towerHeight = 5.0f;   // Întl?imea turnului
        float platformHeight = 0.0f; // Întl?imea platformei pentru observare

        // Coordonate pentru baza turnului (cub)
        /*Se definesc coordonatele celor 8 vârfuri ale cubului care formeaza baza turnului. 
        Acestea sunt definite in 3D, pe axele X, Y si Z.
        Punctele p1, p2, p3, p4 sunt cele patru colturi ale bazei inferioare, iar p5, p6, p7, 
        p8 reprezinta colturi ale bazei superioare.*/
        glm::vec3 p1(-towerBaseSize / 2, 0.0f, -towerBaseSize / 2); // Stânga jos spate
        glm::vec3 p2(towerBaseSize / 2, 0.0f, -towerBaseSize / 2);  // Dreapta jos spate
        glm::vec3 p3(towerBaseSize / 2, 0.0f, towerBaseSize / 2);   // Dreapta jos fata
        glm::vec3 p4(-towerBaseSize / 2, 0.0f, towerBaseSize / 2);  // Stânga jos fata

        glm::vec3 p5(-towerBaseSize / 2, towerHeight, -towerBaseSize / 2); // Stânga sus spate
        glm::vec3 p6(towerBaseSize / 2, towerHeight, -towerBaseSize / 2);  // Dreapta sus spate
        glm::vec3 p7(towerBaseSize / 2, towerHeight, towerBaseSize / 2);   // Dreapta sus fata
        glm::vec3 p8(-towerBaseSize / 2, towerHeight, towerBaseSize / 2);  // Stânga sus fata

        // Adaug? vertectii pentru cub (baza turnului)
        //Fiecare vârf este adaugat la vectorul watchtowerVertices, iar culoarea este setata la un maro deschis
        watchtowerVertices.push_back(VertexFormat(p1, glm::vec3(0.5, 0.25, 0.1))); // Maro
        watchtowerVertices.push_back(VertexFormat(p2, glm::vec3(0.5, 0.25, 0.1)));
        watchtowerVertices.push_back(VertexFormat(p3, glm::vec3(0.5, 0.25, 0.1)));
        watchtowerVertices.push_back(VertexFormat(p4, glm::vec3(0.5, 0.25, 0.1)));

        watchtowerVertices.push_back(VertexFormat(p5, glm::vec3(0.5, 0.25, 0.1)));
        watchtowerVertices.push_back(VertexFormat(p6, glm::vec3(0.5, 0.25, 0.1)));
        watchtowerVertices.push_back(VertexFormat(p7, glm::vec3(0.5, 0.25, 0.1)));
        watchtowerVertices.push_back(VertexFormat(p8, glm::vec3(0.5, 0.25, 0.1)));

        // Indicii pentru cub
        unsigned int cubeIndices[] = {
            /*Indicii definesc cum sunt conectate vârfurile pentru a forma triunghiuri. 
            Fiecare triunghi este definit de 3 indici care indica vârfurile ce formeaza triunghiul respectiv.
            Fiecare fata a cubului este impartita in doua triunghiuri, iar acest lucru se 
            face pentru a crea o suprafata plana.*/
            0, 1, 2, 2, 3, 0, // Fata de jos
            4, 5, 6, 6, 7, 4, // Fata de sus
            0, 1, 5, 5, 4, 0, // Fata spate
            1, 2, 6, 6, 5, 1, // Fata dreapta
            2, 3, 7, 7, 6, 2, // Fata fata
            3, 0, 4, 4, 7, 3  // Fata stânga
        };

        watchtowerIndices.insert(watchtowerIndices.end(), cubeIndices, cubeIndices + sizeof(cubeIndices) / sizeof(unsigned int));

        // Coordonate pentru platforma turnului
        // Se definesc cele 4 colturi ale platformei turnului (platformBase1, platformBase2, platformBase3, platformBase4) 
        // care sunt plasate deasupra cubului, la inaltimea turnului plus inaltimea platformei.
        glm::vec3 platformBase1(-towerBaseSize / 2, towerHeight + platformHeight, -towerBaseSize / 2); // Stânga jos platform?
        glm::vec3 platformBase2(towerBaseSize / 2, towerHeight + platformHeight, -towerBaseSize / 2);  // Dreapta jos platform?
        glm::vec3 platformBase3(towerBaseSize / 2, towerHeight + platformHeight, towerBaseSize / 2);   // Dreapta sus platform?
        glm::vec3 platformBase4(-towerBaseSize / 2, towerHeight + platformHeight, towerBaseSize / 2);  // Stânga sus platform?
        // platformTop reprezinta varful platformei (un punct central).
        glm::vec3 platformTop(0.0f, towerHeight + platformHeight + 0.5f, 0.0f); // Vârful platformei

        // Adaug? vertec?ii pentru platform?
        watchtowerVertices.push_back(VertexFormat(platformBase1, glm::vec3(0.8, 0.6, 0.4))); // Platformas
        watchtowerVertices.push_back(VertexFormat(platformBase2, glm::vec3(0.8, 0.6, 0.4)));
        watchtowerVertices.push_back(VertexFormat(platformBase3, glm::vec3(0.8, 0.6, 0.4)));
        watchtowerVertices.push_back(VertexFormat(platformBase4, glm::vec3(0.8, 0.6, 0.4)));
        watchtowerVertices.push_back(VertexFormat(platformTop, glm::vec3(0.8, 0.6, 0.4)));

        // Indicii pentru platforma
        // Indicii definesc conexiunile triunghiurilor pentru platforma turnului.
        unsigned int platformIndices[] = {
            8, 9, 10, 10, 11, 8, // Platform? jos
            8, 9, 12, 9, 10, 12,  // Platform? stânga/dreapta
            10, 11, 12, 11, 8, 12  // Platform? fa??/spate
        };

        watchtowerIndices.insert(watchtowerIndices.end(), platformIndices, platformIndices + sizeof(platformIndices) / sizeof(unsigned int));

        // Crearea mesh-ului pentru turn
        meshes["watchtower"] = new Mesh("generated watchtower");
        meshes["watchtower"]->InitFromData(watchtowerVertices, watchtowerIndices);
    }

    //copac1
    {
        vector<VertexFormat> treeVertices;
        vector<unsigned int> treeIndices;

        // Dimensiuni
        trunkHeight = 3.0f;  // In?l?imea trunchiului
        trunkRadius = 0.2f;  // Raza trunchiului
        /*foliageHeight: inaltimea coroanei este de 2.0f.
        foliageBaseRadius: Raza bazei coroanei este de 1.5f, 
        iar raza varfului coroanei este mai mica (0.5f).*/
        foliageHeight = 2.0f;  // În?l?imea coroanei
        foliageBaseRadius = 1.5f;  // Raza de la baz? a coroanei
        float foliageTopRadius = 0.5f;  // Raza vârfului coroanei

        // Calculam înaltimea totala a trunchiului, care trebuie sa se prelungeasca pâna la dimensiunea coroanei
        float totalHeight = trunkHeight + foliageHeight;

        // Coordonate pentru trunchiul copacului (cilindru)
        glm::vec3 trunkBase(0.0f, 0.0f, 0.0f);  // Baza trunchiului
        glm::vec3 trunkTop(0.0f, trunkHeight, 0.0f);  // Vârful trunchiului

		/*Trunchiul este modelat folosind un cerc împartit în 20 de laturi 
        (adic? un poligon cu 20 de varfuri).
        Fiecare varf al trunchiului este definit de un unghi care este calculat
        în func?ie de numarul de laturi (trunkSides).
        Coordonatele fiecarui varf sunt calculate folosind functiile trigonometrice 
        cos ?i sin pentru a obtine puncte pe cercul de baza si pe cercul de varf al trunchiului.*/
        int trunkSides = 20;  // Numarul de laturi pentru trunchi
        for (int i = 0; i < trunkSides; i++) {
            float angle = i * 2.0f * glm::pi<float>() / trunkSides;
            float x = trunkRadius * cos(angle);
            float z = trunkRadius * sin(angle);
            glm::vec3 basePoint(x, 0.0f, z);  // Baza trunchiului
            glm::vec3 topPoint(x, trunkHeight, z);  // Vârful trunchiului

            treeVertices.push_back(VertexFormat(basePoint, glm::vec3(0.4, 0.2, 0.1))); // Culoare maro închis
            treeVertices.push_back(VertexFormat(topPoint, glm::vec3(0.4, 0.2, 0.1)));  // Culoare maro închis
        }

        // Indicii pentru trunchi (cilindru)
        for (int i = 0; i < trunkSides; i++) {
            /*Acestia sunt indicii pentru conectarea varfurilor in triunghiuri 
            pentru a crea suprafata laterala a trunchiului.
            Fiecare pereche de varfuri (un varf de la baza trunchiului si unul de 
            la varful trunchiului) formeaza doua triunghiuri. De asemenea, se leaga 
            triunghiurile succesive pentru a forma cilindrul.*/
            int next = (i + 1) % trunkSides;
            treeIndices.push_back(i * 2); treeIndices.push_back(i * 2 + 1); treeIndices.push_back(next * 2);  // Trunchi lateral
            treeIndices.push_back(next * 2); treeIndices.push_back(i * 2 + 1); treeIndices.push_back(next * 2 + 1);
        }

        // Coordonate pentru coroana copacului (conuri)
        glm::vec3 coneTop1(0.0f, trunkHeight + foliageHeight, 0.0f);  // Vârful primului con

        /*Coroana este reprezentata printr-un con format din 20 de laturi.
        Similar trunchiului, fiecare varf al coroanei este calculat folosind 
        functiile trigonometrice pentru a obtine puncte pe cercurile de la baza si varful conului.*/
        int foliageSides = 20;  // Num?rul de laturi pentru conuri
        for (int i = 0; i < foliageSides; i++) {
            float angle = i * 2.0f * glm::pi<float>() / foliageSides;

            // Primul con (partea inferioar?)
            glm::vec3 basePoint1(foliageBaseRadius * cos(angle), trunkHeight, foliageBaseRadius * sin(angle));
            treeVertices.push_back(VertexFormat(basePoint1, glm::vec3(0.1, 0.3, 0.1))); // Verde închis
            treeVertices.push_back(VertexFormat(coneTop1, glm::vec3(0.1, 0.3, 0.1))); // Verde închis
        }

        // Indicii pentru coroan? (conuri)
        /*Indicii pentru corana sunt similari cu cei ai trunchiului, dar acum se conecteaza varfurile
        coroanei pentru a forma doua triunghiuri pentru fiecare latura a conului.*/
        for (int i = 0; i < foliageSides; i++) {
            int next = (i + 1) % foliageSides;
            // Primul con
            treeIndices.push_back(i * 2 + trunkSides * 2); treeIndices.push_back(i * 2 + trunkSides * 2 + 1); treeIndices.push_back(next * 2 + trunkSides * 2);
            treeIndices.push_back(next * 2 + trunkSides * 2); treeIndices.push_back(i * 2 + trunkSides * 2 + 1); treeIndices.push_back(next * 2 + trunkSides * 2 + 1);
        }

        // Crearea mesh-ului pentru copac
        meshes["tree"] = new Mesh("generated tree");
        meshes["tree"]->InitFromData(treeVertices, treeIndices);
    }
    // Checkpoint
    /*Acest cod genereaza un model 3D pentru un punct de control (checkpoint) care 
    include un cerc vertical cu o lumina halo in jurul sau si un stick care sustine cercul.
    Codul este impartit in mai multe sectiuni pentru a crea fiecare parte a obiectului
    (cercul, lumina halo si stickul).*/
    {
        vector<VertexFormat> checkpointVertices;
        vector<unsigned int> checkpointIndices;

        // Circle Dimensions
        float radius = 1.0f;        // Radius of the vertical circle
        float thickness = 0.3f;     // Thickness of the circle (small depth)
        float glowRadius = radius + 0.1f; // Slightly larger radius for the glow effect
        float glowThickness = 0.05f;      // Smaller thickness for the glow
        int segments = 36;          // Number of segments for the circle

        glm::vec3 frontColor = glm::vec3(0.0f, 1.0f, 0.0f); // Green for the front face
        glm::vec3 backColor = glm::vec3(1.0f, 1.0f, 0.0f);  // Yellow for the back face
        glm::vec3 glowColor = glm::vec3(1.0f, 1.0f, 0.5f);  // Light yellow for the glow
        glm::vec3 stickColor = glm::vec3(0.8f, 0.8f, 0.8f); // Light gray for the stick

        // Stick Dimensions
        float stickLength = 1.5f;   // Length of the stick
        float stickThickness = 0.1f; // Thickness of the stick

        // Adjust position so the circle is at the very top of the stick
        float circleBaseY = stickLength + radius;  // Top of stick aligns with bottom of circle
        /*Fata din fata a cercului: Varfurile sunt plasate la o inaltime calculata pe baza 
        unui cerc (folosind functiile trigonometrice cos si sin).
        Fata din spate a cercului: Varfurile sunt plasate cu o adancire 
        (sau ridicare) a coordonatei z, in functie de grosimea cercului.*/
        // Generate vertices for the vertical circle (Front and Back)
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle) + circleBaseY;

            // Front side of the circle (green color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, thickness / 2.0f), frontColor));

            // Back side of the circle (yellow color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, -thickness / 2.0f), backColor));
        }

        // Add indices for the circle (Front and Back faces)
        for (int i = 0; i < segments; ++i) {
            /*Crearea indicilor pentru fiecare fata a cercului. Acestia sunt 
            folositi pentru a lega varfurile in triunghiuri, formand o retea
            de triunghiuri care reprezinta cercul.*/
            int next = (i + 1) % (segments + 1);

            // Front face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2 + 1);

            // Back face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2);
        }

        // Generate vertices for the glow ring
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / segments;
            float x = glowRadius * cos(angle);
            float y = glowRadius * sin(angle) + circleBaseY; // Place the glow ring slightly above the circle

            // Front side of the glow (light yellow)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, glowThickness / 2.0f), glowColor));

            // Back side of the glow (light yellow)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, -glowThickness / 2.0f), glowColor));
        }
        /*Varfurile pentru lumina halo sunt plasate intr-un cerc cu o raza 
        usor mai mare decat cercul principal (glowRadius).
        Acestea sunt plasate pe fata din fata si din spate a luminii halo.*/
        // Add indices for the glow ring (similar to the circle's indices)
        int glowBaseIndex = (segments + 1) * 2;
        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % (segments + 1);

            // Front face of the glow (as a quad split into two triangles)
            checkpointIndices.push_back(glowBaseIndex + i * 2);
            checkpointIndices.push_back(glowBaseIndex + next * 2);
            checkpointIndices.push_back(glowBaseIndex + next * 2 + 1);
            checkpointIndices.push_back(glowBaseIndex + i * 2);
            checkpointIndices.push_back(glowBaseIndex + next * 2 + 1);
            checkpointIndices.push_back(glowBaseIndex + i * 2 + 1);

            // Back face of the glow (as a quad split into two triangles)
            checkpointIndices.push_back(glowBaseIndex + i * 2 + 1);
            checkpointIndices.push_back(glowBaseIndex + next * 2 + 1);
            checkpointIndices.push_back(glowBaseIndex + next * 2);
            checkpointIndices.push_back(glowBaseIndex + i * 2 + 1);
            checkpointIndices.push_back(glowBaseIndex + next * 2);
            checkpointIndices.push_back(glowBaseIndex + i * 2);
        }

        // batul acadelei
        // Generate vertices for the stick
        glm::vec3 bottom = glm::vec3(0.0f, 0.0f, -stickThickness / 2.0f);
        glm::vec3 top = glm::vec3(0.0f, stickLength, -stickThickness / 2.0f);

        // Add stick vertices (front and back faces)
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, -stickThickness / 2.0f), stickColor));

        // Add indices for the stick
        int stickBaseIndex = (segments + 1) * 2 + (segments + 1) * 2;
        unsigned int stickIndices[] = {
            stickBaseIndex, stickBaseIndex + 1, stickBaseIndex + 2, stickBaseIndex, stickBaseIndex + 2, stickBaseIndex + 3, // Front face
            stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 4, stickBaseIndex + 6, stickBaseIndex + 7, // Back face
            stickBaseIndex, stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex, stickBaseIndex + 5, stickBaseIndex + 1, // Side faces
            stickBaseIndex + 1, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 1, stickBaseIndex + 6, stickBaseIndex + 2,
            stickBaseIndex + 2, stickBaseIndex + 6, stickBaseIndex + 7, stickBaseIndex + 2, stickBaseIndex + 7, stickBaseIndex + 3,
            stickBaseIndex + 3, stickBaseIndex + 7, stickBaseIndex + 4, stickBaseIndex + 3, stickBaseIndex + 4, stickBaseIndex
        };
        checkpointIndices.insert(checkpointIndices.end(), std::begin(stickIndices), std::end(stickIndices));

        // Create the mesh for the lollipop checkpoint with halo glow
        meshes["checkpoint"] = new Mesh("generated lollipop checkpoint with halo glow");
        meshes["checkpoint"]->InitFromData(checkpointVertices, checkpointIndices);
    }

    //next_checkpoint
    {
        vector<VertexFormat> checkpointVertices;
        vector<unsigned int> checkpointIndices;

        // Circle Dimensions
        float radius = 1.0f;        // Radius of the vertical circle
        float thickness = 0.3f;     // Thickness of the circle (small depth)
        float glowRadius = radius + 0.1f; // Slightly larger radius for the glow effect
        float glowThickness = 0.05f;      // Smaller thickness for the glow
        int segments = 36;          // Number of segments for the circle

        glm::vec3 frontColor = glm::vec3(0.32f, 0.32f, 0.32f); // GRAY for the front face
        glm::vec3 backColor = glm::vec3(0.5f, 0.5f, 0.5f);  // GRAY for the back face
        glm::vec3 glowColor = glm::vec3(1.0f, 0.5f, 1.0f);  // Light pink for the glow
        glm::vec3 stickColor = glm::vec3(0.8f, 0.8f, 0.8f); // Light gray for the stick (unchanged)


        // Stick Dimensions
        float stickLength = 1.5f;   // Length of the stick
        float stickThickness = 0.1f; // Thickness of the stick

        // Adjust position so the circle is at the very top of the stick
        float circleBaseY = stickLength + radius;  // Top of stick aligns with bottom of circle

        // Generate vertices for the vertical circle (Front and Back)
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle) + circleBaseY;

            // Front side of the circle (green color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, thickness / 2.0f), frontColor));

            // Back side of the circle (yellow color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, -thickness / 2.0f), backColor));
        }

        // Add indices for the circle (Front and Back faces)
        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % (segments + 1);

            // Front face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2 + 1);

            // Back face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2);
        }

        // Generate vertices for the stick
        glm::vec3 bottom = glm::vec3(0.0f, 0.0f, -stickThickness / 2.0f);
        glm::vec3 top = glm::vec3(0.0f, stickLength, -stickThickness / 2.0f);

        // Add stick vertices (front and back faces)
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, -stickThickness / 2.0f), stickColor));

        // Add indices for the stick
        int stickBaseIndex = (segments + 1) * 2;
        unsigned int stickIndices[] = {
            stickBaseIndex, stickBaseIndex + 1, stickBaseIndex + 2, stickBaseIndex, stickBaseIndex + 2, stickBaseIndex + 3, // Front face
            stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 4, stickBaseIndex + 6, stickBaseIndex + 7, // Back face
            stickBaseIndex, stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex, stickBaseIndex + 5, stickBaseIndex + 1, // Side faces
            stickBaseIndex + 1, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 1, stickBaseIndex + 6, stickBaseIndex + 2,
            stickBaseIndex + 2, stickBaseIndex + 6, stickBaseIndex + 7, stickBaseIndex + 2, stickBaseIndex + 7, stickBaseIndex + 3,
            stickBaseIndex + 3, stickBaseIndex + 7, stickBaseIndex + 4, stickBaseIndex + 3, stickBaseIndex + 4, stickBaseIndex
        };
        checkpointIndices.insert(checkpointIndices.end(), std::begin(stickIndices), std::end(stickIndices));

        // Create the mesh for the lollipop checkpoint
        meshes["checkpointDeactivated"] = new Mesh("generated lollipop checkpoint");
        meshes["checkpointDeactivated"]->InitFromData(checkpointVertices, checkpointIndices);
    }
    //deactivated_checkpoint
    {
        vector<VertexFormat> checkpointVertices;
        vector<unsigned int> checkpointIndices;

        // Circle Dimensions
        float radius = 1.0f;        // Radius of the vertical circle
        float thickness = 0.3f;     // Thickness of the circle (small depth)
        float glowRadius = radius + 0.1f; // Slightly larger radius for the glow effect
        float glowThickness = 0.05f;      // Smaller thickness for the glow
        int segments = 36;          // Number of segments for the circle

        glm::vec3 frontColor = glm::vec3(0.4392f, 0.576f, 0.858f); // Bright blue for the front face
        glm::vec3 backColor = glm::vec3(0.184f, 0.18f, 0.309f);  // Bright pink for the back face
        glm::vec3 glowColor = glm::vec3(1.0f, 0.5f, 1.0f);  // Light pink for the glow
        glm::vec3 stickColor = glm::vec3(0.6f, 0.6f, 0.6f); // Light gray for the stick (unchanged)


        // Stick Dimensions
        float stickLength = 1.5f;   // Length of the stick
        float stickThickness = 0.1f; // Thickness of the stick

        // Adjust position so the circle is at the very top of the stick
        float circleBaseY = stickLength + radius;  // Top of stick aligns with bottom of circle

        // Generate vertices for the vertical circle (Front and Back)
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle) + circleBaseY;

            // Front side of the circle (green color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, thickness / 2.0f), frontColor));

            // Back side of the circle (yellow color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, -thickness / 2.0f), backColor));
        }

        // Add indices for the circle (Front and Back faces)
        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % (segments + 1);

            // Front face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2 + 1);

            // Back face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2);
        }

        // Generate vertices for the stick
        glm::vec3 bottom = glm::vec3(0.0f, 0.0f, -stickThickness / 2.0f);
        glm::vec3 top = glm::vec3(0.0f, stickLength, -stickThickness / 2.0f);

        // Add stick vertices (front and back faces)
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, -stickThickness / 2.0f), stickColor));

        // Add indices for the stick
        int stickBaseIndex = (segments + 1) * 2;
        unsigned int stickIndices[] = {
            stickBaseIndex, stickBaseIndex + 1, stickBaseIndex + 2, stickBaseIndex, stickBaseIndex + 2, stickBaseIndex + 3, // Front face
            stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 4, stickBaseIndex + 6, stickBaseIndex + 7, // Back face
            stickBaseIndex, stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex, stickBaseIndex + 5, stickBaseIndex + 1, // Side faces
            stickBaseIndex + 1, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 1, stickBaseIndex + 6, stickBaseIndex + 2,
            stickBaseIndex + 2, stickBaseIndex + 6, stickBaseIndex + 7, stickBaseIndex + 2, stickBaseIndex + 7, stickBaseIndex + 3,
            stickBaseIndex + 3, stickBaseIndex + 7, stickBaseIndex + 4, stickBaseIndex + 3, stickBaseIndex + 4, stickBaseIndex
        };
        checkpointIndices.insert(checkpointIndices.end(), std::begin(stickIndices), std::end(stickIndices));

        // Create the mesh for the lollipop checkpoint
        meshes["checkpointNext"] = new Mesh("generated lollipop checkpoint");
        meshes["checkpointNext"]->InitFromData(checkpointVertices, checkpointIndices);
    }


    //completed_checkpoint
    {
        vector<VertexFormat> checkpointVertices;
        vector<unsigned int> checkpointIndices;

        // Circle Dimensions
        float radius = 0.5f;        // Radius of the vertical circle
        float thickness = 0.3f;     // Thickness of the circle (small depth)
        float glowRadius = radius + 0.1f; // Slightly larger radius for the glow effect
        float glowThickness = 0.05f;      // Smaller thickness for the glow
        int segments = 36;          // Number of segments for the circle

        glm::vec3 frontColor = glm::vec3(1.0f, 1.0f, 0.8f); // Bright blue for the front face
        glm::vec3 backColor = glm::vec3(1.0f, 1.0f, 0.8f);  // Bright pink for the back face
        glm::vec3 glowColor = glm::vec3(1.0f, 0.5f, 1.0f);  // Light pink for the glow
        glm::vec3 stickColor = glm::vec3(1.0f, 1.0f, 0.8f); // Light gray for the stick (unchanged)


        // Stick Dimensions
        float stickLength = 1.5f;   // Length of the stick
        float stickThickness = 0.1f; // Thickness of the stick

        // Adjust position so the circle is at the very top of the stick
        float circleBaseY = stickLength + radius;  // Top of stick aligns with bottom of circle

        // Generate vertices for the vertical circle (Front and Back)
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle) + circleBaseY;

            // Front side of the circle (green color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, thickness / 2.0f), frontColor));

            // Back side of the circle (yellow color)
            checkpointVertices.push_back(VertexFormat(glm::vec3(x, y, -thickness / 2.0f), backColor));
        }

        // Add indices for the circle (Front and Back faces)
        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % (segments + 1);

            // Front face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(i * 2 + 1);

            // Back face (as a quad split into two triangles)
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2 + 1);
            checkpointIndices.push_back(next * 2);
            checkpointIndices.push_back(i * 2);
        }

        // Generate vertices for the stick
        glm::vec3 bottom = glm::vec3(0.0f, 0.0f, -stickThickness / 2.0f);
        glm::vec3 top = glm::vec3(0.0f, stickLength, -stickThickness / 2.0f);

        // Add stick vertices (front and back faces)
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, bottom.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(stickThickness, top.y, -stickThickness / 2.0f), stickColor));
        checkpointVertices.push_back(VertexFormat(glm::vec3(-stickThickness, top.y, -stickThickness / 2.0f), stickColor));

        // Add indices for the stick
        int stickBaseIndex = (segments + 1) * 2;
        unsigned int stickIndices[] = {
            stickBaseIndex, stickBaseIndex + 1, stickBaseIndex + 2, stickBaseIndex, stickBaseIndex + 2, stickBaseIndex + 3, // Front face
            stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 4, stickBaseIndex + 6, stickBaseIndex + 7, // Back face
            stickBaseIndex, stickBaseIndex + 4, stickBaseIndex + 5, stickBaseIndex, stickBaseIndex + 5, stickBaseIndex + 1, // Side faces
            stickBaseIndex + 1, stickBaseIndex + 5, stickBaseIndex + 6, stickBaseIndex + 1, stickBaseIndex + 6, stickBaseIndex + 2,
            stickBaseIndex + 2, stickBaseIndex + 6, stickBaseIndex + 7, stickBaseIndex + 2, stickBaseIndex + 7, stickBaseIndex + 3,
            stickBaseIndex + 3, stickBaseIndex + 7, stickBaseIndex + 4, stickBaseIndex + 3, stickBaseIndex + 4, stickBaseIndex
        };
        checkpointIndices.insert(checkpointIndices.end(), std::begin(stickIndices), std::end(stickIndices));

        // Create the mesh for the lollipop checkpoint
        meshes["checkpointCompleted"] = new Mesh("generated lollipop checkpoint");
        meshes["checkpointCompleted"]->InitFromData(checkpointVertices, checkpointIndices);
    }

    // Arrow
    {
        vector<VertexFormat> arrowVertices;
        vector<unsigned int> arrowIndices;

        // Arrow Dimensions
        float shaftLength = 0.3f;      // Length of the arrow's shaft
        float shaftRadius = 0.05f;      // Radius of the shaft
        float headLength = 0.5f;       // Length of the arrowhead
        float headRadius = 0.03f;       // Radius of the arrowhead
        int segments = 36;             // segments: Numarul de segmente pentru cercuri.
        // Cu cat este mai mare acest numar, cu atat va fi mai fina si mai detaliata forma 
        // cercurilor, folosite pentru corpul sage?ii si baza varfului sage?ii.

        glm::vec3 shaftColor = glm::vec3(0.8f, 0.8f, 0.8f);   // Light gray for the shaft
        glm::vec3 headColor = glm::vec3(1.0f, 0.0f, 0.0f);    // Red for the arrowhead
        /*Uun ciclu pentru a calcula varfurile cilindrului care formeaza corpul sage?ii.
        Cercul este generat prin trigonometrie: pentru fiecare segment, se calculeaza 
        coordonatele y si z folosind functiile trigonometrice sin si cos.
        Partea de jos a corpului este plasata la origine (0.0f, y, z).
        Partea de sus a corpului este plasata de-a lungul axei X, la o distanta de shaftLength de la origine.*/
        // Generate vertices for the shaft (a simple cylinder)
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / segments;
            float z = shaftRadius * cos(angle);   // Change x to z for horizontal
            float y = shaftRadius * sin(angle);

            // Bottom part of the shaft (positioned at the origin)
            arrowVertices.push_back(VertexFormat(glm::vec3(0.0f, y, z), shaftColor));

            // Top part of the shaft (positioned along X axis, extending from the origin)
            arrowVertices.push_back(VertexFormat(glm::vec3(shaftLength, y, z), shaftColor));
        }

        /*Indicii sunt adaugati pentru fatetele laterale ale corpului sage?ii.
        Fiecare pereche de varfuri adaugata formeaza doua triunghiuri care constituie o fateta laterala a cilindrului.*/
        // Add indices for the shaft (side faces)
        int baseIndex = 0;
        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % segments;

            // Side faces of the shaft (as a quad split into two triangles)
            arrowIndices.push_back(baseIndex + i * 2);
            arrowIndices.push_back(baseIndex + next * 2);
            arrowIndices.push_back(baseIndex + next * 2 + 1);
            arrowIndices.push_back(baseIndex + i * 2);
            arrowIndices.push_back(baseIndex + next * 2 + 1);
            arrowIndices.push_back(baseIndex + i * 2 + 1);
        }

        // Generate vertices for the arrowhead (a cone-like shape)
        /*Varful sage?ii este generat sub forma unui con cu varful la coordonatele tip 
        si baza la capatul corpului sage?ii.
        La fel ca in cazul corpului, varfurile sunt plasate pe un cerc, dar acum acestea 
        formeaza un con cu varful la (shaftLength + headLength, 0.0f, 0.0f).*/
        glm::vec3 tip = glm::vec3(shaftLength + headLength, 0.0f, 0.0f);  // Tip of the arrowhead (moved to the right along X axis)
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / segments;
            float z = headRadius * cos(angle);    // Change x to z for horizontal
            float y = headRadius * sin(angle);

            // Base of the arrowhead (at the top of the shaft)
            arrowVertices.push_back(VertexFormat(glm::vec3(shaftLength, y, z), headColor));

            // Tip of the arrowhead (the point of the arrow)
            arrowVertices.push_back(VertexFormat(tip, headColor));
        }

        // Add indices for the arrowhead (side faces)
        /*Indicii pentru fatetele laterale ale varfului sunt adaugati sub forma unui fan de triunghiuri.
        Fiecare triunghi are varfurile intre baza varfului si tipul sage?ii.*/
        int headBaseIndex = arrowVertices.size() - (segments + 1) * 2;
        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % segments;

            // Side faces of the arrowhead (as a triangle fan)
            arrowIndices.push_back(headBaseIndex + i * 2);
            arrowIndices.push_back(headBaseIndex + next * 2);
            arrowIndices.push_back(headBaseIndex + i * 2 + 1);
        }

        // Create the mesh for the arrow
        meshes["arrow"] = new Mesh("generated arrow");
        meshes["arrow"]->InitFromData(arrowVertices, arrowIndices);
    }

    // Lab Shader
    {
        // shader facut dupa modelul din laborator
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
}

/*Functie de start a unui frame intr-o aplicatie grafica, care se ocupa cu configurarea proiectiei si 
curatarea bufferelor inainte de a incepe sa deseneze pe ecran.*/
void Tema2::FrameStart()
{
    /*projectionMatrix: Se seteaza matricea de proiectie care va fi utilizata pentru transformarea coordonatelor in spatiul 3D.*/
    /*Daca orthoProjection este fals: Se foloseste o proiectie perspectiva (glm::perspective), care este utilizata in majoritatea aplicatiilor 3D pentru a crea iluzia de adancime, astfel incat obiectele aflate la distanta sa apara mai mici. Parametrii sunt:
      RADIANS(projectionFov): Campul de vizibilitate al camerei, in grade, convertit in radiani.
      window->props.aspectRatio: Raportul de aspect al ferestrei, care defineste proportiile latime/inaltime ale ferestrei.
      0.01f, 200.0f: La fel ca in cazul proiectiei ortografice, aceste valori stabilesc distanta aproape si distanta de 
      vizibilitate maxima.*/
    projectionMatrix = orthoProjection
        ? glm::ortho(-projectionWidth / 2, projectionWidth / 2, -projectionHeight / 2, projectionHeight / 2, .01f, 200.0f)
        : glm::perspective(RADIANS(projectionFov), window->props.aspectRatio, 0.01f, 200.0f);
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1); //glClearColor(0, 0, 0, 1): Seteaza culoarea de fundal a ecranului (buffer-ul de culoare) 
    // la negru ((0, 0, 0) pentru RGB si 1 pentru opacitate).
    /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT): Curata bufferul de culoare si bufferul de adancime.
    GL_COLOR_BUFFER_BIT sterge buffer-ul de culoare (deci fundalul devine negru), iar GL_DEPTH_BUFFER_BIT
    sterge buffer-ul de adancime (pentru a reseta valorile de adancime, astfel incat obiectele sa fie desenate
    corect, in functie de adancimea lor in scena).*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}




void Tema2::Update(float deltaTimeSeconds)
{
	///////////////////////////////////////////////TEREN///////////////////////////////////////////////
	
		glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-50, 0, -50));
		RenderMesh(meshes["grid"], shaders["LabShader"], modelMatrix);

	///////////////////////////////////////////////CASA/////////////////////////////////////////////////
		for (int i = 0; i < nrHouses; i++)
		{
			glm::vec3 position = housePositions[i];
			//printf("House position: %f %f %f\n", position.x, position.y, position.z);
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, position);
			RenderMesh(meshes["watchtower"], shaders["VertexColor"], modelMatrix);

		}
      
    ////////////////////////////////////////////////COPAC///////////////////////////////////////////////
		for (int i = 0; i < nrTrees; i++)
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, treePositions[i]);
            RenderMesh(meshes["tree"], shaders["VertexColor"], modelMatrix);
        }

		////////////////////////////////////////////////CHECKPOINT///////////////////////////////////////////////
   
        if (checkpointStates[currentCheckpoint] == DEACTIVATED) {
			checkpointStates[currentCheckpoint] = NOT_ENTERED;
           
        }

        for (int i = 0; i < nrCheckpoints; i++)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, nextCheckpoints[i]);
            // modelMatrix = glm::translate(modelMatrix, glm::vec3(0,0,0));
            if (checkpointStates[i] == COMPLETED) {
				RenderMesh(meshes["checkpointCompleted"], shaders["VertexColor"], modelMatrix);
            }
			else if (i != currentCheckpoint && i != currentCheckpoint+1)
              RenderMesh(meshes["checkpointDeactivated"], shaders["VertexColor"], modelMatrix);
			else if (i == currentCheckpoint)
			{
				RenderMesh(meshes["checkpoint"], shaders["VertexColor"], modelMatrix);
			}
			else if (i == currentCheckpoint + 1)
			{
				RenderMesh(meshes["checkpointNext"], shaders["VertexColor"], modelMatrix);
			}
        }
      

	////////////////////////////////////////////////DRONA///////////////////////////////////////////////

    // Now you have the local axes (forward, right, up) for the drone

    // Position and rotation of the drone
        {
            droneModelMatrix = glm::mat4(1);
            droneModelMatrix = glm::translate(droneModelMatrix, glm::vec3(translateX, 1 + translateZ, translateY));
            droneModelMatrix = glm::rotate(droneModelMatrix, RADIANS(45.0f + rotationAngleDrona), glm::vec3(0, 1, 0));


            // check if collision with ground is made:
			if (CheckGroundCollision(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])) || CheckCollisionWithObstacles(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])))
            {
				// If the drone has collided with the ground, stop the drone from falling
                // move drone behind
                RenderMesh(meshes["Drona"], shaders["VertexColor"], oldModelMatrix);
			}
            else {
                RenderMesh(meshes["Drona"], shaders["VertexColor"], droneModelMatrix);
                oldModelMatrix = droneModelMatrix;
            }
	
        }
    // Propellers (adjust their position based on local axes)
    for (int i = 0; i < 4; ++i) {
        propellerModelMatrices[i] = glm::mat4(1);
        propellerModelMatrices[i] = glm::translate(propellerModelMatrices[i], glm::vec3(translateX, 1 + translateZ, translateY)); // Global position
        
        // Calculate propeller offset in local space using the local right and forward axes
        glm::vec3 rotatedOffset = RotateAroundYAxis(propellerOffsets[i], RADIANS(-rotationAngleDrona)); // Rotate propeller position based on drone's angle
        
        // Apply the rotated offset to move the propeller relative to the drone
        propellerModelMatrices[i] = glm::translate(propellerModelMatrices[i], rotatedOffset);

        // Rotate propeller along its own axis
        propellerModelMatrices[i] = glm::rotate(propellerModelMatrices[i], RADIANS(rotationAngleElice), glm::vec3(0, 1, 0)); // Spin propeller
        if (CheckGroundCollision(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])) || CheckCollisionWithObstacles(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])))
        {
			propellerModelMatrices[i] = oldPropellerModelMatrices[i];
        }
        RenderMesh(meshes["elice"], shaders["VertexColor"], propellerModelMatrices[i]);
		oldPropellerModelMatrices[i] = propellerModelMatrices[i];

       
    }


    ///////////////////////////////////////////////////////////////ARROW//////////////////////////////////////////////////////////////////
	{
		
		arrowMatrix = glm::mat4(1);
     
		// rotate it
        arrowMatrix = glm::translate(arrowMatrix, glm::vec3(translateX, 1.0f + translateZ, translateY));

        // Calculate propeller offset in local space using the local right and forward axes
        glm::vec3 rotatedOffset = RotateAroundYAxis(glm::vec3 (0,0.8f,-1), RADIANS(-rotationAngleDrona)); // Rotate propeller position based on drone's angle

        // Apply the rotated offset to move the propeller relative to the drone
       arrowMatrix = glm::translate(arrowMatrix, rotatedOffset);

		glm::vec3 checkpointPosition = nextCheckpoints[currentCheckpoint];
		// get arrow vector
		glm::vec3 arrowVector = glm::vec3 (arrowMatrix[3][0], 0, arrowMatrix[3][2]);
		glm::vec3 checkpointVector = glm::vec3(checkpointPosition.x, 0, checkpointPosition.z) ;
        // Compute the direction vector from the arrow to the checkpoint
        glm::vec3 direction = checkpointVector - arrowVector;
        // get length
		float length = glm::length(direction);

	//	float angle = asin(checkpointPosition.z / length);
        float angle = atan2(direction.z, direction.x); // Correct angle around Y-axis
		//angle += RADIANS(rotationAngleDrona);

		// the new angle is  the rotated angle + the angle between the arrow and the checkpoint
		//angle += rotationAngleDrona;
        
        // Apply the rotation to the arrow
       arrowMatrix = glm::rotate(arrowMatrix, -angle , glm::vec3(0, 1, 0));  //
		//arrowMatrix = glm::rotate(arrowMatrix, RADIANS(180.0f), glm::vec3(1, 0, 0));

        if (CheckGroundCollision(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])) || CheckCollisionWithObstacles(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])))
        {
			arrowMatrix = oldArrowMatrix;
		}
        
		RenderMesh(meshes["arrow"], shaders["VertexColor"], arrowMatrix);
		oldArrowMatrix = arrowMatrix;
	}


	////////////////////////////////////////////////CAMERA///////////////////////////////////////////////

    // Calculate the drone's forward direction
     glm::vec3 forward2 = glm::normalize(glm::vec3(
         -sin(RADIANS(  rotationAngleDrona)), // X component
         0,                                       // Y remains constant
         -cos(RADIANS( rotationAngleDrona)) // Z component
     ));

     // Offset to place the camera behind the drone
     float offsetDistance = 1.5f; // Distance behind the drone
	 float offsetHeight = 0.3f; // Height above the drone
     glm::vec3 behindOffset = -forward2 * offsetDistance;
	 // Move backwards
	 behindOffset.y = offsetHeight;

     if (CheckGroundCollision(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])) || CheckCollisionWithObstacles(glm::vec3(droneModelMatrix[3][0], droneModelMatrix[3][1], droneModelMatrix[3][2])))
     {
         translateX = oldCameraX;
         translateY = oldCameraY;
         translateZ = oldCameraZ;

     }

     // Camera position (slightly behind the drone's position, with an upward offset)
     glm::vec3 cameraPosition = glm::vec3(translateX, 1.5f + translateZ, translateY) + behindOffset;

     // Camera target (looking forward from the drone)
     glm::vec3 targetPosition = glm::vec3(translateX, 1.5f + translateZ, translateY) + forward2;

     // Update camera's view matrix
     camera->Set(cameraPosition, targetPosition, glm::vec3(0, 1, 0)); // Set new camera position, target, and up vector
     oldCameraX = translateX;
     oldCameraY = translateY;
     oldCameraZ = translateZ;
  
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
    
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 2.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            camera->TranslateForward(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->TranslateRight(-deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera->TranslateForward(-deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera->TranslateRight(deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(deltaTime * cameraSpeed);
        }
    }

    if (window->KeyHold(GLFW_KEY_1)) {
        projectionFov -= deltaTime * 10;
    }
    if (window->KeyHold(GLFW_KEY_2)) {
        projectionFov += deltaTime * 10;
    }

    if (window->KeyHold(GLFW_KEY_3)) {
        projectionWidth += 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_4)) {
        projectionWidth -= 5 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_5)) {
        projectionHeight += 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_6)) {
        projectionHeight -= 5 * deltaTime;
    }

    float rotationSpeed = 800; // Degrees per second
	float rotationLowSpeed = 200;
    rotationAngleElice += rotationSpeed * deltaTime; // deltaTime should be the time elapsed between frames
    if (rotationAngleElice >= 360.0f) {
        rotationAngleElice -= 360.0f; // Keep the angle within 0 to 360 degrees
    }

    float movementSpeed = 4.0f; // Units per second
  

    // Movement control: Update translation based on local axes
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        // Move the drone forward along the forward vector
        translateX += sin(RADIANS( rotationAngleDrona)) * movementSpeed * deltaTime;
        translateY += cos(RADIANS( rotationAngleDrona)) * movementSpeed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_UP)) {
        // Move the drone backward along the forward vector (opposite direction)
        translateX -= sin(RADIANS( rotationAngleDrona)) * movementSpeed * deltaTime;
        translateY -= cos(RADIANS( rotationAngleDrona)) * movementSpeed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        // Move the drone left along the right vector
        translateX += sin(RADIANS(90.0f + rotationAngleDrona)) * movementSpeed * deltaTime;
        translateY += cos(RADIANS(90.0f + rotationAngleDrona)) * movementSpeed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_LEFT)) {
        // Move the drone right along the right vector
        translateX -= sin(RADIANS(90.0f + rotationAngleDrona)) * movementSpeed * deltaTime;
        translateY -= cos(RADIANS(90.0f + rotationAngleDrona)) * movementSpeed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_U)) {
        // Move the drone upward along the up vector
        translateZ += movementSpeed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_H)) {
        // Move the drone downward along the up vector (opposite direction)
    
        translateZ -= movementSpeed * deltaTime;
    }

    // Rotation control: Update rotation angle around the Y-axis (around the "up" vector)
    if (window->KeyHold(GLFW_KEY_J)) {
        rotationAngleDrona += rotationLowSpeed * deltaTime;
        if (rotationAngleDrona >= 360.0f) {
            rotationAngleDrona -= 360.0f;
        }
    }

    if (window->KeyHold(GLFW_KEY_K)) {
        rotationAngleDrona -= rotationLowSpeed * deltaTime;
        if (rotationAngleDrona < 0.0f) {
            rotationAngleDrona += 360.0f;
        }
    }


}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    // TODO(student): Switch projections
    if (key == GLFW_KEY_O) orthoProjection = true;
    else if (key == GLFW_KEY_P) orthoProjection = false;

    //////////////////////////////////
   
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOY);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}