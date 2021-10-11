//
// Created by steffen on 11.10.21.
//

#ifndef VOLUMESHOS_SHADER_H
#define VOLUMESHOS_SHADER_H

#include <string>
#include <unordered_map>
#include "glm/mat4x4.hpp"

namespace vOS
{
    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        ~Shader();
        void bind();
        void unbind();
        int getUniform(const std::string& name);
        void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

    private:

        unsigned int m_shaderID;
        std::unordered_map<std::string, int*> m_locations;
    };
}



#endif //VOLUMESHOS_SHADER_H
