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

        void render(const VertexArrayObject& vao, const RenderData& data) override;

    private:

        bool m_use_phong = true;
        bool m_render_wireframe = false;
        Shader* m_mesh_shader;
    };
}