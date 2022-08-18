#version 330 core

in vec2 v_uv;

uniform float u_gamma;
uniform vec3 u_color;

out vec4 FragColor;

void main()
{
    vec4 color = vec4(u_color.rgb, 1.0);
    FragColor = vec4(pow(color.rgb, vec3(1.0 / u_gamma)), color.a);
}