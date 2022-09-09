# volumeshOS


ImGui          : https://github.com/ocornut/imgui 

OpenVolumeMesh : https://www.graphics.rwth-aachen.de/software/openvolumemesh/

## Content
- [How to Install](#how-to-install)
- [Features](#features)
- [How to Use](#how-to-use)
- [Credits](#credits)
- [License](#license)


## How to Install

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

add_executable(your_target your_files..)
target_link_libraries(target volumeshOS_core)
```

## Features

### Mesh Related Features

#### Transformation
#### Slicing
#### Peeling
#### Cell Size
#### Edge Roundings
#### Digging
#### Isolation

### Graphical Features

#### Shadows
#### Ambient Occlusion
#### Transparency

### Other

#### Selection
#### Ground


## How to Use

### 


## Credits


## License
