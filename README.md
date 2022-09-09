# **volumeshOS**

VolumeshOS is a lightweight framework to observe 3-dimensional meshes ...

...

ImGui          : https://github.com/ocornut/imgui 

OpenVolumeMesh : https://www.graphics.rwth-aachen.de/software/openvolumemesh/

## Content
- [How to Install](#how-to-install)
- [Features](#features)
- [How to Use](#how-to-use)
- [Credits](#credits)
- [License](#license)


## **How to Install**

### Dependencies
Before getting started some libraries are required to use volumeshOS:

Ubuntu 20.04:
```console
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```
```console
#OpenGL:
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```

### Cmake
```console
add_subdirectory(path/to/volumeshOS)

add_executable(target files..)
target_link_libraries(target volumeshOS_core)
```

## **Features**
VolumeshOS includes a variety of features. Most of them support the exploration of the inside of a mesh or regulate the visual impression. 

### Rendering Modes

### Mesh Related Features

#### Transformation
Every mesh can be translated, scaled or rotated. 

#### Slicing
To slice into a mesh a plane is defined by the camera front. Cells visibility depends if their center lies in front of the plane or behind it. It is also possible to lock a certain direction to keep moving the camera.

#### Peeling
Another way to get into inner layers is by peeling outer layers. By turning on Transparency cells transition from opaque to transparent. The direction of peeling get be reversed from outside->inside to inside->outside. It is recommended to use this with e.g slicing.

#### Cell Size
Different cell sizes enable a look between the cells and reveal hidden faces.

#### Edge Roundings
To set volumetric meshes apart from surface meshes the threedimensional impression can be improved by rounding the edges of cells.

#### Digging
When active, selecting a cell turns it invisible temporarily.

#### Isolation
When active, selecting a cell turns every other cell invisible.

### Graphical Features
In addition to the listed mesh related features there are some ways to stress three-dimensionality.

#### Shadows
Shadows adds more depth to the scene. 

#### Ambient Occlusion
For even more realistic shadows ambient occlusion can be applied. Especially the combination with rounded cells gives a nice look.

#### Transparency
VolumeshOS provides two ways to portray transparency. [Depth peeling](https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.18.9286&rep=rep1&type=pdf) gives a way more realistic and correct transparency, but less performant. A better performing but not that realistc approach is [WBOIT](http://casual-effects.blogspot.com/2014/03/weighted-blended-order-independent.html).

### Other

#### Camera 
The camera has two different modes: Orbit and Fly. In Orbit mode the camera circles around its target. In Fly mode you can fly through the scene using Hotkeys.

#### Selection
When active, a certain mode can be chosen, so either vertices, edges, faces, cells or everything is selectable. This is used for digging and isolate cells and enables focusing a specific cell by double clicking on it.

#### Ground
There is also a ground plane where shadows can be projected on. The User has the choice of a grid, a plane or both. 

#### Shapes
A load of shapes (box, cylinder, sphere, cone, arrow) can be set into the scene. 

## **How to Use**
Getting an overview of the features is recommended to get an idea what kind of options can be set.
So in addition to some maniging function, every parameter adjustable in the gui can be set or get.

The simplest program looks like this:

```cpp
#include "volumeshOS.h"

using namespace volumeshOS;

int main()
{
    load("path/to/ovm_file");
    open();
}
```


## **Credits**


## **License**
