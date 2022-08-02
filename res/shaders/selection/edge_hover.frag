#version 330 core

uniform vec4 u_color;
uniform float u_gamma;

out vec4 fragment_color;

void main()
{
    fragment_color = vec4(pow(u_color.rgb, vec3(1.0 / u_gamma)), u_color.a);
}