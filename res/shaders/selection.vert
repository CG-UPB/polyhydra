#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in int a_face_id;

flat out int v_face_id;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;


void main()
{
    v_face_id = a_face_id;
    gl_Position = u_projection * u_view * u_mesh_transform * u_mesh_transform * vec4(a_pos, 1.0);
}