
#include "glad/glad.h"

#include "Shader.h"

#include <iostream>

#include "../../fs/FileManager.h"
#include "../../util/StringUtil.h"

namespace vOS
{
    std::unordered_map<std::string, Shader*> Shader::s_shaders;

    Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
    {
        std::string vertexSource = FileManager::load_as_string(vertexPath, true);
        std::string fragmentSource = FileManager::load_as_string(fragmentPath, true);

        m_shaderID = glCreateProgram();
        unsigned int vertexID = glCreateShader(GL_VERTEX_SHADER);
        unsigned int fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

        const GLchar* vertBuf = vertexSource.c_str();
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

        const GLchar* fragBuf = fragmentSource.c_str();
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

    void Shader::set_uniform_mat4f(const std::string& name, const glm::mat4& value)
    {
        glUniformMatrix4fv(get_uniform(name), 1, GL_FALSE, &value[0][0]);
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

    Shader* Shader::get(const std::string& shader_name)
    {
        auto shader = s_shaders.find(shader_name);
        if (shader == s_shaders.end())
        {
            throw std::invalid_argument("Could not find shader: " + shader_name);
        }
        return s_shaders[shader_name];
    }

    void Shader::load_all()
    {
        struct ShaderSourcePath
        {
            std::filesystem::path vertex;
            std::filesystem::path fragment;
        };

        std::unordered_map<std::string, ShaderSourcePath> shader_source_paths;

        std::string separator(&std::filesystem::path::preferred_separator);
        std::filesystem::path shader_path = "shaders";
        for (auto& file: std::filesystem::recursive_directory_iterator(FileManager::get_resource_path() / shader_path))
        {
            auto path_split = split_str(file.path().string(), separator);
            auto name_with_extension = split_str(path_split[path_split.size() - 1], ".");
            std::string name_without_extension = name_with_extension[0];
            std::string extension = name_with_extension[1];

            auto it = shader_source_paths.find(name_without_extension);
            if (it == shader_source_paths.end())
            {
                shader_source_paths[name_without_extension] = ShaderSourcePath();
            }
            auto& shader_source_path = shader_source_paths[name_without_extension];

            std::filesystem::path* source;
            if (extension == "vert")
            { source = &shader_source_path.vertex; }
            else if (extension == "frag")
            { source = &shader_source_path.fragment; }
            else
            { throw std::runtime_error("Unknown shader file extension: " + extension); }

            *source = file.path();
        }

        for (auto& shader_source_path : shader_source_paths)
        {
            s_shaders[shader_source_path.first] = new Shader(
                    shader_source_path.second.vertex,
                    shader_source_path.second.fragment
            );
        }
    }

    void Shader::delete_all()
    {
        for (auto& shader : s_shaders)
        {
            delete shader.second;
        }
    }
}