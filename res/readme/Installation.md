[Table of Content](TableOfContent.md)
***

# Installation

## CMake
Polyhydra uses CMake for building. Integrate this into your CMake file:
```console
add_subdirectory("path/to/polyhydra")

add_executable(target files..)
target_link_libraries(target polyhydra::polyhydra)
```

## Dependencies
Before getting started some libraries are required to use polyhydra:

For instance, on Ubuntu 20.04:
```console
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```
...and possibly for OpenGL:
```console
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
