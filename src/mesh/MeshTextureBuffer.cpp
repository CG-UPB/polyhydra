#include "polyhydra/mesh/MeshTextureBuffer.h"

#include "polyhydra/mesh/MeshProperties.h"
#include <algorithm>
#include <utility>

namespace polyhydra::Internal
{

static constexpr GLsizeiptr MIN_TEXTURE_BUFFER_SIZE = 16;

namespace
{
void specify_texture_buffer(uint32_t texture_buffer, uint32_t texture, uint32_t binding, GLsizeiptr size,
                            const void* data)
{
    glBindBuffer(GL_TEXTURE_BUFFER, texture_buffer);
    glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glActiveTexture(binding);
    glBindTexture(GL_TEXTURE_BUFFER, texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, texture_buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
}
} // namespace

MeshTextureBuffer::MeshTextureBuffer(std::shared_ptr<Mesh> mesh, uint32_t binding) : m_mesh(std::move(mesh))
{
    m_binding = binding;

    glGenBuffers(1, &m_texture_buffer);
    glGenTextures(1, &m_texture);

    specify_texture_buffer(m_texture_buffer, m_texture, m_binding, MIN_TEXTURE_BUFFER_SIZE, nullptr);
}

MeshTextureBuffer::~MeshTextureBuffer()
{
    glDeleteBuffers(1, &m_texture_buffer);
    glDeleteTextures(1, &m_texture);
}

void MeshTextureBuffer::update_buffer(uint32_t size, const std::vector<float>& data) const
{
    specify_texture_buffer(m_texture_buffer, m_texture, m_binding,
                           std::max(static_cast<GLsizeiptr>(size), MIN_TEXTURE_BUFFER_SIZE), data.data());
}

uint32_t MeshTextureBuffer::get_binding() const
{
    return m_binding;
}

uint32_t MeshTextureBuffer::get_texture() const
{
    return m_texture;
}

} // namespace polyhydra::Internal
