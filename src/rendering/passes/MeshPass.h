#pragma once

#include "RenderPass.h"

namespace vOS
{

    class MeshPass : public RenderPass
    {
    public:
        MeshPass();
        ~MeshPass() = default;

        void set_wireframe_mode(bool mode);
        [[nodiscard]] bool get_wireframe_mode() const;

        void set_use_phong(bool use);
        [[nodiscard]] bool get_use_phong() const;

        void set_rendering_mode(float f){m_rendering_mode = f;}

        void render(const VertexArrayObject& vao, const RenderData& data) override;

        void set_mesh_shader(Shader* shader){m_mesh_shader = shader;}
    private:

        float m_rendering_mode = 0;
        bool m_use_phong = true;
        bool m_render_wireframe = false;
        Shader* m_mesh_shader;
    };
}