#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>
#include <Core/Engine.h>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1() : clearColor(0.0f, 0.0f, 0.0f), position(0.0f, 0.5f, 0.0f) // Ini?ializ?ri
{
    // Initialize color list
    colors = {
        glm::vec3(0.0f, 0.0f, 0.0f), // Black
        glm::vec3(1.0f, 0.0f, 0.0f), // Red
        glm::vec3(0.0f, 1.0f, 0.0f), // Green
        glm::vec3(0.0f, 0.0f, 1.0f)  // Blue
    };
    // Ad?ug?m meshe-urile în vector
    meshes_list.push_back("box");    // Cube
    meshes_list.push_back("teapot");  // Teapot
    meshes_list.push_back("sphere");   // Sphere
}



Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    {
        Mesh* mesh = new Mesh("teapot");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }



}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{

    float speed = 2.0f;
    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glm::ivec2 resolution = window->props.resolution;

    // Use the current clear color
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);

    // Clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





    // Set viewport and render objects
    glViewport(0, 0, resolution.x, resolution.y);
    RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));
    RenderMesh(meshes["sphere"], glm::vec3(0, 0.5f, 1), glm::vec3(0.5f));

    // print this stringL: [meshes_list[currentMeshIndex]
    printf("Current mesh: %s\n", meshes_list[currentMeshIndex].c_str());


    RenderMesh(meshes[meshes_list[currentMeshIndex]], position, glm::vec3(0.5f));


}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 *
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input
    float speed = 2.0f;

    // Move forward/backward on the z-axis
    if (window->KeyHold(GLFW_KEY_W)) {
        position.z -= speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        position.z += speed * deltaTime;
    }

    // Move left/right on the x-axis
    if (window->KeyHold(GLFW_KEY_A)) {
        position.x -= speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        position.x += speed * deltaTime;
    }

    // Move up/down on the y-axis
    if (window->KeyHold(GLFW_KEY_SPACE)) {
        position.y += speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
        position.y -= speed * deltaTime;
    }


}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event


    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.
    if (key == GLFW_KEY_F) {
        currentColorIndex = (currentColorIndex + 1) % colors.size(); // Cycle index
        clearColor = colors[currentColorIndex]; // Update clear color
    }

    // Schimb? meshe-ul la ap?sarea tastei C
    if (key == GLFW_KEY_J) {
        currentMeshIndex = (currentMeshIndex + 1) % meshes_list.size(); // Ciclic prin meshe-uri
    }


}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event

}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
