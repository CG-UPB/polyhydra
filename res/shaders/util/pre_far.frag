#version 330 core

in vec2 v_uv;

uniform float u_far;

layout (location = 1) out vec3 position;

void main()
{
    position = vec3(0.0, 0.0, -u_far);
}