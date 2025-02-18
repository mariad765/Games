#pragma once

#include "components/simple_scene.h"



namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

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
        void InitTerrain(float size);
        void RenderTerrain(); // Add this line

    private:
        float translateX1; // For square 1
        float translateY1; // For square 1
        float translateX2; // For square 2
        float translateY2; // For square 3
        int index;
        int index2;

    protected:
        float cx, cy;
        float cx1, cy1;
        glm::vec2  point;
        glm::mat3 modelMatrix;
        glm::mat3 modelMatrixTank2;
        glm::mat3 modelMatrixLifeBar;
        glm::mat3 modelMatrixLifeBarTank2;
        glm::mat3 barrelModelMatrix;
        glm::mat3 barrelModelMatrixTank2;
        glm::mat3 projectileModelMatrix;
        glm::mat3 projectileModelMatrixTank2;
        glm::mat3 trajectoryModelMatrix;
        glm::mat3 trajectoryModelMatrixTank2;
     
        glm::vec3 tankTipPosition; // Poziția vârfului cercului tancului
        glm::vec3 lifeBarPosition;
        glm::vec3 lifeBarPositionTank2;

        std::string pointName;
        std::string pointNameT2;

        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        float tankX;
        float tankXTank2;
        float tankY;
        float tankYTank2;
        float bAngle;
        float bAngleTank2;
        float projectileCounterMine;
        float projectileCounterMineTank2;
        float lifeBarPercentage;
        float lifeBarPercentageTank2;
        float tankAngle;
        float tankAngleTank2;
        bool isMovingRight;
        bool isMovingRightTank2;
        bool isMovingLeft;
        bool isMovingLeftTank2;
        bool modifyTrajectory;
        bool modifyTrajectoryTank2;
        bool barrelToTheRight;
        bool barrelToTheRightTank2;
        bool barrelToTheLeft;
        bool barrelToTheLeftTank2;
        bool tankDied;
        bool tankDiedTank2;

        std::vector<glm::vec2> terrainPoints;
        std::vector<glm::vec2> projectilePoints;
        std::vector<glm::vec2> projectilePointsTank2;
  

    private:
 
        GLuint vao; // Vertex Array Object ID
     
    
        GLuint vbo; // Vertex Buffer Object ID
        // TODO(student): If you need any other class variables, define them here.

    };
}   // namespace m1
