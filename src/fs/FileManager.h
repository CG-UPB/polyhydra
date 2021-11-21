#pragma once

#include <string>
#include <filesystem>

namespace vOS
{
    class FileManager
    {
    public:
        static std::filesystem::path& get_resource_path();
        static std::string load_as_string(const std::filesystem::path& file_path, bool abs_path = false);
    };
}