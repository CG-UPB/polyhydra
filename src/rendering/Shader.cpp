//
// Created by steffen on 11.10.21.
//

#include <GL/glew.h>
#include <iostream>

#include "Shader.h"
#include "../fs/FileManager.h"

namespace vOS
{

    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
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

    Shader::~Shader()
    {
        for (auto& it: m_locations)
        {
            delete it.second;
        }
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
        int *location = m_locations[name];
        if (location != nullptr)
        {
            return *location;
        }
        int loc = glGetUniformLocation(m_shaderID, name.c_str());
        if (loc == -1)
        {
            std::cout << "Could not find uniform: " << name << std::endl;
        }
        m_locations[name] = new int(loc);
        return loc;
    }

    void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
    {
        glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, &matrix[0][0]);
    }
}