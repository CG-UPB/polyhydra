#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
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
        static std::shared_ptr<Shader> get(const std::string& shader_name);

        /**
         * Utility shaders.
         */
        static std::shared_ptr<Shader> background_shader() { return get("background"); }
        //static std::shared_ptr<Shader> ground_shader() { return get("ground"); }
        static std::shared_ptr<Shader> basic_shape_shader() { return get("basic_shape"); }
        static std::shared_ptr<Shader> quad_circle_shader() { return get("quad_circle"); }
        static std::shared_ptr<Shader> flat_color_shader() { return get("flat_color"); }
        static std::shared_ptr<Shader> vertex_only_shader() { return get("vertex_only"); };

        /**
         * SelectionMode shaders.
         */
        static std::shared_ptr<Shader> selection_face() { return get("selection_face"); }
        static std::shared_ptr<Shader> selection_vertex_shader() { return get("selection_vertex"); }
        static std::shared_ptr<Shader> selection_edge_shader() { return get("selection_edge"); }
        static std::shared_ptr<Shader> edge_hover_shader() { return get("edge_hover"); }
        static std::shared_ptr<Shader> pre_mesh_phong_shader() { return get("pre_mesh_phong"); }

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
         * Sets a uniform sampler2D for this shader.
         *
         * @param name name of the uniform
         * @param binding opengl texture binding, for example GL_TEXTURE0, GL_TEXTURE1, ...
         * @param texture_id id of the texture to be bound
         */
        void set_uniform_sampler2D(const std::string& name, uint32_t binding, uint32_t texture_id);

        /**
         * Sets a uniform sampler2D for this shader.
         *
         * @param name name of the uniform
         * @param binding opengl texture binding, for example GL_TEXTURE0, GL_TEXTURE1, ...
         * @param texture_id id of the texture to be bound
         */
        void set_uniform_sampler2DArray(const std::string& name, uint32_t binding, uint32_t texture_id);

        /**
         * Sets a uniform sampler2DMS for this shader.
         *
         * @param name name of the uniform
         * @param binding opengl texture binding, for example GL_TEXTURE0, GL_TEXTURE1, ...
         * @param texture_id id of the texture to be bound
         */
        void set_uniform_sampler2DMS(const std::string& name, uint32_t binding, uint32_t texture_id);

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
         * Sets a uniform vec3 array for this shader.
         *
         * @param name name of the uniform
         * @param values value the uniform is set to
         */
        void set_uniform_vec3f_array(const std::string& name, const std::vector<glm::vec3>& values);

        /**
         * Sets a uniform vec4 array for this shader.
         *
         * @param name name of the uniform
         * @param values value the uniform is set to
         */
        void set_uniform_vec4f_array(const std::string& name, const std::vector<glm::vec4>& values);

        /**
         * Sets a uniform vec4 for this shader.
         *
         * @param name name of the uniform
         * @param value value the uniform is set to
         */
        void set_uniform_vec4f(const std::string& name, const glm::vec4& value);

    private:

        // all loaded shaders by name
        static std::unordered_map<std::string, std::shared_ptr<Shader>> s_shaders;
        static std::unordered_map<std::string, std::string> s_common_sources;

        static std::string get_shader_source(const FS_NAMESPACE::path& shader_path);

    private:

        /**
         * Creates a new shader object from the given resource paths.
         *
         * @param vertexPath vertex resource path
         * @param fragmentPath fragment resource path
         * @param geometryPath geometry resource path (may be empty)
         */
        Shader(const FS_NAMESPACE::path& vertexPath, const FS_NAMESPACE::path& fragmentPath, const FS_NAMESPACE::path& geometryPath);

        // opengl program id
        uint32_t m_shaderID;

        // we are caching the uniform locations, so we don't need to query them every frame
        std::unordered_map<std::string, int> m_locations;
    };
}
