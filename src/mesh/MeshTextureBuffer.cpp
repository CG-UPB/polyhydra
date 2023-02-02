#include "MeshTextureBuffer.h"

#include <utility>
#include "mesh/MeshProperties.h"


namespace volumeshOS::Internal
{

    MeshTextureBuffer::MeshTextureBuffer(std::shared_ptr<Mesh>  mesh, uint32_t binding)
            : m_mesh(std::move(mesh))
    {
        m_binding = binding;

        // Create Texture Buffer
        glGenBuffers(1, &m_texture_buffer);
        // Create Texture
        glGenTextures(1, &m_texture);

        glBindBuffer(GL_TEXTURE_BUFFER, m_texture_buffer);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        glActiveTexture(m_binding);
        glBindTexture(GL_TEXTURE_BUFFER, m_texture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, m_texture_buffer);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

    }

    MeshTextureBuffer::~MeshTextureBuffer()
    {
        glDeleteBuffers(1, &m_texture_buffer);
        glDeleteTextures(1, &m_texture);
    }


    void MeshTextureBuffer::update_buffer(uint32_t size, const std::vector<float>& data) const
    {
        glBindBuffer(GL_TEXTURE_BUFFER, m_texture_buffer);
        glBufferData(GL_TEXTURE_BUFFER, size, data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    uint32_t MeshTextureBuffer::get_binding() const
    {
        return m_binding;
    }

    uint32_t MeshTextureBuffer::get_texture() const
    {
        return m_texture;
    }

}