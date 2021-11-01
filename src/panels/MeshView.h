#pragma once

#include "../Window.h"
#include "../rendering/FrameBufferObject.h"
#include "../rendering/Shader.h"
#include "../rendering/VertexArrayObject.h"

class PropertyView;

namespace vOS
{
    class MeshView: public WindowPanel
    {
    public:
        MeshView(int width, int height);
        ~MeshView() override;
        void show() override;

    private:

        void handleResize();
        void handleMouseControl();
        void renderMesh();

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
        FrameBufferObject* m_meshFrameBuffer;
        Shader* m_meshShader;

        friend class PropertyView;
    };
}