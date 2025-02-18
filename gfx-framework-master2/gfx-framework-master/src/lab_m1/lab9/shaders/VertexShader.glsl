#version 330

/*Aceste variabile definesc atributele de intrare pentru fiecare punct 
(vertex) care vine din bufferul de obiecte 3D.

layout(location = 0) in vec3 v_position; — Coordonatele pozi?iei punctului în spa?iul obiectului (model space).
layout(location = 1) in vec3 v_normal; — Vectorul normal al punctului (util pentru iluminare,
                                        dar nu este folosit în acest shader).
layout(location = 2) in vec2 v_texture_coord; — Coordonatele texturii asociate fiec?rui punct.
layout(location = 3) in vec3 v_color; — Culoarea atribuit? punctului, dar în acest shader, aceast? 
                                        variabil? nu este folosit?.*/
// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

/*Acestea sunt matrici de transformare globale care sunt aplicate tuturor punctelor.

Model — Matricea care transform? punctul din spa?iul obiectului (model space) în spa?iul lumii (world space) 
            (transla?ie, rota?ie, scalare).
View — Matricea care transform? punctul din spa?iul lumii (world space)
        în spa?iul camerei (view space).
Projection — Matricea care transform? punctul din spa?iul camerei (view space) în spa?iul 
            de proiec?ie (clip space), unde punctele sunt aduse în domeniul de afi?are (-1 la 1).*/
// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
out vec2 texcoord;


void main()
{
    // TODO(student): Pass v_texture_coord as output to fragment shader
    /* Transmite coordonatele texturii (UV) primite ca input (v_texture_coord) c?tre ie?irea texcoord, 
        care va fi folosit? în fragment shader.*/
    texcoord = v_texture_coord;
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);  // se ia de la sfarsit la inceput
}