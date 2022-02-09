
#include <cstdio>
#include "glad/glad.h"
#include "PrePassFrameBufferObject.h"

namespace vOS {

    PrePassFrameBufferObject::PrePassFrameBufferObject(int width, int height)
    {
        init(width, height);
    }

    PrePassFrameBufferObject::~PrePassFrameBufferObject()
    {
        clean_up();
    }

    unsigned int PrePassFrameBufferObject::create_framebuffer()
    {
        unsigned int fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        GLenum buffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers);
        return fbo;
    }

    void PrePassFrameBufferObject::resize(int width, int height)
    {
        clean_up();
        init(width, height);
    }

    void PrePassFrameBufferObject::bind()
    {
        glGetIntegerv(GL_VIEWPORT, m_previousViewPort);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previousFrameBufferID);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
        glViewport(0, 0, m_width, m_height);
    }

    void PrePassFrameBufferObject::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_previousFrameBufferID);
        glViewport(m_previousViewPort[0], m_previousViewPort[1], m_previousViewPort[2], m_previousViewPort[3]);
    }

    void PrePassFrameBufferObject::init(int width, int height)
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previousFrameBufferID);
        m_width = width;
        m_height = height;
        m_frameBufferID = create_framebuffer();
        m_depth_texture = create_depth_texture();
        m_normal_texture = create_color_attachment(GL_COLOR_ATTACHMENT0);
        m_position_texture = create_color_attachment(GL_COLOR_ATTACHMENT1);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "Error: %u\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PrePassFrameBufferObject::clean_up()
    {
        glDeleteFramebuffers(1, &m_frameBufferID);
        glDeleteTextures(1, &m_depth_texture);
    }

    unsigned int PrePassFrameBufferObject::create_depth_texture()
    {
        unsigned int tex[1];
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, tex[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex[0], 0);
        return tex[0];
    }

    unsigned int PrePassFrameBufferObject::get_depth_texture_id() const
    {
        return m_depth_texture;
    }

    unsigned int PrePassFrameBufferObject::create_color_attachment(unsigned int attachment)
    {
        unsigned int tex[1];
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, tex[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex[0], 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        return tex[0];
    }

    unsigned int PrePassFrameBufferObject::get_normal_texture_id() const
    {
        return m_normal_texture;
    }

    unsigned int PrePassFrameBufferObject::get_position_texture_id() const
    {
        return m_position_texture;
    }
}