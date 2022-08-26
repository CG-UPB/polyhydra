#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
{
    /**
     * Attachment definition. Use this to configure the texture you want to attach to the framebuffer.
     */
    struct FrameBufferAttachment
    {
        int internal_format      = -1;
        int format               = -1;
        int type                 = -1;
        int attachment           = -1;
        int texture_filter       = -1;
        int texture_wrap         = -1;
        int texture_comp_func    = -1;
        int texture_comp_mode    = -1;
        int border_color         = -1;
        int generate_mipmap      = false;
        bool multisample         = false;
    };

    /**
     * Abstraction of an OpenGL Framebuffer with arbitrary attachments.
     */
    class FrameBufferObject
    {
    public:

        /**
         * Copies the attachment of one framebuffer to the attachment of another framebuffer.
         *
         * @param attachment attachment to be copied
         * @param mask mask, for example GL_COLOR_BUFFER_BIT
         * @param src framebuffer to be copied from
         * @param dest framebuffer to be copied to
         */
        static void copy(int attachment, int mask, const std::shared_ptr<FrameBufferObject>& src, const std::shared_ptr<FrameBufferObject>& dest);

        /**
         * Creates a new FrameBufferObject with a specified width and height, and attachments.
         *
         * @param width width
         * @param height height
         * @param attachments attachments
         */
        FrameBufferObject(int width, int height, std::vector<FrameBufferAttachment>  attachments);
        ~FrameBufferObject();

        /**
         * Attaches an existing texture to this framebuffer. The caller must ensure that this framebuffer is bound,
         * and that the texture has the correct format. So this framebuffer does not take the ownership of the texture.
         *
         * @param attachment attachment the texture is bound to
         * @param texture the texture to attach
         * @param multisample is the texture multisampled?
         */
        void attach_texture(int attachment, uint32_t texture, bool multisample = false);

        /**
         * Resizes this framebuffer to a new width and height.
         *
         * @param width new width
         * @param height new height
         */
        void resize(int width, int height);

        /**
         * Binds this framebuffer to be used for operations.
         */
        void bind();

        /**
         * Unbinds this framebuffer.
         */
        void unbind();

        /**
         * Returns the texture id of the specified attachment.
         *
         * @param attachment attachment
         * @return texture id
         */
        [[nodiscard]] uint32_t get_texture(int attachment);

        /**
         * Returns the OpenGL id of this framebuffer.
         *
         * @return framebuffer id
         */
        [[nodiscard]] uint32_t get_id() const;

        /**
         * Returns the current width of this framebuffer.
         *
         * @return current width
         */
        [[nodiscard]] int get_width() const;

        /**
         * Returns the current height of this framebuffer.
         *
         * @return current height
         */
        [[nodiscard]] int get_height() const;

        // default attachment lists
        static const std::vector<FrameBufferAttachment> RGBA_AND_DEPTH;
        static const std::vector<FrameBufferAttachment> RGBA_AND_DEPTH_MULTISAMPLE;

    private:

        /**
         * Initializes texture attachments.
         *
         * @param width width
         * @param height height
         */
        void init(int width, int height);

        /**
         * Releases framebuffer and texture resources.
         */
        void clean_up();

        /**
         * Creates an OpenGL Framebuffer.
         *
         * @return the framebuffer id
         */
        uint32_t create_framebuffer();

        /**
         * Creates an attachment for this framebuffer with a given specification.
         *
         * @param attachment attachment specification
         * @return the attachment texture id
         */
        [[nodiscard]] uint32_t create_attachment(const FrameBufferAttachment& attachment) const;

        /**
         * Checks if necessary values of a given attachment are set.
         *
         * @param attachment attachment to be checked
         */
        void check_attachment_valid(const FrameBufferAttachment& attachment) const;

        // maximum number of supported msaa samples
        static int s_num_samples;

        // current width and height
        int m_width;
        int m_height;

        // internal ids and attachments
        std::unordered_map<int, uint32_t> m_attachment_textures;
        std::vector<FrameBufferAttachment> m_attachments;
        std::vector<uint32_t> m_draw_buffers;
        std::vector<uint32_t> m_texture_ids;
        uint32_t m_framebuffer_id;

        // save previous configuration to restore them later
        int m_previous_frameBuffer_id;
        int m_previous_viewport[4];
    };
}