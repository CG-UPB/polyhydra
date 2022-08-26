#pragma once

#include "MeshObject.h"
#include "nlohmann/json.hpp"

namespace volumeshOS::Internal
{
    struct MeshSerializer
    {
        static void write_to_file(MeshObject& mesh, const std::string& file_name);

        static void read_from_file(MeshObject& mesh, const std::string& file_name);

        static std::string serialize(MeshObject& mesh);

        static void deserialize(MeshObject& mesh, const std::string& json_string);
    };
}
