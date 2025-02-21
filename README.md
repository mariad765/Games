# 3D Drome Game Demo

Experience the excitement of the **3D Drome Game** with this demo video. 

## Description of project
### Vertex Shader and Fragment Shader Collaboration

The vertex shader in this project is used to generate realistic terrain geometry using procedural techniques, while the fragment shader refines the visual output with per-pixel color blending and texturing.

#### Vertex Shader: Procedural Terrain Generation
- **Attribute Processing:**  
  The shader takes in vertex attributes such as position (`v_position`), normal (`v_normal`), and color (`v_color`).

- **Procedural Noise for Height Calculation:**  
  - **Fractal Noise:**  
    The shader computes the terrain height using a fractal noise function. This function layers multiple octaves of 2D Perlin noise, controlled by parameters like frequency (set to 0.26), octaves (7 layers), and persistence (0.5), to generate detailed, natural variations in the terrain.
  - **Lake Enhancement:**  
    The `enhanceWithLakes` function modifies the calculated height by depressing areas below a certain threshold. This creates realistic lake depressions by blending the terrain height with a defined lake depth using smooth transitions.
  - **Terrain Offset:**  
    A fixed offset is applied to lower the overall terrain, ensuring the landscape has the desired base level.

- **Vertex Position Transformation:**  
  The modified vertex position, now incorporating the procedural height adjustments, is transformed by the Model, View, and Projection matrices to determine its final screen position (`gl_Position`).

- **Passing Data to the Fragment Shader:**  
  The vertex shader outputs both the modified position (`f_position`) and the original color (`f_color`) to the fragment shader. These values are interpolated across the rendered primitives for further per-pixel processing.

#### Fragment Shader: Fine-Tuning the Visuals
- **Color Blending and Texturing:**  
  The fragment shader takes the interpolated data from the vertex shader and applies detailed color blending based on the modified vertex position. This step is essential for adding realistic lighting, shading, and texture transitions across different terrain features like lakes, grasslands, and mountain peaks.

- **Collaborative Workflow:**  
  Together, the vertex shader and fragment shader create a pipeline:
  - The **vertex shader** calculates and adjusts the geometric structure of the terrain using procedural noise and height adjustments.
  - The **fragment shader** enhances these shapes by applying per-pixel effects that add depth and realism, such as smooth color transitions and detailed texturing.

 ### C++ Game Logic
 
Beyond the graphical rendering handled by OpenGL shaders, the game’s core logic is implemented in C++. This code is responsible for gameplay mechanics such as checkpoint management, collision detection, procedural terrain generation, and object placement.

### Checkpoint Management
- **Checkpoint States:**  
  An enumeration defines the different states a checkpoint can have:
  - `NOT_ENTERED`: The checkpoint has not been activated yet.
  - `ENTERED_GREEN`: The drone has passed through the green collision box, indicating a valid checkpoint pass.
  - `COMPLETED`: The checkpoint is finished (rendered in gray) and cannot be re-entered.
  - `DEACTIVATED`: The checkpoint is inactive, appears in shades of gray, and has no effect on gameplay.
  
  These states are stored in a vector, ensuring that each checkpoint’s progress is tracked throughout the game.

### Collision Detection and Bounding Volumes
- **Axis-Aligned Bounding Boxes (AABB):**  
  The `AABB` struct is used to define collision volumes for various objects (houses, trees, drone, and checkpoints). Each AABB is characterized by minimum and maximum 3D coordinates.
  
- **AABB Collision Function:**  
  The function `CheckAABBCollision` checks for overlaps along the x, y, and z axes between two AABB instances. This is critical for determining if objects are colliding in the game world.

- **Ground Collision:**  
  The `CheckGroundCollision` function ensures that the drone does not intersect the terrain. By sampling multiple points around the drone’s perimeter and comparing each against the terrain height (computed via procedural noise), it verifies that the drone remains above the ground.

### Procedural Terrain Generation
- **Perlin and Fractal Noise:**  
  The code utilizes functions like `fade`, `grad`, `perlinNoise`, and `fractalNoise` to generate smooth, natural-looking terrain. These noise functions combine multiple octaves of Perlin noise to create complex height variations.

- **Enhancing Terrain with Lakes:**  
  The function `enhanceWithLakes` depresses terrain heights below a certain threshold to form lake areas. It uses `smoothstep` and `mix` functions to create smooth transitions between water and land, adding realism to the landscape.

