#version 330 core

in vec2 v_uv;

uniform float u_gamma;

uniform vec4 u_top_color;
uniform vec4 u_bottom_color;

out vec4 FragColor;

void main()
{
    vec4 color = mix(u_top_color, u_bottom_color, v_uv.y * v_uv.y * v_uv.y);
    FragColor = vec4(pow(color.rgb, vec3(1.0 / u_gamma)), color.a);
}