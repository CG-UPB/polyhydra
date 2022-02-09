
#include "glad/glad.h"

#include "FrameBufferObject.h"

#include <iostream>

namespace vOS
{
    int FrameBufferObject::s_num_samples = -1;

    FrameBufferObject::FrameBufferObject(int width, int height, bool multisample): m_multisample(multisample)
    {
        if (s_num_samples < 0)
        {
            // use 4 samples if possible, else use the max supported value
            int max_supported_samples;
            glGetIntegerv(GL_MAX_SAMPLES, &max_supported_samples);
            s_num_samples = std::min(max_supported_samples, 4);
            std::cout << "Using " << s_num_samples << " MSAA samples" << std::endl;
        }
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
        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(1, buffers);
        return fbo;
    }

    unsigned int FrameBufferObject::create_texture_attachment()
    {
        unsigned int tex[1];
        glGenTextures(1, tex);
        if (m_multisample)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex[0]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGBA8, m_width, m_height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex[0], 0);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, tex[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[0], 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        return tex[0];
    }

    unsigned int FrameBufferObject::create_color_attachment(unsigned int attachment)
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

    unsigned int FrameBufferObject::create_depth_texture_attachment()
    {
        unsigned int tex[1];
        glGenTextures(1, tex);
        if (m_multisample)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex[0]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_DEPTH_COMPONENT, m_width, m_height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, tex[0], 0);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, tex[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex[0], 0);
        }
        return tex[0];
    }

    void FrameBufferObject::bind()
    {
        glGetIntegerv(GL_VIEWPORT, m_previousViewPort);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previousFrameBufferID);
        if (m_multisample)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
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
        m_peel_textureID = create_color_attachment(GL_COLOR_ATTACHMENT1);
        m_depth_textureID = create_depth_texture_attachment();
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

    unsigned int FrameBufferObject::get_peel_texture_id() const
    {
        return m_peel_textureID;
    }

    unsigned int FrameBufferObject::get_depth_texture_id() const
    {
        return m_depth_textureID;
    }

    unsigned int FrameBufferObject::get_id() const
    {
        return m_frameBufferID;
    }

    int FrameBufferObject::get_width() const
    {
        return m_width;
    }

    int FrameBufferObject::get_height() const
    {
        return m_height;
    }

    void FrameBufferObject::copy(const FrameBufferObject* src, const FrameBufferObject* dest)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src->get_id());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->get_id());
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(
                0, 0,
                src->get_width(), src->get_height(),
                0, 0,
                dest->get_width(), dest->get_height(),
                GL_COLOR_BUFFER_BIT,
                GL_LINEAR);
        glReadBuffer(GL_DEPTH_ATTACHMENT);
        glDrawBuffer(GL_DEPTH_ATTACHMENT);
        glBlitFramebuffer(
                0, 0,
                src->get_width(), src->get_height(),
                0, 0,
                dest->get_width(), dest->get_height(),
                GL_DEPTH_BUFFER_BIT,
                GL_NEAREST);

        glReadBuffer(GL_DEPTH_ATTACHMENT);
        glDrawBuffer(GL_DEPTH_ATTACHMENT);
        glBlitFramebuffer(
                0, 0,
                src->get_width(), src->get_height(),
                0, 0,
                dest->get_width(), dest->get_height(),
                GL_DEPTH_BUFFER_BIT,
                GL_LINEAR);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}
