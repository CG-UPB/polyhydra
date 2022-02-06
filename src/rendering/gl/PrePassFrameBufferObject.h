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
        [[nodiscard]] unsigned int get_color_texture0_id() const;
        [[nodiscard]] unsigned int get_color_texture1_id() const;

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
        unsigned int m_color_texture0;
        unsigned int m_color_texture1;

        int m_previousFrameBufferID;
        int m_previousViewPort[4];
    };
}
