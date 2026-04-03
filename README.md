<h1><img src="res/icons/logo.png" width="50" heigth="50" />Polyhydra - flexible volumetric mesh visualization framework</h1>

**Polyhydra** is a modern and lightweight framework to visualize volumetric mesh data.
With ease of use in mind, **Polyhydra** has been created thoroughly to support the development
of algorithms for volumetric mesh *generation*, *manipulation* or *visualization*.

***
## How to install
#### CMake
Polyhydra uses CMake for building. Integrate this into your CMake file:
```console
add_subdirectory("path/to/polyhydra")

add_executable(target files..)
target_link_libraries(target polyhydra::polyhydra)
```

#### Dependencies
Before getting started some libraries are required to use Polyhydra:

For instance, on Ubuntu 20.04:
```console
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```
...and possibly for OpenGL:
```console
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```

## How to use
The simplest Polyhydra program that simply loads a mesh looks like this:
```c
#include "polyhydra.h"

using namespace polyhydra;

int main()
{
    load("path/to/ovm_file.ovm");
    open();
}
```

## Documentation
There are three ways to get further information
* [Documentation](res/readme/TableOfContent.md)
* Examples inside the `examples` directory
* `polyhydra.h` in the `src` directory for a full overview of every usable function

## Credits

ImGui          : https://github.com/ocornut/imgui

OpenVolumeMesh : https://www.graphics.rwth-aachen.de/software/openvolumemesh/

***

## License

* See [LICENSE](LICENSE)
