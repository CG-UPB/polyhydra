//
// Created by steffen on 11.10.21.
//

#ifndef VOLUMESHOS_FILEMANAGER_H
#define VOLUMESHOS_FILEMANAGER_H

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


#endif //VOLUMESHOS_FILEMANAGER_H
