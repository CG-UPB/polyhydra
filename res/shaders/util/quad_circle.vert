#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

uniform float u_scale;
uniform vec4 u_position;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_average_cell_size;

out vec2 tex_coord;

void main()
{
    tex_coord = a_tex_coord;

    vec3 camera_right_ws = normalize(vec3(u_view[0][0], u_view[1][0], u_view[2][0]));
    vec3 camera_up_ws = normalize(vec3(u_view[0][1], u_view[1][1], u_view[2][1]));

    float scale = u_scale * u_average_cell_size;

    vec3 pos_ws = u_position.xyz + camera_right_ws * a_pos.x * scale + camera_up_ws * a_pos.y * scale;
    gl_Position = u_projection * u_view * u_transform * vec4(pos_ws, 1.0);
}