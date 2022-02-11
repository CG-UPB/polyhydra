
#include "glad/glad.h"

#include "FrameBufferObject.h"

#include <iostream>
#include <utility>

namespace vOS
{
    // rgba and depth attachments
    const std::vector<FrameBufferAttachment> FrameBufferObject::RGBA_AND_DEPTH = {
            // color
            FrameBufferAttachment{
                    .internal_format    = GL_RGBA,
                    .format             = GL_RGBA,
                    .type               = GL_UNSIGNED_BYTE,
                    .attachment         = GL_COLOR_ATTACHMENT0,
                    .texture_filter     = GL_LINEAR,
                    .texture_wrap       = GL_CLAMP_TO_EDGE
            },
            // depth
            FrameBufferAttachment{
                    .internal_format    = GL_DEPTH_COMPONENT,
                    .format             = GL_DEPTH_COMPONENT,
                    .type               = GL_FLOAT,
                    .attachment         = GL_DEPTH_ATTACHMENT,
                    .texture_filter     = GL_LINEAR
            }
    };

    // rgba and depth attachments with multisampling
    const std::vector<FrameBufferAttachment> FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE = {
            // color
            FrameBufferAttachment{
                    .internal_format    = GL_RGBA,
                    .format             = GL_RGBA,
                    .type               = GL_UNSIGNED_BYTE,
                    .attachment         = GL_COLOR_ATTACHMENT0,
                    .multisample        = true
            },
            // depth
            FrameBufferAttachment{
                    .internal_format    = GL_DEPTH_COMPONENT,
                    .format             = GL_DEPTH_COMPONENT,
                    .type               = GL_FLOAT,
                    .attachment         = GL_DEPTH_ATTACHMENT,
                    .multisample        = true
            }
    };

    int FrameBufferObject::s_num_samples = -1;

    FrameBufferObject::FrameBufferObject(int width, int height, std::vector<FrameBufferAttachment> attachments) :
            m_attachments(std::move(attachments))
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
        // specify all attachments as draw buffers
        std::vector<GLenum> draw_buffers;
        for (auto& attachment: m_attachments)
        {
            int atm = attachment.attachment;
            if (atm >= GL_COLOR_ATTACHMENT0 && atm <= GL_COLOR_ATTACHMENT31)
            {
                draw_buffers.push_back(attachment.attachment);
            }
        }
        unsigned int fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffers((int) draw_buffers.size(), draw_buffers.data());
        return fbo;
    }

    void FrameBufferObject::bind()
    {
        glGetIntegerv(GL_VIEWPORT, m_previous_viewport);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previous_frameBuffer_id);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);
        glViewport(0, 0, m_width, m_height);
    }

    void FrameBufferObject::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_previous_frameBuffer_id);
        glViewport(m_previous_viewport[0], m_previous_viewport[1], m_previous_viewport[2], m_previous_viewport[3]);
    }

    void FrameBufferObject::init(int width, int height)
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previous_frameBuffer_id);
        m_width = width;
        m_height = height;
        m_framebuffer_id = create_framebuffer();
        // create textures from attachments
        for (auto& attachment: m_attachments)
        {
            unsigned int texture = create_attachment(attachment);
            m_texture_ids.push_back(texture);
            m_attachment_textures[attachment.attachment] = texture;
        }
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "Error: %u\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_previous_frameBuffer_id);
    }

    void FrameBufferObject::clean_up()
    {
        glDeleteFramebuffers(1, &m_framebuffer_id);
        glDeleteTextures((int) m_texture_ids.size(), m_texture_ids.data());
        m_attachment_textures.clear();
        m_texture_ids.clear();
    }

    void FrameBufferObject::resize(int width, int height)
    {
        clean_up();
        init(width, height);
    }

    unsigned int FrameBufferObject::get_texture(int attachment)
    {
        auto texture = m_attachment_textures.find(attachment);
        if (texture == m_attachment_textures.end())
        {
            throw std::invalid_argument("Could not find attachment: " + std::to_string(attachment));
        }
        return m_attachment_textures[attachment];
    }

    unsigned int FrameBufferObject::get_id() const
    {
        return m_framebuffer_id;
    }

    int FrameBufferObject::get_width() const
    {
        return m_width;
    }

    int FrameBufferObject::get_height() const
    {
        return m_height;
    }

    void FrameBufferObject::copy(int attachment, int mask, const FrameBufferObject* src, const FrameBufferObject* dest)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src->get_id());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->get_id());
        glReadBuffer(attachment);
        glDrawBuffer(attachment);
        glBlitFramebuffer(
                0, 0,
                src->get_width(), src->get_height(),
                0, 0,
                dest->get_width(), dest->get_height(),
                mask,
                GL_LINEAR
        );
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    unsigned int FrameBufferObject::create_attachment(const FrameBufferAttachment& attachment) const
    {
        check_attachment_valid(attachment);

        unsigned int tex[1];
        glGenTextures(1, tex);
        if (attachment.multisample)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex[0]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, s_num_samples, attachment.internal_format, m_width,
                                    m_height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment.attachment, GL_TEXTURE_2D_MULTISAMPLE, tex[0], 0);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, tex[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, attachment.internal_format, m_width, m_height, 0, attachment.format,
                         attachment.type, nullptr);
            if (attachment.texture_filter != -1)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, attachment.texture_filter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, attachment.texture_filter);
            }
            if (attachment.texture_wrap != -1)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, attachment.texture_wrap);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, attachment.texture_wrap);
            }
            if (attachment.texture_comp_func != -1)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, attachment.texture_comp_func);
            }
            if (attachment.texture_comp_mode != -1)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, attachment.texture_comp_mode);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment.attachment, GL_TEXTURE_2D, tex[0], 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        return tex[0];
    }

    void FrameBufferObject::check_attachment_valid(const FrameBufferAttachment& attachment) const
    {
        if (attachment.internal_format == -1)
        {
            throw std::invalid_argument("Invalid framebuffer attachment: internal_format must be set");
        }
        else if (attachment.format == -1)
        {
            throw std::invalid_argument("Invalid framebuffer attachment: format must be set");
        }
        else if (attachment.type == -1)
        {
            throw std::invalid_argument("Invalid framebuffer attachment: type must be set");
        }
        else if (attachment.attachment == -1)
        {
            throw std::invalid_argument("Invalid framebuffer attachment: attachment must be set");
        }
    }
}
