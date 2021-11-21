
#include "FileManager.h"

#include <fstream>
#include <sstream>

namespace vOS
{

    std::string FileManager::load_as_string(const std::filesystem::path& file_path, bool abs_path)
    {
        std::filesystem::path path = file_path;
        if (!abs_path)
        {
            path = FileManager::get_resource_path() / file_path;
        }
        std::ifstream in(path);
        std::stringstream buffer;
        buffer << in.rdbuf();
        in.close();
        return buffer.str();
    }

    std::filesystem::path& FileManager::get_resource_path()
    {
        static std::filesystem::path s_resourcePath("./res");
        return s_resourcePath;
    }
}