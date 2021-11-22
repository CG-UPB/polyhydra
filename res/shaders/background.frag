#version 330 core

in vec2 v_uv;

uniform vec4 u_color;

out vec4 FragColor;

void main()
{
    FragColor = u_color;
}