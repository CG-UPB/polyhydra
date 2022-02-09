#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_transform;

out vec3 v_pos;
out vec3 v_normal;

void main()
{
    v_pos = vec3(u_mesh_transform * u_transform * vec4(a_pos, 1.0));
    v_normal = mat3(transpose(inverse(u_mesh_transform * u_transform))) * a_normal;
    gl_Position = u_projection * u_view * u_mesh_transform * u_transform * vec4(a_pos, 1.0);
}