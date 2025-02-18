#version 330

// Input
in vec2 texcoord;
/*Acesta reprezint? coordonatele de texturare pentru fiecare pixel, care sunt trimise din vertex shader.*/

// Uniform properties
/*Acestea sunt cele dou? texturi care vor fi folosite pentru calcularea culorii finale.*/
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
/*Un uniform care controleaz? modul în care cele dou? texturi sunt combinate */
uniform int mixed;
/* Aceste uniforme sunt folosite pentru a aplica transform?ri pe coordonatele texturii*/
uniform int trans1;
uniform int trans2;
/* Aceast? uniform? reprezint? timpul curent al jocului, care este folosit pentru a anima texturile.*/
uniform float time;



// Output
// Aceasta reprezinta culoarea finala a pixelului, care va fi trimisa catre framebuffer.
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    //out_color = vec4(1);
    
    vec2 new_coord = texcoord;
    // Apply transformations to new_coord
    /*Se creeaz? o copie a coordonatelor de texturare, new_coord.
      Daca trans1 este activ (valoare 1), se adauga valoarea time la coordonata X a texturii,
      ceea ce face ca textura sa se deplaseze pe orizontala în func?ie de timpul curent. Aceasta
      poate produce un efect de mi?care a texturii.
      Dac? trans2 este activ (valoare 1), se adauga o valoare constanta la coordonata Y a texturii, 
      mutând astfel textura pe vertical?.*/

    if(trans1 == 1){
        new_coord.x += time;
    }

    if (trans2 == 1) {
        new_coord.y += 30.0f;
    }
  /*Se foloseste functia texture2D pentru a obtine culorile din cele doua texturi la coordonatele new_coord.*/
    vec4 color1 = texture2D(texture_1, new_coord);   
    vec4 color2 = texture2D(texture_2, new_coord); 
    
    vec4 color;

    /*Daca mixed == 1, se face un amestec între color1 si color2 folosind functia mix.
    Amestecul este facut la jumatate (0.5f).
    Dac? alfa (color.a) este mai mica sau egal? cu 0.5, se foloseste discard, ceea ce înseamn? 
    c? pixelul respectiv nu va fi desenat (sau va fi complet transparent).
    Dac? mixed != 1, se folose?te doar culoarea din color1. De asemenea, dac? alfa este mic?, 
    pixelul este abandonat (acesta devine transparent).*/

    if (mixed == 1) {
        color = mix(color1, color2, 0.5f); 
        if (color.a <= 0.5f) {
        discard;
    }
    } else {
        color = color1;
        if (color.a <= 0.5f) { // alpha/discard
        discard;
    }
    }

    // Set the final color
	out_color = color;



}