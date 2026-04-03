#pragma once

#include "polyhydra/polyhydraPCH.h"

namespace polyhydra::Internal
{
class TextureIcon
{
  public:
    explicit TextureIcon(const FS_NAMESPACE::path& file_path);
    ~TextureIcon();

    [[nodiscard]] int get_width() const;
    [[nodiscard]] int get_height() const;
    [[nodiscard]] int get_n_channels() const;
    [[nodiscard]] uint32_t get_id() const;

  private:
    int m_width = -1;
    int m_height = -1;
    int m_n_channels = -1;
    uint32_t m_texture_id = -1;
};
} // namespace polyhydra::Internal
