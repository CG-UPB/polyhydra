#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_center;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_cell_size;


void main()
{
    vec3 pos = a_center + (a_pos - a_center) * u_cell_size;
    gl_Position = u_projection * u_view * u_mesh_transform * vec4(pos, 1.0);
}