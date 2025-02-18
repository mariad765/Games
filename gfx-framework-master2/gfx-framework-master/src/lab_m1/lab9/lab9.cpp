#include "lab_m1/lab9/lab9.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

glm::vec3 up_2;


Lab9::Lab9()
{
}


Lab9::~Lab9()
{
}


void Lab9::Init()
{/*Se define?te calea directorului unde sunt stocate fi?ierele de texturi.
      PATH_JOIN() combina calea directorului principal cu loca?ia specific? a 
      texturilor folosite în proiect.
      Aceast? cale va fi folosit? pentru a înc?rca diferite imagini care vor 
      fi aplicate ca texturi pe obiectele 3D.*/
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9", "textures");

    // Load textures
    {
        /*Pas 1: Se creeaz? un obiect Texture2D, care reprezint? o textur?.
          Pas 2: Se încarc? imaginea "grass_bilboard.png" de pe disc folosind Load2D()
          ?i se specific? c? textura va fi repetat? (GL_REPEAT) pe obiectul 3D.
          Pas 3: Textura este stocat? în mapTextures sub cheia "grass", astfel încât 
          s? poat? fi accesat? ulterior.*/
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "grass_bilboard.png").c_str(), GL_REPEAT);
        mapTextures["grass"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "crate.jpg").c_str(), GL_REPEAT);
        mapTextures["crate"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "earth.png").c_str(), GL_REPEAT);
        mapTextures["earth"] = texture;
    }

    {
        // aici lucram cu un subdirector
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo", "bamboo.png").c_str(), GL_REPEAT);
        mapTextures["bamboo"] = texture;
    }

    {
        /*Apeleaz? func?ia CreateRandomTexture(25, 25) pentru a crea o textura aleatorie de 25x25 pixeli.
          Rezultatul este o textur? 2D generat? din pixeli colora?i aleatoriu.
          Textura este apoi stocata în mapa mapTextures sub numele "random".
          Aceast? textur? poate fi folosit? pentru a textura suprafe?e sau obiecte 3D.*/
        mapTextures["random"] = CreateRandomTexture(25, 25);
    }

    // Load meshes
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

    {
        Mesh* mesh = new Mesh("bamboo");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo"), "bamboo.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a simple quad
    {
        vector<glm::vec3> vertices  /*Este un punct 3D definit prin coordonatele (x, y, z).
                                    Deoarece p?tratul este 2D, z = 0 pentru to?i vertec?ii.*/
        { /*Pozi?iile punctelor (col?urilor) p?tratului*/
            glm::vec3(0.5f,   0.5f, 0.0f),    // top right
            glm::vec3(0.5f,  -0.5f, 0.0f),    // bottom right
            glm::vec3(-0.5f, -0.5f, 0.0f),    // bottom left
            glm::vec3(-0.5f,  0.5f, 0.0f),    // top left
        };

        vector<glm::vec3> normals /*Este un vector normal pentru fiecare vertex.
                                    Normala reprezint? direc?ia perpendicular? pe suprafa??, 
                                    folosit? în calculele de iluminare.*/
        {
            glm::vec3(0, 1, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0)
        };

        vector<glm::vec2> textureCoords
        { /*Acesta define?te coordonatele de textur? (u, v) pentru fiecare vertex.
            Acestea sunt pozi?ii relative în textura care este aplicat? pe suprafa??.
            Valorile (0,0) ? (1,1) sunt folosite pentru a acoperi întreaga textur? pe suprafa??.*/
            // TODO(student): Complete texture coordinates for the square
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f)

        };

        vector<unsigned int> indices =
        { /*Indec?ii specific? ordinea vertec?ilor pentru fiecare triunghi.
            Fiecare p?trat este format din 2 triunghiuri.*/
            0, 1, 3,
            1, 2, 3
        };

        Mesh* mesh = new Mesh("square");
        mesh->InitFromData(vertices, normals, textureCoords, indices);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        /*Se creeaz? un nou obiect de tip Shader, iar numele s?u este "LabShader".
        Acest shader va fi folosit pentru a controla modul în care sunt afi?ate obiectele pe ecran, 
        prin calcularea pozi?iei fiec?rui vertex ?i a culorii fiec?rui pixel (fragment).
        Vertex Shader – Procesarea pozi?iei fiec?rui vertex.
        Fragment Shader – Calcularea culorii fiec?rui pixel.
        */
        Shader* shader = new Shader("LabShader");
        /*Se încarc? fi?ierul VertexShader.glsl din directorul shaders ?i se ata?eaz? la obiectul shader.*/
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        /*Compilarea celor dou? fi?iere GLSL ?i crearea unui program de shader care le une?te.
        Vertex Shader ?i Fragment Shader sunt legate împreun? pentru a forma un program complet.*/
        shaders[shader->GetName()] = shader;
    }
    /*mix(texture1, texture2, bonus_value_mix) va amesteca textura 1 ?i 
    textura 2 în propor?ie de bonus_value_mix.*/
    bonus_value_mix = 0;
    /* Folosit pentru a controla efectul de mixare între dou? texturi*/
    bonus_value_translate_1 = 0; // translatie pe o axa
    bonus_value_translate_2 = 0;
}


