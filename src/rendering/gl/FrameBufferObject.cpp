
#include "FrameBufferObject.h"

namespace volumeshOS::Internal
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
                    .texture_filter     = -1,
                    .texture_wrap       = -1,
                    .texture_comp_func  = -1,
                    .texture_comp_mode  = -1,
                    .border_color       = -1,
                    .generate_mipmap    = false,
                    .multisample        = true
            },
            // depth
            FrameBufferAttachment{
                    .internal_format    = GL_DEPTH24_STENCIL8,
                    .format             = GL_DEPTH_STENCIL,
                    .type               = GL_UNSIGNED_INT_24_8,
                    .attachment         = GL_DEPTH_STENCIL_ATTACHMENT,
                    .texture_filter     = -1,
                    .texture_wrap       = -1,
                    .texture_comp_func  = -1,
                    .texture_comp_mode  = -1,
                    .border_color       = -1,
                    .generate_mipmap    = false,
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
            Log::info("Using " + std::to_string(s_num_samples) + " MSAA samples");
        }
        init(width, height);
    }

    FrameBufferObject::~FrameBufferObject()
    {
        clean_up();
    }

    void FrameBufferObject::attach_texture(int attachment, uint32_t texture, bool multisample)
    {
        int target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, texture, 0);
        std::vector<uint32_t> new_draw_buffers = m_draw_buffers;
        new_draw_buffers.push_back(attachment);
        glDrawBuffers((int) new_draw_buffers.size(), new_draw_buffers.data());
    }

    uint32_t FrameBufferObject::create_framebuffer()
    {
        // specify all attachments as draw buffers
        m_draw_buffers.clear();
        for (auto& attachment: m_attachments)
        {
            int atm = attachment.attachment;
            if (atm >= GL_COLOR_ATTACHMENT0 && atm <= GL_COLOR_ATTACHMENT31)
            {
                m_draw_buffers.push_back(attachment.attachment);
            }
        }
        uint32_t fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        if(!m_draw_buffers.empty())
        {
            glDrawBuffers((int) m_draw_buffers.size(), m_draw_buffers.data());
        }
        else
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

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
            uint32_t texture = create_attachment(attachment);
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

    uint32_t FrameBufferObject::get_texture(int attachment)
    {
        auto texture = m_attachment_textures.find(attachment);
        if (texture == m_attachment_textures.end())
        {
            throw std::invalid_argument("Could not find attachment: " + std::to_string(attachment));
        }
        return m_attachment_textures[attachment];
    }

    uint32_t FrameBufferObject::get_id() const
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

    void FrameBufferObject::copy(int attachment, int mask, const std::shared_ptr<FrameBufferObject>& src, const std::shared_ptr<FrameBufferObject>& dest)
    {
        copy(attachment, attachment, mask, src, dest);
    }

    void FrameBufferObject::copy(int read_attachment, int draw_attachment, int mask,
                                 const std::shared_ptr<FrameBufferObject>& src,
                                 const std::shared_ptr<FrameBufferObject>& dest)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src->get_id());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->get_id());
        glReadBuffer(read_attachment);
        glDrawBuffer(draw_attachment);
        glBlitFramebuffer(
                0, 0,
                src->get_width(), src->get_height(),
                0, 0,
                dest->get_width(), dest->get_height(),
                mask,
                ((mask & GL_DEPTH_BUFFER_BIT) == GL_DEPTH_BUFFER_BIT) ? GL_NEAREST : GL_LINEAR
        );
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    uint32_t FrameBufferObject::create_attachment(const FrameBufferAttachment& attachment) const
    {
        check_attachment_valid(attachment);

        uint32_t tex[1];
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
            if (attachment.border_color != -1)
            {
                float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
            }
            if (attachment.generate_mipmap)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
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
