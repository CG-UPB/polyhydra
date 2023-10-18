# Installation

## CMake
VolumeshOS uses CMake for building. Integrate this into your CMake file:
```console
add_subdirectory("path/to/volumeshOS")

add_executable(target files..)
target_link_libraries(target volumeshOS_core)
```

## Dependencies
Before getting started some libraries are required to use volumeshOS:

Ubuntu 20.04:
```console
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```
```console
#OpenGL:
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```




