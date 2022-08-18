#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out vec3 v_pos;
out vec3 v_normal;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

uniform float u_height;

void main()
{

    mat4 view_transform = u_view * u_transform;

    vec3 pos = a_pos + vec3(0.0, u_height, 0.0);

    v_pos = vec3(u_transform * vec4(pos, 1.0));
    v_normal = mat3(transpose(inverse(view_transform))) * a_normal;

    gl_Position = u_projection * u_view * vec4(v_pos, 1.0);
}