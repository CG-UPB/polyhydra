#pragma once

#include "RenderPass.h"
#include "../shapes/Shape.h"

namespace volumeshOS::Internal
{

    /**
     *  Renders Shape Objects such as Cubes, Cylinders and Spheres
     *  Statically callable
     */
    class ShapePass : public RenderPass
    {
    public:
        /**
         * Deconstructor
         */
        ~ShapePass();

        /**
         * Removes all Shapes in the Viewer
         */
        static void remove_all();
        /**
         * Removes Shape identified by given id
         * @param id
         */
        static void remove_shape(unsigned int id);
        /**
         * Adds given Shape to the Viewer
         * @param shape
         */
        static void add_shape(Shape* shape);
        /**
         * Returns Shape identified by given id
         * @param shape_id
         * @return
         */
        [[nodiscard]] static const Shape& get_shape(unsigned int shape_id);


        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData &data, std::shared_ptr<MeshObject> mesh) override;

    private:
        // List of all Shapes
        static std::vector<Shape*> s_shapes;
    };
}
