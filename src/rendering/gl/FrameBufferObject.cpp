
#include "glad/glad.h"

#include "FrameBufferObject.h"

#include <iostream>

namespace vOS
{
    FrameBufferObject::FrameBufferObject(int width, int height)
    {
        init(width, height);
    }

    FrameBufferObject::~FrameBufferObject()
    {
        clean_up();
    }

    unsigned int FrameBufferObject::create_framebuffer()
    {
        unsigned int fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, buffers);
        return fbo;
    }

    unsigned int FrameBufferObject::create_texture_attachment()
    {
        unsigned int tex[1];
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, tex[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[0], 0);
        return tex[0];
    }

    unsigned int FrameBufferObject::create_depth_texture_attachment()
    {
        unsigned int tex[1];
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, tex[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex[0], 0);
        return tex[0];
    }

    void FrameBufferObject::bind()
    {
        glGetIntegerv(GL_VIEWPORT, m_previousViewPort);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previousFrameBufferID);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
        glViewport(0, 0, m_width, m_height);
    }

    void FrameBufferObject::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_previousFrameBufferID);
        glViewport(m_previousViewPort[0], m_previousViewPort[1], m_previousViewPort[2], m_previousViewPort[3]);
    }

    void FrameBufferObject::init(int width, int height)
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previousFrameBufferID);
        m_width = width;
        m_height = height;
        m_frameBufferID = create_framebuffer();
        m_textureID = create_texture_attachment();
        m_depth_texture_id = create_depth_texture_attachment();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "Error: %u\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBufferObject::clean_up()
    {
        glDeleteFramebuffers(1, &m_frameBufferID);
        glDeleteTextures(1, &m_textureID);
    }

    void FrameBufferObject::resize(int width, int height)
    {
        clean_up();
        init(width, height);
    }

    unsigned int FrameBufferObject::get_texture_id() const
    {
        return m_textureID;
    }

    unsigned int FrameBufferObject::get_id() const
    {
        return m_frameBufferID;
    }
}
