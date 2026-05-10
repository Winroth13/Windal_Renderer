# Windal Renderer

Authors: Emil Winroth & Johannes Ledendal

A simple 3D-renderer using DirectX11 implementing a number of rendering techniques taught during the course 3D-programming 2.

## Controls

WASD - Movement

Space/Ctrl - Up & Down

Shift - x3 Movement speed

Esc - Deselect entity

T - Toggle translation gizmos

R - Toggle scaling gizmos

## Menus

In the top left corner, the following menus and sub-menus are available:

- File
  - Exit\
    Exits the program.

- View\
  Toggles ImGui windows for debugging and manipulating the scene.
  - Scene Hierarchy\
    Shows all entities in the scene and lets you select them by clicking on them.\
    Selecting an entity shows it both in the Inspector and shows the toggled gizmos in the world.\
    Invisible entities are shown in gray while visible ones are white.\
    Press Esc to deselect an entity.
    
  - Inspector\
    Shows detailed information about the selected entity and lets you manipulate it.
    
  - Diagnostics\
    Shows various statistics like frame rate and resolution.\
    Currently, changing the resoultion is not supported.

- Debug\
  Enables various debug views.

  - Lock Frustrum\
    Locks the frustum used for frustum culling to the camera's current location and orientation.

  - Show Icons\
    Draws icons on entities without a mesh, like lights, cube maps and particle systems.

  - Viewport\
    Various options for non-standard rendering.

    - Default\
      Standard rendering.

    - Wireframe\
      Renders wireframes of the geometry.\
      Does not show icons even if enabled.

    - G-Buffers\
      Renders the contents of the G-Buffers and results of the deffered rendering.\
      They are displayed in the order: Position, Normal, Color, Resulting Render\
      Neither icons nor particles systems are shown as they are rendered using forward rendering.

    - Bounding Boxes\
      Exactly like Default, but also draws the bounding boxes.\
      The following have bounding boxes: Model Entities, View Frustum, Frustum Culling Quadtree\
      Model bounding boxes are green if visible or red if culled. Useful if combined with the Lock Frutum-toggle.\
      The view frustum is drawn in blue.\
      Each level of the octree is drawn in a new color.

## Techniques

We have implemented the following techniques:

### Deferred Rendering

Rendering of geometry is done first with a geometry pre-pass to determine what is visible to the camera and then the light calculations are done in a seperate compute shader before being displayed on screen.

The buffers used for storing the geometry information can be seen using the toggle `Debug -> Viewport -> G-Buffers`.

### Shadow Mapping

Every light renders a depth map of what they can see, which is later used to determine if rendered geometry is in shadow. Shadow mapping can be paused on point lights to increase performance by not rendering depth every frame and they are paused by default. Unpause them by checking `Dynamic Shadows`.

The effect is widely visible throughout the scene.

### Particle System

Particles are processed as a cluster of points by a compute shader every frame before being drawn as billboards using a geometry shader in forward rendering.

Both the fires and their smoke are created using particle systems.

### Dynamic Cube Mapping

Renders the world from a set point in space to later use for reflections. Can be pausd like point light shadow maps unchecking `Dynamic`. is not paused by default.

There is a sphere using a dynamic cube map right above the starting location of the camera.

### Tessellation (Displacement Mapping)

Some materials use a displacement map to offset its geometry using a hull and domain shader. The amount of tessellation is determined by the distance to the camera, meanig they are more detailed when the camera is close and saves on computation when far away.

The inner brick walls of the Sponza-model use displacement mapping.

### Hierarchical Frustum Culling

Static models are stored in a spatial tree in the form of an quadtree. This quadtree is then used to efficiently determine what is visible to the rendering camera.

The entire Sponza-model is static. You can see the quadtree by enabling bounding box drawing using `Debug -> Viewport -> Bounding Boxes`.

### Normal Mapping

Most of the materials in the scene uses normal maps to offset their normals and create the illusion of more detailed geometry.

They are the most visible when looking at the Normal-buffer using `Debug -> Viewport -> G-Buffers`, looking at the cube-mapped sphere or the rotating brick cube, both right above the starting location of the camera.
