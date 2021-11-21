
#include <glad/glad.h>

#include "Shader.h"

#include <iostream>

#include "../fs/FileManager.h"

namespace vOS
{

    Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
    {
        std::string vertexSource = FileManager::loadAsString(vertexPath);
        std::string fragmentSource = FileManager::loadAsString(fragmentPath);

        m_shaderID = glCreateProgram();
        unsigned int vertexID = glCreateShader(GL_VERTEX_SHADER);
        unsigned int fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

        const GLchar *vertBuf = vertexSource.c_str();
        glShaderSource(vertexID, 1, &vertBuf, nullptr);
        glCompileShader(vertexID);
        int success;
        char infoLog[512];
        glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexID, 512, nullptr, infoLog);
            std::cout << "Error when compiling vertex shader: " << infoLog << std::endl;
        }

        const GLchar *fragBuf = fragmentSource.c_str();
        glShaderSource(fragmentID, 1, &fragBuf, nullptr);
        glCompileShader(fragmentID);
        glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentID, 512, nullptr, infoLog);
            std::cout << "Error when compiling fragment shader: " << infoLog << std::endl;
        }

        glAttachShader(m_shaderID, vertexID);
        glAttachShader(m_shaderID, fragmentID);
        glLinkProgram(m_shaderID);

        glGetProgramiv(m_shaderID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_shaderID, 512, nullptr, infoLog);
            std::cout << "Error when linking shader program: " << infoLog << std::endl;
        }

        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
    }

    void Shader::bind() const
    {
        glUseProgram(m_shaderID);
    }

    void Shader::unbind() const
    {
        glUseProgram(0);
    }

    int Shader::get_uniform(const std::string& name)
    {
        auto location = m_locations.find(name);
        if (location == m_locations.end())
        {
            int loc = glGetUniformLocation(m_shaderID, name.c_str());
            if (loc == -1)
            {
                std::cout << "Could not find uniform: " << name << std::endl;
            }
            m_locations.emplace(name, loc);
        }
        return m_locations[name];
    }

    void Shader::set_uniform_mat4f(const std::string& name, const glm::mat4& matrix)
    {
        glUniformMatrix4fv(get_uniform(name), 1, GL_FALSE, &matrix[0][0]);
    }

    void Shader::set_uniform_float(const std::string& name, float value)
    {
        glUniform1f(get_uniform(name), value);
    }

    void Shader::set_uniform_int(const std::string& name, int value)
    {
        glUniform1i(get_uniform(name), value);
    }

    void Shader::set_uniform_bool(const std::string& name, bool value)
    {
        glUniform1i(get_uniform(name), (int) value);
    }

    void Shader::set_uniform_vec2f(const std::string& name, const glm::vec2& value)
    {
        glUniform2f(get_uniform(name), value.x, value.y);
    }

    void Shader::set_uniform_vec3f(const std::string& name, const glm::vec3& value)
    {
        glUniform3f(get_uniform(name), value.x, value.y, value.z);
    }

    void Shader::set_uniform_vec4f(const std::string& name, const glm::vec4& value)
    {
        glUniform4f(get_uniform(name), value.x, value.y, value.z, value.w);
    }
}