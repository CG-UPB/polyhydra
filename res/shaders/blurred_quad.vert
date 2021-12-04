#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

uniform float u_scale;
uniform vec4 u_position;
uniform float u_rotation;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

out vec2 tex_coord;

void main()
{
    mat4 rotation = mat4(
        cos(u_rotation), -sin(u_rotation), 0.0, 0.0,
        sin(u_rotation), cos(u_rotation), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    tex_coord = a_tex_coord;
    vec4 view_pos = u_view * u_transform * u_position;
    gl_Position = u_projection * (view_pos + inverse(rotation) * vec4(a_pos * -view_pos.z * u_scale, 0.0));
}