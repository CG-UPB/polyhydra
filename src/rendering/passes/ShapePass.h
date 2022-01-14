#pragma once

#include "RenderPass.h"
#include "../shapes/Shape.h"

namespace vOS
{


    class ShapePass : public RenderPass
    {
    public:

        ~ShapePass();

        static void remove_all();
        static void remove_shape(unsigned int id);
        static void add_shape(Shape* shape);
        [[nodiscard]] static const Shape& get_shape(unsigned int shape_id);

        void render(VertexArrayObject* vao, const RenderData &data, int mesh_id) override;

    private:
        static std::vector<Shape*> s_shapes;
    };
}
