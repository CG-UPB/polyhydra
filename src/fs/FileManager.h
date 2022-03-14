#pragma once

#include <string>
#include FS_HEADER

namespace vOS
{
    class FileManager
    {
    public:
        static FS_NAMESPACE::path& get_resource_path();
        static std::string load_as_string(const FS_NAMESPACE::path& file_path, bool abs_path = false);
    };
}
