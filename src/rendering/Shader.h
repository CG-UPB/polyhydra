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

        static void load_all();
        static void delete_all();
        static Shader* get(const std::string& shader_name);

        // convenience shader references
        static Shader* mesh_shader() { return get("mesh"); }
        static Shader* property_shader() { return get("property"); }
        static Shader* background_shader() { return get("background"); }
        static Shader* basic_shape_shader() { return get("basic_shape"); }
        static Shader* selection_shader() { return get("selection"); }

        void bind() const;
        void unbind() const;

        // uniform location of a variable
        int get_uniform(const std::string& name);

        // send uniform variables to the shader
        void set_uniform_mat4f(const std::string& name, const glm::mat4& matrix);
        void set_uniform_float(const std::string& name, float value);
        void set_uniform_int(const std::string& name, int value);
        void set_uniform_bool(const std::string& name, bool value);
        void set_uniform_vec2f(const std::string& name, const glm::vec2& value);
        void set_uniform_vec3f(const std::string& name, const glm::vec3& value);
        void set_uniform_vec4f(const std::string& name, const glm::vec4& value);

    private:

        Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        ~Shader() = default;

        static std::unordered_map<std::string, Shader*> s_shaders;

        unsigned int m_shaderID;

        // we are caching the uniform locations, so we don't need to query them every frame
        std::unordered_map<std::string, int> m_locations;
    };
}