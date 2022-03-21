
#include <glad/glad.h>

#include "stb_image.h"

#include "TextureIcon.h"
#include <exception>

namespace vOS
{
    TextureIcon::TextureIcon(const std::filesystem::path &file_path)
    {
        unsigned char* image_data = stbi_load(
                file_path.string().c_str(),
                &m_width,
                &m_height,
                &m_n_channels,
                0
        );

        if (image_data == nullptr)
        {
            std::string message = "Failed to load texture: ";
            message.append(file_path.string());
            throw std::runtime_error(message.c_str());
        }
        int src_format = m_n_channels == 4 ? GL_RGBA : GL_RGB;
        glGenTextures(1, &m_texture_id);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, src_format, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);
    }

    TextureIcon::~TextureIcon()
    {
        glDeleteTextures(1, &m_texture_id);
    }

    int TextureIcon::get_width() const
    {
        return m_width;
    }

    int TextureIcon::get_height() const
    {
        return m_height;
    }

    int TextureIcon::get_n_channels() const
    {
        return m_n_channels;
    }

    unsigned int TextureIcon::get_id() const
    {
        return m_texture_id;
    }
}