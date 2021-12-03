#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_offset;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    vec4 view_pos = u_view * u_mesh_transform * vec4(a_offset, 1.0);
    gl_Position = u_projection * (view_pos + vec4(a_pos * -view_pos.z * 0.01, 0.0));
}