#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

uniform float u_scale;
uniform vec4 u_position;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

out vec2 tex_coord;

void main()
{
    tex_coord = a_tex_coord;
    vec4 view_pos = u_view * u_transform * u_position;
    gl_Position = u_projection * (view_pos + vec4(a_pos * -view_pos.z * u_scale, 0.0));
}