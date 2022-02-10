#version 330 core

in vec2 v_uv;

uniform vec4 u_top_color;
uniform vec4 u_bottom_color;

out vec4 FragColor;

void main()
{
    vec4 color = mix(u_top_color, u_bottom_color, v_uv.y * v_uv.y * v_uv.y);
    FragColor = color;
}