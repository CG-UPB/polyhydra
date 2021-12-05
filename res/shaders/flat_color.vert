#version 330 core

layout (location = 0) in vec3 a_pos;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    gl_Position = u_projection * u_view * u_transform * vec4(a_pos, 1.0);
}