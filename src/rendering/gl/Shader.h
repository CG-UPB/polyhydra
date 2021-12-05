#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include "glm/mat4x4.hpp"

namespace vOS
{
    class Shader
    {
    public:

        /**
         * Loads all shaders in the res/shaders folder.
         */
        static void load_all();

        /**
         * Deletes all shaders that were previously loaded.
         */
        static void delete_all();

        /**
         * Returns the specified shader instance.
         *
         * @param shader_name name of the shader (convention: filename without extension)
         * @return the shader instance
         */
        static Shader* get(const std::string& shader_name);

        /**
         * Convenience shader references.
         *
         * @return the specified shader
         */
        static Shader* mesh_shader() { return get("mesh"); }
        static Shader* property_shader() { return get("property"); }
        static Shader* background_shader() { return get("background"); }
        static Shader* basic_shape_shader() { return get("basic_shape"); }
        static Shader* selection_face() { return get("selection_face"); }
        static Shader* selection_vertex_shader() { return get("selection_vertex"); }
        static Shader* selection_edge_shader() { return get("selection_edge"); }
        static Shader* flat_color_shader() { return get("flat_color"); }
        static Shader* quad_circle_shader() { return get("quad_circle"); }
        static Shader* edge_hover_shader() { return get("edge_hover"); }

        /**
         * Binds this shader for opengl. A shader must be bound to draw something or to set the shader uniforms.
         */
        void bind() const;

        /**
         * Unbinds this shader instance.
         */
        void unbind() const;

        /**
         * Returns the opengl location of a given shader uniform.
         *
         * @param name name of the uniform in the shader
         * @return the uniform location or -1, if the uniform does not exist in the shader
         */
        int get_uniform(const std::string& name);

        /**
         * Sets a uniform mat4 for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_mat4f(const std::string& name, const glm::mat4& value);

        /**
         * Sets a uniform float for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_float(const std::string& name, float value);

        /**
         * Sets a uniform int for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_int(const std::string& name, int value);

        /**
         * Sets a uniform boolean for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_bool(const std::string& name, bool value);

        /**
         * Sets a uniform vec2 for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_vec2f(const std::string& name, const glm::vec2& value);

        /**
         * Sets a uniform vec3 for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_vec3f(const std::string& name, const glm::vec3& value);

        /**
         * Sets a uniform vec4 for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_vec4f(const std::string& name, const glm::vec4& value);

    private:

        Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        ~Shader() = default;

        // all loaded shaders by name
        static std::unordered_map<std::string, Shader*> s_shaders;

        unsigned int m_shaderID;

        // we are caching the uniform locations, so we don't need to query them every frame
        std::unordered_map<std::string, int> m_locations;
    };
}