void Lab9::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab9::Update(float deltaTimeSeconds)
{
    time = Engine::GetElapsedTime();

    // TODO(student): Choose an object and add a second texture to it.
    // For example, for the sphere, you can have the "earth" texture
    // and the "random" texture, and you will use the `mix` function
    // in the fragment shader to mix these two textures.

    {
        /*mix(texture1, texture2, bonus_value_mix) va amesteca textura 1 
        ?i textura 2 în propor?ie de bonus_value_mix.*/
        bonus_value_mix = 1; // cube
        bonus_value_translate_2 = 0; // translate_1 sphere
        bonus_value_translate_1 = 0; // translate_2 grass

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 1.5f, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(0.0f), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["random"], mapTextures["earth"]);
    }

    // However, you may have the unpleasant surprise that the "random"
    // texture now appears onto all objects in the scene, even though
    // you are only passing the second texture for a single object!
    // Why does this happen? How can you solve it?
    {
        bonus_value_mix = 0;
        bonus_value_translate_1 = 0;
        bonus_value_translate_2 = 1;

        glm::mat4 modelMatrix = glm::mat4(1);
        /*modelMatrix define?te pozi?ia, rota?ia ?i scara cubului.*/
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 1, -3));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, mapTextures["earth"]);
    }

    {
        bonus_value_mix = 0;
        bonus_value_translate_2 = 0;
        bonus_value_translate_1 = 0;
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.5f, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["crate"]);
    }

    {
        bonus_value_mix = 0;
        bonus_value_translate_1 = 0;
        bonus_value_translate_2 = 0;

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, 0.5f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(75.0f), glm::vec3(1, 1, 0));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["random"]);
    }

    {
        bonus_value_mix = 0;
        bonus_value_translate_2 = 0; // nu se roteste
        bonus_value_translate_1 = 1; // miscare la iarba

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
        //modelMatrix = glm::translate(modelMatrix, up_2);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        // rotatia dupa camera;
        modelMatrix = glm::rotate(modelMatrix, GetSceneCamera()->m_transform->GetRotationEulerRad().y, glm::vec3(0, 1, 0));
        //glm::lookAt();
        RenderSimpleMesh(meshes["square"], shaders["LabShader"], modelMatrix, mapTextures["grass"]);
    }

    {
        bonus_value_mix = 0;
        bonus_value_translate_2 = 0;
        bonus_value_translate_1 = 0;

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, -0.5f, -3));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderSimpleMesh(meshes["bamboo"], shaders["LabShader"], modelMatrix, mapTextures["bamboo"]);
    }
}


void Lab9::FrameEnd()
{
    DrawCoordinateSystem();
}


