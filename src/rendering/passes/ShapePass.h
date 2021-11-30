#pragma once

#include "RenderPass.h"
#include "../shapes/Shape.h"

namespace vOS
{
    class ShapePass : public RenderPass
    {
    public:

        ~ShapePass();

        static unsigned int add_shape(std::tuple<Shape*, unsigned int, bool>* shape);
        [[nodiscard]] static const Shape& get_shape(unsigned int shape_id);

        void render(const VertexArrayObject &vao, const RenderData &data) override;

    private:

        static std::vector<Shape*> s_shapes;
    };
}
