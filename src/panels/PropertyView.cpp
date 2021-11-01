
#include "PropertyView.h"

namespace vOS
{

    vOS::PropertyView::PropertyView(const FrameBufferObject& fbo): m_framebuffer(fbo)
    {
        std::vector<float> quad_vertices = {
                -0.5, 0.5, 0.0,
                -0.5, -0.5, 0.0,
                0.5, -0.5, 0.0,
                0.5, 0.5, 0.0
        };
        std::vector<unsigned int> quad_indices = {
                0, 1, 2,
                2, 3, 0
        };
        std::vector<float> quad_texture_coordinates = {
                0.0, 0.0,
                0.0, 1.0,
                1.0, 1.0,
                1.0, 0.0
        };
        m_vao = new VertexArrayObject(quad_vertices, quad_indices, quad_texture_coordinates);
        std::filesystem::path shaderPath = "shaders";
        m_shader = new Shader(shaderPath / "property.vert", shaderPath / "property.frag");
    }

    PropertyView::~PropertyView()
    {
        delete m_vao;
        delete m_shader;
    }

    void vOS::PropertyView::show()
    {

    }
}