- **Terrain Height Calculation:**  
  The `GetTerrainHeight` function combines fractal noise and lake enhancements with a terrain offset. This determines the final height at any (x, z) coordinate, ensuring that objects are placed on appropriate terrain elevations.

### Object Placement and Utility Functions
- **Random Object Positioning:**  
  The `RandomObjectPosition` function generates random positions for objects like houses and trees. It uses `GetTerrainHeight` to ensure the object is placed on the terrain and checks with `IsPositionTooClose` to avoid overlapping with other objects.

- **Minimum Distance Check:**  
  The `IsPositionTooClose` function iterates through existing object positions to ensure that new objects are not spawned too near one another, maintaining spatial separation in the game world.

- **Vector Rotation:**  
  The `RotateAroundYAxis` function rotates a vector about the Y-axis, which is useful for orienting objects (for example, aligning the drone’s propellers) relative to the game scene.


### Collision detection


#### AABB Collision Testing

At the heart of the collision system is the use of Axis-Aligned Bounding Boxes (AABBs). Each object in the scene, whether it is the drone, a house, a tree, or a checkpoint, is enclosed in an AABB. These boxes simplify collision calculations by aligning with the coordinate axes.

##### Drone vs. Trees and Houses

For each tree, two AABBs are maintained (one for the trunk and one for the foliage), while each house has a single AABB. The drone's AABB is compared against these to detect any collisions, ensuring that the drone avoids obstacles in the environment.

##### Drone vs. Checkpoints

Checkpoints are more complex as they consist of multiple collision zones:

- **Stick AABB:** Represents the structural support of the checkpoint.
- **Green Circle AABB:** The correct passage zone where the drone must pass to register the checkpoint.
- **Yellow Circle AABB:** An auxiliary zone that, if hit when the checkpoint is not entered, triggers a collision.

The collision logic updates checkpoint states accordingly:

- If the checkpoint is **DEACTIVATED**, collisions with its yellow area are always detected.
- If the checkpoint is **NOT_ENTERED**, the drone can only pass if it collides with the green area; otherwise, a collision is registered.
- On successful passage through the green area, the checkpoint state changes (e.g., to **ENTERED_GREEN** and then **COMPLETED**), and the game advances to the next checkpoint target.

##### Ground Collision Detection

The system also checks if the drone is colliding with the ground. By sampling multiple points along the drone's perimeter and comparing each point's height with the corresponding terrain height, the game ensures that the drone does not inadvertently sink into or clip through the ground.



## Game Instructions

## Controls

### Camera Movement
- **Hold Right Mouse Button** to enable camera movement.
- **W** / **S** - Move the camera forward/backward.
- **A** / **D** - Move the camera left/right.
- **Q** / **E** - Move the camera up/down.

### Field of View and Projection
- **1** / **2** - Decrease/increase the field of view.
- **3** / **4** - Increase/decrease the projection width.
- **5** / **6** - Increase/decrease the projection height.
- **O** - Switch to orthographic projection.
- **P** - Switch to perspective projection.

### Drone Movement
- **Arrow Down** - Move the drone forward.
- **Arrow Up** - Move the drone backward.
- **Arrow Right** - Move the drone left.
- **Arrow Left** - Move the drone right.
- **U** - Move the drone upward.
- **H** - Move the drone downward.

### Drone Rotation
- **J** - Rotate the drone counterclockwise.
- **K** - Rotate the drone clockwise.

### Additional Features
- **T** - Toggle rendering of the camera target.

## Mouse Controls
- **Hold Right Mouse Button** and move the mouse to rotate the camera:
  - **Without CTRL**: First-person rotation.
  - **With CTRL**: Third-person rotation.

## Gameplay Mechanics
- Navigate the drone through obstacles like trees and houses.
- Pass through checkpoints correctly to progress.
- Avoid collisions with obstacles and the ground.
- Adjust camera and projection settings for better visibility.

Enjoy piloting the drone!
  

## Watch the Demo

[Demo for the 3D Drome Game](https://drive.google.com/file/d/1FUJPU4plDg61NeqXLyO-JpKXVa5TkZoW/view?usp=sharing)

*Note: The video is hosted on Google Drive. For the best viewing experience, ensure you have a stable internet connection.*
