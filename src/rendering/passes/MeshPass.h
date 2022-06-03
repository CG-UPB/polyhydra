#pragma once

#include "RenderPass.h"
#include "../Renderer.h"

namespace vOS
{
    class Renderer;

    /**
     * Renders Meshes according to its set mesh properties
     */
    class MeshPass : public RenderPass
    {
    public:

        explicit MeshPass(Renderer* renderer);
        /**
         * Deconstructor
         */
        ~MeshPass() = default;

        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;


    private:

        Renderer* m_renderer;
    };
}