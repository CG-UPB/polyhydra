//
// Created by steffen on 11.10.21.
//

#ifndef VOLUMESHOS_FILEMANAGER_H
#define VOLUMESHOS_FILEMANAGER_H

#include <string>

namespace vOS
{
    class FileManager
    {
    public:
        static const char* resourcePath;

        static std::string loadAsString(const std::string& filePath);
    };
}


#endif //VOLUMESHOS_FILEMANAGER_H
