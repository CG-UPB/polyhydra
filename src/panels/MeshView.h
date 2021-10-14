//
// Created by steffen on 11.10.21.
//

#ifndef VOLUMESHOS_MESHVIEW_H
#define VOLUMESHOS_MESHVIEW_H

#include "../Window.h"
#include "../rendering/FrameBufferObject.h"
#include "../rendering/Shader.h"
#include "../rendering/VertexArrayObject.h"

namespace vOS
{
    class MeshView: public WindowPanel
    {
    public:
        MeshView(int width, int height);
        ~MeshView() override;
        void show() override;

    private:

        // used for the arc ball
        bool m_arcBallOn;
        bool m_lastDown;
        double m_lastX;
        double m_lastY;

        // current mvp and world matrix
        glm::mat4 m_meshTransform;
        glm::mat4 m_meshWorld;
        glm::mat4 m_meshView;
        glm::mat4 m_meshProjection;

        int m_viewportPanelWidth;
        int m_viewportPanelHeight;

        // opengl rendering
        VertexArrayObject* m_vertexArrayObject;
        FrameBufferObject* m_meshFrameBuffer;
        Shader* m_meshShader;
    };
}


#endif //VOLUMESHOS_MESHVIEW_H
