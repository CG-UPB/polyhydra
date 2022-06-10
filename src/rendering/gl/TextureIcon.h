#pragma once

#include "vospch.h"

namespace vOS
{
    class TextureIcon
    {
    public:

        explicit TextureIcon(const FS_NAMESPACE::path& file_path);
        ~TextureIcon();

        [[nodiscard]] int get_width() const;
        [[nodiscard]] int get_height() const;
        [[nodiscard]] int get_n_channels() const;
        [[nodiscard]] unsigned int get_id() const;

    private:

        int m_width = -1;
        int m_height = -1;
        int m_n_channels = -1;
        unsigned int m_texture_id = -1;
    };
}
