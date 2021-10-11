//
// Created by steffen on 11.10.21.
//

#include "FileManager.h"
#include <fstream>
#include <sstream>

namespace vOS
{
    const char* FileManager::resourcePath = "./res";

    std::string FileManager::loadAsString(const std::string& filePath)
    {
        std::stringstream path;
        path << FileManager::resourcePath << "/" << filePath;
        std::ifstream in(path.str());
        std::stringstream buffer;
        buffer << in.rdbuf();
        in.close();
        return buffer.str();
    }
}