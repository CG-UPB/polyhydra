#pragma once

#include "MeshObject.h"
#include "nlohmann/json.hpp"

namespace volumeshOS::Internal
{
    class MeshSerializer
    {
    public:

        std::string serialize(const MeshObject& mesh);

        void deserialize(const MeshObject& mesh, const std::string& data);
    };
}
