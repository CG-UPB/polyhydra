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
        Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        ~Shader() = default;
        void bind();
        void unbind();

        // uniform location of a variable
        int getUniform(const std::string& name);

        // send uniform variables to the shader
        void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
        void setUniform1f(const std::string& name, float value);
        void setUniform2f(const std::string& name, glm::vec2& value);
        void setUniform3f(const std::string& name, glm::vec3& value);
        void setUniform4f(const std::string& name, glm::vec4& value);

    private:

        unsigned int m_shaderID;

        // we are caching the uniform locations, so we don't need to query them every frame
        std::unordered_map<std::string, int> m_locations;
    };
}