# Meshes

We currently only support volume meshes created with OpenVolumeMesh
For further information about mesh creation see [OpenVolumeMesh](https://www.graphics.rwth-aachen.de/software/openvolumemesh/).

## Add a Mesh
There are several ways to load a mesh:
- with a saved ```.ovm``` file

```cpp
std:string path = "path_to_ovm_file"
auto mesh_a = load(path);
```

- with an ovm object
```cpp
OpenVolumeMesh::GeometricPolyhedralMeshV3d mesh;
// ... your code to create a volume mesh
auto mesh_b = load(mesh)
```

- with a file dialogue
```cpp
auto mesh_c = load_from_dialog("Title");
```

## VMesh
After callaing the `load` function we get a so called `VMesh`.
We can use this to call functions directly on the mesh.
So these two calls are identical:

```cpp
VMesh mesh = load("path");

mesh.set_name("name");
set_name(mesh, "name")
```

## Mesh Setup
Now we can start some setup for our mesh:
> **_NOTE:_**
> It`s not necessary to call all of these functions. This is just an example of their usage.

#### Load and Name
```cpp
auto mesh = load(&ovm_mesh);
mesh.set_name(name);
```
The name is displayed in the mesh list inside the viewer.

#### Color
```cpp
mesh.use_base_color(true);

std::array<float, 4> color = {0.0f, 1.0f, 0.0f, 1.0f};

mesh.set_color(color);

// ch = CellHandle, fh = FaceHandle, hfh = HalfFaceHandle
mesh.set_color(ch, color);
mesh.set_color(fh, color);
mesh.set_color(hfh, color);
```
Depending on the `use_base_color` value either one base color is displayed for the whole mesh or each Cell. By passing an OVM Handle object a color can be set directly to a cell, face or halfface.

#### Transformation
```cpp
std::array<float, 3> origin = {0.0f, 0.0f, 0.0f}
mesh.set_origin(origin)

std::array<float, 4> position = {0.0f, 1.0f, 0.0f, 0.5f}
mesh.set_position(position);

mesh.set_scale(1.0f);
mesh.use_scale_normalization(false);

mesh.set_rotation(90.0, 30.0, 10.0);
// mesh.set_rotation(rot_vector);
// mesh.set_rotation(anlge, axis_vector);
mesh.reset_rotation();
```
By default a mesh or rather ist point of gravity is placed in the worlds center and scaled to fit in front of the camera. 
The `origin` position can set multiple meshes in the same system (e.g a mesh splitted into two different files). Rotation and scaling is also done around the origin.
When setting `scale_normalization = false`  the mesh stays in its original size and is not scaled to fit in front of the camera.

#### Lighting
```cpp
// chose a lighting mode
mesh.set_lighting_mode(LightingMode::PBR);

// settings for phong lighting
mesh.set_ambient(1.0f);
mesh.set_diffuse(1.0f);
mesh.set_specular(0.15f);
mesh.set_specular_coefficient(8.0f);

// settings for physically based lighting (PBR)
mesh.set_metallic(0.15f);
mesh.set_roughness(0.65f);
```
We provide two different lighting modes: `LightingMode::PHONG` and `LightingMode::PBR`for a more realistic look. 
Each mesh can be lighted differently.

#### Cells

```cpp
// slice into the mesh; direction is given by view direction
mesh.set_slice_factor(0.0f);
mesh.set_slice_locked(false);
// peel the mesh; opens inner layers;
mesh.set_peel_level(0.0f);
// when true peeling is done from the inside to the outside
mesh.use_reverse_peeling(false);
// set cell size; moves cell vertices to its center
mesh.set_cell_size(1.0f);
// rounds the edges of a cell
mesh.set_cell_rounding(0.0f);
```
To get an insight of the meshes there are some functions to use. It is recommended to test its functionality inside the viewer.

#### Lines and Points
```cpp
mesh.render_cells(true);

mesh.render_lines(true);
mesh.set_line_width(1.5);

mesh.render_points(true);
mesh.set_point_size(0.5);
```
Cells, Lines and Points can be rendered in any combination.

#### Extras
```cpp
mesh.use_backface_culling(true);

mesh.use_two_sided_lighting(false);

// set tessellation level (1-64) for bezier meshes:
mesh.set_tessellation_level(1);

mesh.set_visibility(true);
```
- Sometimes it is desired to have a look inside cells e.g when making specific faces transparent. To prevent a weird look `use_backface_culling` can be turned off.
- `use_two_sided_lighting` set to "false" also lights the back of a mesh
- For Bezier meshes `tessellation_level` sets the degree of tessellation. A higher value results into a more 'round' curve, but also uses more rendering resources.
- It is also possible 

## Load/Save Configuration
```cpp
VMesh mesh = load("path/to/ovm/mesh");
mesh.load_configuration("path/to/saved/configuration");
mesh.save_configuration("path/to/configuration");
```
It is possible to save the settings of a mesh in a file and load it to any other mesh.
This enables fine-tuning inside the viewer and load it in code afterward.