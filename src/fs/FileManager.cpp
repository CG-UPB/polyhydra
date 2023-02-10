
#include "FileManager.h"

namespace volumeshOS::Internal
{

    std::string FileManager::load_as_string(const FS_NAMESPACE::path& file_path, bool abs_path)
    {
        FS_NAMESPACE::path path = file_path;
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

    FS_NAMESPACE::path& FileManager::get_resource_path()
    {
        static FS_NAMESPACE::path s_resourcePath(VOS_RESOURCE_BASE_PATH);
        return s_resourcePath;
    }
}
