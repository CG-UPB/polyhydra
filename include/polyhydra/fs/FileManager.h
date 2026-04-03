#pragma once

#include "polyhydra/polyhydraPCH.h"

namespace polyhydra::Internal
{
class FileManager
{
  public:
    static FS_NAMESPACE::path& get_resource_path();
    static std::string load_as_string(const FS_NAMESPACE::path& file_path, bool abs_path = false);
};
} // namespace polyhydra::Internal
