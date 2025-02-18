#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Lab1 : public gfxc::SimpleScene
    {
    public:
        Lab1();
        ~Lab1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void  OnKeyHOld(int key, int mods, float deltaTime);

        // TODO(student): Class variables go here
    private:
        glm::vec3 clearColor; // Current background color
        std::vector<glm::vec3> colors; // List of colors to cycle through
        int currentColorIndex = 0; // Index to track the current color
        glm::vec3 position; // Pozi?ia obiectului
        std::vector<std::string> meshes_list; // Lista meshe-urilor
        int currentMeshIndex = 0; // Indexul meshe-ului curent
    };
}   // namespace m1
