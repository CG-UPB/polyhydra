#version 330 core

uniform vec4 u_hover_color;

out vec4 fragment_color;

void main()
{
    fragment_color = u_hover_color;
}