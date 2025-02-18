#version 330

// Input
in vec3 f_color;
in vec3 f_position;

// Output
layout(location = 0) out vec4 out_color;

// Uniforms
uniform float Time;  // You can use time for animated textures if needed

// Fade function for smoother interpolation
float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

// Gradient function for Perlin noise
float grad(int hash, vec2 p) {
    int h = hash & 3; // 4 possible gradient directions
    float u = h < 2 ? p.x : p.y;
    float v = h < 2 ? p.y : p.x;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// 2D Perlin noise function
float perlinNoise(vec2 st) {
    vec2 i = floor(st); // Grid cell
    vec2 f = fract(st); // Local coordinates in cell

    // Hash for each corner
    int h00 = int(mod(dot(i, vec2(127.1, 311.7)), 256.0));
    int h10 = int(mod(dot(i + vec2(1.0, 0.0), vec2(127.1, 311.7)), 256.0));
    int h01 = int(mod(dot(i + vec2(0.0, 1.0), vec2(127.1, 311.7)), 256.0));
    int h11 = int(mod(dot(i + vec2(1.0, 1.0), vec2(127.1, 311.7)), 256.0));

    // Gradient contributions
    float g00 = grad(h00, f);
    float g10 = grad(h10, f - vec2(1.0, 0.0));
    float g01 = grad(h01, f - vec2(0.0, 1.0));
    float g11 = grad(h11, f - vec2(1.0, 1.0));

    // Fade curves
    vec2 fadeXY = vec2(fade(f.x), fade(f.y));

    // Bilinear interpolation
    float n0 = mix(g00, g10, fadeXY.x);
    float n1 = mix(g01, g11, fadeXY.x);
    return mix(n0, n1, fadeXY.y);
}

// Fractal noise function with multiple octaves
float fractalNoise(vec2 st, int octaves, float persistence) {
    float total = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxValue = 0.0; // Normalization factor

    for (int i = 0; i < octaves; i++) {
        total += perlinNoise(st * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }

    return total / maxValue; // Normalize result to 0-1 range
}

// Function to add texture to the lakes
vec3 lakeTexture(float yPosition) {
    float lakeNoise = fractalNoise(vec2(f_position.x, f_position.z) * 0.2, 3, 0.5); // Apply noise to lake area
    return mix(vec3(0.0, 0.4, 0.7), vec3(0.0, 0.5, 0.5), lakeNoise);  // Blend deeper blue and teal for lakes
}

// Enhance terrain color based on height
void main()
{
    // Threshold for lake (you can adjust this value)
    float lake_threshold = -0.08;  // Assumes lakes are at lower y positions (below -0.1)
    
    // Default color (land)
    float noise_value = f_position.y / 2.0;  // Normalize height value to [0, 1]

    // Natural colors for different terrains
    vec3 color1 = vec3(0.0, 0.3, 0.1);    // Darker green for forested areas
    vec3 color2 = vec3(0.0, 0.3, 0.1);    // Rich brown for mid-ground (mountain base)
    vec3 color3 = vec3(0.0, 0.4, 0.7);    // Deeper blue for lakes
    vec3 color4 = vec3(0.0, 0.6, 0.5);    // Teal for shallow water or wetlands
    vec3 color5 = vec3(0.35, 0.16, 0.14); // Lighter brown for higher mountain peaks
    vec3 color7 = vec3(0.13, 0.37, 0.31); // Lighter green for lower areas (grasslands or fields)


    // Adjusting terrain colors based on height
    vec3 terrain_color;

    // Below lake threshold, add texture to lakes
    if (f_position.y < lake_threshold) {
        terrain_color = lakeTexture(f_position.y);
    } 
    // Areas between 0 and lake threshold use lighter blue (shallow water)
    else if (f_position.y < 0.0) {
        // Smooth transition between shallow water and lake
        float transitionFactor = smoothstep(lake_threshold, 0.0, f_position.y);
        terrain_color = mix(color3, color4, transitionFactor);
    }
    else {
        // For lower than dark green areas (grasslands or fields)
        if (f_position.y < 0.5) {
            float lowGreenFactor = smoothstep(0.0, 0.5, f_position.y); // Transition from dark green to lighter green
            terrain_color = mix(color7, color1, lowGreenFactor); // Blend with lighter green
        }
        // Mountain base with rich brown for mid-ground
        else {
           
            // Smoothly interpolate between color1 and color2 based on noise_value
            float smoothNoiseValue = smoothstep(0.0, 1.0, noise_value); // Apply smoothstep to the noise value for smoother transition
            terrain_color = mix(color1, color2, smoothNoiseValue);

            // Apply a smooth transition to lighter mountain peak colors
            float peak_threshold = 0.4;  // Height where the peak begins
            float peak_value = smoothstep(peak_threshold, peak_threshold + 0.5, f_position.y);  // Smoothstep for transition to peak
            terrain_color = mix(terrain_color, color5, peak_value); // Blend for varied terrain

        }
    }

    // Assign final color
    out_color = vec4(terrain_color, 1.0); 
}
