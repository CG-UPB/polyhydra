#version 330 core

const float far = 1000.0;

in vec2 v_uv;

layout (location = 1) out vec3 position;

void main()
{
    position = vec3(0.0, 0.0, -far);
}