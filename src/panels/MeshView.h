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
        ~MeshView();
        void show() override;

    private:

        float m_meshPosition[3];
        float m_meshScale[3];
        float m_meshRotation[3];

        int m_viewportPanelWidth;
        int m_viewportPanelHeight;

        VertexArrayObject* m_vertexArrayObject;
        FrameBufferObject* m_meshFrameBuffer;
        Shader* m_meshShader;
    };
}


#endif //VOLUMESHOS_MESHVIEW_H
