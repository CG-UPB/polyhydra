//
// Created by steffen on 11.10.21.
//

#ifndef VOLUMESHOS_FRAMEBUFFEROBJECT_H
#define VOLUMESHOS_FRAMEBUFFEROBJECT_H

namespace vOS
{
    class FrameBufferObject
    {
    public:
        FrameBufferObject(int width, int height);
        ~FrameBufferObject();
        void resize(int width, int height);
        void bind();
        void unbind();

        unsigned int getTextureID() const;
        unsigned int getID() const;

    private:

        void init(int width, int height);
        void cleanUp();

        unsigned int createFrameBuffer();
        unsigned int createTextureAttachment();

        int m_width;
        int m_height;

        unsigned int m_frameBufferID;
        unsigned int m_textureID;
        int m_previousFrameBufferID;
        int m_previousViewPort[4];
    };
}


#endif //VOLUMESHOS_FRAMEBUFFEROBJECT_H
