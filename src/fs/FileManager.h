#pragma once

#include <string>
#include <filesystem>

namespace vOS
{
    class FileManager
    {
    public:
        static std::filesystem::path& getResourcePath();
        static std::string loadAsString(const std::filesystem::path& filePath);
    };
}