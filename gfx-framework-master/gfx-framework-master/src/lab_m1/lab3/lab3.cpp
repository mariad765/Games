#include "lab_m1/lab3/lab3.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab3::Lab3()
{
}


Lab3::~Lab3()
{
}


void Lab3::Init()
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

    // TODO(student): Compute coordinates of a square's center, and store
    // then in the `cx` and `cy` class variables (see the header). Use
    // `corner` and `squareSide`. These two class variables will be used
    // in the `Update()` function. Think about it, why do you need them?

 // Initialize translation variables
    translateX1 = 0; // For square 1
    translateX2 = 0; // For square 2
    translateX3 = 0; // For square 3

    // Initialize scale factors (if needed)
    scaleX = 1;
    scaleY = 1;

    // Define the center coordinates for rotation
     cx = 500 + squareSide / 2; // Adjusted for square's position
    cy = 250 + squareSide / 2; // Adjusted for square's position

	cx1 = 100 + squareSide / 2; // Adjusted for square's position
	cy1 = 250 + squareSide / 2; // Adjusted for square's position

    // Initialize angularStep
    angularStep = 0;

    // Create squares at different starting positions
    glm::vec3 corner1 = glm::vec3(100, 250, 0); // Position for square 1
    Mesh* square1 = object2D::CreateSquare("square1", corner1, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    glm::vec3 corner2 = glm::vec3(300, 250, 0); // Position for square 2
    Mesh* square2 = object2D::CreateSquare("square2", corner2, squareSide, glm::vec3(0, 1, 0));
    AddMeshToList(square2);

    glm::vec3 corner3 = glm::vec3(500, 250, 0); // Position for square 3
    Mesh* square3 = object2D::CreateSquare("square3", corner3, squareSide, glm::vec3(0, 0, 1));
    AddMeshToList(square3);
}


void Lab3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab3::Update(float deltaTimeSeconds)
{

    // Start with an identity matrix

    modelMatrix = glm::mat3(1);



    
    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Scale(scaleX, scaleX);
    modelMatrix *= transform2D::Translate(-cx, -cy);


  
    // Render the square
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    // Move left and right for square 2
    translateX2 += 100 * deltaTimeSeconds; // Move to the right
    if (translateX2 > 200) { // If it exceeds the limit
        translateX2 = 200; // Set to the limit
    }
    else if (translateX2 < -200) { // If it goes below the limit
        translateX2 = -200; // Set to the negative limit
    }

    // Create a smooth back and forth movement
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(400 + translateX2 * sin(glfwGetTime()), 250); // Adjust X position
    RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);
    // Update the rotation for square 3
    angularStep += 2 * deltaTimeSeconds; // Rotate at a speed of 2 radians per second

    // Initialize the model matrix for square 3
    modelMatrix = glm::mat3(1); // Identity matrix

    // Translate to square's center, rotate, then translate back
    modelMatrix *= transform2D::Translate(cx, cy); // Translate to the center
    modelMatrix *= transform2D::Rotate(angularStep); // Rotate around the center
    modelMatrix *= transform2D::Translate(-cx, -cy); // Translate back

    // Render square 3
    RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);

}

void Lab3::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab3::OnInputUpdate(float deltaTime, int mods)
{
}


void Lab3::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Lab3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab3::OnWindowResize(int width, int height)
{
}
