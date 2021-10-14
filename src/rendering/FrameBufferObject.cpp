//
// Created by steffen on 11.10.21.
//

#include <glad/glad.h>

#include "FrameBufferObject.h"

#include <iostream>

vOS::FrameBufferObject::FrameBufferObject(int width, int height)
{
    init(width, height);
}

unsigned int vOS::FrameBufferObject::createFrameBuffer()
{
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffers);
    return fbo;
}

unsigned int vOS::FrameBufferObject::createTextureAttachment()
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

vOS::FrameBufferObject::~FrameBufferObject()
{
    cleanUp();
}

void vOS::FrameBufferObject::bind()
{
    glGetIntegerv(GL_VIEWPORT, m_previousViewPort);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previousFrameBufferID);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    glViewport(0, 0, m_width, m_height);
}

void vOS::FrameBufferObject::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_previousFrameBufferID);
    glViewport(m_previousViewPort[0], m_previousViewPort[1], m_previousViewPort[2], m_previousViewPort[3]);
}

unsigned int vOS::FrameBufferObject::getTextureID() const
{
    return m_textureID;
}

unsigned int vOS::FrameBufferObject::getID() const
{
    return m_frameBufferID;
}

void vOS::FrameBufferObject::init(int width, int height)
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_previousFrameBufferID);
    m_width = width;
    m_height = height;
    m_frameBufferID = createFrameBuffer();
    m_textureID = createTextureAttachment();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error: %u\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void vOS::FrameBufferObject::cleanUp()
{
    glDeleteFramebuffers(1, &m_frameBufferID);
    glDeleteTextures(1, &m_textureID);
}

void vOS::FrameBufferObject::resize(int width, int height)
{
    cleanUp();
    init(width, height);
}
