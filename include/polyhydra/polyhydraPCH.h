#pragma once

// OpenGL definitions
#include <glad/glad.h>

// STL
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#if defined(_MSC_VER)
// Make MS math.h define M_PI
#define _USE_MATH_DEFINES
#endif
#include <array>
#include <chrono>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>

// OpenVolumeMesh
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>

// GLM
#include <glm/gtx/transform.hpp>

// ImGui
#include <imgui.h>
#include <imgui_internal.h>

// Filesystem
#include FS_HEADER

// Log
#include "util/Log.h"

// Types
#include "util/Types.h"

// Other
#define PI 3.14159265358979323846f