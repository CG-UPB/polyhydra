#version 330 core

in vec2 v_uv;

uniform vec3 u_color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(u_color.rgb, 1.0);
}