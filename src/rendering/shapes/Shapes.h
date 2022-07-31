#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
{
    using ShapeType = std::size_t;

    class VertexArrayObject;
    struct BaseShape
    {
        explicit BaseShape(ShapeID id): id(id)
        {}

        const ShapeID id;

        MeshID parent_mesh      = INVALID_MESH_ID;
        OVMCell cell            = {};

        glm::vec3 position      = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale         = {1.0f, 1.0f, 1.0f};
        glm::vec4 rotation      = {0.0f, 1.0f, 0.0f, 0.0f};
        glm::vec3 color         = {0.2f, 0.2f, 1.0f};

    public:

        [[nodiscard]] virtual std::unique_ptr<VertexArrayObject> get_vao() const;

        [[nodiscard]] virtual ShapeType get_type() const;

        [[nodiscard]] virtual ShapeType get_hashed_type() const
        {
            ShapeType res = 0;
            hash_combine(res, get_type());
            return res;
        }

    public:
        static ShapeType next_type();

    protected:

        template<typename T>
        static inline void hash_combine(ShapeType& seed, const T& value)
        {
            std::hash<T> hash;
            seed ^= hash(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

    private:
        static ShapeType s_current_type_id;
    };

    template<typename T>
    struct Shape : public BaseShape
    {
        using BaseShape::BaseShape;

        static const ShapeType TYPE;

        [[nodiscard]] ShapeType get_type() const final
        {
            return TYPE;
        }
    };

    template<typename T>
    const ShapeType Shape<T>::TYPE(Shape::next_type());

    struct BoxShape : public Shape<BoxShape>
    {
        using Shape::Shape;
        [[nodiscard]] std::unique_ptr<VertexArrayObject> get_vao() const final;
    };

    struct CylinderShape : public Shape<CylinderShape>
    {
        using Shape::Shape;
        [[nodiscard]] std::unique_ptr<VertexArrayObject> get_vao() const final;
    };

    struct SphereShape : public Shape<SphereShape>
    {
        using Shape::Shape;
        [[nodiscard]] std::unique_ptr<VertexArrayObject> get_vao() const final;
    };

    struct ConeShape : public Shape<ConeShape>
    {
        using Shape::Shape;
        [[nodiscard]] std::unique_ptr<VertexArrayObject> get_vao() const final;
    };

    struct ArrowShape : public Shape<ArrowShape>
    {
        using Shape::Shape;

        float base_width_percentage = 0.5f;
        float tip_height_percentage = 0.7f;

        [[nodiscard]] std::unique_ptr<VertexArrayObject> get_vao() const final;

        [[nodiscard]] ShapeType get_hashed_type() const final
        {
            auto res = Shape::get_hashed_type();
            hash_combine(res, base_width_percentage);
            hash_combine(res, tip_height_percentage);
            return res;
        }
    };

}