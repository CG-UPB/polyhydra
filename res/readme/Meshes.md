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
After callaing the ```load``` function we get a so called ```VMesh```.
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
> It`s not necessary to call all of these. This is just an example of their usage.
```cpp
    auto mesh = load(&ovm_mesh);
    mesh.set_name(name);

    // these functions can be used to get insight of cells
    mesh.use_backface_culling(true);
    mesh.use_two_sided_lighting(false);

    // if true there is one color that can be set inside the viewer.
    // otherwise each cell/face has an own color value
    mesh.use_base_color(true);


    // example usage of mesh related functions
    mesh.set_position(position);
    mesh.set_scale(1.0f);
    mesh.use_scale_normalization(false);
    //mesh.set_rotation(...);

    // if using base color set color as follows
    mesh.set_color(std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});

    // otherwise set color for each cell
    for(auto cit : ovm_mesh.cells())
    {
        mesh.set_color(cit, std::array<float, 4>{0.0f, 1.0f, 0.0f, 0.5f});
        mesh.set_cell_size(0.97f);
    }

    // settings for phong lighting
    mesh.set_ambient(1.0f);
    mesh.set_diffuse(1.0f);
    mesh.set_specular(0.15f);
    mesh.set_specular_coefficient(8.0f);

    // settings for physically based lighting (PBR)
    mesh.set_metallic(0.15f);
    mesh.set_roughness(0.65f);

    // set lighting mode: Phong or PBR
    mesh.set_lighting_mode(LightingMode::PBR);

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
    //mesh.set_cell_rounding(0.0f);

    // set tessellation level (1-64) for bezier meshes:
    mesh.set_tessellation_level(1);
    mesh.set_visibility(true);
```


## Load/Save Configuration