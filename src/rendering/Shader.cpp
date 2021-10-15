
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

    void Shader::bind()
    {
        glUseProgram(m_shaderID);
    }

    void Shader::unbind()
    {
        glUseProgram(0);
    }

    int Shader::getUniform(const std::string& name)
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

    void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
    {
        glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, &matrix[0][0]);
    }

    void Shader::setUniform1f(const std::string& name, float value)
    {
        glUniform1f(getUniform(name), value);
    }

    void Shader::setUniform2f(const std::string& name, glm::vec2& value)
    {
        glUniform2f(getUniform(name), value.x, value.y);
    }

    void Shader::setUniform3f(const std::string& name, glm::vec3& value)
    {
        glUniform3f(getUniform(name), value.x, value.y, value.z);
    }

    void Shader::setUniform4f(const std::string& name, glm::vec4& value)
    {
        glUniform4f(getUniform(name), value.x, value.y, value.z, value.w);
    }
}