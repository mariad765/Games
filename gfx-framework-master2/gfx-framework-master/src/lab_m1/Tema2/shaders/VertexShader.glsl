#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// Output
out vec3 f_color;
out vec3 f_position;

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

// Smoothly blend between lakes and terrain
float enhanceWithLakes(float height) {
    float lakeThreshold = -0.15;   // Increased threshold to reduce lakes
    float lakeDepth = -0.3;        // Depth for lakes

    // Use smoothstep to create a smooth transition between the lake and terrain
    float smoothness = 0.05;       // Controls the smoothness of the lake boundary
    float transition = smoothstep(lakeThreshold - smoothness, lakeThreshold + smoothness, height);

    // Blend between the lake depth and the height using smoothstep
    if (height < lakeThreshold) {
        return mix(lakeDepth, height, transition); // Smooth lake transition
    }

    return height; // Leave other heights unchanged
}

void main() {
    // Frequency and persistence values control terrain detail
    float frequency = 0.26;       // Adjust to scale the noise pattern
    int octaves = 7;              // More octaves for detailed noise
    float persistence = 0.5;      // Balances noise contribution across octaves

    // Compute height using fractal noise
    float height = fractalNoise(v_position.xz * frequency, octaves, persistence);

    // Add lakes to depressions and ensure terrain stays at or above 0
    height = enhanceWithLakes(height);

    // Lower the terrain by adjusting the terrain offset
    float terrainOffset = 0.2f;   // Negative value to lower the terrain
    height += terrainOffset;      // Lower the terrain height

    // Adjust the vertex position
    vec3 modified_position = vec3(v_position.x, height, v_position.z);

    // Pass values to fragment shader
    f_color = v_color;
    f_position = modified_position;

    // Compute final vertex position
    gl_Position = Projection * View * Model * vec4(modified_position, 1.0);
}
