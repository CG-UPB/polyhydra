//
// Created by steffen on 11.10.21.
//

#include "FileManager.h"

#include <fstream>
#include <sstream>

namespace vOS
{

    std::string FileManager::loadAsString(const std::filesystem::path& filePath)
    {
        std::ifstream in(FileManager::getResourcePath() / filePath);
        std::stringstream buffer;
        buffer << in.rdbuf();
        in.close();
        return buffer.str();
    }

    std::filesystem::path& FileManager::getResourcePath()
    {
        static std::filesystem::path s_resourcePath("./res");
        return s_resourcePath;
    }
}