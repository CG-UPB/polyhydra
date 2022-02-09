#pragma once

namespace vOS {

    class PrePassFrameBufferObject {

    public:
        PrePassFrameBufferObject(int width, int height);
        ~PrePassFrameBufferObject();

        void bind();
        void unbind();

        void resize(int width, int height);

        [[nodiscard]] unsigned int get_depth_texture_id() const;
        [[nodiscard]] unsigned int get_normal_texture_id() const;
        [[nodiscard]] unsigned int get_position_texture_id() const;

    private:

        void init(int width, int height);

        void clean_up();

        unsigned int create_framebuffer();
        unsigned int create_depth_texture();
        unsigned int create_color_attachment(unsigned int attachment);

        int m_width;
        int m_height;

        unsigned int m_frameBufferID;
        unsigned int m_depth_texture;
        unsigned int m_normal_texture;
        unsigned int m_position_texture;

        int m_previousFrameBufferID;
        int m_previousViewPort[4];
    };
}
