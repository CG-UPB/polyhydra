<h1><a href="https://gitcg.informatik.uni-osnabrueck.de/teaching/volumeshos"><img src="res/icons/logo.png" width="50" heigth="50" /></a><a href="https://gitcg.informatik.uni-osnabrueck.de/teaching/volumeshos">volumeshOS</a> - flexible volumetric mesh visualization framework</h1>

**volumeshOS** is a modern and lightweight framework to visualize volumetric mesh data. 
With ease of use in mind, **volumeshOS** has been created thoroughly to support the development
of algorithms for volumetric mesh *generation*, *manipulation* or *visualization*.

***
## How to install
#### CMake
VolumeshOS uses CMake for building. Integrate this into your CMake file:
```console
add_subdirectory("path/to/volumeshOS")

add_executable(target files..)
target_link_libraries(target volumeshOS_core)
```

#### Dependencies
Before getting started some libraries are required to use volumeshOS:

Ubuntu 20.04:
```console
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```
```console
#OpenGL:
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
## How to use
The simplest volumeshOS program that simply loads a mesh looks like this:
```c
#include "volumeshOS.h"

using namespace volumeshOS;

int main()
{
    load("path/to/ovm_file");
    open();
}
```

## Documentation
There are three ways to get further information
* [Documentation](res/readme/TableOfContent.md) 
* Examples inside the `examples` directory
* `volumeshOS.h` in the `src` directory for a full overview of every usable function

## Credits

ImGui          : https://github.com/ocornut/imgui

OpenVolumeMesh : https://www.graphics.rwth-aachen.de/software/openvolumemesh/

***

## License

* See [LICENSE](LICENSE)
