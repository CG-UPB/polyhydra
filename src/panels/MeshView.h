#pragma once

#include "../ImguiRenderer.h"
#include "../rendering/FrameBufferObject.h"
#include "../rendering/Shader.h"
#include "../rendering/VertexArrayObject.h"
#include "../algorithms/VosWindow.h"

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
        bool m_W_button_pressed;
        double m_lastX;
        double m_lastY;

        glm::vec3 m_light;
        glm::vec3 m_camera;
        glm::vec3 m_light_color;
        glm::vec3 m_object_color;


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