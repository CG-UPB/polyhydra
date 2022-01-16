#pragma once

namespace vOS
{
    class FrameBufferObject
    {
    public:

        FrameBufferObject(int width, int height, bool multisample = false);
        ~FrameBufferObject();

        void resize(int width, int height);

        void bind();
        void unbind();

        [[nodiscard]] unsigned int get_texture_id() const;
        [[nodiscard]] unsigned int get_id() const;
        [[nodiscard]] int get_width() const;
        [[nodiscard]] int get_height() const;

    private:

        void init(int width, int height);
        void clean_up();

        unsigned int create_framebuffer();
        unsigned int create_texture_attachment();
        unsigned int create_depth_texture_attachment();

        int m_width;
        int m_height;
        bool m_multisample;

        unsigned int m_frameBufferID;
        unsigned int m_textureID;
        unsigned int m_depth_texture_id;
        int m_previousFrameBufferID;
        int m_previousViewPort[4];
    };
}