void Lab9::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
    /*Acest bloc verific? dac? obiectele mesh, shader ?i dac? programul shader este 
    valid (shader->GetProgramID() returneaz? un ID valid al programului de shader). */
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // TODO(student): Set any other shader uniforms that you need
    /*Uniformele în OpenGL sunt variabile care sunt trimise de aplica?ia principal?
    (CPU) c?tre shader-ul (GPU) pentru a influen?a procesul de renderizare al obiectelor
    3D. Ele sunt folosite pentru a trimite valori care nu se schimb? de la un vertex sau
    fragment la altul pe parcursul unui apel de desenare (de exemplu, timp, matrice de 
    transformare, valori de textur?, etc.). Acestea sunt transmise o singur? dat? pe
    întreaga durat? a unui apel de desenare ?i sunt accesibile pentru toate instan?ele 
    shader-ului care ruleaz? (to?i vertexii sau fragmentele procesate).*/

    /*Acest uniform poate fi folosit pentru a activa un efect sau un comportament vizual special,
    amestecarea a dou? texturi sau schimbarea unui parametru de shader. 
    bonus_value_mix este o valoare întreag? care va fi folosit? pentru a controla acest efect.*/

    int mixed_location = glGetUniformLocation(shader->program, "mixed");
    glUniform1i(mixed_location, bonus_value_mix);

    /*Uniformul time este utilizat pentru a transmite timpul scurs de la începutul aplica?iei la shader. */
    int location_time = glGetUniformLocation(shader->GetProgramID(), "time");
    glUniform1f(location_time, time);

    /*translate1_location este loca?ia uniformului trans1 în programul shader.
    bonus_value_translate_2 este valoarea care este transmis? la uniformul trans1.
    Este o variabil? de tip int, care  controleaza un parametru legat de translare*/
    int translate1_location = glGetUniformLocation(shader->program, "trans1");
    glUniform1i(translate1_location, bonus_value_translate_2);

    /**/
    int rotate_location2 = glGetUniformLocation(shader->program, "trans2");
    glUniform1i(rotate_location2, bonus_value_translate_1);


    if (texture1) /*Verific? dac? exist? o textur? texture1 valid? (adica dac? este alocat? sau înc?rcat? corect).*/
    {
        /*Activeaz? loca?ia texturii 0. OpenGL poate gestiona mai multe texturi simultan, iar fiecare textur? este 
        asociat? cu o loca?ie. În acest caz, activ?m loca?ia GL_TEXTURE0, care este prima loca?ie de textur? disponibil?.*/
        // TODO(student): Do these:
        // - activate texture location 0
        glActiveTexture(GL_TEXTURE0);
        // - bind the texture1 ID
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        // - send theuniform value
        /*Trimite valoarea 0 (locatia activ? pentru texture1) în shader la uniformul numit texture_1. 
        Shader-ul va utiliza aceast? valoare pentru a ?ti de unde sa obtina texturile pentru acest obiect.*/
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        glActiveTexture(GL_TEXTURE1);
        // - bind the texture2 ID
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        // - send the uniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


Texture2D* Lab9::CreateRandomTexture(unsigned int width, unsigned int height)
{
    GLuint textureID = 0;
    unsigned int channels = 3;
    unsigned int size = width * height * channels;
    unsigned char* data = new unsigned char[size];
    /*extureID: Se iniaializeaza la 0, iar mai târziu va fi folosit pentru 
    a stoca ID-ul texturii generate.
    channels = 3: Textura va folosi 3 canale (rosu, verde si albastru), ceea ce
    înseamna ca fiecare pixel va fi reprezentat de 3 valori.
   size = width * height * channels: Calculam dimensiunea totala a texturii
   (numarul de pixeli înmulait cu numarul de canale per pixel).
   data = new unsigned char[size]: Alocam un buffer pentru a stoca datele texturii 
   (în acest caz, valori aleatoare pentru fiecare canal de culoare).*/

    // TODO(student): Generate random texture data
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 255;
    }

    // TODO(student): Generate and bind the new texture ID
    glGenTextures(1, &textureID); /*Se genereaza un ID pentru o textura si îl stocam în textureID.*/
    glBindTexture(GL_TEXTURE_2D, textureID); /*Leaga textura la contextul curent de OpenGL, 
                                            astfel încât urmatoarele operatiuni sa afecteze aceasta textura.*/

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }
    /*GLEW_EXT_texture_filter_anisotropic: Verifica daca extensia de filtrare anizotrop? este disponibila pe platforma.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4): Daca este disponibila extensia, se seteaz? factorul 
    maxim de anizotropie pentru textura. Un factor de 4 înseamna ca texturile vor fi filtrate cu un grad de anizotropie de
    4, ceea ce poate îmbun?t??i calitatea texturilor la unghiuri mari.*/
    // TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
    GLint wrapping_mode = GL_REPEAT;
    /*Se seteaza modul de înfasurare a texturii pe axele S (orizontala) ?i T (verticala) la GL_REPEAT.
    Acest lucru înseamna ca texturile se vor repeta atunci când coordonatele de textura ies din intervalul [0, 1].*/


    /*Aceste set?ri controleaz? modul în care sunt interpolate textele la mic?orare sau m?rire.*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode);

    /*Seteaza modul de înfasurare atât pe axa S cât si pe axa T.*/

    /*GLint min_filter = GL_NEAREST;
    GLint mag_filter = GL_LINEAR_MIPMAP_LINEAR;*/



    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    /*Aceasta încarc? datele aleatorii generate în memoria GPU pentru a fi utilizate ca textur?.*/
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TODO(student): Generate texture mip-maps
    /*Aceasta functie creeaza mipmaps pentru textura. Mipmaps sunt imagini de dimensiuni mai
    mici ale aceleasi texturi, care sunt folosite pentru a îmbunatati performanta si calitatea
    vizuala atunci când texturile sunt vizualizate de la distanta (în loc sa foloseasca aceleasi
    dimensiuni de textura pentru toate distantele). Mipmaps sunt generate pentru texturi de tip 
    2D (în acest caz).*/
    glGenerateMipmap(GL_TEXTURE_2D);
    //glGenerateMipmap(GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

    //7 !!!!!!!!
    /* Acesta controleaza filtrul folosit pentru minificare (când texturile sunt micsorate pe ecran). 
    Setarea GL_LINEAR înseamna ca texturile vor fi interpolate liniar între textele din mipmaps pentru
    a obtine un rezultat mai neted.*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    /*GL_TEXTURE_MAG_FILTER: Acesta controleaza filtrul folosit pentru magnificatie (când texturile sunt
    marite pe ecran). GL_NEAREST înseamna ca texturile vor fi scalate folosind cel mai apropiat pixel,
    fara a aplica interpolare, ceea ce poate duce la o imagine mai putin neteda.*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);*/

    CheckOpenGLError();

    // Save the texture into a wrapper Texture2D class for using easier later during rendering phase
    Texture2D* texture = new Texture2D();
    texture->Init(textureID, width, height, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab9::OnInputUpdate(float deltaTime, int mods)
{
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
    }
}


void Lab9::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Lab9::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab9::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab9::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab9::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab9::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab9::OnWindowResize(int width, int height)
{
}